#include "ESP.h"

// Track if temperature alert has been sent (to avoid duplicate alerts)
volatile uint8_t alert_sent = 0;

// Function to check if the ESP response matches the expected response
void waitForResponse(char* expectedResponse) {
    while (1) {
        if (strstr(CpyBuffer, expectedResponse) != NULL) {
            break;  // Response matches, exit the loop
        }
        delay_ms(100);
    }
    //IWDG_Refresh(); 
}

// Function to send a command to the ESP and wait for the expected response
void ESP_SendCommand(char *cmd, char *expectedResponse, uint32_t timeout) {
    SendTxt_usart2(cmd);   // Send the command to the ESP
    waitForResponse(expectedResponse);  // Wait for the expected response
    delay_ms(timeout);  // Wait for the specified timeout
}

// Initialize the ESP module and perform basic setup
void ESP_Init_Station_Client(void) {
    // Send commands and wait for expected responses
    ESP_SendCommand("AT\r\n", "\r\nOK\r\n", 1000);  // Check communication with ESP
    ESP_SendCommand("AT+RST\r\n", "\r\nOK\r\n", 5000);  // Reset ESP
    ESP_SendCommand("AT+CWMODE=1\r\n", "\r\nOK\r\n", 1000);  // Set ESP to Station (Client) mode
    
    sprintf(cmdBuffer, "AT+CWJAP=\"%s\",\"%s\"\r\n", Network, Password);
    ESP_SendCommand(cmdBuffer, "\r\nOK\r\n", 10000);  // Connect to the Wi-Fi network
    
    ESP_SendCommand("AT+CIFSR\r\n", "\r\nOK\r\n", 2000);  // Get the IP address
}

// Function to open a TCP connection with the server
void ESP_OpenTCPConnection(const char *serverIP, uint16_t serverPort) {
    
    // Open the TCP connection to the server
    sprintf(cmdBuffer, "AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", serverIP, serverPort);
    ESP_SendCommand(cmdBuffer, "\r\nOK\r\n", 1000);  // Wait for "CONNECT" response
    ClearBuffer(cmdBuffer, sizeof(cmdBuffer));
}

// Function to open an SSL connection with the server
void ESP_OpenSSLConnection(const char *serverHost, uint16_t serverPort) {
    
    // Open the SSL connection to the server
    sprintf(cmdBuffer, "AT+CIPSTART=\"SSL\",\"%s\",%d\r\n", serverHost, serverPort);
    ESP_SendCommand(cmdBuffer, "\r\nOK\r\n", 5000);  // Wait for "CONNECT" response (5000ms for SSL handshake)
    ClearBuffer(cmdBuffer, sizeof(cmdBuffer));
}

// Function to close the TCP connection with the server
void ESP_CloseTCPConnection(void) {
    // Close the TCP connection
    ESP_SendCommand("AT+CIPCLOSE\r\n", "OK\r\n", 2000);  // Wait for "CLOSE" response
}

// Function to send data to the server via TCP
void ESP_SendDataToServer(const char *serverIP, uint16_t serverPort) {
	
    //temperature = DS1621_Read_Temp();  // Uncomment if temperature reading is needed
    sprintf(DataBuffer, "Temp_DS1621=%.2f C\n",temperature);
    int dataLength = strlen(DataBuffer);  

    // Step 1: Open TCP connection
    ESP_OpenTCPConnection(serverIP, serverPort);

    // Step 2: Send the data
    sprintf(cmdBuffer, "AT+CIPSEND=%d\r\n", dataLength);  // Prepare command with data length
    ESP_SendCommand(cmdBuffer, ">", 5000);  // Wait for ">" prompt to send data
    SendTxt_usart2(DataBuffer);  // Send the actual data string
    waitForResponse("\r\nSEND OK\r\n");  // Wait for confirmation "SEND OK"

    // Step 3: Close the TCP connection
    ESP_CloseTCPConnection();
}

/*
 * Function to send WhatsApp alert via CallMeBot API
 * Sends alert only once when temperature exceeds 25°C
 */
void ESP_SendWhatsAppAlert(float temp)
{
    // Only send alert if temperature > 25 and we haven't sent it yet
    if (temp > 25.0 && !alert_sent)
    {
        alert_sent = 1;  // Mark alert as sent
        
        SendTxt_usart3("Temperature > 25C! Sending WhatsApp alert...\r\n");
        
        // Build the URL for CallMeBot API
        char url_buffer[URL_BUFFER_SIZE];
        char message[100];
        
        // Format the message with temperature value
        sprintf(message, "Alert: Chicken coop temperature is %.1f°C (VERY HOT!)", temp);
        
        // Build the full URL (URL encoding: space = %20)
        // GET /whatsapp.php?phone=[number]&text=[message]&apikey=[key]
        sprintf(url_buffer, 
                "GET /whatsapp.php?phone=%s&text=%s&apikey=%s HTTP/1.1\r\n"
                "Host: api.callmebot.com\r\n"
                "Connection: close\r\n\r\n",
                CALLMEBOT_PHONE_NUMBER, 
                message,
                CALLMEBOT_API_KEY);
        
        // Open TCP connection to CallMeBot server
        ESP_OpenTCPConnection("api.callmebot.com", 80);
        
        // Send HTTP GET request
        int urlLength = strlen(url_buffer);
        sprintf(cmdBuffer, "AT+CIPSEND=%d\r\n", urlLength);
        ESP_SendCommand(cmdBuffer, ">", 5000);
        SendTxt_usart2(url_buffer);
        waitForResponse("\r\nSEND OK\r\n");
        
        // Wait for response from server
        delay_ms(2000);
        
        // Close TCP connection
        ESP_CloseTCPConnection();
        
        SendTxt_usart3("WhatsApp alert sent successfully!\r\n");
    }
    else if (temp <= 25.0)
    {
        // Reset alert flag when temperature drops below threshold
        alert_sent = 0;
    }
}



void ESP_SendTempJSON(const char *serverHost, uint16_t serverPort)
{
    // Build JSON body: {"temperature":25.50}
    char jsonBody[64];
    sprintf(jsonBody, "{\"temperature\":%.2f}", temperature);
    int bodyLen = strlen(jsonBody);

    // Build full HTTP POST request
    char httpRequest[400];
    sprintf(httpRequest,
            "POST /api/temperature HTTP/1.1\r\n"
            "Host: %s\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: %d\r\n"
            "Connection: close\r\n"
            "\r\n"
            "%s",
            serverHost, bodyLen, jsonBody);

    int requestLen = strlen(httpRequest);

    // Step 1: Open SSL connection
    ESP_OpenSSLConnection(serverHost, serverPort);

    // Step 2: Tell ESP how many bytes we're sending
    sprintf(cmdBuffer, "AT+CIPSEND=%d\r\n", requestLen);
    ESP_SendCommand(cmdBuffer, ">", 5000);

    // Step 3: Send the HTTP request
    SendTxt_usart2(httpRequest);
    waitForResponse("\r\nSEND OK\r\n");

    // Step 4: (Optional) wait a moment for server response
    delay_ms(1000);

    // Step 5: Close TCP connection
    ESP_CloseTCPConnection();
}