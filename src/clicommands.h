#pragma once

#include <Arduino.h>
#include "CLICommandClass.h"
#include "lamp.h"
#include "mqtt.h"
#include "APPInfoHelper.h"

class SetColorCmd : public CommandClass{
    private:
        char *cmdname;
        char *msghelp;
    public:
        SetColorCmd(){
            cmdname = "color";
            msghelp = "color: sets color on the lamp\nExample: color -h [HtmlColor] [duration]\r\n";
        }
        ~SetColorCmd(){
            delete cmdname;
            delete msghelp;
        }
        char *getCmdName(){ return cmdname; }
        char *getMsgHelp(){ return msghelp; }
        void execute(char *wbuf, const char *cmd){
            int numparam = getNumberOfParameters(cmd, ' ');
            long color;
            if(numparam != 5)
            {
                strcpy(wbuf, "Amount parameters of command is not right\r\n");
            }
            else
            {
                if(strncmp(getParamFromCmd(cmd, 1, ' '), "-h", 2) == 0)
                {
                    if(strchr(getParamFromCmd(cmd, 1, ' '), '#') != 0)
                    {
                        color = strtol(&getParamFromCmd(cmd, 2, ' ')[1], NULL, 16);    
                    }
                    else
                    {
                        color = strtol(getParamFromCmd(cmd, 2, ' '), NULL, 16);
                    }
                    
                    if(strncmp(getParamFromCmd(cmd, 3, ' '), "-t", 2) == 0)
                    {
                        uint16_t tau = strtoul(getParamFromCmd(cmd, 4, ' '), NULL, 10);
                        Lamp::setColorBlend(HtmlColor(color), tau);
                        strcpy(wbuf, "OK\r\n");    
                    }
                    else
                    {
                        strcpy(wbuf, "Error param [3]\r\n");    
                    }
                }else{
                    strcpy(wbuf, "Error param [1]\r\n");
                }
            }
        }
};

class OnOffCmd : public CommandClass{
    private:
        char *cmdname;
        char *msghelp;
    public:
        OnOffCmd()
        {
            cmdname = "lamp";
            msghelp = "lamp: switch on or switch of the Lamp\nExample: lamp on; lamp off\r\n";
        }
        ~OnOffCmd()
        {
            delete cmdname;
            delete msghelp;
        }
        char *getCmdName(){ return cmdname; }
        char *getMsgHelp(){ return msghelp; }
        void execute(char *wbuf, const char *cmd)
        {
            int numparam = getNumberOfParameters(cmd, ' ');
            if(numparam != 2)
            {
                strcpy(wbuf, "Amount parameters of command is not right\r\n");
            }
            else
            {
                if(strncmp(getParamFromCmd(cmd, 1), "on", 2) == 0)
                {
                    Lamp::turnOn();
                    strcpy(wbuf, "OK\r\n");
                }
                else if(strncmp(getParamFromCmd(cmd, 1, ' '), "off", 3) == 0)
                {
                    Lamp::turnOff();
                    strcpy(wbuf, "OK\r\n");
                }
                else
                {
                    strcpy(wbuf, "Error param [2]\r\n");
                }
            }
        }
};

class SetBrightCmd : public CommandClass{
    private:
        char *cmdname;
        char *msghelp;
    public:
        SetBrightCmd()
        {
            cmdname = "bright";
            msghelp = "bright: control brightness of lamp\nExample: bright [min] [max] [tau]\r\n";
        }
        ~SetBrightCmd()
        {
            delete cmdname;
            delete msghelp;
        }
        char *getCmdName(){ return cmdname; }
        char *getMsgHelp(){ return msghelp; }
        void execute(char *wbuf, const char *cmd)
        {
            int numparam = getNumberOfParameters(cmd, ' ');
            if(numparam != 4)
            {
                strcpy(wbuf, "Amount parameters of command is not right\r\n");
            }
            else
            {   
                /**
                * @todo здесь надо оптимизировать по указателю на следующий символ в функции strtoul
                */
                uint8_t bmin = strtoul(getParamFromCmd(cmd, 1, ' '), NULL, 10);
                uint8_t bmax = strtoul(getParamFromCmd(cmd, 2, ' '), NULL, 10);
                uint16_t btau = strtoul(getParamFromCmd(cmd, 3, ' '), NULL, 10);
                Lamp::setBrighgt(bmin, bmax, btau);
                strcpy(wbuf, "OK\r\n");
            }
        }
};

class BlinkCmd : public CommandClass{
    private:
        char *cmdname;
        char *msghelp;
    public:
        BlinkCmd()
        {
            cmdname = "blink";
            msghelp = "blink: the lamp blinks the specified number of times\nExample: blink [N] [tau]\r\n";
        }
        ~BlinkCmd()
        {
            delete cmdname;
            delete msghelp;
        }
        char *getCmdName(){ return cmdname; }
        char *getMsgHelp(){ return msghelp; }
        void execute(char *wbuf, const char *cmd)
        {
            int numparam = getNumberOfParameters(cmd, ' ');
            if(numparam != 3)
            {
                strcpy(wbuf, "Amount parameters of command is not right\r\n");
            }
            else
            {   
                /**
                * @todo здесь надо оптимизировать по указателю на следующий символ в функции strtoul
                */
                uint8_t N   = strtoul(getParamFromCmd(cmd, 1, ' '), NULL, 10);
                uint16_t tau = strtoul(getParamFromCmd(cmd, 2, ' '), NULL, 10);

                Lamp::blink(N, tau);
                strcpy(wbuf, "OK\r\n");
            }
        }
};

class GetInfoCmd : public CommandClass{
    private:
        char *cmdname;
        char *msghelp;
    public:
        GetInfoCmd()
        {
            cmdname = "getinfo";
            msghelp = "getinfo: returns in log topic info about network data\r\n";
        }
        ~GetInfoCmd()
        {
            delete cmdname;
            delete msghelp;
        }
        char *getCmdName(){ return cmdname; }
        char *getMsgHelp(){ return msghelp; }
        void execute(char *wbuf, const char *cmd)
        {
            APPInfoHelper info;
            for(uint8_t i = 0; i < info.getString().length(); ++i){
                wbuf[i] = info.getString().charAt(i);
            }
            wbuf[info.getString().length()] = 0x00;
        }
};