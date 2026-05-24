# Chicken Coop Temperature Monitoring System

A low-power IoT system for monitoring chicken coop temperatures using an STM32F407 microcontroller with WiFi connectivity via an ESP8266 module.

## Overview

This project implements an automated temperature monitoring solution for chicken coops. The system:
- Reads temperature data from a DS1621 temperature sensor via I2C
- Enters low-power standby mode to conserve battery
- Wakes up periodically (configurable interval, default 300 seconds) using the RTC
- Connects to a WiFi network via ESP8266 module
- Sends temperature data to a remote server over TCP/IP
- Sends WhatsApp alerts when temperature exceeds 25°C

## Hardware

- **MCU**: STM32F407VGTx (ARM Cortex-M4)
- **Temperature Sensor**: DS1621 (I2C interface)
- **WiFi Module**: ESP8266 (USART communication)
- **Power Management**: RTC-based wakeup from standby mode

## Project Structure

```
chicken-temp-monitor/
├── main.c              # Main application entry point
├── ESP.c/ESP.h         # ESP8266 WiFi module driver
├── I2C.c/I2C.h         # I2C communication interface
├── USART.c/USART.h     # UART communication (for debugging and ESP)
├── UTIL.c/UTIL.h       # Utility functions
├── DebugConfig/        # Debug configuration
├── Objects/            # Compiled object files (build artifacts)
└── Listings/           # Assembly listings (build artifacts)

backend/               # Backend server code (if applicable)
```

## Building the Project

This project uses **Keil µVision5** IDE. To build:

1. Open `chicken-temp-monitor/chicken-temp-monitor.uvprojx` in Keil µVision
2. Select the appropriate target configuration
3. Click "Build" or press F7
4. Generated files will be in the `Objects/` directory

## Communication Interfaces

### I2C1 (Inter-Integrated Circuit)
- **Pins**: PB6 (SCL), PB7 (SDA)
- **Speed**: 400 kHz
- **Purpose**: Temperature sensor (DS1621) communication
- **Pull-ups**: Open-drain configuration with external pull-up resistors

### USART2
- **Purpose**: Communication with ESP8266 WiFi module
- **Baud Rate**: 9600
- **Mapped to**: GPIOA pins (PA2 RX, PA3 TX)

### USART3
- **Pins**: PB10 (TX), PB11 (RX)
- **Baud Rate**: 9600
- **Purpose**: Debug output and serial console
- **Features**: RXNEIE (RX interrupt) and IDLEIE (Idle line interrupt) enabled

## Configuration

Key settings in `ESP.h`:
- **WiFi Network**: "S22 de Yassin"
- **WiFi Password**: "Banani12345"
- **Server IP**: "10.119.156.219" (configurable)
- **Server Port**: 80 (HTTP)
- **RTC Wakeup Interval**: 300 seconds (5 minutes, configurable)
- **CallMeBot Phone Number**: "+212612345678" (for WhatsApp alerts, configurable)
- **CallMeBot API Key**: "YOUR_API_KEY_HERE" (for WhatsApp alerts, configurable)

### Temperature Alert System

When the temperature exceeds 25°C, the system automatically sends a WhatsApp alert via CallMeBot:
- **Alert Message**: "Alert: Chicken coop temperature is XX.X°C (VERY HOT!)"
- **Alert Type**: One-time alert per temperature threshold breach
- **Configuration**: See CallMeBot settings above

## Key Features

- **Low Power**: Standby mode between temperature readings
- **Wireless Data Transfer**: TCP/IP via ESP8266
- **Sensor Integration**: DS1621 temperature sensor support
- **Debugging**: USART3 for debug output

## Low-Power Standby Mode

The system is designed to maximize battery life by using the STM32F407's standby mode. This is critical for remote installations where power consumption must be minimized.

### How It Works

1. **System Starts**: MCU initializes all peripherals (I2C, USART, RTC)
2. **Read & Transmit**: Reads temperature from DS1621 sensor and sends data to server
3. **Check Alerts**: If temperature > 25°C, sends WhatsApp alert
4. **Enter Standby**: MCU enters ultra-low power standby mode
5. **RTC Wakeup**: After configured interval (default 300 seconds), RTC wakes the MCU
6. **Loop Repeats**: System resumes from step 2

### Power Consumption

| State | Current Draw | Notes |
|-------|--------------|-------|
| **Active (Normal Operation)** | ~50-80 mA | WiFi module + MCU running |
| **Standby Mode** | ~10-20 µA | RTC running, everything else off |
| **Sleep with WiFi Ready** | ~50 mA | Between readings (minimal) |

**Battery Life Improvement:**
- Without standby: ~8-10 hours (continuous operation at 65 mA average)
- **With standby + 300s interval**: ~2-3 **months** (depends on battery capacity)

**Example:**
- Reading cycle: 10 seconds
- Standby interval: 300 seconds (5 minutes)
- Duty cycle: 10/310 = 3.2%
- Average current: 65 mA × 3.2% = 2.1 mA
- With 5000 mAh battery: ~100 days (3.3 months)

### Implementation Details

