/* Parts: buttons (x3), Relays (x3), Temp Sensor(x1), LCD(x1), potentiometer(x1)
 * 
 * Description: 
 * Upon startup, the LCD will display the temperature and the countdown time(00:00). If OnButton is pushed once, the AddTimeButton 
 * will be enabled for input and if the temperature is also between 0 – 259F, then solenoid A will turn on. If the temperature is either lower than 0F or higher than 
 * 259F, then AddTimeButton will be enabled but solenoid A will not turn on. If OnButton is pushed for a second time, then AddTimeButton will be disabled and 
 * solenoid A, B and C will all turn off. Therefore, every time the onButton is pushed, the opposite state will occur (either from OFF to ON or ON to OFF). Additionally, 
 * if AddTimeButton is pushed before OnButton, then nothing will happen.
 * 
 * If AddTimeButton is pushed once, then 15 minutes is added to the countdown timer and if temperature is also between 0-250F, then solenoids B and C will turn on. 
 * Each additional push to AddTimeButton will add 15 minutes to the countdown timer but will have no effect on solenoids B and C. If the temperature reaches 250F, 
 * the countdown timer begins counting down. When the countdown timer reaches zero, solenoids A, B and C all turn off.
 */


#include <LiquidCrystal.h> // includes the LiquidCrystal Library 

#define relay1 7
#define relay2 6
#define relay3 5
#define relay4 4
//Note: https://seeeddoc.github.io/Relay_Shield_V2.0/ , Pins 4-7 are taken by relay function and cannot be overloaded by other pins. 


#define MAX_TEMP  250

LiquidCrystal lcd(8, 9, 19, 18, 17, 16); // Creates an LC object. Parameters: (rs, enable, d4, d5, d6, d7)

//Pins:
int buttonPin = 11;   //pin for button 1 (Digital)
int buttonPin2 = 13;   //pin for button 2 (Analog)

//Time variables:
int period = 1000;
int val = 0; //value from the buttonPin
unsigned long timeNow = 0;
unsigned long oneSecond = 1000UL;
unsigned long startTime = 0;
int secNow = 0;
int displayMin = 0;
int displaySec = 0;

//Temp variables:
float temp;


//De-bouncing variables:
int buttonState = LOW;
int buttonState2 = LOW;
int lastButtonState = LOW;
int lastButtonState2 = LOW;
int relayState = LOW;
int relayState2 = LOW;
long lastDebounceTime = 0;
long lastDebounceTime2 = 0;
long debounceDelay = 50;

void setup() {

  Serial.begin(9600);// setup Serial Monitor to display information
  pinMode(relay1, OUTPUT);// connected to Relay 1
  pinMode(relay2, OUTPUT);// connected to Relay 2
  pinMode(relay3, OUTPUT);// connected to Relay 3
  pinMode(relay4, OUTPUT);// connected to Relay 4
  digitalWrite(relay1, LOW);
  digitalWrite(relay2, LOW);
  digitalWrite(relay3, LOW);
  digitalWrite(relay4, LOW);

  lcd.begin(16, 2); // Initializes the interface to the LCD screen, and specifies the dimensions (width and height) of the display }
  lcd.clear();
  
  pinMode(buttonPin, INPUT);
  pinMode(buttonPin2, INPUT);// analog pin, switch
 
  

}

void loop() {
  
  int reading = digitalRead(buttonPin);//Return HIGH if enabled, otherwise LOW

  if (reading !=lastButtonState){
    lastDebounceTime = millis();
  }

  if((millis() - lastDebounceTime) > debounceDelay){
   
    if (reading != buttonState){
      buttonState = reading;
      
      if (buttonState == HIGH){
        relayState = !relayState;
      }
    }
    
  }
  digitalWrite(relay1, relayState);
  lastButtonState = reading;
  

  int reading2 = digitalRead(buttonPin2);
  if (reading2 != lastButtonState2){
    lastDebounceTime2 = millis();
  }
  if((millis() - lastDebounceTime2) > debounceDelay){
    if (reading2 != buttonState2){
      buttonState2 = reading2;
      
      if ((reading2 == HIGH)){    //not sure why but when analog button is pushed, it goes from HIGH to LOW
        digitalWrite(relay2, HIGH);
        digitalWrite(relay3, HIGH);
      
        secNow = secNow + 30;
      }
    }  
  }
  lastButtonState2 =reading2;
  displayTemp();
  temp = getTemp();
  if ((millis() - startTime >= oneSecond) && (temp > 100))
    {
      if (secNow == 0 ){
        digitalWrite(relay1, LOW);
        digitalWrite(relay2, LOW);
        digitalWrite(relay3, LOW);
        digitalWrite(relay4, LOW);
      }
      displayCountDown(secNow);
      if(secNow!=0)
         secNow--;
      startTime += oneSecond;
    }
    
}

float getTemp(){
  float temp1;
  temp1 = analogRead(A0); //read the temp from A0
  temp1= temp1 *9 / 5;
  temp1 = temp1 + 32;
  return temp1;
}

void displayTemp(){
  float temp2 = getTemp();
  lcd.setCursor(0,2);
  lcd.print("TEMP: ");
  lcd.print(temp2);
  lcd.print(" F");
}

void displayCountDown(int sec){
  lcd.clear();
  int displayMin = sec/60;
  if (displayMin < 10) lcd.print("0");
  lcd.print(displayMin);
  lcd.print(":");
  int displaySec = sec % 60;
  if (displaySec < 10) lcd.print("0");
  lcd.print(sec % 60);
}
