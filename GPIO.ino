
// Switch Pins
#define C_UP_PIN 17 // Color Up
#define C_DN_PIN 18 // Color Down
#define M_F_PIN  22 // Mode Forward  (right)
#define M_B_PIN  19 // Mode Backward (left)

#define DEBOUNCE_MAX_VAL 0x9FF

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
    firstPass = false;                // previousState will be defined, firstPass is false.
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


void GPIO_debounce()
{
  static uint32_t deb_counter = 0;
  static uint32_t reg_counter = 0;
  static uint8_t button_val = 0;
  static uint8_t button_reg_val = 0;

  debugStatePrint();
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
        button_val =  (!digitalRead(C_UP_PIN) << 3) |
                      (!digitalRead(C_DN_PIN) << 2) |
                      (!digitalRead(M_F_PIN)  << 1) |
                      (!digitalRead(M_B_PIN));
        button_reg_val = button_val;
        Serial.println("-----------buttonVal--------------");
        Serial.println(button_val, HEX);
        Serial.println("");
        deb_state = register_st;
      }
      break;

    case register_st:
      reg_counter++;
      //Serial.println(reg_counter, HEX);
      button_val =  (!digitalRead(C_UP_PIN) << 3) |
                    (!digitalRead(C_DN_PIN) << 2) |
                    (!digitalRead(M_F_PIN)  << 1) |
                    (!digitalRead(M_B_PIN));
      if (reg_counter > 0x2FFFF) {
        //TODO implement event when holding button prolonged
        reg_counter = 0;
        LED_state = off_st;
        deb_state = held_st;
      }
      else if (!button_val) {
        // button Logic
        if (button_reg_val & 0x08) {
          // button UP
          LED_state = (LED_state + 1) % NUM_STATES;
        }
        else if (button_reg_val & 0x04) {
          // button DOWN
          LED_state = (LED_state == 0) ? (NUM_STATES - 1) : (LED_state - 1);
        }
        else if (button_reg_val & 0x02) {
          // button LEFT
        }
        else {
          // Switch RIGHT

        }
        deb_state = wrap_up_st;
      }
      break;

    case held_st:
      button_val =  (!digitalRead(C_UP_PIN) << 3) |
                    (!digitalRead(C_DN_PIN) << 2) |
                    (!digitalRead(M_F_PIN)  << 1) |
                    (!digitalRead(M_B_PIN));
      if (!button_val) {
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
}

// ISR located in here because interrupt on button change
void ISR() {
  isr_flag = 0;
}
