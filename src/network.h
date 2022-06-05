#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>

#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>

//#include <ESP8266WebServer.h>
//#include <ESPAsyncTCP.h>
//#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

#include "lamp.h"
#include "mqtt.h"
#include "clicommands.h"

#define SSID_STA        "Netzwerk"
#define PASSWORD_STA    "ubuntu56@brahamDC"

#define SSID_AP         "EfesLAMP"
#define PASSWORD_AP     "12345678"

IPAddress local_ip(192,168,0,1);
IPAddress gateway(192,168,0,1);
IPAddress subnet(255,255,255,0);

AsyncWebServer webserver(80);
ESP8266WiFiMulti wifimulti;
MDNSResponder mmdns;

class Network{
    private:
        MQTT mqtt;

        uint64_t lasttime;

        boolean ap_started;
        boolean sta_started;
        boolean servers_started;
    public:
        Network(){
            lasttime = millis();
            ap_started = false;
            sta_started = false;
        }
        void begin(void){
            WiFi.begin(SSID_STA, PASSWORD_STA);
            CLICommands::CLI()->registerCommand(new SetColorCmd());
            CLICommands::CLI()->registerCommand(new OnOffCmd());
            CLICommands::CLI()->registerCommand(new SetBrightCmd());
            CLICommands::CLI()->registerCommand(new BlinkCmd());
            Lamp::setColorBlend(HtmlColor(0xff0000), 100);
        }

        boolean isWiFiConnected(void){
            if(WiFi.status() != WL_CONNECTED) return false;
            return true;
        }

        void tick(void){
            if(this->isWiFiConnected() == false){
                if(ap_started == false){
                    if((millis() - lasttime) > 10000){
                        WiFi.softAPConfig(local_ip, gateway, subnet);
                        WiFi.softAP(SSID_AP, PASSWORD_AP);

                        Lamp::setColorBlend(HtmlColor(0x00ff00), 500);
                        ap_started = true;
                    }
                }else{
                    
                }
            }else{
                if(sta_started == false){
                    Lamp::setColorBlend(HtmlColor(0x0000ff), 5000);
                    sta_started = true;

                    mqtt.begin();

                    webserver.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
                        request->send(200, "text/plain", "HALLO FROM EfesLAMP");
                    });
                    
                    webserver.begin();
                    AsyncElegantOTA.begin(&webserver);
                    mmdns.begin("EfesLAMP", WiFi.localIP());
                    mmdns.addService("http", "tcp", 80);
                }
            }

            if(sta_started == true){
                mqtt.tick();
                mmdns.update();
            }
        }
};


