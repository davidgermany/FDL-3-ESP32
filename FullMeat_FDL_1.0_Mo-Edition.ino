#include <ESP32Servo.h>
#include <Wire.h>  ///////////////SDA=D21  SCL=22////////////////////////////////////
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>



//VARIABLEN
int spinUpTime = 500;  // BlMotor startzeit, bevor dart vor geschoben wird (feature: noch machen: spinuptime relativ zu escSpeed)
unsigned long spinUpStartTime = 0;
unsigned long ausdrehen = 100; //extra zeit nach dem schießen um den Lauf wirklich zu leeren, um verstopfung vorzubeugen. 100ms
bool setMillisLock = 0;
int balkenL = 0;
int balkenR = 0;
int speedESC = 0;
bool unlocked = false;
int feedSpeed = 255;
int fireMode = 0;  //Feuermodus: 0 Voll Automatisch, 1 Halb Automatisch, 2 2er Salve
int burst = fireMode;
bool einparken = false; //wenn true, dann wird der Feeder eingeparkt, wenn der Trigger losgelassen wird.
float voltage = 0;
int triggerReleased = 1;
bool batterySafety = 0;
unsigned long unlockedMillis = millis();
unsigned long voltageTime = millis();
//KNÖPFE
const int button1 = 15;
const int button2 = 4;
const int button3 = 16;
const int button4 = 17;
const int button5 = 5;
const int button6 = 18;
const int buttonT = 19;
const int buttonPT = 33;
const int buttonESWV = 14;
const int buttonESWH = 27;
//POTIS
const int potL = 34;
const int potR = 35;
bool potsLocked = 0;

//FEEDMOTOR
const int feedMotorR = 25;
const int feedMotorL = 26;
const int PwmFreq = 5000;
const int PwmChannel1 = 2;
const int PwmChannel2 = 3;
const int PwmResolution = 8;





#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

Servo ESC;  // create servo object to control the ESC
void setup() {
  speedESC = 0;
  // digitalWrite(feedMotorR, LOW);
  Serial.begin(9800);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextSize(2);

  ESC.attach(32, 1000, 2000);  // (pin, min pulse width, max pulse width in microseconds)

  pinMode(buttonPT, INPUT_PULLUP);
  pinMode(buttonT, INPUT_PULLUP);
  pinMode(buttonESWV, INPUT_PULLUP);
  pinMode(buttonESWH, INPUT_PULLUP);
  pinMode(feedMotorL, OUTPUT);
  pinMode(feedMotorR, OUTPUT);
  pinMode(potL, INPUT);
  pinMode(potR, INPUT);
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);
  pinMode(button4, INPUT_PULLUP);
  pinMode(button5, INPUT_PULLUP);
  pinMode(button6, INPUT_PULLUP);

  // configure PWM functionalitites
  ledcSetup(PwmChannel1, PwmFreq, PwmResolution);
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(feedMotorL, PwmChannel1);
  //ledcAttachPin(feedMotorL, PwmChannel);
  ledcSetup(PwmChannel2, PwmFreq, PwmResolution);
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(feedMotorR, PwmChannel2);
  //ledcAttachPin(feedMotorL, PwmChannel);
}

