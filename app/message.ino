#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>
#include <DHT.h>

#if SIMULATED_DATA

void initSensor()
{
    // use SIMULATED_DATA, no sensor need to be inited
}

float readTemperature()
{
    return random(20, 30);
}

float readHumidity()
{
    return random(30, 40);
}

#else

static DHT dht(DHT_PIN, DHT_TYPE);
void initSensor()
{
    dht.begin();
}

float readTemperature()
{
    return dht.readTemperature();
}

float readHumidity()
{
    return dht.readHumidity();
}

#endif

void readMessage(int messageId, char *payload)
{
    float temperature = readTemperature();
    float humidity = readHumidity();
    StaticJsonBuffer<MESSAGE_MAX_LEN> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["deviceId"] = DEVICE_ID;
    root["messageId"] = messageId;

    // NAN is not the valid json, change it to NULL
    if(std::isnan(temperature))
    {
        root["temperature"] = NULL;
    }
    else
    {
        root["temperature"] = temperature;
    }

    if(std::isnan(humidity))
    {
        root["humidity"] = NULL;
    }
    else
    {
        root["humidity"] = humidity;
    }
    root.printTo(payload, MESSAGE_MAX_LEN);
}

void executeCommand(char * json)
{
    StaticJsonBuffer<MESSAGE_MAX_LEN> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(json);
    if (!root.success())
    {
        LogInfo("Parse command %s failed", json);
        return;
    }

    if(!root.containsKey("command") || !root["command"].is<const char *>())
    {
        LogInfo("No command string value found in %s", json);
        return;
    }

    const char * command = root["command"];
    if(strcmp(command, "blink") == 0)
    {
        blinkLED();
    }
    else if(strcmp(command, "start") == 0)
    {
        messageSending = true;
    }
    else if(strcmp(command, "stop") == 0)
    {
        messageSending = false;
    }
}
