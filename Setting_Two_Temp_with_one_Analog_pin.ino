#include <LiquidCrystal_I2C.h>

// Define button pins and constants
#define INCREASE_BUTTON 2
#define CONFIRM_BUTTON 3
#define DECREASE_BUTTON 4
#define BUTTON A1

const int MAX_TEMP = 100;
const int MIN_TEMP = 0;
const unsigned long debounceDelay = 100;

int setTemp1 = 0;
int setTemp2 = 0;
int lastTemp;
int temp;

// State enumeration for different menu screens
enum State {
  SELECT_TEMP,
  SETTING_TEMP,
  CONFIRMING_TEMP,
  SUCCESS,
  FAILURE,
  DISPLAY_TEMP
};
State currentState = SELECT_TEMP;

// Initialize the LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Button state variables
int ButtonState = LOW;
int lastButtonState = LOW;
unsigned long lastDebounceTime = 0;

//To confirm Yes or No
int menuYes = 0;
int menuNo = 0;
int menuOk = 0;

//To select the temperature to set.
int menuTemp1 = 0;
int menuTemp2 = 0;
int menuOkTemp = 0;


unsigned long myDelay = 0;

void setup() {
  Serial.begin(9600);
  //  pinMode(INCREASE_BUTTON, INPUT);
  //  pinMode(CONFIRM_BUTTON, INPUT);
  //  pinMode(DECREASE_BUTTON, INPUT);
  pinMode(BUTTON, INPUT);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");
  delay(2000);
  lcd.clear();
}

// Function to handle debouncing for a specific button
int debounce(int buttonPin, int &lastButtonState, int &buttonState, unsigned long &lastDebounceTime) {
  int reading = analogRead(buttonPin);

     
  // If the button state has changed, reset the debouncing timer
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
    //        Serial.println("DT update");
  }

  // Check if the debounce delay has passed and the state is stable
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // Only return true on a transition from LOW to HIGH (button press)
    //        Serial.println("200ms passed");
    if (reading != buttonState) {
      //            buttonState = reading;
      Serial.println("Button Updated");
      return reading;
    }

  }

  // Update the last button state
  lastButtonState = reading;
  return 0;
}

// Function to handle temperature setting screen
void handleTemperatureSetting(int buttonPressed, int &setTempX) {

  lcd.setCursor(0, 0);
  lcd.print("Set Temp");
  lcd.print(temp);
  lcd.print(":");
  //Serial.println("Set Temp:");
  lcd.setCursor(0, 1);
  lcd.print(setTempX);
  //    Serial.println(setTemp);

  if (buttonPressed == INCREASE_BUTTON && setTempX < MAX_TEMP) {
    setTempX += 5;
    lcd.setCursor(0, 1);
    lcd.print("       "); // Clear previous value
    lcd.setCursor(0, 1);
    lcd.print(setTempX);
    //        Serial.println(setTemp);
  } else if (buttonPressed == DECREASE_BUTTON && setTempX > MIN_TEMP) {
    setTempX -= 5;
    lcd.setCursor(0, 1);
    lcd.print("       "); // Clear previous value
    lcd.setCursor(0, 1);
    lcd.print(setTempX);
    //        Serial.println(setTemp);
  } else if (buttonPressed == CONFIRM_BUTTON && setTempX != 0) {
    currentState = CONFIRMING_TEMP;
    lcd.clear();
  }
}

// Function to handle confirmation screen
void handleConfirmation(int buttonPressed, int setTempX) {
  if (menuYes == 0 && menuNo == 0) {
    lcd.setCursor(0, 0);
    lcd.print("Confirm Temp?");
    //    Serial.println("Confirm Temp?");
    lcd.setCursor(0, 1);
    lcd.print(setTempX);
    //    Serial.println(setTemp);
    lcd.setCursor(6, 1);
    lcd.print(" Yes  No");
    //    Serial.println("Yes No");
    menuOk = 0;
  }
  if (buttonPressed == INCREASE_BUTTON) {
    menuYes = 1;
    menuNo = 0;
  }
  else if (buttonPressed == DECREASE_BUTTON) {
    menuNo = 1;
    menuYes = 0;
  }
  else if (buttonPressed == CONFIRM_BUTTON) menuOk = 1;

  if (menuYes == 1) {
    lcd.setCursor(6, 1);
    lcd.print(">Yes  No");
    //        Serial.println(">Yes No");
    if (menuYes == 1 && menuOk == 1) {
      currentState = SUCCESS;
      menuYes = 0; menuOk == 0;
    }
  }
  else if (menuNo == 1) {
    lcd.setCursor(6, 1);
    lcd.print(" Yes >No");
    //        Serial.println("Yes >No");
    if (menuNo == 1 && menuOk == 1) {
      currentState = FAILURE;
      menuNo = 0; menuOk == 0;
    }
  }
  //    else if (buttonPressed == CONFIRM_BUTTON) {
  //      if (digitalRead(INCREASE_BUTTON) == HIGH) {  // Confirm yes
  //        currentState = SUCCESS;
  //      } else if (digitalRead(DECREASE_BUTTON) == HIGH) {  // Confirm no
  //        currentState = FAILURE;
  //      }
  //    }
}

