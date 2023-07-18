#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncTCP.h>
#include <string.h>

/* Network */
const char *ssid = "MOVING-MESSAGE";
const char *pw = "eisdiele";
AsyncWebServer server(80);

/* http query parameters*/
const char *PARAM_INPUT_PWR = "power";
const char *PARAM_INPUT_MODE = "direction";
const char *PARAM_INPUT_STRING = "string";
const char *PARAM_INPUT_SPEED = "speed";
const char *PARAM_INPUT_SLOWMODE = "slowmode";

/* Message control */
bool power = true;
bool runLeft = false;
bool slowmode = false;
int offset = 0;
int speed = 8;

/* uC pinout */
#define D1 D1
#define C1 D2
#define STR D3
#define LEDS D4
#define BCD0 D7
#define BCD1 D6
#define BCD2 D5

/* Display size */
bool canvas[7][92] = {};

/* Fonts */
bool loading[7][1] = {
    {1},
    {1},
    {1},
    {1},
    {1},
    {1},
    {1}};
bool dash[7][4] = {
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {1, 1, 1, 1},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}};
bool one[7][3] = {
    {0, 0, 1},
    {0, 1, 1},
    {1, 0, 1},
    {0, 0, 1},
    {0, 0, 1},
    {0, 0, 1},
    {0, 0, 1}};
bool two[7][5] = {
    {0, 1, 1, 1, 0},
    {1, 0, 0, 0, 1},
    {0, 0, 0, 1, 0},
    {0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0},
    {1, 0, 0, 0, 0},
    {1, 1, 1, 1, 1}};
bool three[7][5] = {
    {1, 1, 1, 1, 0},
    {0, 0, 0, 0, 1},
    {0, 0, 0, 0, 1},
    {0, 1, 1, 1, 0},
    {0, 0, 0, 0, 1},
    {0, 0, 0, 0, 1},
    {1, 1, 1, 1, 0}};
bool four[7][5] = {
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {0, 1, 1, 1, 1},
    {0, 0, 0, 0, 1},
    {0, 0, 0, 0, 1},
    {0, 0, 0, 0, 1}};
bool five[7][5] = {
    {1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0},
    {1, 0, 0, 0, 0},
    {0, 1, 1, 1, 0},
    {0, 0, 0, 0, 1},
    {0, 0, 0, 0, 1},
    {1, 1, 1, 1, 0}};
bool six[7][5] = {
    {0, 1, 1, 1, 0},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0},
    {1, 1, 1, 1, 0},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {0, 1, 1, 1, 0}};
bool seven[7][5] = {
    {1, 1, 1, 1, 1},
    {0, 0, 0, 0, 1},
    {0, 0, 0, 1, 0},
    {0, 1, 1, 1, 0},
    {0, 1, 0, 0, 0},
    {1, 0, 0, 0, 0},
    {1, 0, 0, 0, 0}};
bool eight[7][5] = {
    {0, 1, 1, 1, 0},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {0, 1, 1, 1, 0},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {0, 1, 1, 1, 0}};
bool nine[7][5] = {
    {0, 1, 1, 1, 0},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {0, 1, 1, 1, 1},
    {0, 0, 0, 0, 1},
    {0, 0, 0, 0, 1},
    {1, 1, 1, 1, 0}};
bool zero[7][4] = {
    {1, 1, 1, 1},
    {1, 0, 0, 1},
    {1, 0, 0, 1},
    {1, 0, 0, 1},
    {1, 0, 0, 1},
    {1, 0, 0, 1},
    {1, 1, 1, 1}};
bool a[7][5] = {
    {0, 1, 1, 1, 0},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 1, 1, 1, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1}};
bool b[7][5] = {
    {1, 1, 1, 1, 0},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 1, 1, 1, 0},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 1, 1, 1, 0}};
bool c[7][4] = {
    {0, 1, 1, 1},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {0, 1, 1, 1}};
bool d[7][5] = {
    {1, 1, 1, 0, 0},
    {1, 0, 0, 1, 0},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 1, 0},
    {1, 1, 1, 0, 0}};
bool e[7][4] = {
    {1, 1, 1, 1},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 1, 1, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 1, 1, 1}};
