void addTimeButtonDebounceFunc(int ButtonPin, int relayPin1, int relayPin2){      //Function for debouncing Add Time Button and controlling relayB and C.
 addTimeButtonReading = digitalRead(ButtonPin);


if (((onButtonTemp >= MAX_ADDTIMEBUTTON_TEMP)&& (addTimeButtonShutOffTime <= millis())) || ((onButtonTemp < 0.0) && (addTimeButtonShutOffTime <= millis())) ){    //Changed to take a sample size and stop the oscillation//
  digitalWrite(relayPin1, LOW);
  digitalWrite(relayPin2, LOW);
  Serial.println("***********");
  addTimeButtonShutOffTime = millis()+500;
  addTimeButtonCutOffFlag = 1;
  }

 if (addTimeButtonReading != lastAddTimeButtonState){
    addTimeButtonLastDebounceTime = millis();
  }
 if(((millis() - addTimeButtonLastDebounceTime) > debounceDelay) && (relayState == HIGH) && (onButtonFlag == 1)){
  
    if ((addTimeButtonReading  == LOW)){    //button is pushed, it goes from HIGH to LOW
      
      digitalWrite(relayPin1, HIGH);
      digitalWrite(relayPin2, HIGH);
      delay(400);   //This is here because the debouncing the add-time button is not working correctly
      addTimeButtonFlag = 1;
      
      secNow = secNow + 5;
    }
}
 if ((addTimeButtonFlag == 1) && (getTempFunc() < 132)){
   digitalWrite(relayPin1, HIGH);
   digitalWrite(relayPin2, HIGH);
 }
 if (onButtonFlag == 0){      //turns off all relay when relayA is turned off
    digitalWrite(relayB, LOW);
    digitalWrite(relayC, LOW);
    addTimeButtonFlag = 0;
  }
  lastAddTimeButtonState = addTimeButtonReading;
}
