#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPTelnet.h>

#include "clicommands.h"

static ESPTelnet _telnet;
static char wbuf[128];

class Telnet{
    private:
        
    public:
        Telnet(){}
        void begin(){
            //CLICommands::CLI()->registerCommand(new SetColorCmd());

            _telnet.onConnect([](String ip){
                _telnet.println("================================================");
                _telnet.println("============ Welcome to EfesLAMP ===============");
                _telnet.println("================================================");
                _telnet.print(">> ");
            });
            _telnet.onConnectionAttempt([](String ip){});
            _telnet.onReconnect([](String ip){});
            _telnet.onDisconnect([](String ip){});
            _telnet.onInputReceived([](String payload){
                CLICommands::CLI()->execute((char *)payload.c_str(), wbuf);
                _telnet.println(wbuf);
                _telnet.print(">> ");
            });
            _telnet.begin(23);
        }
        void tick(){
            _telnet.loop();
        }
};
