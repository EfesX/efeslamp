#pragma once

#include <Arduino.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include "CLICommandClass.h"


#define MQTT_PRINT_DEBUG 1

static PubSubClient *smqtt;

void mqtt_printf(char *msg){
#if (MQTT_PRINT_DEBUG == 1)
    smqtt->publish("/efeslamp/lamp_0001/dbg/", msg);
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
        char wbuf[128];
    public:
        MQTTProcessor(){}
        
        void setClient(PubSubClient *_mqtt){
            p_mqtt = _mqtt;
        }

        void execute(char* topic, byte* payload, uint16_t len){
            if(!strcmp(topic, "/efeslamp/lamp_0001/cmd/color/")){
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

class MQTT {
    private:
        WiFiClient wificlient;
        PubSubClient *_mqtt;
        uint64_t lasttime;
    public:
        MQTT(){}
        
        void begin(){
            _mqtt = new PubSubClient("broker.mqttdashboard.com", 1883, [](char* topic, byte* payload, uint16_t len){
                if(len > 64) return;
                else{
                    for(uint8_t i = 0; i < len; i++){
                        __payload[i] = payload[i];
                    }
                    __payload[len] = '\r\n';
                    mqttproc.execute(topic, (byte *) __payload, len);
                }
            }, wificlient);
            _mqtt->connect("EfesLamp_0001");
            _mqtt->subscribe("/efeslamp/lamp_0001/cmd/color/");
            mqttproc.setClient(_mqtt);
            smqtt = _mqtt;
            lasttime = millis();
        }

        void tick(void){
            _mqtt->loop();
        }
};

