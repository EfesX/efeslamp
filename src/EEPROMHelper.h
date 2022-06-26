#pragma once

#include <Arduino.h>
#include <EEPROM.h>

#define BLOCK_SIZE               64
#define EEPROM_SSID_POS           0
#define EEPROM_PASS_POS          64
#define EEPROM_CLOUD_ID_POS     128
#define EEPROM_CLOUD_PASS_POS   192


class EEPROMHelper{
    private:
        String _reader(uint8_t POSITION){
            String res = "";
            int len = 0;
            char k;

            while(len < BLOCK_SIZE){
                k = (char) EEPROM.read(POSITION + len);
                ++len;
                if((uint8_t)k != 0x00){
                    res += String(k);
                }else{
                    break;
                }
            }
            return res;
        }

        void _writer(uint8_t POSITION, String _str){
            for(uint8_t i = 0; i < _str.length(); ++i)
            {
                EEPROM.write(POSITION + i, _str.charAt(i));
            }
            EEPROM.write(POSITION + _str.length(), 0x00);
            EEPROM.commit();
        }
    public:
        EEPROMHelper(){
            EEPROM.begin(256);
        }

        String read(uint8_t POSITION){
            return _reader(POSITION);
        }
        boolean write(uint8_t POSITION, String _str){
            if(_str.length() >= BLOCK_SIZE) return false;
            _writer(POSITION, _str);
            return true;
        }

        
};