bool f[7][4] = {
    {1, 1, 1, 1},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 1, 1, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0}};
bool g[7][5] = {
    {1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0},
    {1, 0, 0, 0, 0},
    {1, 0, 1, 1, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 1, 1, 1, 1}};
bool h[7][5] = {
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 1, 1, 1, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1}};
bool i[7][3] = {
    {1, 1, 1},
    {0, 1, 0},
    {0, 1, 0},
    {0, 1, 0},
    {0, 1, 0},
    {0, 1, 0},
    {1, 1, 1}};
bool j[7][4] = {
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {0, 0, 0, 1},
    {1, 0, 0, 1},
    {0, 1, 1, 0}};
bool k[7][5] = {
    {1, 0, 0, 0, 1},
    {1, 0, 0, 1, 0},
    {1, 0, 1, 0, 0},
    {1, 1, 0, 0, 0},
    {1, 0, 1, 0, 0},
    {1, 0, 0, 1, 0},
    {1, 0, 0, 0, 1}};
bool l[7][4] = {
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 0, 0, 0},
    {1, 1, 1, 1}};
bool m[7][5] = {
    {1, 0, 0, 0, 1},
    {1, 1, 0, 1, 1},
    {1, 0, 1, 0, 1},
    {1, 0, 1, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1}};
bool n[7][5] = {
    {1, 0, 0, 0, 1},
    {1, 1, 0, 0, 1},
    {1, 0, 1, 0, 1},
    {1, 0, 1, 0, 1},
    {1, 0, 0, 1, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1}};
bool o[7][5] = {
    {0, 1, 1, 1, 0},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {0, 1, 1, 1, 0}};
bool p[7][5] = {
    {1, 1, 1, 1, 0},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 1, 1, 1, 0},
    {1, 0, 0, 0, 0},
    {1, 0, 0, 0, 0},
    {1, 0, 0, 0, 0}};
bool q[7][5] = {
    {0, 1, 1, 1, 0},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 1, 0, 1},
    {1, 0, 0, 1, 1},
    {0, 1, 1, 1, 1}};
bool r[7][5] = {
    {1, 1, 1, 1, 0},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 1, 1, 1, 0},
    {1, 0, 1, 0, 0},
    {1, 0, 0, 1, 0},
    {1, 0, 0, 0, 1}};
bool s[7][5] = {
    {0, 1, 1, 1, 1},
    {1, 0, 0, 0, 0},
    {1, 0, 0, 0, 0},
    {0, 1, 1, 1, 0},
    {0, 0, 0, 0, 1},
    {0, 0, 0, 0, 1},
    {1, 1, 1, 1, 0}};
bool t[7][5] = {
    {1, 1, 1, 1, 1},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0}};
bool u[7][5] = {
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {0, 1, 1, 1, 0}};
bool v[7][5] = {
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {0, 1, 0, 1, 0},
    {0, 1, 0, 1, 0},
    {0, 0, 1, 0, 0}};
bool w[7][5] = {
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {1, 0, 1, 0, 1},
    {1, 0, 1, 0, 1},
    {1, 1, 0, 1, 1},
    {1, 0, 0, 0, 1}};
bool x[7][5] = {
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {0, 1, 0, 1, 0},
    {0, 0, 1, 0, 0},
    {0, 1, 0, 1, 0},
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1}};
bool y[7][5] = {
    {1, 0, 0, 0, 1},
    {1, 0, 0, 0, 1},
    {0, 1, 0, 1, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0}};
bool z[7][5] = {
    {1, 1, 1, 1, 1},
    {0, 0, 0, 0, 1},
    {0, 0, 0, 1, 0},
    {0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0},
    {1, 0, 0, 0, 0},
    {1, 1, 1, 1, 1}};

