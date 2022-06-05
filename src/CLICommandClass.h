#pragma once

#include <Arduino.h>
#include <CircularBuffer.h>

#include "light.h"

static uint8_t getNumberOfParameters(const char *cmd, char delim = ' '){
    uint8_t res = 0;
    boolean delimToChTrans[2] = {false, true};
    for(uint8_t i = 0; i < strlen(cmd) - 1; ++i)
    {
        if((delimToChTrans[0] == false) && (delimToChTrans[1] == true))
        {
            ++res;
        }
        delimToChTrans[0] = delimToChTrans[1];
        delimToChTrans[1] = (cmd[i] == delim) ? true : false;
    }
    return res;
}

static const char *getParamFromCmd(const char *cmd, int numParam, char delim = ' '){
    for(uint8_t i = 0; i < numParam; i++){
        while ((*cmd) != delim) cmd++;
        cmd++;
    }
    return cmd;
}

class CommandClass{
    public:
        virtual ~CommandClass(){}
        virtual void execute(char *wbuf, const char *cmd) = 0;
        virtual char *getCmdName(void) = 0;
        virtual char *getMsgHelp(void) = 0;
};

class CLICommands{
    private:
        CircularBuffer<CommandClass*, 4> cmds;
        CommandClass *cmd;
        CLICommands(){}
    public:
        
        ~CLICommands(){
            while(cmds.isEmpty() == false){
                delete cmds.first();
                cmds.shift();
            }
        }

        static CLICommands *CLI(void){
            static CLICommands instance;
            return &instance;
        }

        boolean registerCommand(CommandClass *cmd){
            if(cmds.available() > 0){
                return cmds.push(cmd);
            }
            return false;
        }
        boolean execute(const char *cmd, char *wbuf){
            assert((cmd != NULL) && (wbuf != NULL));

            Serial.println(cmd);

            if(strncmp(getParamFromCmd(cmd, 0, ' '), "help", 4) == 0){
                if(getNumberOfParameters(cmd, ' ') > 0){
                    for(uint8_t i = 0; i < cmds.size(); i++){
                        if(strncmp(cmds[i]->getCmdName(), getParamFromCmd(cmd, 1, ' '), strlen(cmds[i]->getCmdName())) == 0){
                            strcpy(wbuf, cmds[i]->getMsgHelp());
                            return 0;
                        }
                    }
                }else{
                    strcpy(wbuf, "List of commands:");
                    for(uint8_t i = 0; i < cmds.size(); i++){
                        strcpy(&wbuf[strlen(wbuf)], "\n");
                        strcpy(&wbuf[strlen(wbuf)], cmds[i]->getCmdName());
                    }
                    return 0;
                }
                strcpy(wbuf, "\nError: command for help not found");
            }else{
                for(uint8_t i = 0; i < cmds.size(); i++){
                    if(strncmp(cmds[i]->getCmdName(), getParamFromCmd(cmd, 0, ' '), strlen(cmds[i]->getCmdName())) == 0){
                        cmds[i]->execute(wbuf, cmd);
                        return 0;
                    }
                }
            }
            strcpy(wbuf, "Command not found");
            return 1;
        }
};