**RTC Wakeup Configuration:**
```c
// In main.c
RTC_Wakeup_Init(300);  // Wake up every 300 seconds (5 minutes)
```
- The RTC uses the **LSI oscillator** (Low-Speed Internal 32 kHz)
- Maintains timing accuracy even during deep sleep
- Configurable wake interval in seconds (1-65535 supported)

**Entering Standby Mode:**
```c
// In UTIL.c - enter_standby_mode() function
// - Clears wakeup flag
// - Enables PWR peripheral
// - Sets PDDS bit (Powerdown Deepsleep)
// - Executes WFI (Wait For Interrupt)
// - System consumes only µA of current until RTC fires
```

**Wakeup Process:**
```c
// Check if woke from standby (in main.c)
if (PWR->CSR & (1 << 1))
{
    SendTxt_usart3("Woke from STANDBY\r\n");
    PWR->CR |= (1 << 2);  // Clear Wakeup flag
    PWR->CR |= (1 << 3);  // Clear Standby flag
}
```

### Configuring Wakeup Interval

To change how often the system reads and transmits:

**Option 1: Use Config Updater Agent (Recommended)**
```
In Copilot Chat: /Config Updater
Select: "5. RTC Wakeup Interval (seconds between readings)"
Enter new value (e.g., 600 for 10 minutes)
```

**Option 2: Manual Edit**
- Open `main.c`
- Find line: `RTC_Wakeup_Init(300);`
- Change `300` to desired seconds
- Recompile and upload

**Recommended Intervals:**
| Interval | Frequency | Use Case | Estimated Battery Life |
|----------|-----------|----------|------------------------|
| 60 | Every 1 minute | Intensive monitoring | ~1-2 weeks |
| 300 | Every 5 minutes | Normal operation | ~2-3 months |
| 600 | Every 10 minutes | Extended deployment | ~4-5 months |
| 1800 | Every 30 minutes | Long-term monitoring | ~1 year |
| 3600 | Every hour | Ultra-low power | ~2+ years |

## Development Tools & Agents

### Config Updater Agent

This project includes a custom **Config Updater** agent that simplifies configuration changes without manual editing.

**How to use:**
1. In Copilot Chat, type `/Config Updater`
2. The agent will ask which setting you want to change:
   - WiFi Network Name (SSID)
   - WiFi Password
   - ESP Server IP Address
   - Server Port Number
   - RTC Wakeup Interval (in seconds)
   - CallMeBot Phone Number (for WhatsApp alerts)
   - CallMeBot API Key (for WhatsApp alerts)
3. Provide the new value
4. The agent automatically updates all relevant files:
   - `ESP.h` - Macro definitions
   - `main.c` - Function calls
   - `README.md` - Documentation

**Example usage:**
```
User: "/Config Updater"
Agent: "What would you like to configure? (1) WiFi SSID (2) WiFi Password (3) Server IP ..."
User: "3"
Agent: "Current Server IP: 10.119.156.219. Enter new IP:"
User: "192.168.1.100"
Agent: "✓ Updated ESP.h and main.c with new IP: 192.168.1.100"
```

**Files modified:**
- Updates network settings: `ESP.h`
- Updates server connection in `main.c`
- Updates RTC interval in `main.c` (RTC_Wakeup_Init call)
- Updates WhatsApp settings: `ESP.h` (phone and API key)
- Updates documentation: `README.md`
- When you clone this repository, the agent is **automatically available** in VS Code
- No installation or copying needed — it's project-scoped
- All team members get the same agent configuration

## Quick Reference - Configuration Settings

| Setting | File | Location | Current Value |
|---------|------|----------|----------------|
| WiFi SSID | ESP.h | Line ~11 | "S22 de Yassin" |
| WiFi Password | ESP.h | Line ~12 | "Banani12345" |
| Server IP | main.c | Line ~42 | "10.119.156.219" |
| Server Port | main.c | Line ~42 | 80 |
| RTC Wakeup Interval | main.c | Line ~33 | 300 seconds |
| WhatsApp Phone | ESP.h | Line ~18 | "+212612345678" |
| WhatsApp API Key | ESP.h | Line ~19 | "YOUR_API_KEY_HERE" |
| Temperature Alert Threshold | ESP.c | Line ~85 | 25°C |

## How to Setup WhatsApp Alerts

1. Visit [CallMeBot WhatsApp API](https://www.callmebot.com/blog/free-api-whatsapp-messages/)
2. Get your API key from the website
3. Use the Config Updater agent to set:
   - Your phone number (with country code, e.g., +1234567890)
   - Your API key
4. When temperature exceeds 25°C, you'll receive a WhatsApp message
5. **Note**: Alert is sent only once per threshold breach; it resets when temp drops below 25°C

## Development Notes

- Buffers are defined in main.c for command, RX, and data handling
- Temperature readings are taken before entering standby mode
- The system automatically reconnects to the WiFi network upon wakeup
- Debug output can be monitored via USART3
- **Standby mode**: All peripherals except RTC are powered down
- **RTC Configuration**: Uses 32 kHz LSI oscillator for accurate timing during sleep
- **Wakeup Sequence**: Takes ~2-3 seconds to reinitialize WiFi and send data
- **No data loss**: Temperature and alert status are preserved in RAM during standby

## License

[Add appropriate license information]

## Author

[Add author/team information]