// Function to show success screen
void showSuccessScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp Set Success.");
  //    Serial.println("Success");
  delay(2000);
  lcd.clear();
  //  lastTemp = setTemp;

}

// Function to show failure screen
void showFailureScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp Not Set.");
  //    Serial.println("Not Set");
  delay(2000);
  lcd.clear();
}

// Function to get button press with debounce handling
int getButtonPress() {
  int ButtonValue = debounce(BUTTON, lastButtonState, ButtonState, lastDebounceTime);
  Serial.print("Button Value: ");
  Serial.println(ButtonValue);
  if (ButtonValue >= 85 && ButtonValue <= 100) {
    return INCREASE_BUTTON;
  } else if (ButtonValue >= 335 && ButtonValue <= 350) {
    return CONFIRM_BUTTON;
  } else if (ButtonValue >= 505 && ButtonValue <= 520) {
    return DECREASE_BUTTON;
  }
  return -1;  // No button pressed
}

void selectTemp(int buttonPressed) {
  if (menuTemp1 == 0 && menuTemp2 == 0) {
    lcd.setCursor(0, 0);
    lcd.print("Select Temp:");
    //    Serial.println("Confirm Temp?");
    lcd.setCursor(0, 1);
    lcd.print(" Temp1     Temp2");
    //    Serial.println("Yes No");
    menuOkTemp = 0;
  }
  if (buttonPressed == INCREASE_BUTTON) {
    menuTemp1 = 1;
    menuTemp2 = 0;
  }
  else if (buttonPressed == DECREASE_BUTTON) {
    menuTemp2 = 1;
    menuTemp1 = 0;
  }
  else if (buttonPressed == CONFIRM_BUTTON) menuOkTemp = 1;

  if (menuTemp1 == 1) {
    lcd.setCursor(0, 1);
    lcd.print(">Temp1     Temp2");
    //        Serial.println(">Yes No");
    if (menuTemp1 == 1 && menuOkTemp == 1) {
      currentState = SETTING_TEMP;
      temp = 1;
      menuTemp1 = 0; menuOkTemp == 0;
      lcd.clear();
    }
  }
  else if (menuTemp2 == 1) {
    lcd.setCursor(0, 1);
    lcd.print(" Temp1    >Temp2");
    //        Serial.println("Yes >No");
    if (menuTemp2 == 1 && menuOkTemp == 1) {
      currentState = SETTING_TEMP;
      temp = 2;
      menuTemp2 = 0; menuOkTemp == 0;
      lcd.clear();
    }
  }
}


void displayTemp (int buttonPressed){
  lcd.setCursor(0,0);
  lcd.print("Temp 1 is: ");
  lcd.print(setTemp1);
  lcd.print(" oC");
  lcd.setCursor(0,1);
  lcd.print("Temp 2 is: ");
  lcd.print(setTemp2);
  lcd.print(" oC");

  if (buttonPressed == CONFIRM_BUTTON){
    currentState = SELECT_TEMP;
  }
}

void loop() {
  int buttonPressed = getButtonPress();

  switch (currentState) {
    case SELECT_TEMP:
      selectTemp(buttonPressed);
      break;
    case SETTING_TEMP:
      if (temp == 1)handleTemperatureSetting(buttonPressed, setTemp1);
      else if (temp == 2)handleTemperatureSetting(buttonPressed, setTemp2);
      break;
    case CONFIRMING_TEMP:
      if (temp == 1)handleConfirmation(buttonPressed, setTemp1);
      else if (temp == 2) handleConfirmation(buttonPressed, setTemp2);
      break;
    case SUCCESS:
      showSuccessScreen();
      currentState = DISPLAY_TEMP;
      break;
    case FAILURE:
      showFailureScreen();
      currentState = SETTING_TEMP;
      break;
    case DISPLAY_TEMP:
      displayTemp(buttonPressed);
      }
}
