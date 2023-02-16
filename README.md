
# 7 Channel Radio Controller

Radio control (often abbreviated to R/C or simply RC) is the use of control signals transmitted by radio to remotely control a device. Examples of simple radio control systems are garage door openers and keyless entry systems for vehicles, in which a small handheld radio transmitter unlocks or opens doors. Radio control is also used for control of model vehicles from a hand-held radio transmitter. Industrial, military, and scientific research organizations make use of radio-controlled vehicles as well. A rapidly growing application is control of unmanned aerial vehicles (UAVs or drones) for both civilian and military uses, although these have more sophisticated control systems than traditional applications.

This project describe about 7-channel RC controller for driven many things. Such as RC drones, cars, quadcopters, etc.

## Components Used

### Hardware 

Transmitter Circuit

1. 1 Arduino nano (ATMEGA 328P)
2. 2 toggle switch
3. 2 Joysticks
4. 1 NRF24L01 module
5. 1 potentiometer
6. 1 battery (9V) or sets of batteries equivalent to 9V
7. 1 battery holder

##
Receiver Circuit

1. 1 Arduino nano (ATMEGA 328P)
2. 1 NRF24L01 module
3. 2 LEDs
4. 2 330 ohms resistors
5. 1 coreless dc motor(7×14)
6. 1 Mosfet P0903BDG
7. 1 10K ohm resistor
8. 1 Schottky diode 1N5819

### Software

1. VS Code (Arduino IDE Extension)


## Reciever circuit

<img src="https://firebasestorage.googleapis.com/v0/b/channel-radio.appspot.com/o/RECEIVER%20.png?alt=media&token=8c72902a-ffbd-41e8-b9f2-a7d126003231"></img>


## Transmitter circuit

<img src="https://firebasestorage.googleapis.com/v0/b/channel-radio.appspot.com/o/TRANSMITTER%20.png?alt=media&token=e4dd55b7-21a4-4b19-a71c-aaab674b0675"></img>


## Code for reciever

```javascript
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
```


## Code for transmitter

```javascript
/*
Author : Mohd Aman Ansari
embed
https://embed.org.in
*/
#include <SPI.h>     //Include SPI library
#include <nRF24L01.h>
#include <RF24.h>      //Include nRF24L01 & RF24 library

const uint64_t pipeOut = 0xE7E7F0F0E1LL; //Remember to keep this exactly the same for the receiver code also

RF24 radio(9, 10);

struct MyData {
  byte throttle;
  byte yaw;
  byte pitch;
  byte roll;
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
  data.AUX1 = 0;
  data.AUX2 = 0;
}

void setup()
{

  radio.begin();
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(pipeOut);
  resetData();
}

int mapJoystickValues(int val, int lower, int middle, int upper, bool reverse)
{
  val = constrain(val, lower, upper);
  if ( val < middle )
    val = map(val, lower, middle, 0, 128);
  else
    val = map(val, middle, upper, 128, 255);
  return ( reverse ? 255 - val : val );
}

void loop()
{
  data.throttle = mapJoystickValues( analogRead(A0), 13, 524, 1015, true );
  data.yaw      = mapJoystickValues( analogRead(A1),  1, 505, 1020, true );
  data.pitch    = mapJoystickValues( analogRead(A2), 12, 544, 1021, true );
  data.roll     = mapJoystickValues( analogRead(A3), 34, 522, 1020, true );
  data.AUX1     = digitalRead(4); //The 2 toggle switches
  data.AUX2     = digitalRead(5);

  radio.write(&data, sizeof(MyData));
}