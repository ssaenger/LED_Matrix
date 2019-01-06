#include "GPIO.h"

// GPIO_debounce counter values
#define DEBOUNCE_MAX_VAL 0xABC   // Higher val = longer to register button press
#define HOLD_TIME_VAL    0x3ABCD // higher val = longer hold time to register

enum deb_states { wait_st,
                  debounce_st,
                  register_st,
                  held_st,
                  wrap_up_st
};
deb_states deb_state;



void GPIO_init()
{
  // Pins for the switches
  pinMode(C_UP_PIN, INPUT_PULLUP);
  pinMode(C_DN_PIN, INPUT_PULLUP);
  pinMode(M_F_PIN,  INPUT_PULLUP);
  pinMode(M_B_PIN,  INPUT_PULLUP);

  // Interrupt pins
  attachInterrupt(digitalPinToInterrupt(C_UP_PIN), ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(C_DN_PIN), ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(M_F_PIN),  ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(M_B_PIN),  ISR, FALLING);

  deb_state = wait_st;
}

void debugStatePrint() {
  static deb_states previousState;
  static bool firstPass = true;
  // Only print the message if:
  // 1. This the first pass and the value for previousState is unknown.
  // 2. previousState != currentState - this prevents reprinting the same state name over and over.
  if (previousState != deb_state || firstPass) {
    firstPass = false;             // previousState will be defined, firstPass is false.
    previousState = deb_state;     // keep track of the last state that you were in.
    switch(deb_state) {            // This prints messages based upon the state that you were in.
      case wait_st:
        Serial.println("wait_st");
        break;
      case debounce_st:
        Serial.println("debounce_st");
        break;
      case register_st:
        Serial.println("register_st");
        break;
      case held_st:
        Serial.println("held_st");
        break;
      case wrap_up_st:
        Serial.println("wrap_up_str");
        break;
     }
  }
}

// Debounces the buttons and performs the wanted actions
// This returns the value of the button press. wasHeld holds
// if this was held or not.
buttonVal_t GPIO_debounce(uint8_t* wasHeld)
{
  static uint32_t deb_counter = 0;
  static uint32_t reg_counter = 0;
  static buttonVal_t currButton_val   = 0;
  static buttonVal_t registeredButton_val = 0;
  uint8_t success = 0; // Assume don't know what button value is.

  switch(deb_state) {
    case wait_st:
    if (!isr_flag){
      deb_state = debounce_st;
    }
      break;

    case debounce_st:
      deb_counter++;
      if (deb_counter > DEBOUNCE_MAX_VAL) {
        deb_counter = 0;
        currButton_val =  (!digitalRead(C_UP_PIN) << 3) |
                          (!digitalRead(C_DN_PIN) << 2) |
                          (!digitalRead(M_F_PIN)  << 1) |
                          (!digitalRead(M_B_PIN));
        registeredButton_val = currButton_val; // We have found our button value
        Serial.println("-----------buttonVal--------------");
        Serial.println(currButton_val, HEX);
        Serial.println("");
        deb_state = register_st;
      }
      break;

    case register_st:
      reg_counter++;
      currButton_val =  (!digitalRead(C_UP_PIN) << 3) |
                        (!digitalRead(C_DN_PIN) << 2) |
                        (!digitalRead(M_F_PIN)  << 1) |
                        (!digitalRead(M_B_PIN));
      if (reg_counter > HOLD_TIME_VAL) {
        // We've held the buttons for a determined amount of time.
        reg_counter = 0;
        success = 1; // Indicate success in determining action to take

        // Update button state
        *wasHeld = 1;
        deb_state = held_st;
      }
      else if (!currButton_val) {
         // We've let go of the buttons
        reg_counter = 0;
        success = 1; // Indicate success in determining action to take
        *wasHeld = 0;
        deb_state = wrap_up_st;
      }
      else {
        // Not needed here, since default value is 0, but indicate
        // don't know what to do yet.
        success = 0;
        *wasHeld = 0;
      }
      break;

    case held_st:
      currButton_val =  (!digitalRead(C_UP_PIN) << 3) |
                        (!digitalRead(C_DN_PIN) << 2) |
                        (!digitalRead(M_F_PIN)  << 1) |
                        (!digitalRead(M_B_PIN));
      if (!currButton_val) {
        deb_state = wrap_up_st;
      }
      break;

    case wrap_up_st:
      deb_counter++;
      if (deb_counter > DEBOUNCE_MAX_VAL) {
        deb_counter = 0;
        noInterrupts();
        isr_flag = 1;
        interrupts();
        deb_state = wait_st;
      }
      break;
  }
  debugStatePrint();

  if (success) {
    return registeredButton_val;
  }
  return 0;
}

// ISR located in here because interrupt on button change
void ISR() {
  isr_flag = 0;
}
