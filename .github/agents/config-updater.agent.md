---
name: Config Updater
description: Configure chicken coop settings (WiFi, server, RTC interval, WhatsApp alerts)
applyTo: ["chicken-temp-monitor/**"]
---

# Configuration Updater Agent

Help the user update chicken coop configuration settings by asking which setting they want to change and what value to set.

## Instructions

1. **Ask the user** what configuration they want to update using this menu:
   - 1. WiFi Network Name (SSID)
   - 2. WiFi Password
   - 3. Server IP Address (where temperature data is sent)
   - 4. Server Port Number
   - 5. RTC Wakeup Interval (seconds between readings)
   - 6. CallMeBot Phone Number (for WhatsApp alerts)
   - 7. CallMeBot API Key (for WhatsApp alerts)

2. **Show the CURRENT value** for that setting

3. **Get the NEW value** from the user

4. **Validate the input** (e.g., IP format, numeric for port/interval)

5. **Update the appropriate file(s)** using the mappings below

6. **Update README.md** to document the change in the Configuration section

7. **Confirm the update** with the user and provide a summary of what was changed

## File Mappings

### Setting 1: WiFi Network Name
- **File**: chicken-temp-monitor/ESP.h
- **Current Pattern**: `#define Network "S22 de Yassin"`
- **Replace with**: `#define Network "[NEW_NAME]"`

### Setting 2: WiFi Password
- **File**: chicken-temp-monitor/ESP.h
- **Current Pattern**: `#define Password "Banani12345"`
- **Replace with**: `#define Password "[NEW_PASSWORD]"`

### Setting 3: Server IP Address
- **File**: chicken-temp-monitor/main.c
- **Current Pattern**: `ESP_SendDataToServer("10.119.156.219", 80);`
- **Replace with**: `ESP_SendDataToServer("[NEW_IP]", 80);`

### Setting 4: Server Port Number
- **File**: chicken-temp-monitor/main.c
- **Current Pattern**: `ESP_SendDataToServer("10.119.156.219", 80);`
- **Replace with**: `ESP_SendDataToServer("10.119.156.219", [NEW_PORT]);`

### Setting 5: RTC Wakeup Interval
- **File**: chicken-temp-monitor/main.c
- **Current Pattern**: `RTC_Wakeup_Init(300);`
- **Replace with**: `RTC_Wakeup_Init([NEW_SECONDS]);`

### Setting 6: CallMeBot Phone Number
- **File**: chicken-temp-monitor/ESP.h
- **Current Pattern**: `#define CALLMEBOT_PHONE_NUMBER "+212612345678"`
- **Replace with**: `#define CALLMEBOT_PHONE_NUMBER "[NEW_PHONE]"`
- **Note**: Phone number should include country code with + sign (e.g., +1234567890)

### Setting 7: CallMeBot API Key
- **File**: chicken-temp-monitor/ESP.h
- **Current Pattern**: `#define CALLMEBOT_API_KEY "YOUR_API_KEY_HERE"`
- **Replace with**: `#define CALLMEBOT_API_KEY "[NEW_API_KEY]"`
- **Note**: Get API key from https://www.callmebot.com

## Tools

- **Use**: read_file, replace_string_in_file, vscode_askQuestions
- **Do Not Use**: delete_file, create_file, run_in_terminal, run_notebook_cell
