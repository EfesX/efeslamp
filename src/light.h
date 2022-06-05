#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include <CircularBuffer.h>

#define NUM_LEDS 22
#define DATA_PIN D1

static CRGB leds[NUM_LEDS];
static CRGB lastcolor;
static uint8_t lastbright;

class Command{
protected:
    CFastLED *fastled;
public:
    Command(){};
    Command(CFastLED *_fastled) : fastled(_fastled){};
    virtual ~Command(){};
    virtual int execute() = 0;
    void setFastLED(CFastLED *_fastled){
        fastled = _fastled;
    }
};
class SetColorCommand : public Command{
private:
    CRGB engage_color;
    uint8_t bright;
public:
    SetColorCommand(CFastLED *_fastled, CRGB rgb, uint8_t _bright = 50) : Command(_fastled){
        engage_color = rgb;
        bright = _bright;
    };
    int execute(void){
        fastled->showColor(engage_color, bright);
        lastcolor = engage_color;
        lastbright = bright;
        return 0;
    };
};
class SetColorTransitionCommand : public Command{
private:
    CRGB engage_color, color;
    uint8_t bright;
    uint64_t lasttime;
    uint16_t timestep;
public:
    SetColorTransitionCommand(CFastLED *_fastled, CRGB _engage_color, uint8_t _bright = 50, uint16_t _timestep = 5) : Command(_fastled){
        engage_color = _engage_color;
        bright = _bright;
        lasttime = millis();
        timestep = _timestep;
    };
    int execute(void){
        if((millis() - lasttime) > timestep){
            if(engage_color != lastcolor){
                color.r = (engage_color.r == lastcolor.r ? lastcolor.r : (engage_color.r > lastcolor.r ? lastcolor.r + 1 : lastcolor.r - 1));
                color.g = (engage_color.g == lastcolor.g ? lastcolor.g : (engage_color.g > lastcolor.g ? lastcolor.g + 1 : lastcolor.g - 1));
                color.b = (engage_color.b == lastcolor.b ? lastcolor.b : (engage_color.b > lastcolor.b ? lastcolor.b + 1 : lastcolor.b - 1));
            }else{
                return 0;
            }
            fastled->showColor(color, bright);
            lastbright = bright;
            lastcolor = color;
            lasttime = millis();
        }
        return 1;
    };
};
class SetBrightCommand : public Command{
private:
    uint8_t bright;
public:
    SetBrightCommand(CFastLED *_fastled, uint8_t _bright = 50) : Command(_fastled){
        bright = _bright;
    };
    int execute(void){
        fastled->showColor(lastcolor, bright);
        lastbright = bright;
        return 0;
    };
};

class SetBrightTransitionCommand : public Command{
private:
    uint8_t count;
    int8_t deep;
    uint8_t timestep;
    uint64_t lasttime;
    uint8_t workbright;
    uint8_t dir;
    uint8_t lim;
public:
    SetBrightTransitionCommand(CFastLED *_fastled, uint8_t _count = 2, int8_t _deep = 50, uint8_t _timestep = 20) : Command(_fastled){
        count    = _count - 1;
        deep     = _deep;
        timestep = _timestep;
        lasttime = millis();
        workbright = 0;
        deep = _deep;
    };
    int execute(void){
        if((millis() - lasttime) > timestep){
            switch (dir){
                case 0: // init
                    workbright = lastbright;

                    lim = lastbright - deep;
                    if(lim > lastbright) lim = 0;

                    dir = 1;
                    break;

                case 1: // decrease
                    if((workbright - 1) == lim){
                        dir = 2;
                    }
                    workbright--;
                    fastled->showColor(lastcolor, workbright);
                    break;

                default: // increase
                    workbright++;
                    fastled->showColor(lastcolor, workbright);
                    if(workbright == lastbright){
                        if(count == 0) return 0;
                        else {
                            count--;
                            dir = 1;
                        }
                    }
                    break;
            }
            lasttime = millis();
        }
        return 1;
    };
};


class SetBlinkCommand : public Command{
private:
    uint8_t countblink;
    uint32_t tau;
    uint8_t i;
    uint64_t lasttime;
    uint8_t state;
public:
    SetBlinkCommand(CFastLED *_fastled, uint8_t _countblink, uint32_t _tau = 20) : Command(_fastled){
        countblink = _countblink;
        tau = _tau;
        i = 0;
        lasttime = millis();
        state = 0;
    };
    int execute(void){
        if(i == countblink){
            return 0;
        }else{
            if((millis() - lasttime) > tau){
                switch (state){
                    case 0:
                        fastled->showColor(lastcolor, 0);
                        state = 1;
                        break;
                    default:
                        fastled->showColor(lastcolor, lastbright);
                        state = 0;
                        ++i;
                        break;
                }
                lasttime = millis();
            }
        }
        return 1;
    };
};



class EfesLAMP{
    private:
        Command *cmd;
        EfesLAMP(){
            deepbreath = 0;
        }
        //MsgBoxClass<Command*, 8> cmds;
        CircularBuffer<Command*, 16> cmds;
        uint8_t deepbreath;
    public:
        CFastLED *fastled;
        
        static EfesLAMP *lamp(CFastLED &_fastled){
            static EfesLAMP instance;
            instance.fastled = &_fastled;
            _fastled.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
            //delay(50);
            //_fastled.show();
            return &instance;
        }

        bool pushColor(CRGB color, uint8_t bright = 50){
            if(cmds.available() > 0){
                cmd = new SetColorCommand(fastled, color, bright);
                return cmds.push(cmd);
            }
            return false;
        }
        bool pushColorTransition(CRGB color, uint8_t bright = 50, uint16_t timestep = 5){
            if(cmds.available() > 0){
                cmd = new SetColorTransitionCommand(fastled, color, bright, timestep);
                return cmds.push(cmd);
            }
            return false;
        }
        bool pushBright(uint8_t bright){
            if(cmds.available() > 0){
                cmd = new SetBrightCommand(fastled, bright);
                return cmds.push(cmd);
            }
            return false;
        }
        bool pushBrightTransition(uint8_t count, int8_t deep, uint8_t timestep = 20){
            if(cmds.available() > 0){
                cmd = new SetBrightTransitionCommand(fastled, count, deep, timestep);
                return cmds.push(cmd);
            }
            return false;
        }
        bool pushBlink(uint8_t countblink, uint32_t tau_ms){
            if(cmds.available() > 0){
                cmd = new SetBlinkCommand(fastled, countblink, tau_ms);
                return cmds.push(cmd);
            }
            return false;
        }
        int tick(){
            if(cmds.isEmpty() == false){
                if(cmds.first()->execute() == 0){
                    delete cmds.first();
                    cmds.shift();
                    return 0;
                }
            }
            return 1;
        }

};























/*
class Light : public CFastLED{
    private:
        CRGB leds[NUM_LEDS];
        CRGB color, engage_color;
        uint64_t last_time_color, last_time_bright;
        uint8_t engage_bright;
        uint8_t color_ts, bright_ts;
    public:
        Light();
        ~Light();
        void init(void);
        void tick(void);
        void setColor(CRGB color);
        void setBright(uint8_t bright);
        void animStart();
        void animStop();
};
*/