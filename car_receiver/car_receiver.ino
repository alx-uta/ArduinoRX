/**
   Arduino RC Receiver - Car
   https://github.com/alx-uta/ArduinoRX
   Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <printf.h> // Required for "radioTx.printDetails();"
#include <Servo.h>
#include "config.h"

Servo rcServo;
Servo dcMotor;


//nRF24L01
RF24 radioTx(TX_CE, TX_CSN);
RF24 radioRx(RX_CE, RX_CSN);


unsigned long lastReceiveTime = 0;
unsigned long currentTime = 0;

struct package {
  // Left Switch
  byte s1;

  // Left Pot
  byte p1;

  // Left Joystick
  byte j1u; // Up
  byte j1d; // Down
  byte j1l; // Left
  byte j1r; // Right
  byte j1b; // Button

  // Left Thumb Joystick
  byte tj1u; // Up
  byte tj1d; // Down
  byte tj1l; // Left
  byte tj1r; // Right

  // Left Btn
  byte b1;

  // Right Switch
  byte s2;

  // Right Pot
  byte p2;

  // Right Joystick
  byte j2u; // Up
  byte j2d; // Down
  byte j2l; // Left
  byte j2r; // Right
  byte j2b; // Button

  // Right Thumb Joystick
  byte tj2u; // Up
  byte tj2d; // Down
  byte tj2l; // Left
  byte tj2r; // Right

  // Right Btn
  byte b2;

  // Rotary Encoder
  byte re;

  // Rotary Encoder - Push
  byte rep;

  // Green Btn
  byte gb;

  // Red Btn
  byte rb;

  // Transmitter Time
  byte transmitterTime;
};
package data;

// Store the current values
struct packageTx {
  byte voltage;
  byte voltage_2;
};

// Create a variable with the above structure
packageTx dataTx;

void setup() {
  if (debug)
    Serial.begin(115200);

  dcMotor.attach(DC_MOTOR); // attaches the servo on pin 9 to the servo object

  if (debug)
    Serial.println("Zero Throttle");

  dcMotor.write(zeroThrottle);  // Set the output to the middle or "zero" position. CONNECT THE ESC DURING THIS DELAY!!

  if (debug)
    Serial.println("Setup Complete");

  if (debug)
    printf_begin();

  rcServo.attach(RC_SERVO);

  /**
     TX
  */
  radioTx.begin();
  radioTx.setPALevel(RF24_PA_LOW);  // RF24_PA_MAX is default.
  radioTx.setChannel(TX_CHANNEL);
  radioTx.setDataRate(RF24_250KBPS);
  radioTx.setPayloadSize(sizeof(dataTx)); // float datatype occupies 4 bytes
  //  radioTx.setAutoAck(false);
  radioTx.openWritingPipe(address[1]);     // always uses pipe 0
  radioTx.stopListening(); // put radio in TX mode

  if (debug)
    radioTx.printPrettyDetails();

  /**
     RX
  */
  radioRx.begin();
  radioRx.setPALevel(RF24_PA_LOW);
  radioRx.setChannel(RX_CHANNEL);
  radioRx.setDataRate(RF24_250KBPS);
  radioRx.setPayloadSize(sizeof(data));
  radioRx.openReadingPipe(1, address[0]);
  //  radioRx.setAutoAck(false);
  radioRx.startListening(); // put radio in RX mode

  if (debug)
    radioRx.printPrettyDetails();

  defaultData();
}
void loop() {

  /**
     If radioTx.available() read the data
  */
  if (radioRx.available()) {
    radioRx.read(&data, sizeof(package));

    if (debug)
    {
      Serial.print("s1: ");
      Serial.print(data.s1);
      Serial.print(";");

      Serial.print("p1: ");
      Serial.print(data.p1);
      Serial.print(";");

      Serial.print("j1ud: ");
      Serial.print(data.j1u);
      Serial.print(":");
      Serial.print(data.j1d);
      Serial.print(";");

      Serial.print("j1lr: ");
      Serial.print(data.j1l);
      Serial.print(":");
      Serial.print(data.j1r);
      Serial.print(";");

      Serial.print("j1b: ");
      Serial.print(data.j1b);
      Serial.print(";");

      Serial.print("tj1ud: ");
      Serial.print(data.tj1u);
      Serial.print(":");
      Serial.print(data.tj1d);
      Serial.print(";");

      Serial.print("tj1lr: ");
      Serial.print(data.tj1l);
      Serial.print(":");
      Serial.print(data.tj1r);
      Serial.print(";");

      Serial.print("b1: ");
      Serial.print(data.b1);
      Serial.print(";");

      Serial.print("s2: ");
      Serial.print(data.s2);
      Serial.print(";");

      Serial.print("p2: ");
      Serial.print(data.p2);
      Serial.print(";");

      Serial.print("j2u: ");
      Serial.print(data.j2u);
      Serial.print(":");
      Serial.print(data.j2d);
      Serial.print(";");

      Serial.print("j2l: ");
      Serial.print(data.j2l);
      Serial.print(":");
      Serial.print(data.j2r);
      Serial.print(";");

      Serial.print("j2b: ");
      Serial.print(data.j2b);
      Serial.print(";");

      Serial.print("tj2u: ");
      Serial.print(data.tj2u);
      Serial.print(":");
      Serial.print(data.tj2d);
      Serial.print(";");

      Serial.print("tj2l: ");
      Serial.print(data.tj2l);
      Serial.print(":");
      Serial.print(data.tj2r);
      Serial.print(";");

      Serial.print("b2: ");
      Serial.print(data.b2);
      Serial.print(";");

      Serial.print("gb: ");
      Serial.print(data.gb);
      Serial.print(";");

      Serial.print("rb: ");
      Serial.print(data.rb);
      Serial.print(";");

      Serial.println();
    }

    servoMove(data);
    dcMove(data);
  }

  dataTx.voltage = 2.3; //TODO
  radioTx.write(&dataTx, sizeof(packageTx));
}


