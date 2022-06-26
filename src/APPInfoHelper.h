#pragma once

#include <Arduino.h>
#include "network.h"
#include "lamp.h"

extern EEPROMHelper eeprom;

class APPInfoHelper{
    private:
        String data;
    public:

        APPInfoHelper(){
            data  = "";
            data += "{";
            data += "'wifissid' : '"    + eeprom.read(EEPROM_SSID_POS)          + "',";
            data += "'wifipass' : '"    + eeprom.read(EEPROM_PASS_POS)          + "',";
            data += "'ipaddr' : '"      + WiFi.localIP().toString()             + "',";
            data += "'cloudid' : '"     + eeprom.read(EEPROM_CLOUD_ID_POS)      + "',";
            data += "'cloudpass' : '"   + eeprom.read(EEPROM_CLOUD_PASS_POS)    + "',";
            data += "'R' : '"           + String(Lamp::getCurrentColor().R)     + "',";
            data += "'G' : '"           + String(Lamp::getCurrentColor().G)     + "',";
            data += "'B' : '"           + String(Lamp::getCurrentColor().B)     + "',";
            data += "'brightMin' : '"   + String(Lamp::getBrightMin())          + "',";
            data += "'brightMax' : '"   + String(Lamp::getBrightMax())          + "',";
            data += "'brightTau' : '"   + String(Lamp::getBrightTau())          + "'";
            data += "}";
        }
        

        String getString(){
            return data;
        }
};

/*
"
{
    'wifissid'  : 'Netzwerk',
    'wifipass'  : 'ubuntu56@brahamDC',
    'apstarted' : 'true',
    'ipaddr'    : '192.168.0.1',
    'cloudid'   : 'sdfsdfs34534fsdf',
    'cloudpass' : 'dfsdfdsfdfsd'
}
"
*/