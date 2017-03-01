// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Please use an Arduino IDE 1.6.8 or greater

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiUdp.h>

#include <AzureIoTHub.h>
#include <AzureIoTProtocol_HTTP.h>

#include "config.h"

static bool messagePending = false;
static bool messageSending = true;

static char * connectionString;
static char * ssid;
static char * pass;

void blinkLED()
{
    digitalWrite(LED_PIN, HIGH);
    delay(500);
    digitalWrite(LED_PIN, LOW);
}

void initWifi()
{
    // Attempt to connect to Wifi network:
    LogInfo("Attempting to connect to SSID: %s", ssid);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED)
    {
        // Get Mac Address and show it.
        // WiFi.macAddress(mac) save the mac address into a six length array, but the endian may be different. The huzzah board should
        // start from mac[0] to mac[5], but some other kinds of board run in the oppsite direction.
        uint8_t mac[6];
        WiFi.macAddress(mac);
        LogInfo("You device with MAC address %02x:%02x:%02x:%02x:%02x:%02x connects to %s failed! Waiting 10 seconds to retry.",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], ssid);
        WiFi.begin(ssid, pass);
        delay(10000);
    }
    LogInfo("Connected to wifi %s", ssid);
}

void initTime()
{
    time_t epochTime;
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");

    while (true)
    {
        epochTime = time(NULL);

        if (epochTime == 0)
        {
            LogInfo("Fetching NTP epoch time failed! Waiting 2 seconds to retry.");
            delay(2000);
        }
        else
        {
            LogInfo("Fetched NTP epoch time is: %lu", epochTime);
            break;
       }
    }
}

static IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle;
void setup()
{
    pinMode(LED_PIN, OUTPUT);

    initSerial();
    delay(2000);
    readCredentials();

    initWifi();
    initTime();
    initSensor();
    initIoThubClient();

    iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(connectionString, HTTP_Protocol);
    if (iotHubClientHandle == NULL)
    {
        LogInfo("Failed on IoTHubClient_CreateFromConnectionString");
        while(1);
    }

    // Because it can poll "after 2 seconds" polls will happen
    // effectively at ~3 seconds.
    // Note that for scalabilty, the default value of minimumPollingTime
    // is 25 minutes. For more information, see:
    // https://azure.microsoft.com/documentation/articles/iot-hub-devguide/#messaging
    int minimumPollingTime = 2;
    if (IoTHubClient_LL_SetOption(iotHubClientHandle, "MinimumPollingTime", &minimumPollingTime) != IOTHUB_CLIENT_OK)
    {
        LogInfo("failure to set option \"MinimumPollingTime\"\r\n");
    }

    IoTHubClient_LL_SetMessageCallback(iotHubClientHandle, receiveMessageCallback, NULL);
}

static int messageCount = 1;
void loop()
{
    if(!messagePending && messageSending)
    {
        char messagePayload[MESSAGE_MAX_LEN];
        readMessage(messageCount, messagePayload);
        sendMessage(iotHubClientHandle, messagePayload);
        messageCount++;
    }
    IoTHubClient_LL_DoWork(iotHubClientHandle);
    delay(INTERVAL);
}
