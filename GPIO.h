#ifndef GPIO_H
#define GPIO_H

// Switch Pins
#define C_UP_PIN 17 // Color Up
#define C_DN_PIN 18 // Color Down
#define M_F_PIN  22 // Mode Forward  (right)
#define M_B_PIN  19 // Mode Backward (left)

// The possible values for the button presses. The buttons are mapped
// to the pins above.
typedef uint8_t buttonVal_t;
#define BUTTON_UP    0x08
#define BUTTON_DOWN  0x04
#define BUTTON_LEFT  0x02
#define BUTTON_RIGHT 0x01

buttonVal_t GPIO_debounce(uint8_t* wasHeld);

#endif /* GPIO_H */