void dcMove(package data) {
  int dcMoveVal;
  // Default Value
  dcMoveVal = zeroThrottle;

  if (data.j1u > 2) {
    dcMoveVal = map(data.j1u, 0, 255, zeroThrottle, maxThrottle);
  }
  if (data.j1d > 2) {

    dcMotor.write(zeroThrottle);
    dcMoveVal = minThrottle;
  }

  if (debug) {
    Serial.print("DC:");
    Serial.print(data.j1u);
    Serial.print("-");
    Serial.print(data.j1d);
    Serial.print(":");
    Serial.print(dcMoveVal);

    Serial.println();
  }

  dcMotor.write(dcMoveVal);
}
void servoMove(package data) {
  int servoPos;
  // Default Value
  servoPos = 90;
  if (data.j2r > 2) {
    servoPos = map(data.j2r, 0, 255, 90, 140);
  }
  if (data.j2l > 2) {
    servoPos = map(data.j2l, 0, 255, 90, 40);
  }

  rcServo.write(servoPos);
}

void defaultData() {
  rcServo.write(90);
  // Left Switch
  data.s1 = 1;

  // Left Pot
  data.p1 = 0;

  // Left Joystick
  data.j1u = 0; // Up
  data.j1d = 0; // Down
  data.j1l = 0; // Left
  data.j1r = 0; // Right
  data.j1b = 1; // Button

  // Left Thumb Joystick
  data.tj1u = 0; // Up
  data.tj1d = 0; // Down
  data.tj1l = 0; // Left
  data.tj1r = 0; // Right

  // Left Btn
  data.b1 = 1;

  // Right Switch
  data.s2 = 1;

  // Right Pot
  data.p2 = 0;

  // Right Joystick
  data.j2u = 0; // Up
  data.j2d = 0; // Down
  data.j2l = 0; // Left
  data.j2r = 0; // Right
  data.j2b = 1; // Button

  // Right Thumb Joystick
  data.tj2u = 0; // Up
  data.tj2d = 0; // Down
  data.tj2l = 0; // Left
  data.tj2r = 0; // Right

  // Right Btn
  data.b2 = 1;

  // Rotary Encoder
  // data.re;

  // Rotary Encoder - Push
  data.rep = 1;

  // Green Btn
  data.gb = 1;

  // Red Btn
  data.rb = 1;

  // Transmitter Time
  data.transmitterTime = millis();
}
