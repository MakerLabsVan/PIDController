int onButtonDebounceFunc(int buttonPin, int relayPin){      //Function for debouncing the ON Button and controlling RelayA

  
  onButtonTemp = getTempFunc();
  onButtonReading = digitalRead(buttonPin);


  if (onButtonReading !=lastOnButtonState){
    onButtonLastDebounceTime = millis();
  }

  Serial.println(onButtonTemp);
  
  if (((onButtonTemp >= MAX_ONBUTTON_TEMP)&& (shutOffTime <= millis())) || ((onButtonTemp < 0.0) && (shutOffTime <= millis())) ){    //Changed to take a sample size and stop the oscillation//
    digitalWrite(relayPin,LOW);
    Serial.println("***********");
    shutOffTime = millis()+500;
    onButtonCutOffFlag = 1;

  }
  
  
  if((millis() - onButtonLastDebounceTime) > debounceDelay){
 
    if (onButtonReading != onButtonState){

      onButtonState = onButtonReading;

      if ((onButtonState == LOW)){

        relayState = !relayState;
 
        if ((relayState == HIGH)){
          onButtonFlag = 1;
        }
        else{
          onButtonFlag = 0;
        }
      
      }
    }
  if(onButtonCutOffFlag != 1){      //  turn on the relay only if the temperature is below the maximum operating temperature.
    digitalWrite(relayPin,relayState);
  }
 }
 
  lastOnButtonState = onButtonReading;
  if(getTempFunc() < 135){        //to prevent on/off spam of relay, turn back on only when the temperature cools to 135.
    onButtonCutOffFlag = 0;
  }
  
  return relayState;
}
