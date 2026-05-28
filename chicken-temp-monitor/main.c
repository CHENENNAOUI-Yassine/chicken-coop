#include "stm32f4xx.h"
#include "ESP.h"
#include "UTIL.h"
#include <stdbool.h>


// Buffers
char cmdBuffer[CMD_BUFFER_SIZE];
char RX2_Buffer[TX_RX_BUFFER_SIZE];
char DataBuffer[DATA_BUFFER_SIZE];
char CpyBuffer[DATA_BUFFER_SIZE];

int j = 0;
float temperature;



int main(void)
{
    USART2_Init();
    USART3_Init();

    SendTxt_usart3("MCU STARTED\r\n");

    if (PWR->CSR & (1 << 1))
    {
        SendTxt_usart3("Woke from STANDBY\r\n");

        PWR->CR |= (1 << 2); // Clear Wakeup flag
        PWR->CR |= (1 << 3); // Clear Standby flag
    }

    /* Configure RTC FIRST */
    RTC_Wakeup_Init(300);

    I2C1_Init();
    DS1621_Init();

    ESP_Init_Station_Client();

    temperature = DS1621_Read_Temp();

    // Check temperature and send WhatsApp alert if needed
    ESP_SendWhatsAppAlert(temperature);

    // Wake Render dyno before sending data
    ESP_WakeRender("chicken-coop.onrender.com", 443);

    // Send temperature data to Render with retry logic
    uint8_t sendResult = ESP_SendTempJSON("chicken-coop.onrender.com", 443);
    if (sendResult == 0)
    {
        // Cold start detected - Render needs more time
        SendTxt_usart3("Render cold start - retrying in 20s...\r\n");
        delay_ms(20000);
        ESP_SendTempJSON("chicken-coop.onrender.com", 443);
    }

    ESP_SendDataToServer("10.119.156.219", 80);

    SendTxt_usart3("Going to STANDBY...\r\n");

    delay_ms(1000);

    enter_standby_mode();

    while (1){}
		}