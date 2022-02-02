#include <SoftwareSerial.h>
SoftwareSerial sim(10, 11);
#include <Wire.h> // Library for I2C communication
#include <LiquidCrystal_I2C.h> // Library for LCD

LiquidCrystal_I2C lcd(0x27, 16, 2);

//Sensors / LEDs / Buzzer
const int redLED = 6, greenLED = 4, buzzer = 8, button = 2, pir=13;
int PIRVal=0, activityCounter, ctr=0;
bool state = false;
String cmd; 

//GSM Module
int _timeout;
String message;
String number = "+639054336481";    //Reni
 
void setup(){
  //Misc. Pins
  pinMode(pir, INPUT); //PIR
  pinMode(button, INPUT);
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(buzzer, OUTPUT);
  Serial.begin(9600);

  //LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,2);
  
  //GSM Initialization
  Serial.println("Booting Security System...");
  delay(2000);
  message.reserve(50);
  sim.begin(9600);
  delay(1000);
  Serial.println("Start");
}


void sendMessage(){
  sim.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);
  //Serial.println ("Set SMS Number");
  sim.println("AT+CMGS=\"" + number + "\"\r"); //Mobile phone number to send message
  delay(1000);
  String SMS = "Warning: Movement is Detected!";
  sim.println(SMS);
  delay(100);
  sim.println((char)26);// ASCII code of CTRL+Z
  delay(200);
  message = _readSerial();
}


void callNumber() {
  sim.print (F("ATD"));
  sim.print (number);
  sim.print (F(";\r\n"));
  message = _readSerial();
  Serial.println(message);
}


String _readSerial(){
  _timeout = 0;
  while  (!sim.available() && _timeout < 12000){
    delay(13);
    _timeout++;
  }
  if (sim.available()) {
    return sim.readString();
  }
}


void loop(){
  bool a;
  a = digitalRead(button);
  lcd.clear();

  if (a == LOW){
    lcd.print("Device is on.");
    Serial.println("Turning On");
    state = true;
    delay(1250);
  }
  
    if (state==true){
      do{
        lcd.clear();
        a = digitalRead(button);
        PIRVal = digitalRead(pir);
        Serial.println(PIRVal);

        if (a == LOW){
          lcd.print("Device is off.");
          Serial.println("Turning Off");
          digitalWrite(greenLED, LOW);
          state = false;
          delay(2000);
          break;
        }
        
        if(PIRVal == HIGH){
          lcd.print("Motion Detected");
          Serial.println("Motion Detected");
          digitalWrite(greenLED, LOW);
          for(int i=0; i < 5; i++){
                 digitalWrite(redLED, HIGH);
                 tone(buzzer, 349);
                 delay(200);
                 digitalWrite(redLED, LOW);
                 noTone(buzzer);
                 delay(200);
          }
          lcd.clear();
          ctr++;
          delay(500);
          if (ctr >= 5 ){
            Serial.println("Calling User...");
            lcd.print("Now Calling");
            lcd.setCursor(0,1);
            lcd.print("User!");
            callNumber();
            delay(7500);
            ctr=0;
          }
          else{
            Serial.println("GSM Sending SMS...");
            lcd.print("Sending Warning");
            lcd.setCursor(0,1);
            lcd.print("Message! ");
            lcd.print(ctr);
            sendMessage();
            delay(3000);
          }
        }
        else {
          lcd.print("No Motion");
          Serial.println("Motion Is Not Detected");
          digitalWrite(redLED, LOW);
          digitalWrite(greenLED, HIGH);
          noTone(buzzer);
          delay(1000);
        }
      }while(state==true);
    }
    else if (state == false){
        do{
            a = digitalRead(button);
            if (a == LOW){
              break;
            }
        }while(state==false);
    }
 }
