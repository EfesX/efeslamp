#pragma once

#include <Arduino.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include "CLICommandClass.h"
#include "EEPROMHelper.h"


#define MQTT_PRINT_DEBUG 1

static const char* mqttserver = "130.193.44.244"; //mqtt.cloud.yandex.net
//static const char* yandexIoTCoreDeviceId = "arel66o1mos60829i19a";
//static const char* mqttpassword = "ubuntu56abrahamDC";
static const int mqttport = 8883;

static String deviceID;
static String devicePASS;

static String topicCommands = "commands";
static String topicEvents = "events";
static String topicLog = "log";

static const char* rootCA = "-----BEGIN CERTIFICATE-----\n \
MIIFGTCCAwGgAwIBAgIQJMM7ZIy2SYxCBgK7WcFwnjANBgkqhkiG9w0BAQ0FADAf\
MR0wGwYDVQQDExRZYW5kZXhJbnRlcm5hbFJvb3RDQTAeFw0xMzAyMTExMzQxNDNa\
Fw0zMzAyMTExMzUxNDJaMB8xHTAbBgNVBAMTFFlhbmRleEludGVybmFsUm9vdENB\
MIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAgb4xoQjBQ7oEFk8EHVGy\
1pDEmPWw0Wgw5nX9RM7LL2xQWyUuEq+Lf9Dgh+O725aZ9+SO2oEs47DHHt81/fne\
5N6xOftRrCpy8hGtUR/A3bvjnQgjs+zdXvcO9cTuuzzPTFSts/iZATZsAruiepMx\
SGj9S1fGwvYws/yiXWNoNBz4Tu1Tlp0g+5fp/ADjnxc6DqNk6w01mJRDbx+6rlBO\
aIH2tQmJXDVoFdrhmBK9qOfjxWlIYGy83TnrvdXwi5mKTMtpEREMgyNLX75UjpvO\
NkZgBvEXPQq+g91wBGsWIE2sYlguXiBniQgAJOyRuSdTxcJoG8tZkLDPRi5RouWY\
gxXr13edn1TRDGco2hkdtSUBlajBMSvAq+H0hkslzWD/R+BXkn9dh0/DFnxVt4XU\
5JbFyd/sKV/rF4Vygfw9ssh1ZIWdqkfZ2QXOZ2gH4AEeoN/9vEfUPwqPVzL0XEZK\
r4s2WjU9mE5tHrVsQOZ80wnvYHYi2JHbl0hr5ghs4RIyJwx6LEEnj2tzMFec4f7o\
dQeSsZpgRJmpvpAfRTxhIRjZBrKxnMytedAkUPguBQwjVCn7+EaKiJfpu42JG8Mm\
+/dHi+Q9Tc+0tX5pKOIpQMlMxMHw8MfPmUjC3AAd9lsmCtuybYoeN2IRdbzzchJ8\
l1ZuoI3gH7pcIeElfVSqSBkCAwEAAaNRME8wCwYDVR0PBAQDAgGGMA8GA1UdEwEB\
/wQFMAMBAf8wHQYDVR0OBBYEFKu5xf+h7+ZTHTM5IoTRdtQ3Ti1qMBAGCSsGAQQB\
gjcVAQQDAgEAMA0GCSqGSIb3DQEBDQUAA4ICAQAVpyJ1qLjqRLC34F1UXkC3vxpO\
nV6WgzpzA+DUNog4Y6RhTnh0Bsir+I+FTl0zFCm7JpT/3NP9VjfEitMkHehmHhQK\
c7cIBZSF62K477OTvLz+9ku2O/bGTtYv9fAvR4BmzFfyPDoAKOjJSghD1p/7El+1\
eSjvcUBzLnBUtxO/iYXRNo7B3+1qo4F5Hz7rPRLI0UWW/0UAfVCO2fFtyF6C1iEY\
/q0Ldbf3YIaMkf2WgGhnX9yH/8OiIij2r0LVNHS811apyycjep8y/NkG4q1Z9jEi\
VEX3P6NEL8dWtXQlvlNGMcfDT3lmB+tS32CPEUwce/Ble646rukbERRwFfxXojpf\
C6ium+LtJc7qnK6ygnYF4D6mz4H+3WaxJd1S1hGQxOb/3WVw63tZFnN62F6/nc5g\
6T44Yb7ND6y3nVcygLpbQsws6HsjX65CoSjrrPn0YhKxNBscF7M7tLTW/5LK9uhk\
yjRCkJ0YagpeLxfV1l1ZJZaTPZvY9+ylHnWHhzlq0FzcrooSSsp4i44DB2K7O2ID\
87leymZkKUY6PMDa4GkDJx0dG4UXDhRETMf+NkYgtLJ+UIzMNskwVDcxO4kVL+Hi\
Pj78bnC5yCw8P5YylR45LdxLzLO68unoXOyFz1etGXzszw8lJI9LNubYxk77mK8H\
LpuQKbSbIERsmR+QqQ==\
-----END CERTIFICATE-----\n";

