#  Copyright (c) ChernV (@otter18), 2021.

import random
from tokenize import String
import telebot
import paho.mqtt.client as paho
import ssl
import time

bot = telebot.TeleBot("5449859235:AAFMXP9TBZyyiLkiJfzdIr-zyMhpDymw30o")

lampCommands = {
    'выкл' : {
        'in' : ['off', 'выкл'],
        'out': ['Сделано', 'Готово'],
        'mqtt': 'lamp off'
    },
    'вкл' : {
        'in' : ['on', 'вкл'],
        'out': ['Сделано', 'Готово'],
        'mqtt': 'lamp on'
    },
    'красный': {
        'in' : ['красный'],
        'out': ['Сделано', 'Готово'],
        'mqtt': 'color -h 0xff0000 -t 750'
    },
    'зеленый': {
        'in' : ['зеленый'],
        'out': ['Сделано', 'Готово'],
        'mqtt': 'color -h 0x00ff00 -t 750'
    },
    'синий': {
        'in' : ['синий'],
        'out': ['Сделано', 'Готово'],
        'mqtt': 'color -h 0x0000ff -t 750'
    },
    'моргнуть': {
        'in' : ['моргнуть'],
        'out': ['Сделано', 'Готово'],
        'mqtt': 'blink 3 500'
    },
}


def sendMQTTCommand(cmd : String):
    mqttc = paho.Client()
    mqttc.tls_set("rootCA.crt", "cert_lampregister.pem", "key_lampregister.pem", ssl.CERT_REQUIRED, ssl.PROTOCOL_TLS)
    mqttc.connect("mqtt.cloud.yandex.net", 8883, 60)
    mqttc.loop_start()
    
    mqttc.publish("$devices/arel66o1mos60829i19a/commands", cmd)


# --------------------- bot ---------------------
@bot.message_handler(commands=['help'])
def say_welcome(message):
    bot.send_message(message.chat.id,
        "вкл, on \nвыкл, off\nкрасный\nсиний\nзеленый\nморгнуть",
        parse_mode='markdown')


@bot.message_handler(func=lambda message: True)
def sendCommandToLamp(message):
    print(type(message))
    for t, resp in lampCommands.items():
        if sum([e in message.text.lower() for e in resp['in']]):
            sendMQTTCommand(resp['mqtt'])
            bot.send_message(message.chat.id, random.choice(resp['out']))
            return
    bot.send_message(message.chat.id, 'Такой команде я не обучен (')
    bot.send_message(message.chat.id, 'Введи /help для просмотра доступных команд')


# ---------------- local testing ----------------
if __name__ == '__main__':
    bot.infinity_polling()