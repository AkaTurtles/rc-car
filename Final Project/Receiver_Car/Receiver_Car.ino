#include <SoftwareSerial.h> // This allows us to send info to the PC's serial monitor.
#include <VirtualWire.h> // This is the RF commuincation library
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);// set LCD address

void setup()
{
  Serial.begin(9600);  // Debugging only
  Serial.println("setup");
  //LCD Screen
  lcd.begin();
  lcd.backlight();  // Initialise the IO and ISR

  //Radio Receiver
  vw_setup(2000);  // Bits per sec. 200 is a good number. Must match that of the TX.
  vw_rx_start();       // Start the receiver PLL running
  
  pinMode(2, OUTPUT);
  //motors
  pinMode(3, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);

}

float drive;
float steer;
int8_t buflen = VW_MAX_MESSAGE_LEN;
int8_t buf[VW_MAX_MESSAGE_LEN];

void loop() {
  float val;
  if (vw_get_message((int8_t *)buf, &buflen)) { // Non-blocking.

    digitalWrite(13, true); // Flash a light to show received good message
    // Message with a good checksum received, dump it.

    steer = (buf[0]);
    drive = (buf[1]);
    lcd.clear();
    lcd.print(drive); lcd.print(" | ");
    lcd.println(steer);
    digitalWrite(13, false);

    //when getting interference buffer becomes 0
    //so any num > 0 and < 18 is my inputs
    if (steer + drive > 0 && steer + drive < 18) {//only take my inputs
      if (steer < 4)//right
        rotateRight(100, steer * 85);//maps 0, 4 to 0, 255
      else if (steer > 5)//left
        rotateLeft(100, (steer - 6) * 64); //maps 6, 9, to 0, 255
      else if (steer >= 4 && steer <= 5) {//if you're not steering
        if (drive <= 4)//tilt forwards
          backward(100, drive * 64);//maps 0, 4 to 0, 255
        else if (drive > 5)//tilt backwards
          slowForward(100, (drive - 5) * 64); //drive*64 is 255 this maps the simple inputs as speed that we can differentiate
        else if (drive == 5)//relitivly parallel to floor (between 3 & 4)
          motorsOff(100);
      }
    }
  }
  delay(10);
}

//3 & 5 are LEft
//6 & 9 are RIGHt
void motorsOff(int forHowLong) {
  digitalWrite(3, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(9, LOW);
  delay(forHowLong);
}

void slowForward(int forHowLong, float i) {
  motorsOff(20);
  digitalWrite(3, i);//L fop
  digitalWrite(5, LOW);
  digitalWrite(6, i);//Rf
  digitalWrite(9, LOW);
  delay(forHowLong);
}

void forward(int forHowLong, float i) {
  motorsOff(20);
  digitalWrite(3, i);//L fop
  digitalWrite(5, LOW);
  digitalWrite(6, i);//Rf
  digitalWrite(9, LOW);
  delay(forHowLong);
}

void pivotLeft(int forHowLong, float i) {
  motorsOff(20);
  digitalWrite(3, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, i);
  digitalWrite(9, LOW);
  delay(forHowLong);
}

void pivotRight(int forHowLong, float i) {
  motorsOff(20);
  digitalWrite(3, i);//
  digitalWrite(5, LOW);//
  digitalWrite(6, LOW);//
  digitalWrite(9, LOW);//
  delay(forHowLong);
}

void rotateLeft(int forHowLong, float i) {
  motorsOff(20);
  digitalWrite(3, LOW);
  digitalWrite(5, i);
  digitalWrite(6, i);
  digitalWrite(9, LOW);
  delay(forHowLong);
}

void rotateRight(int forHowLong, float i) {
  motorsOff(20);
  digitalWrite(3, i);//
  digitalWrite(5, LOW);//
  digitalWrite(6, LOW);//
  digitalWrite(9, i);//
  delay(forHowLong);
}
void backward(int forHowLong, float i) {
  motorsOff(20);
  digitalWrite(3, LOW);
  digitalWrite(5, i);
  digitalWrite(6, LOW);
  digitalWrite(9, i);
  delay(forHowLong);
}