void loop() {
  //                    13v----29,3K┬--to ADC Input GPIO36
  //                                |
  //                               10K
  //                                |
  //                               Gnd
  voltage = (float)analogRead(36) / 900;
  //Serial.println(voltage);


  
  if(potsLocked == false){
  spinUpTime = analogRead(potR);
  spinUpTime = map(spinUpTime, 4095, 0, 500,250);
  }
  
  if(potsLocked == false){
  speedESC = analogRead(potL);
  //Serial.println(potL);               // reads the value of the potentiometer (value between 0 and 4095)
  

  speedESC = map(speedESC, 4095, 0, 40, 100);  // scale it to use it with the servo library (value between 0 and 180) 180 is the MAX speed for the escs
  //Serial.println(speedESC);
  //Serial.println(feedSpeed);
  if (unlocked == true){
    speedESC = analogRead(potL);
 speedESC = map(speedESC, 4095, 0, 40, 180);
  }
  }




  //if (digitalRead(buttonPT) == LOW && voltage >= 3.20) {
  if (digitalRead(buttonPT) == LOW && batterySafety == true) {
    //Serial.println("PreTrigger");
    ESC.write(speedESC);
    if (setMillisLock == 0) {
      spinUpStartTime = millis();
      setMillisLock = 1;
    }
  } 
  else if (ausdrehen >= millis()) {    ledcWrite(PwmChannel1, 0);  ledcWrite(PwmChannel2, 0);} //ausdrehen lassen
  else {
    ESC.write(0); //ESC AUS
    setMillisLock = 0;
  }
  Serial.println(digitalRead(buttonESWV));
  Serial.println(digitalRead(buttonESWH));

  // Trigger

  if (digitalRead(buttonT) == LOW && batterySafety == true) {
    //Serial.println("Trigger");
    if (millis() - spinUpStartTime >= spinUpTime) {
      if (fireMode >= 1 && triggerReleased == 1) {
        Serial.println("fireMode Salve/Halbauto");
        ledcWrite(PwmChannel1, feedSpeed);
        triggerReleased = 0;

        burst = fireMode;
        while (burst != 0) {
          while (digitalRead(buttonESWV) == 1) {
            //Serial.println("bis ESWV feeden");
          }
          while (digitalRead(buttonESWH) == 1) {
            //Serial.println("bis ESWV feeden");
          }
          burst--;
        }
              //einparken

        ledcWrite(PwmChannel1, 255);
        //Serial.println("fertig gefeedet");
        ledcWrite(PwmChannel2, 255);
        einparken = false;
        ausdrehen = millis() +200;
      }

      else if (fireMode == 0) {             //VOLLAUTO
        ledcWrite(PwmChannel1, feedSpeed);  // Feedmotor dreht Linksrum
        einparken = true;
       
          while (digitalRead(buttonESWV) == 1) {
            //Serial.println("bis ESWV feeden");
          }
          while (digitalRead(buttonESWH) == 1) {
            //Serial.println("bis ESWV feeden");
          }
          ausdrehen = millis() +200;
        
      }
    }


  }
  
   else { //FEEDMOTOR AUS
    ledcWrite(PwmChannel1, 0);
    ledcWrite(PwmChannel2, 0);
    triggerReleased = 1;
    if (einparken == true){
              //einparken

        ledcWrite(PwmChannel1, 255);
        //Serial.println("fertig gefeedet");
        ledcWrite(PwmChannel2, 255);
        einparken = false;
    }
  }
// 
//
//button3         button6
//button2   OLED  button5
//button1         button4
  if (digitalRead(button1) == LOW) {
    fireMode = 1;
    burst = 1;
  }
  if (digitalRead(button2) == LOW) {
    fireMode = 2;
    burst = 2;
  }
  if (digitalRead(button3) == LOW) {
    fireMode = 3;
    burst = 3;
  }
  if (digitalRead(button4) == LOW) {
    fireMode = 0;
    burst = 0;
  }
  if (digitalRead(button5) == LOW) {
   CREDITS(); //macht noch nichts

   if ((digitalRead(button1) == LOW)){

 if (unlocked == true){unlocked = false;} else unlocked = true;
     fireMode = 0;
    burst = 0;
 delay(300);
   }
     potsLocked = false;
  }
    if (digitalRead(button6) == LOW) {

  potsLocked = true;

  }
  
//BATTERIELEERSCHUTZ
if(voltage >=3.8){ batterySafety = true;voltageTime = millis() + 2000;}
else if(voltage >=3.5){voltageTime = millis() + 2000;}

//if(voltage <= 3.3){batterySafety = false;} 
if(voltageTime >= millis()){batterySafety = true;} 
else {batterySafety = false; unlocked = false;}


  OLED();
}
