#include <logging.h>

#include <Array.h>

#include <uartWiFi.h>

#include <logging.h>

UARTWifi uartWiFi;


#define STA

#define WIFI_SSID "aled"
#define WIFI_PWD "12345678"

#ifndef ENCRYPTION_TYPE
#define ENCRYPTION_TYPE WIFI_AUTH_OPEN
#endif


#define TCP_BUFFER_SIZE 128
Array<ACCESS_POINT, MAX_SCAN_RESULT_COUNT> scan_results;
char tcp_buffer[TCP_BUFFER_SIZE];


SERVER_CONFIG config;

//Motor 1
const int motorPin1 = 8;//forward
const int motorPin2 = 5;//backward
//Motor 2
const int motorPin3 = 6;
const int motorPin4 = 7;
int speed = 180;

void setup()
{
  initMotor();
  
  uartWiFi.begin();
  connect_to_station(WIFI_SSID, WIFI_PWD);

  start_web_server();
}

void loop()
{
    delay(400);
    buttonAction();
    Array<TCP_CHANNEL, 4> tcp_channels;
    if (!uartWiFi.getClientStatus(&tcp_channels)){
      return;
    }
    for (unsigned int i = 0; i < tcp_channels.size(); i++)  {
      uartWiFi.readData(tcp_buffer, tcp_channels[i].channel, TCP_BUFFER_SIZE);
      moveCar(tcp_buffer[0]);
    }
}

void initMotor(){
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
  
}

void moveCar(char command){
  switch(command){
    case 'l':
      analogWrite(motorPin1, speed);
      analogWrite(motorPin2, 0);
      analogWrite(motorPin3, 0);
      analogWrite(motorPin4, speed);
    break;
    case 'r':
      analogWrite(motorPin1, 0);
      analogWrite(motorPin2, speed);
      analogWrite(motorPin3, speed);
      analogWrite(motorPin4, 0);
    break;
    case 'f':    
      analogWrite(motorPin1, speed);
      analogWrite(motorPin2, 0);
      analogWrite(motorPin3, speed);
      analogWrite(motorPin4, 0);
    break;
    case 'b':
      analogWrite(motorPin1, 0);
      analogWrite(motorPin2, speed);
      analogWrite(motorPin3, 0);
      analogWrite(motorPin4, speed);
    break;
    case 's':
      analogWrite(motorPin1, 0);
      analogWrite(motorPin2, 0);
      analogWrite(motorPin3, 0);
      analogWrite(motorPin4, 0);
    
  }
}

void start_web_server()
{
    if (!uartWiFi.createServer(8080)){
        LogErr("Failed to start web server");
        while (1);
    }
    LogInfo("Web server started");
    uartWiFi.getServerStatus(&config);
    if (config.mode == SERVER_MODE::OPEN){
        LogInfo("Server is running");
    }
    else{
        LogErr("Server is not running");
    }
}

void connect_to_station(const char *ssid, const char *pwd)
{
  uartWiFi.scan(&scan_results);
  LogInfo("Access points found : %d", scan_results.size());
  LogInfo("Connecting to %s", ssid);
  uartWiFi.configureWiFiMode(STATION);
  if (uartWiFi.connectAP(ssid, pwd)) {
    LogInfo("Connected to %s", ssid);
  }
  else{
    LogErr("Failed to connect to %s", ssid);
    while (1);
  }
  scan_results.clear();
  WIFI_MODE mode = uartWiFi.getWiFiMode();
  if (mode != WIFI_MODE::STATION){
    LogErr("Wifi mode is not STATION");
    while (1);
  }
  JOIN_AP_CONFIG conf;
  if (!uartWiFi.getAccessPoint(&conf))
  {
    LogErr("Failed to get access point");
    while (1);
  }
  LogInfo("Current access point: %s", conf.ssid);
}
