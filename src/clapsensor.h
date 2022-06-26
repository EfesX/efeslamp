#pragma once

#include <Arduino.h>

#define CLAP_PIN D3

class ClapSensor{
    private:
        typedef enum{
            IDLE,
            ONE_CLAP,
            TWO_CLAP
        }CLAP_STATE;

        uint8_t val;
        uint64_t lasttime;
        uint8_t state;
    public:
        ClapSensor(){
            pinMode(CLAP_PIN, INPUT);
            lasttime = millis();
            val = 1;
            state = IDLE;

        }
        void tick(void){
            val = digitalRead(CLAP_PIN);
            switch (state){
                case IDLE:
                    if(val == 0){
                        if(millis() - lasttime > 25){
                            state = ONE_CLAP;
                            lasttime = millis();
                        }
                    }
                    break;

                case ONE_CLAP:
                    if((millis() - lasttime) > 500){
                        state = IDLE;
                        lasttime = millis();
                    }else{
                        if(val == 0){
                            if((millis() - lasttime) > 25){
                                state = TWO_CLAP;
                                lasttime = millis();
                            }
                        }
                    }
                    break;

                case TWO_CLAP:
                    //EfesLAMP::lamp(FastLED)->pushBlink(2, 250);
                    
                    state = IDLE;
                    break;
                
                default:
                    state = IDLE;
                    break;
            }
        }
};