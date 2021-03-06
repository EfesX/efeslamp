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

#include "EEPROMHelper.h"

#include "htmlpage.h"

#include "APPInfoHelper.h"

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


EEPROMHelper eeprom;


static void appStart(void)
{
    webserver.on("/wifi", 
        HTTP_ANY, [](AsyncWebServerRequest *request) {
            int numparam = request->params();

            if(numparam == 0){
                request->send(200, "text/html", wifistarthtml);
            }
            else if(numparam == 2)
            {
                request->send(200, "text/plaintext", "OK");
                eeprom.write(EEPROM_SSID_POS, request->getParam(0)->value());
                eeprom.write(EEPROM_PASS_POS, request->getParam(1)->value());

                Lamp::setColorBlend(HtmlColor(0xff0000), 1000);
                Lamp::setBrighgt(20, 80, 500);
                

                //ESP.restart();
            }
            else
            {
                request->send(404, "text/html");
            } 
        }
    );

    webserver.on("/cloud",
        HTTP_GET, [](AsyncWebServerRequest *request) {
            int numparam = request->params();
            if(numparam == 0) request->send(200, "text/plaintext", "CLOUD_ID: " + eeprom.read(EEPROM_CLOUD_ID_POS) + "\nCLOUD_PASS: " + eeprom.read(EEPROM_CLOUD_PASS_POS));
            else if(numparam == 2){
                if((request->getParam(0)->name() == "cloudid") && (request->getParam(1)->name() == "cloudpass")){
                    eeprom.write(EEPROM_CLOUD_ID_POS,   request->getParam(0)->value());
                    eeprom.write(EEPROM_CLOUD_PASS_POS, request->getParam(1)->value());
                    request->send(200, "text/plaintext", "Accepted");
                }else{
                    request->send(404, "text/plaintext", "Error params");
                }
            }else{
                request->send(404, "text/html");
            }
        }
    );

    webserver.on("/info",
        HTTP_ANY, [](AsyncWebServerRequest *request) {
            APPInfoHelper data;
           request->send(200, "text/plaintext", data.getString());
        }
    );

    webserver.begin();
    AsyncElegantOTA.begin(&webserver);
    mmdns.begin("efeslamp", WiFi.localIP());
    mmdns.addService("http", "tcp", 80);
}


class Network{
    private:
        MQTT mqtt;

        uint64_t lasttime;

        boolean ap_started;
        
        boolean servers_started;
    public:
        boolean sta_started;

        Network(){
            lasttime = millis();
            ap_started = false;
            sta_started = false;
        }
        void begin(void){

            String ssid = eeprom.read(EEPROM_SSID_POS);
            String pass = eeprom.read(EEPROM_PASS_POS);

            WiFi.begin(ssid, pass);
            CLICommands::CLI()->registerCommand(new SetColorCmd());
            CLICommands::CLI()->registerCommand(new OnOffCmd());
            CLICommands::CLI()->registerCommand(new SetBrightCmd());
            CLICommands::CLI()->registerCommand(new BlinkCmd());
            CLICommands::CLI()->registerCommand(new GetInfoCmd());
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

                        appStart();

                        ap_started = true;
                    }
                }else{
                    
                }
            }else{
                if(sta_started == false){
                    Lamp::setColorBlend(HtmlColor(0x0000ff), 5000);
                    sta_started = true;

                    mqtt.begin();

                    appStart();
                }
            }

            if(sta_started == true){
                mqtt.tick();
                mmdns.update();
            }
        }
};


