#ifndef ESP_UTILS_H
#define ESP_UTILS_H

#include "stm32f4xx.h"  
#include <stdio.h>       
#include "string.h"
#include "USART.h"
#include "UTIL.h"
#include "I2C.h"

#define Network "S22 de Yassin"
#define Password "Banani12345"

//#define IP_ADDRESS2 "10.119.156.219" //(ESP2)
//#define IP_ADDRESS2 "10.119.156.237" //(ESP1)

// CallMeBot WhatsApp Configuration - CHANGE THESE
#define CALLMEBOT_PHONE_NUMBER "+212612345678"  // Your phone number with country code
#define CALLMEBOT_API_KEY "YOUR_API_KEY_HERE"   // Get from https://www.callmebot.com

#define CMD_BUFFER_SIZE 500
#define DATA_BUFFER_SIZE 500
#define URL_BUFFER_SIZE 600
//#define serverIP "192.168.8.101"
//#define serverPort 80

extern float temperature;
extern char cmdBuffer[CMD_BUFFER_SIZE]; 
extern char DataBuffer[DATA_BUFFER_SIZE];
extern volatile uint8_t alert_sent;  // Track if temperature alert was already sent



void ESP_Init_Station_Client(void);

void ESP_SendCommand(char *cmd, char *expectedResponse, uint32_t timeout);

void waitForResponse(char* expectedResponse);

void ESP_OpenTCPConnection(const char *serverIP, uint16_t serverPort);

void ESP_OpenSSLConnection(const char *serverHost, uint16_t serverPort);

void ESP_CloseTCPConnection(void);

void ESP_SendDataToServer(const char *serverIP, uint16_t serverPort);

void ESP_SendWhatsAppAlert(float temp);

void ESP_WakeRender(const char *serverHost, uint16_t serverPort);

uint8_t ESP_SendTempJSON(const char *serverHost, uint16_t serverPort);

#endif // ESP_UTILS_H
