import paho.mqtt.client as paho
import ssl
import time

def send_command(event):
    if 'request' in event and  'original_utterance' in event['request'] and len(event['request']['original_utterance']) > 0:
        req = event['request']['original_utterance']

        mqttc = paho.Client()
        mqttc.tls_set("/function/code/rootCA.crt", "/function/code/cert_lampregister.pem", "/function/code/key_lampregister.pem", ssl.CERT_REQUIRED, ssl.PROTOCOL_TLS)
        mqttc.connect("mqtt.cloud.yandex.net", 8883, 60)
        mqttc.loop_start()

        if ("красный" in req):
            mqttc.publish("$devices/arel66o1mos60829i19a/commands", "color -h 0xff0000 -t 1000")
            time.sleep(1)
            return "Сделано!"
        elif ("зеленый" in req):
            mqttc.publish("$devices/arel66o1mos60829i19a/commands", "color -h 0x00ff00 -t 1000")
            time.sleep(1)
            return "Сделано!"
        elif ("синий" in req):
            mqttc.publish("$devices/arel66o1mos60829i19a/commands", "color -h 0x0000ff -t 1000")
            time.sleep(1)
            return "Сделано!"
        elif ("моргнуть" in req):
            mqttc.publish("$devices/arel66o1mos60829i19a/commands", "blink 3 500")
            time.sleep(1)
            return "Сделано!"
        elif ("включить" in req):    
            mqttc.publish("$devices/arel66o1mos60829i19a/commands", "lamp on")
            time.sleep(1)
            return "Сделано!"
        elif ("выключить" in req):
            mqttc.publish("$devices/arel66o1mos60829i19a/commands", "lamp off")
            time.sleep(1)
            return "Сделано!"
        else:
            return "Слышь, ты! Кожаный мешок с костями! Или ты говоришь правильный запрос или я увольняюсь!"

    else:
        return "Приказывайте. Исполню любое ваше желание."



def handler(event, context):
    """
    Entry-point for Serverless Function.
    :param event: request payload.
    :param context: information about current execution context.
    :return: response to be serialized as JSON.
    """
    res = ""

    try:
        res = send_command(event)
    except:
        res = "Произошла какая-то ошибка"        

    return {
        'version': event['version'],
        'session': event['session'],
        'response': {
            'text': res,
            'end_session': 'true'
        },
    }




