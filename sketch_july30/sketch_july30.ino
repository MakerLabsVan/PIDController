#include <LiquidCrystal.h> // includes the LiquidCrystal Library 

#define relayA 7
#define relayB 6
#define relayC 5
#define relayD 4

//Note: https://seeeddoc.github.io/Relay_Shield_V2.0/ , Pins 4-7 are taken by relay function and cannot be overloaded by other pins. 

#define MAX_ONBUTTON_TEMP    140
#define ONBUTTON_BACKOPEN_TEMP    135
#define MAX_ADDTIMEBUTTON_TEMP    145
#define TEMP_STARTCOUNT  140


/* Parts: buttons (x3), Relays (x3), Temp Sensor(x1), LCD(x1), potentiometer(x1)
 * 
 * Description: 
 * Upon startup, the LCD will display the temperature and the countdown time(00:00). If OnButton is pushed once, the AddTimeButton 
 * will be enabled for input and if the temperature is also between 0 – 259F, then solenoid A will turn on. If the temperature is either lower than 0F or higher than 
 * 259F, then AddTimeButton will be enabled but solenoid A will not turn on. If OnButton is pushed for a second time, then AddTimeButton will be disabled and 
 * solenoid A, B and C will all turn off. Therefore, every time the onButton is pushed, the opposite state will occur (either from OFF to ON or ON to OFF). Additionally, 
 * if AddTimeButton is pushed before OnButton, then nothing will happen.I f either the onButton or AddTimeButton are pressed while the temperature is out of the desired range 
 * (0-259), then all button presses will be ignored.
 * 
 * 
 * If AddTimeButton is pushed once, then 15 minutes is added to the countdown timer and if temperature is also between 0-250F, then solenoids B and C will turn on. 
 * Each additional push to AddTimeButton will add 15 minutes to the countdown timer but will have no effect on solenoids B and C. If the temperature reaches 250F, 
 * the countdown timer begins counting down. When the countdown timer reaches zero, solenoids A, B and C all turn off. If temperature reaches above or below the temperature range
 * of either solenoids A, B or C, the countdown timer will still be active. The countdown timer will only stop when onButton is pressed or when the timer reaches zero.
 */

LiquidCrystal lcd(9, 3, 19, 18, 17, 16); // Creates an LC object. Parameters: (rs, enable, d4, d5, d6, d7)

//Pins:
int onButtonPin = 2;   //pin for button 1 (Digital)
int addTimeButtonPin = 8;   //pin for button 2 

//Time variables:
int period = 1000;
int val = 0; //value from the onButtonPin
unsigned long timeNow = 0;
unsigned long oneSecond = 1000UL;
unsigned long startTime = 0;
int secNow = 0;
int secLoop = 0;
int displayMin = 0;
int displaySec = 0;
long addTimeButtonShutOffTime = 0;


//Temp variables:
float temp;


//De-bouncing variables:
int onButtonState = LOW;
int addTimeButtonState = LOW;
int lastOnButtonState = LOW;
int lastAddTimeButtonState = LOW;
int relayStateA = LOW;
int relayStateBC = LOW;
bool relayState = LOW;
int onButtonFlag = 0;
int addTimeButtonFlag = 0;
int addTimeButtonCutOffFlag = 0;



long onButtonLastDebounceTime = 0;
long addTimeButtonLastDebounceTime = 0;
long lastDisplayTime = 0;
long debounceDelay = 50;
long shutOffTime = 0;

int onButtonReading = LOW;
int addTimeButtonReading = HIGH;
int onButtonCutOffFlag = 0;

int timerFlag = 0;
int countDownFlag = 0;
float onButtonTemp = 0;
long debug; //make sure to delete

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);// setup Serial Monitor to display information
  pinMode(relayA, OUTPUT);// connected to Relay 1
  pinMode(relayB, OUTPUT);// connected to Relay 2
  pinMode(relayC, OUTPUT);// connected to Relay 3
  pinMode(relayD, OUTPUT);
  
  digitalWrite(relayA, LOW);
  digitalWrite(relayB, LOW);
  digitalWrite(relayC, LOW);
  digitalWrite(relayD, LOW);
  

  lcd.begin(16, 2); // Initializes the interface to the LCD screen, and specifies the dimensions (width and height) of the display }
  lcd.clear();
  
  pinMode(onButtonPin, INPUT_PULLUP);
  pinMode(addTimeButtonPin, INPUT_PULLUP);

  shutOffTime = millis(); //for autoshutdown when >140

}
//////////////////////////////- Main Loop -/////////////////////////////////////////

void loop() 
{


  onButtonDebounceFunc(onButtonPin, relayD);
  addTimeButtonDebounceFunc(addTimeButtonPin, relayB, relayC);
  displayTempFunc();
  displayCountDownFunc(secNow);
  countDownMechanismFunc();   //Working as supposed to

  
  //Serial.println(relayState);

  
}

/////////////////////////////-functions start-/////////////////////////////////////
  

float getTempFunc(){      //function for converting analog signal to Fahrenheit
  float temp1;
  temp1 = analogRead(A0); //read the temp from A0
  temp1= temp1 *9 / 5;
  temp1 = temp1 + 32;
  return temp1;
}

void displayTempFunc(){     //Function for displaying temperature
  lcd.setCursor(0,2);
  lcd.print("TEMP: ");
  float temp2 = getTempFunc();
  if ((millis() - lastDisplayTime) > 500){
    lcd.print(temp2);
    lcd.print(" F");
    lastDisplayTime = millis();
  }
}
void displayCountDownFunc(int sec){
  lcd.setCursor(0,0);
  int displayMin = sec/60;
  if (displayMin < 10) lcd.print("0");
  lcd.print(displayMin);
  lcd.print(":");
  int displaySec = sec % 60;
  if (displaySec < 10) lcd.print("0");
  lcd.print(sec % 60);
}

void countDownMechanismFunc(){
 if ((millis() - startTime >= oneSecond))
  {
    if (onButtonFlag == 0){
      secNow = 0 ;
    }
    if ((secNow == 0) && (addTimeButtonFlag == 1)){
      relayState = LOW;
      digitalWrite(relayD, relayState);
      digitalWrite(relayB, LOW);
      digitalWrite(relayC, LOW);
      timerFlag = 1;
      addTimeButtonFlag = 0;
      
    }
   if ((secNow != 0) || (addTimeButtonFlag != 1) || (onButtonFlag != 0)){
      timerFlag = 0;
    }
    
    if(getTempFunc() >= TEMP_STARTCOUNT){
      countDownFlag = 1;
    }
    
    if(((secNow!=0) && countDownFlag == 1)){        ///changed to stop turning off when temperature is below 140//
      secNow--;
    }
    
    if(secNow == 0){
      countDownFlag = 0;
    }
   
    startTime += oneSecond;
  }
}