BearSSL::X509List x509(rootCA);
static PubSubClient *smqtt;

void mqtt_printf(char *msg){
#if (MQTT_PRINT_DEBUG == 1)
    Serial.println(msg);
    smqtt->publish(topicLog.c_str(), msg);
#endif
}


class MQTTCommands{
    public:
        virtual ~MQTTCommands() = 0;
        virtual char *getTopicName() = 0;
        virtual void execute() = 0;
};


class MQTTProcessor {
    private:
        PubSubClient *p_mqtt;
        char wbuf[256];
    public:
        MQTTProcessor(){}
        
        void setClient(PubSubClient *_mqtt){
            p_mqtt = _mqtt;
        }

        void execute(char* topic, byte* payload, uint16_t len){
            Serial.println((char *) payload);
            if(!strcmp(topic, topicCommands.c_str())){
                CLICommands::CLI()->execute((char *) payload, wbuf);
                for(uint8_t i = 0; i < 64; ++i){
                    payload[i] = 0;
                }
                mqtt_printf(wbuf);
            }
        }
};

static MQTTProcessor mqttproc;
static char __payload[64];

extern EEPROMHelper eeprom;

class MQTT {
    private:
        WiFiClientSecure wificlient;
        PubSubClient _mqtt;
        uint64_t lasttime;
        boolean connected;

        void connect()
        {
            wificlient.setInsecure();
            _mqtt.setBufferSize(1024);
            _mqtt.setKeepAlive(15);

            while(!_mqtt.connect("EFESLAMP_001", deviceID.c_str(), devicePASS.c_str())){};
            _mqtt.subscribe(topicCommands.c_str());
        }

    public:
        MQTT(){}
        
        void begin()
        {    
            connected = false;

            deviceID    = eeprom.read(EEPROM_CLOUD_ID_POS);
            devicePASS  = eeprom.read(EEPROM_CLOUD_PASS_POS);


            _mqtt.setClient(wificlient);
            _mqtt.setServer(mqttserver, mqttport);
            _mqtt.setCallback([](char* topic, byte* payload, uint16_t len)
            {
                if(len > 64) return;
                else
                {
                    for(uint8_t i = 0; i < len; i++)
                    {
                        __payload[i] = payload[i];
                    }
                    __payload[len] = '\r\n';
                    mqttproc.execute(topic, (byte *) __payload, len);
                }
            });
            _mqtt.setBufferSize(1024);
            _mqtt.setKeepAlive(15);
            this->connect();

            mqttproc.setClient(&_mqtt);
            smqtt = &_mqtt;
            lasttime = millis();
        }

        static boolean publish(String topic, String msg){
            return smqtt->publish(topic.c_str(), msg.c_str());            
        }

        void tick(void)
        {
            _mqtt.loop();
            if(!_mqtt.connected())
            {
                this->connect();
            }
            
        }
};

