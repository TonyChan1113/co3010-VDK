/*
 * input_reading.c
 *
 *  Created on: Sep 30, 2021
 *      Author: Admin
 */

#include "input_reading.h"

GPIO_TypeDef *portBuffer[NUM_OF_BUTTONS];
uint16_t pinBuffer[NUM_OF_BUTTONS];

GPIO_PinState thisButton[NUM_OF_BUTTONS];
GPIO_PinState lastButton[NUM_OF_BUTTONS];

int buttonCounter[NUM_OF_BUTTONS];
int buttonCounter_500ms = 0;		// for button 2

//	Setup Port and Pin of buttons
//////////////////////////////////////
void set_portBuffer(void){
	portBuffer[0] = GPIOB;
	portBuffer[1] = GPIOB;
	portBuffer[2] = GPIOB;
}
void set_pinBuffer(void){
	pinBuffer[0] = GPIO_PIN_1;
	pinBuffer[1] = GPIO_PIN_2;
	pinBuffer[2] = GPIO_PIN_3;
}
void readButton_preprocess(void){
	set_portBuffer();
	set_pinBuffer();

	for(int i=0; i<NUM_OF_BUTTONS; i++){
		buttonBuffer[i] = RELEASED;
		thisButton[i] = RELEASED;
		lastButton[i] = RELEASED;

		buttonCounter[i] = 0;
	}
}


void readButton(void){
    for (int i = 0; i < NUM_OF_BUTTONS; i++){
        lastButton[i] = thisButton[i];
        thisButton[i] = HAL_GPIO_ReadPin(portBuffer[i], pinBuffer[i]);

        if(thisButton[i] == lastButton[i]){     // it is not a debounce
            switch (buttonBuffer[i]){
            case RELEASED:
                buttonBuffer[i] = thisButton[i];
                break;

            case PRESSED:
                if(thisButton[i] == PRESSED){
                	// Todo when button is pressed
                	if(i==0 && buttonCounter[0] == 0){
                		flag_reset = 1;
                		value = 1;
                		mode ++;			// first button
                	}
                	else if(i == 1 && buttonCounter[1] == 0) value ++;	// second counter
                	else if(i == 2 && buttonCounter[2] == 0) flag_set = 1;

                	// Button reading
                    if(buttonCounter[i] >= COUNT_1S){
                        buttonCounter[i] = 0;
                        buttonBuffer[i] = HOLDED;
                        break;
                    }
                    buttonCounter[i]++;
                }
                else if(thisButton[i] == RELEASED) {
                	buttonCounter[i] = 0;
                	buttonBuffer[i] = RELEASED;
                }
                break;

            case HOLDED:
            	// Todo when button is holded
            	if(i == 1){		// for button 2
                	if(buttonCounter_500ms >= COUNT_500MS){
                		buttonCounter_500ms = 0;
                		value ++;
                	}
                	buttonCounter_500ms ++;

                	if(thisButton[i] == RELEASED) {
                		buttonCounter_500ms = 0;
                		buttonBuffer[i] = RELEASED;
                	}
                	break;
            	}

            	// other button
                if(thisButton[i] == RELEASED) buttonBuffer[i] = RELEASED;
                break;

            default:
                break;
            }
        }
    }
}

int getButton(int index){
	if(index >= NUM_OF_BUTTONS) return -1;
	else return buttonBuffer[index];
}
