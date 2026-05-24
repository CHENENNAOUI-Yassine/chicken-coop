#include "UTIL.h"

/*====================================================
  Clear Buffer
====================================================*/
void ClearBuffer(char *buffer, int size)
{
    for (int j = 0; j < size; j++)
    {
        buffer[j] = '\0';
    }
}

/*====================================================
  Simple delay using SysTick
====================================================*/
void delay_ms(uint32_t ms)
{
    SysTick->LOAD = (SystemCoreClock / 1000) - 1;
    SysTick->VAL  = 0;

    SysTick->CTRL =
        SysTick_CTRL_CLKSOURCE_Msk |
        SysTick_CTRL_ENABLE_Msk;

    for (uint32_t i = 0; i < ms; i++)
    {
        while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
    }

    SysTick->CTRL = 0;
}

/*====================================================
  Enter STANDBY mode
====================================================*/
void enter_standby_mode(void)
{
    /* Enable PWR clock */
    RCC->APB1ENR |= (1 << 28);

    /* Clear Wakeup flag (CWUF) */
    PWR->CR |= (1 << 2);

    /* Enable Wakeup pin (optional) */
    PWR->CSR |= (1 << 8);

    /* Select Standby mode (PDDS = 1) */
    PWR->CR |= (1 << 1);

    /* Cortex enters Deep Sleep */
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    /* Wait For Interrupt -> enter standby */
    __WFI();

    /* Never reaches here */
}

/*====================================================
  RTC Wakeup Init
====================================================*/
void RTC_Wakeup_Init(uint32_t seconds)
{
    /*====================================================
      1. Enable PWR clock
    ====================================================*/
    RCC->APB1ENR |= (1 << 28);

    /*====================================================
      2. Allow access to Backup Domain
    ====================================================*/
    PWR->CR |= (1 << 8);   /* DBP */
    delay_ms(10);

    /*====================================================
      3. Reset Backup Domain
         (only useful during debug/testing)
    ====================================================*/
    RCC->BDCR |=  (1 << 16);   /* BDRST */
    RCC->BDCR &= ~(1 << 16);

    /*====================================================
      4. Enable LSI oscillator
    ====================================================*/
    RCC->CSR |= (1 << 0);   /* LSION */

    while (!(RCC->CSR & (1 << 1)))
    {
        /* Wait for LSIRDY */
    }

    /*====================================================
      5. Select LSI as RTC clock source
         RTCSEL = 10 -> LSI
    ====================================================*/
    RCC->BDCR &= ~(3 << 8);
    RCC->BDCR |=  (2 << 8);

    /*====================================================
      6. Enable RTC
    ====================================================*/
    RCC->BDCR |= (1 << 15);   /* RTCEN */

    /*====================================================
      7. Disable RTC write protection
    ====================================================*/
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;

    /*====================================================
      8. Disable Wakeup Timer
    ====================================================*/
    RTC->CR &= ~(1 << 10);   /* WUTE = 0 */

    while (!(RTC->ISR & (1 << 2)))
    {
        /* Wait until WUTWF = 1 */
    }

    /*====================================================
      9. Enter INIT mode
    ====================================================*/
    RTC->ISR |= (1 << 7);   /* INIT = 1 */

    while (!(RTC->ISR & (1 << 6)))
    {
        /* Wait until INITF = 1 */
    }

    /*====================================================
      10. Prescaler configuration

      LSI ˜ 32 kHz
      Make ck_spre ˜ 1 Hz
    ====================================================*/
    RTC->PRER = (127 << 16) | 249;

    /*
      Async prescaler = 127
      Sync prescaler  = 249

      Better timing than 255 for many STM32 boards
    */

    /*====================================================
      11. Exit INIT mode
    ====================================================*/
    RTC->ISR &= ~(1 << 7);

    /*====================================================
      12. Clear old Wakeup flag
    ====================================================*/
    RTC->ISR &= ~(1 << 10);   /* Clear WUTF */

    /*====================================================
      13. Configure Wakeup Timer

      IMPORTANT:
      WUTR = seconds - 1
    ====================================================*/
    if (seconds > 0)
        RTC->WUTR = seconds - 1;
    else
        RTC->WUTR = 0;

    /*
      WUCKSEL = 100
      ck_spre (1 Hz clock)
    */
    RTC->CR &= ~(7 << 0);
    RTC->CR |=  (4 << 0);

    /*====================================================
      14. Enable Wakeup Interrupt
    ====================================================*/
    RTC->CR |= (1 << 14);   /* WUTIE */

    /*====================================================
      15. EXTI Line 22
    ====================================================*/
    EXTI->IMR  |= (1 << 22);
    EXTI->RTSR |= (1 << 22);

    /* Clear pending EXTI flag */
    EXTI->PR |= (1 << 22);

    /*====================================================
      16. NVIC
    ====================================================*/
    NVIC_EnableIRQ(RTC_WKUP_IRQn);

    /*====================================================
      17. Enable Wakeup Timer
    ====================================================*/
    RTC->CR |= (1 << 10);   /* WUTE = 1 */

    /*====================================================
      18. Re-enable write protection
    ====================================================*/
    RTC->WPR = 0xFF;
}

/*====================================================
  RTC Wakeup Interrupt Handler
====================================================*/
void RTC_WKUP_IRQHandler(void)
{
    /* Clear EXTI pending flag */
    EXTI->PR |= (1 << 22);

    /* Clear RTC Wakeup flag (WUTF) */
    RTC->ISR &= ~(1 << 10);
}