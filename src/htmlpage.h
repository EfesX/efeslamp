#pragma once

const char *wifistarthtml = "<!DOCTYPE html>\
<html>\
    <head>\
        <meta charset='UTF-8'>\
        <title>Подключение к WIFI</title>\
        <style>\
            html {\
                background-color:#214c84;\
                display:flex;\
                justify-content:center;\
            }\
            .regdiv{\
                padding:50px 0;\
            }\
            .regdiv form {\
                max-width:800px;\
                padding:50px 70px;\
                border-radius:10px;\
                box-shadow:4px 4px 15px rgba(0, 0, 0, 0.2);\
                background-color:#fff;\
            }\
            .regdiv .it {\
                border-radius:10px;\
                margin-bottom:25px;\
                padding:10px 20px;\
            }\
            .regdiv .ca {\
                border-radius:30px;\
                padding:10px 20px;\
                font-size:18px;\
                font-weight:bold;\
                background-color:#3f93ff;\
                border:none;\
                color:white;\
                margin-top:20px;\
            }\
        </style>\
    </head>\
    <body>\
        <div class='regdiv'>\
            <form method = 'POST'>\
                <h3 class='tc'>Подключение к WiFi</h3>\
                <div class='fg'>\
                    <input class='it' type='text' name='ssid' maxlength='15' minlength='4' pattern='^[a-zA-Z0-9_.-]*$' id='username' placeholder='SSID' required>\
                </div>\
                <div class='fg'>\
                    <input class='it' type='password' name='pass' minlength='8' id='password' placeholder='PASSWORD' required>\
                </div>\
                <div class='fg'>\
                    <button class='ca' type='submit'>Отправить</button>\
                </div>\
            </form>\
        </div>\
    </body>\
    </html>\
";