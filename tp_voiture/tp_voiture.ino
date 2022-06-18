#include <logging.h>

#include <Array.h>

#include <uartWiFi.h>

#include <logging.h>

UARTWifi uartWiFi;


#define STA
#define TCP_BUFFER_SIZE 128

#define WIFI_SSID "aled"
#define WIFI_PWD "12345678"

#ifndef ENCRYPTION_TYPE
#define ENCRYPTION_TYPE WIFI_AUTH_OPEN
#endif

Array<ACCESS_POINT, MAX_SCAN_RESULT_COUNT> scan_results;
char tcp_buffer[TCP_BUFFER_SIZE];

#ifndef ENCRYPTION_TYPE
#define ENCRYPTION_TYPE WIFI_AUTH_OPEN
#endif

SERVER_CONFIG config;



int PIN_VERT=11;
int PIN_ROUGE=10;
int PIN_BLEU=9;

void setup()
{
  initLED();
  uartWiFi.begin();

  //connect_soft_accesspoint(WIFI_SSID, WIFI_PWD, 6);
  connect_to_station(WIFI_SSID, WIFI_PWD);

  start_web_server();
}

void loop()
{
  delay(100);
  Array<TCP_CHANNEL, 4> tcp_channels;

  if (!uartWiFi.getClientStatus(&tcp_channels))
    return;

  for (unsigned int i = 0; i < tcp_channels.size(); i++)
  {
    if (tcp_channels[i].size <= 0)
    {
      continue;
    }

    uartWiFi.readData(tcp_buffer, tcp_channels[i].channel, TCP_BUFFER_SIZE);
    
    Serial.println(tcp_buffer);
    if(tcp_buffer[0]== 'h'){
      turnOnLED();
    }else if(tcp_buffer[0]=='l'){
      turnOffLED();
    }

    //uartWiFi.send(tcp_channels[i].channel, tcp_buffer);
  }

  tcp_channels.clear();
}

void initLED(){
    pinMode(PIN_VERT,OUTPUT);
    pinMode(PIN_ROUGE,OUTPUT);
    pinMode(PIN_BLEU,OUTPUT);
}
void turnOnLED(){  
    digitalWrite(PIN_VERT, 128);
    digitalWrite(PIN_ROUGE, 128);
    digitalWrite(PIN_BLEU, 128);
}
void turnOffLED(){  
    digitalWrite(PIN_VERT, 0);
    digitalWrite(PIN_ROUGE, 0);
    digitalWrite(PIN_BLEU, 0);
}


void start_web_server()
{
    if (!uartWiFi.createServer(8080))
    {
        LogErr("Failed to start web server");
        while (1);
    }

    LogInfo("Web server started");

    uartWiFi.getServerStatus(&config);

    if (config.mode == SERVER_MODE::OPEN)
    {
        LogInfo("Server is running");
    }
    else
    {
        LogErr("Server is not running");
    }
}
void connect_to_station(const char *ssid, const char *pwd)
{
  uartWiFi.scan(&scan_results);

  LogInfo("Access points found : %d", scan_results.size());

  LogInfo("Connecting to %s", ssid);

  uartWiFi.configureWiFiMode(STATION);

  if (uartWiFi.connectAP(ssid, pwd))
  {
    LogInfo("Connected to %s", ssid);
  }
  else
  {
    LogErr("Failed to connect to %s", ssid);
    while (1);
  }

  scan_results.clear();

  WIFI_MODE mode = uartWiFi.getWiFiMode();

  if (mode != WIFI_MODE::STATION)
  {
    LogErr("Wifi mode is not STATION");
    while (1)
      ;
  }

  JOIN_AP_CONFIG conf;
  if (!uartWiFi.getAccessPoint(&conf))
  {
    LogErr("Failed to get access point");
    while (1)
      ;
  }

  LogInfo("Current access point: %s", conf.ssid);
}