/*
Author : Mohd Aman Ansari
embed
https://embed.org.in
*/

#include <Servo.h>  //Include Servo motor library
#include <SPI.h>    //Include SPI library 
#include <nRF24L01.h>   
#include <RF24.h>      //Include nRF24L01 & RF24 library

const uint64_t pipeIn = 0xE7E7F0F0E1LL; 

RF24 radio(9, 10); //CE and CSN pin 
Servo myServo;

struct MyData {
byte throttle;
byte yaw;
byte pitch;
byte roll;
byte Trim;
byte AUX1;
byte AUX2;
};

MyData data;

void resetData()
{

data.throttle = 0;
data.yaw = 127;
data.pitch = 127;
data.roll = 127;
data.Trim = 0;
data.AUX1 = 0;
data.AUX2 = 0;

}

void setup()
{
Serial.begin(9600); //Set the speed to 9600 bauds if you want.

resetData();
radio.begin();
radio.setAutoAck(false);
radio.setDataRate(RF24_250KBPS);

radio.openReadingPipe(1,pipeIn);
radio.startListening();

pinMode(2,OUTPUT);
pinMode(3,OUTPUT);
myServo.attach(4);
pinMode(5,OUTPUT);

}

/**************************************************/

unsigned long lastRecvTime = 0;

void recvData()
{
while ( radio.available() ) {
radio.read(&data, sizeof(MyData));
lastRecvTime = millis(); //here we receive the data
}
}

/**************************************************/

void loop()
{
recvData();
unsigned long now = millis();

if ( now - lastRecvTime > 1000 ) {

resetData();
}

digitalWrite(2,data.AUX1);
digitalWrite(3,data.AUX2);

int val = map(data.throttle,128,255,0,255);

int val1 = map(data.roll,0,255,0,180);

myServo.write(val1);

if(val > 0){
analogWrite(5,val);
}else{

  analogWrite(5,0);
}

Serial.print("Throttle: "); Serial.print(data.throttle);  Serial.print("    ");
Serial.print("Yaw: ");      Serial.print(data.yaw);       Serial.print("    ");
Serial.print("Pitch: ");    Serial.print(data.pitch);     Serial.print("    ");
Serial.print("Roll: ");     Serial.print(data.roll);      Serial.print("    ");
Serial.print("Trim: ");     Serial.print(data.Trim);      Serial.print("    ");
Serial.print("Aux1: ");     Serial.print(data.AUX1);      Serial.print("    ");
Serial.print("Aux2: ");     Serial.print(data.AUX2);      Serial.print("\n");

}