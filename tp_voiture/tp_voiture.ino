#include <logging.h>

#include <Array.h>

#include <uartWiFi.h>

#include <logging.h>

UARTWifi uartWiFi;


#define STA

#define WIFI_SSID "aled"
#define WIFI_PWD "12345678"
//pour se connecter, ouvrir terminal unix + saisir [nc 192.168.43.100 8080] (en remplaçant adresse par celle utilisée lors de la connexion)

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


int PIN_VERT=11;
int PIN_ROUGE=10;
int PIN_BLEU=9;

int PIN_BUTTON=2;
int valueButton=0;
bool isTurnedOn=false;

void setup()
{
//  initLED();
//  initButton();
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
//      Serial.println(tcp_buffer[0]);
      moveCar(tcp_buffer[0]);
//      switchColorKeyboardEnter();     
//      uartWiFi.send(tcp_channels[i].channel, tcp_buffer);
    }

//  tcp_channels.clear();
}

void initMotor(){
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
  
}
void test(){
  //Motor Control A in both directions
  analogWrite(motorPin1, speed);
  delay(2000);
  analogWrite(motorPin1, 0);
  delay(200);
  analogWrite(motorPin2, speed);
  delay(2000);
  analogWrite(motorPin2, 0);
  //Motor Control B in both directions
  analogWrite(motorPin3, speed);
  delay(2000);
  analogWrite(motorPin3, 0);
  delay(200);
  analogWrite(motorPin4, speed);
  delay(2000);
  analogWrite(motorPin4, 0);
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

void buttonAction(){
   valueButton=digitalRead(PIN_BUTTON); 
    if (valueButton==LOW){
      if(isTurnedOn)
        turnOffLED();
      else
        turnOnLED();   
      isTurnedOn=!isTurnedOn; 
      delay(200);
    }
}

void switchColorKeyboardEnter(){
   if(isTurnedOn){
        if(tcp_buffer[0]== 'r'){
          redLED();
        }else if(tcp_buffer[0]=='g'){
          greenLED();
        }
        else if(tcp_buffer[0]=='b'){
          blueLED();
        }
        else if(tcp_buffer[0]=='a'){
          turnOnLED();
        }
      }
}

void initButton(){
  pinMode(PIN_BUTTON,INPUT);
}

void initLED(){
    pinMode(PIN_VERT,OUTPUT);
    pinMode(PIN_ROUGE,OUTPUT);
    pinMode(PIN_BLEU,OUTPUT);
    turnOffLED();
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

void blueLED(){
    digitalWrite(PIN_BLEU, 128);  
    digitalWrite(PIN_VERT, 0);
    digitalWrite(PIN_ROUGE, 0);
}

void redLED(){
    digitalWrite(PIN_BLEU, 0);  
    digitalWrite(PIN_VERT, 0);
    digitalWrite(PIN_ROUGE, 128);
}

void greenLED(){
    digitalWrite(PIN_BLEU, 0);  
    digitalWrite(PIN_VERT, 128);
    digitalWrite(PIN_ROUGE, 0);
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