/* webpage with button template and input field */
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>MovingMessage</title>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem;}
    p {font-size: 3.0rem;}
    body {margin: auto; padding-bottom: 25px;}
    .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #218B96; border-radius: 6px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #000; -webkit-transition: .4s; transition: .4s; border-radius: 3px}
    .slider2 {direction: rtl}
    input[type=range]::-webkit-slider-runnable-track  {-webkit-appearance: none; box-shadow: none; border: 2px solid black; border-radius: 5px; background: #38ECFF}
    input:checked+.slider {background-color: #38ECFF}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
  </style>
</head>
<body>
  <h1>Moving Message</h1>
  <input type="text" id="0" maxlength="15" oninput="this.value = this.value.toUpperCase()" onkeyup="updateDisplay(this.value)" size="20" style="height:90px;font-size:28pt;text-align:center;">
  %BUTTONS%
  <div><h3>Laufgeschwindigkeit</h3></div>
  <input type="range" onchange="updateSpeed(this)" id="4" min="1" max="16" value="%SLIDERVALUE%" step="1" class="slider2">
 <br> 
<script>function togglePower(element) {
            var xhr = new XMLHttpRequest();
            if(element.checked){ xhr.open("GET", "/run?power=on", true); }
            else { xhr.open("GET", "/run?power=off", true); }
            xhr.send();
        }
        function toggleMode(element) {
            var xhr = new XMLHttpRequest();
            if(element.checked){ xhr.open("GET", "/mode?direction=left", true); }
            else { xhr.open("GET", "/mode?direction=right", true); }
            xhr.send();
        }
        function toggleDemo(element) {
            var xhr = new XMLHttpRequest();
            if(element.checked){ xhr.open("GET", "/demo?slowmode=on", true); }
            else { xhr.open("GET", "/demo?slowmode=off", true); }
            xhr.send();
        }
        function updateDisplay(str) {
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/update?string="+str, true);
            xhr.send();
        }
        function updateSpeed(element) {
            var sliderValue = document.getElementById("4").value;
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/slider?speed="+sliderValue, true);
            xhr.send();
        }
</script>
</body>
</html>
)rawliteral";

String power_state()
{
    if (power)
    {
        return "checked";
    }
    else
    {
        return "";
    }
}

String direction()
{
    if (runLeft)
    {
        return "checked";
    }
    else
    {
        return "";
    }
}

String demo_speed()
{
    if (slowmode)
    {
        return "checked";
    }
    else
    {
        return "";
    }
}

String processor(const String &var)
{
    if (var == "BUTTONS")
    {
        String buttons = "";
        buttons += "<h3>Aus/An</h3><label class=\"switch\"><input type=\"checkbox\" onchange=\"togglePower(this)\" id=\"1\" " + power_state() + "><span class=\"slider\"></span></label>";
        buttons += "<h3>Rechts/Links</h3><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleMode(this)\" id=\"2\" " + direction() + "><span class=\"slider\"></span></label>";
        buttons += "<h3>Demo Modus</h3><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleDemo(this)\" id=\"3\" " + demo_speed() + "><span class=\"slider\"></span></label>";
        return buttons;
    }
    if (var == "SLIDERVALUE")
    {
        return String(speed);
    }
    return String();
}

void write_letter(char letter)
{
    switch (letter)
    {
    case '#':
        for (int posVert = 0; posVert < 7; posVert++)
        {
            canvas[posVert][offset] = loading[posVert][0];
        }
        break;
    case '-':
        for (int posHoriz = 0; posHoriz < 4; posHoriz++)
        {
            for (int posVert = 0; posVert < 7; posVert++)
            {
                canvas[posVert][posHoriz + 1 + offset] = dash[posVert][posHoriz];
            }
        }
        offset -= 1;
        break;
    case '0':
        for (int posHoriz = 0; posHoriz < 4; posHoriz++)
        {
            for (int posVert = 0; posVert < 7; posVert++)
            {
                canvas[posVert][posHoriz + 1 + offset] = zero[posVert][posHoriz];
            }
        }
        offset -= 1;
        break;
    case '1':
        for (int posHoriz = 0; posHoriz < 3; posHoriz++)
        {
            for (int posVert = 0; posVert < 7; posVert++)
            {
                canvas[posVert][posHoriz + 1 + offset] = one[posVert][posHoriz];
            }
        }
        offset -= 2;
        break;
    case '2':
        for (int posHoriz = 0; posHoriz < 5; posHoriz++)
        {
            for (int posVert = 0; posVert < 7; posVert++)
            {
                canvas[posVert][posHoriz + 1 + offset] = two[posVert][posHoriz];
            }
        }
        break;
    case '3':
        for (int posHoriz = 0; posHoriz < 5; posHoriz++)
        {
            for (int posVert = 0; posVert < 7; posVert++)
            {
                canvas[posVert][posHoriz + 1 + offset] = three[posVert][posHoriz];
            }
        }
        break;
    case '4':
        for (int posHoriz = 0; posHoriz < 5; posHoriz++)
        {
            for (int posVert = 0; posVert < 7; posVert++)
            {
                canvas[posVert][posHoriz + 1 + offset] = four[posVert][posHoriz];
            }
        }
        break;
    case '5':
        for (int posHoriz = 0; posHoriz < 5; posHoriz++)
        {
            for (int posVert = 0; posVert < 7; posVert++)
            {
                canvas[posVert][posHoriz + 1 + offset] = five[posVert][posHoriz];
            }
        }
        break;
    case '6':
        for (int posHoriz = 0; posHoriz < 5; posHoriz++)
        {
            for (int posVert = 0; posVert < 7; posVert++)
            {
                canvas[posVert][posHoriz + 1 + offset] = six[posVert][posHoriz];
            }
        }
        break;
    case '7':
        for (int posHoriz = 0; posHoriz < 5; posHoriz++)
        {
            for (int posVert = 0; posVert < 7; posVert++)
            {
                canvas[posVert][posHoriz + 1 + offset] = seven[posVert][posHoriz];
            }
        }
        break;
    case '8':
        for (int posHoriz = 0; posHoriz < 5; posHoriz++)
        {
            for (int posVert = 0; posVert < 7; posVert++)
            {
                canvas[posVert][posHoriz + 1 + offset] = eight[posVert][posHoriz];
            }
        }
        break;
    case '9':
        for (int posHoriz = 0; posHoriz < 5; posHoriz++)
        {
            for (int posVert = 0; posVert < 7; posVert++)
            {
                canvas[posVert][posHoriz + 1 + offset] = nine[posVert][posHoriz];
            }
        }
        break;
    case 'A':
        for (int posHoriz = 0; posHoriz < 5; posHoriz++)
        {
            for (int posVert = 0; posVert < 7; posVert++)
            {
                canvas[posVert][posHoriz + 1 + offset] = a[posVert][posHoriz];
            }
        }
        break;
    case 'B':
        for (int posHoriz = 0; posHoriz < 5; posHoriz++)
        {
            for (int posVert = 0; posVert < 7; posVert++)
            {
                canvas[posVert][posHoriz + 1 + offset] = b[posVert][posHoriz];
            }
        }
        break;
    case 'C':
        for (int posHoriz = 0; posHoriz < 4; posHoriz++)
        {
            for (int posVert = 0; posVert < 7; posVert++)
            {
                canvas[posVert][posHoriz + 1 + offset] = c[posVert][posHoriz];
            }
        }
        offset -= 1;
        break;
    case 'D':
        for (int posHoriz = 0; posHoriz < 5; posHoriz++)
        {
            for (int posVert = 0; posVert < 7; posVert++)
            {
                canvas[posVert][posHoriz + 1 + offset] = d[posVert][posHoriz];
            }
        }
        break;
    case 'E':
        for (int posHoriz = 0; posHoriz < 4; posHoriz++)
        {
            for (int posVert = 0; posVert < 7; posVert++)
            {
                canvas[posVert][posHoriz + 1 + offset] = e[posVert][posHoriz];
            }
        }
        offset -= 1;
        break;
    case 'F':
        for (int posHoriz = 0; posHoriz < 4; posHoriz++)
        {
            for (int posVert = 0; posVert < 7; posVert++)
            {
                canvas[posVert][posHoriz + 1 + offset] = f[posVert][posHoriz];
            }
        }
        offset -= 1;
        break;
    case 'G':
        for (int posHoriz = 0; posHoriz < 5; posHoriz++)
        {
            for (int posVert = 0; posVert < 7; posVert++)
            {
                canvas[posVert][posHoriz + 1 + offset] = g[posVert][posHoriz];
            }
        }
        break;
    case 'H':
        for (int posHoriz = 0; posHoriz < 5; posHoriz++)
        {
            for (int posVert = 0; posVert < 7; posVert++)
            {
                canvas[posVert][posHoriz + 1 + offset] = h[posVert][posHoriz];
            }
        }
        break;
    case 'I':
        for (int posHoriz = 0; posHoriz < 3; posHoriz++)
        {
            for (int posVert = 0; posVert < 7; posVert++)
            {
                canvas[posVert][posHoriz + 1 + offset] = i[posVert][posHoriz];
            }
        }
        offset -= 2;
        break;
    case 'J':
        for (int posHoriz = 0; posHoriz < 4; posHoriz++)
        {
            for (int posVert = 0; posVert < 7; posVert++)
            {
                canvas[posVert][posHoriz + 1 + offset] = j[posVert][posHoriz];
            }
        }
        offset -= 1;
        break;
    case 'K':
        for (int posHoriz = 0; posHoriz < 5; posHoriz++)
        {
            for (int posVert = 0; posVert < 7; posVert++)
            {
                canvas[posVert][posHoriz + 1 + offset] = k[posVert][posHoriz];
            }
        }
        break;
    case 'L':
        for (int posHoriz = 0; posHoriz < 4; posHoriz++)
        {
            for (int posVert = 0; posVert < 7; posVert++)
            {
                canvas[posVert][posHoriz + 1 + offset] = l[posVert][posHoriz];
            }
        }
        offset -= 1;
        break;
    case 'M':
        for (int posHoriz = 0; posHoriz < 5; posHoriz++)
        {
            for (int posVert = 0; posVert < 7; posVert++)
            {
                canvas[posVert][posHoriz + 1 + offset] = m[posVert][posHoriz];
            }
        }
        break;
    case 'N':
        for (int posHoriz = 0; posHoriz < 5; posHoriz++)
        {
            for (int posVert = 0; posVert < 7; posVert++)
            {
                canvas[posVert][posHoriz + 1 + offset] = n[posVert][posHoriz];
            }
        }
        break;
    case 'O':
        for (int posHoriz = 0; posHoriz < 5; posHoriz++)
        {
            for (int posVert = 0; posVert < 7; posVert++)
            {
                canvas[posVert][posHoriz + 1 + offset] = o[posVert][posHoriz];
            }
        }
        break;
    case 'P':
        for (int posHoriz = 0; posHoriz < 5; posHoriz++)
        {
            for (int posVert = 0; posVert < 7; posVert++)
            {
                canvas[posVert][posHoriz + 1 + offset] = p[posVert][posHoriz];
            }
        }
        break;
    case 'Q':
        for (int posHoriz = 0; posHoriz < 5; posHoriz++)
        {
            for (int posVert = 0; posVert < 7; posVert++)
            {
                canvas[posVert][posHoriz + 1 + offset] = q[posVert][posHoriz];
            }
        }
        break;
    case 'R':
        for (int posHoriz = 0; posHoriz < 5; posHoriz++)
        {
            for (int posVert = 0; posVert < 7; posVert++)
            {
                canvas[posVert][posHoriz + 1 + offset] = r[posVert][posHoriz];
            }
        }
        break;
    case 'S':
        for (int posHoriz = 0; posHoriz < 5; posHoriz++)
        {
            for (int posVert = 0; posVert < 7; posVert++)
            {
                canvas[posVert][posHoriz + 1 + offset] = s[posVert][posHoriz];
            }
        }
        break;
    case 'T':
        for (int posHoriz = 0; posHoriz < 5; posHoriz++)
        {
            for (int posVert = 0; posVert < 7; posVert++)
            {
                canvas[posVert][posHoriz + 1 + offset] = t[posVert][posHoriz];
            }
        }
        break;
    case 'U':
        for (int posHoriz = 0; posHoriz < 5; posHoriz++)
        {
            for (int posVert = 0; posVert < 7; posVert++)
            {
                canvas[posVert][posHoriz + 1 + offset] = u[posVert][posHoriz];
            }
        }
        break;
    case 'V':
        for (int posHoriz = 0; posHoriz < 5; posHoriz++)
        {
            for (int posVert = 0; posVert < 7; posVert++)
            {
                canvas[posVert][posHoriz + 1 + offset] = v[posVert][posHoriz];
            }
        }
        break;
    case 'W':
        for (int posHoriz = 0; posHoriz < 5; posHoriz++)
        {
            for (int posVert = 0; posVert < 7; posVert++)
            {
                canvas[posVert][posHoriz + 1 + offset] = w[posVert][posHoriz];
            }
        }
        break;
    case 'X':
        for (int posHoriz = 0; posHoriz < 5; posHoriz++)
        {
            for (int posVert = 0; posVert < 7; posVert++)
            {
                canvas[posVert][posHoriz + 1 + offset] = x[posVert][posHoriz];
            }
        }
        break;
    case 'Y':
        for (int posHoriz = 0; posHoriz < 5; posHoriz++)
        {
            for (int posVert = 0; posVert < 7; posVert++)
            {
                canvas[posVert][posHoriz + 1 + offset] = y[posVert][posHoriz];
            }
        }
        break;
    case 'Z':
        for (int posHoriz = 0; posHoriz < 5; posHoriz++)
        {
            for (int posVert = 0; posVert < 7; posVert++)
            {
                canvas[posVert][posHoriz + 1 + offset] = z[posVert][posHoriz];
            }
        }
        break;
    }
}

void clear_canvas()
{
    for (int row = 0; row < 92; row++)
    {
        for (int col = 0; col < 7; col++)
        {
            canvas[col][row] = 0;
        }
    }
}

void update_text(String inputString)
{
    offset = 0;
    clear_canvas();
    char outputString[16];
    char *pString = outputString;
    inputString.toCharArray(outputString, 16);
    while (*pString)
    {
        write_letter(*pString++);
        offset += 6;
    }
}

void strobe(int strobes)
{
    for (int cycle = 0; cycle < strobes; cycle++)
    {
        for (int row = 0; row < 7; row++)
        {
            /* Shift bits*/
            for (int col = 0; col < 90; col++)
            {
                if (canvas[row][col] == 1)
                {
                    digitalWrite(D1, HIGH);
                }
                else
                {
                    digitalWrite(D1, LOW);
                }
                digitalWrite(C1, HIGH);
                digitalWrite(C1, LOW);
            }
            /* Strobe Row*/
            digitalWrite(STR, HIGH);
            digitalWrite(STR, LOW);

            if (row == 0)
            {
                digitalWrite(BCD0, LOW);
                digitalWrite(BCD1, LOW);
                digitalWrite(BCD2, LOW);
            }
            else if (row == 1)
            {
                digitalWrite(BCD0, HIGH);
                digitalWrite(BCD1, LOW);
                digitalWrite(BCD2, LOW);
            }
            else if (row == 2)
            {
                digitalWrite(BCD0, LOW);
                digitalWrite(BCD1, HIGH);
                digitalWrite(BCD2, LOW);
            }
            else if (row == 3)
            {
                digitalWrite(BCD0, HIGH);
                digitalWrite(BCD1, HIGH);
                digitalWrite(BCD2, LOW);
            }
            else if (row == 4)
            {
                digitalWrite(BCD0, LOW);
                digitalWrite(BCD1, LOW);
                digitalWrite(BCD2, HIGH);
            }
            else if (row == 5)
            {
                digitalWrite(BCD0, HIGH);
                digitalWrite(BCD1, LOW);
                digitalWrite(BCD2, HIGH);
            }
            else if (row == 6)
            {
                digitalWrite(BCD0, LOW);
                digitalWrite(BCD1, HIGH);
                digitalWrite(BCD2, HIGH);
            }
            if (slowmode)
            {
                delay(28);
            }
        }
    }
}

void move_left()
{
    for (int row = 0; row < 7; row++)
    {
        for (int col = 0; col < 92; col++)
        {
            if (col == 91)
            {
                canvas[row][91] = canvas[row][0];
            }
            else
            {
                canvas[row][col] = canvas[row][col + 1];
            }
        }
    }
}

void move_right()
{
    for (int row = 0; row < 7; row++)
    {
        for (int col = 0; col < 92; col++)
        {
            if (col == 91)
            {
                canvas[row][0] = canvas[row][91];
            }
            else
            {
                canvas[row][91 - col] = canvas[row][90 - col];
            }
        }
    }
}

void startup()
{
    offset = 0;
    strobe(1);
    digitalWrite(LEDS, HIGH);
    for (int posHoriz = 0; posHoriz < 92; posHoriz++)
    {
        write_letter('#');
        strobe(5);
        offset++;
    }
    clear_canvas();
    update_text("KNOW-IT");
}

void setup()
{
    /* Config pins as output */
    pinMode(D1, OUTPUT);
    pinMode(C1, OUTPUT);
    pinMode(STR, OUTPUT);
    pinMode(LEDS, OUTPUT);
    pinMode(BCD0, OUTPUT);
    pinMode(BCD1, OUTPUT);
    pinMode(BCD2, OUTPUT);
    /* Open Serial Port for debugging */
    Serial.begin(115200);
    /* Setup Soft-AP */
    Serial.print("Setting up AP ... ");
    Serial.println(WiFi.softAP(ssid, pw) ? "Ready" : "Failed!");
    Serial.print("IP address = ");
    Serial.println(WiFi.softAPIP());

    /* main page */
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "text/html", index_html, processor); });

    /* text update */
    server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request)
              {
            String inputString;
            if (request->hasParam(PARAM_INPUT_STRING))
            {
                inputString = request->getParam(PARAM_INPUT_STRING)->value();
                update_text(inputString);
                Serial.println(inputString);
            }
            else
            {
                inputString = "404";
                Serial.println("Fehler");
            }
            request->send_P(200, "text/plain", "OK"); });

    /* On - Off */
    server.on("/run", HTTP_GET, [](AsyncWebServerRequest *request)
              {
            String inputPower;
            if (request->hasParam(PARAM_INPUT_PWR))
            {
                inputPower = request->getParam(PARAM_INPUT_PWR)->value();
                if (inputPower == "on")
                {
                    power = true;
                }
                else if (inputPower == "off")
                {
                    power = false;
                }
            }
            else
            {
                inputPower = "No message sent";
            }
            request->send(200, "text/plain", "OK"); });
    /* Left - Right */
    server.on("/mode", HTTP_GET, [](AsyncWebServerRequest *request)
              {
            String inputMode;
            if (request->hasParam(PARAM_INPUT_MODE))
            {
                inputMode = request->getParam(PARAM_INPUT_MODE)->value();
                if (inputMode == "left")
                {
                    runLeft = true;
                }
                else if (inputMode == "right")
                {
                    runLeft = false;
                }
            }
            else
            {
                inputMode = "No message sent";
            }
            request->send(200, "text/plain", "OK"); });
    server.on("/demo", HTTP_GET, [](AsyncWebServerRequest *request)
              {
            String inputSlowmode;
            if (request->hasParam(PARAM_INPUT_SLOWMODE))
            {
                inputSlowmode = request->getParam(PARAM_INPUT_SLOWMODE)->value();
                if (inputSlowmode == "on")
                {
                    slowmode = true;
                }
                else if (inputSlowmode == "off")
                {
                    slowmode = false;
                }
            }
            else
            {
                inputSlowmode = "No message sent";
            }
            request->send(200, "text/plain", "OK"); });
    /* speed update */
    server.on("/slider", HTTP_GET, [](AsyncWebServerRequest *request)
              {
            String inputSpeed;
            if (request->hasParam(PARAM_INPUT_SPEED))
            {
                inputSpeed = request->getParam(PARAM_INPUT_SPEED)->value();
                speed = inputSpeed.toInt();
            }
            else
            {
                inputSpeed = "404";
                Serial.println("Fehler");
            }
            request->send_P(200, "text/plain", "OK"); });
    server.begin();
    startup();
}

void loop()
{
    while (power)
    {
        digitalWrite(LEDS, HIGH);
        /* strobes per image, 6 fast, 8 normal, 10 slow*/
        strobe(speed);
        /* Rechtslauf >>>>>>> */
        if (!runLeft)
        {
            move_right();
        }
        /* Linkslauf <<<<<<<  */
        else if (runLeft)
        {
            move_left();
        }
        yield();
    }
    while (!power)
    {
        digitalWrite(LEDS, LOW);
        delay(1000);
        yield();
    }
}