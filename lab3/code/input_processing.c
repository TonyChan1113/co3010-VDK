/*
 * input_processing.c
 *
 *  Created on: 5 thg 10, 2021
 *      Author: Admin
 */

#include "input_processing.h"

#define RED_LIGHT		2	// default 10s
#define GREEN_LIGHT		3	// default 6s
#define YELLOW_LIGHT	4	// default 4s

int vertical_state;
int vertical_counter;

int horizontal_state;
int horizontal_counter;

void display7SEG (int num, GPIO_TypeDef* type, uint16_t A, uint16_t B, uint16_t C, uint16_t D){
	switch(num){
	case -1: // turn off seg led
		HAL_GPIO_WritePin(type, D|C|B|A, SET);
		break;
	case 0:
		HAL_GPIO_WritePin(type, D|C|B|A, RESET);
		break;
	case 1:
		HAL_GPIO_WritePin(type, D|C|B, RESET);
		HAL_GPIO_WritePin(type, A, SET);
		break;
	case 2:
		HAL_GPIO_WritePin(type, D|C|A, RESET);
		HAL_GPIO_WritePin(type, B, SET);
		break;
	case 3:
		HAL_GPIO_WritePin(type, D|C, RESET);
		HAL_GPIO_WritePin(type, B|A, SET);
		break;
	case 4:
		HAL_GPIO_WritePin(type, D|B|A, RESET);
		HAL_GPIO_WritePin(type, C, SET);
		break;
	case 5:
		HAL_GPIO_WritePin(type, D|B, RESET);
		HAL_GPIO_WritePin(type, C|A, SET);
		break;
	case 6:
		HAL_GPIO_WritePin(type, D|A, RESET);
		HAL_GPIO_WritePin(type, C|B, SET);
		break;
	case 7:
		HAL_GPIO_WritePin(type, D, RESET);
		HAL_GPIO_WritePin(type, C|B|A, SET);
		break;
	case 8:
		HAL_GPIO_WritePin(type, C|B|A, RESET);
		HAL_GPIO_WritePin(type, D, SET);
		break;
	case 9:
		HAL_GPIO_WritePin(type, C|B, RESET);
		HAL_GPIO_WritePin(type, D|A, SET);
		break;
	default:
		HAL_GPIO_WritePin(type, D|C|B|A, SET);
		break;
	}
}
void traffic_func(void){
	vertical_counter--;
	int vertical_counter_high = vertical_counter / 10;
	int vertical_counter_low  = vertical_counter % 10;
	switch(vertical_state){
	case RED_LIGHT:
		display7SEG(vertical_counter_high, GPIOA, GPIO_PIN_0, GPIO_PIN_1,
				 GPIO_PIN_2, GPIO_PIN_3);
		display7SEG(vertical_counter_low, GPIOA, GPIO_PIN_4, GPIO_PIN_5,
				 GPIO_PIN_6, GPIO_PIN_7);

		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, RESET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9|GPIO_PIN_10, SET);

		if(vertical_counter <= 0) {
			vertical_counter = green_time;
			vertical_state = GREEN_LIGHT;
		}
		break;

	case GREEN_LIGHT:
		display7SEG(vertical_counter_high, GPIOA, GPIO_PIN_0, GPIO_PIN_1,
				 GPIO_PIN_2, GPIO_PIN_3);
		display7SEG(vertical_counter_low, GPIOA, GPIO_PIN_4, GPIO_PIN_5,
				 GPIO_PIN_6, GPIO_PIN_7);

		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, RESET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8|GPIO_PIN_10, SET);

		if(vertical_counter <= 0) {
			vertical_counter = yellow_time;
			vertical_state = YELLOW_LIGHT;
		}
		break;

	case YELLOW_LIGHT:
		display7SEG(vertical_counter_high, GPIOA, GPIO_PIN_0, GPIO_PIN_1,
				 GPIO_PIN_2, GPIO_PIN_3);
		display7SEG(vertical_counter_low, GPIOA, GPIO_PIN_4, GPIO_PIN_5,
				 GPIO_PIN_6, GPIO_PIN_7);

		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, RESET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8|GPIO_PIN_9, SET);

		if(vertical_counter <= 0) {
			vertical_counter = red_time;
			vertical_state = RED_LIGHT;
		}
		break;

	default:
		break;
	}


	horizontal_counter--;
	int horizontal_counter_high = horizontal_counter / 10;
	int horizontal_counter_low  = horizontal_counter % 10;
	switch(horizontal_state){
		case RED_LIGHT:
			display7SEG(horizontal_counter_high, GPIOA, GPIO_PIN_8,
					GPIO_PIN_9,	GPIO_PIN_10, GPIO_PIN_11);
			display7SEG(horizontal_counter_low, GPIOA, GPIO_PIN_12,
					GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15);

			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, RESET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12|GPIO_PIN_13, SET);

			if(horizontal_counter <= 0) {
				horizontal_counter = green_time;
				horizontal_state = GREEN_LIGHT;
			}
			break;

		case GREEN_LIGHT:
			display7SEG(horizontal_counter_high, GPIOA, GPIO_PIN_8,
					GPIO_PIN_9,	GPIO_PIN_10, GPIO_PIN_11);
			display7SEG(horizontal_counter_low, GPIOA, GPIO_PIN_12,
					GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15);

			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, RESET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11|GPIO_PIN_13, SET);

			if(horizontal_counter <= 0) {
				horizontal_counter = yellow_time;
				horizontal_state = YELLOW_LIGHT;
			}
			break;

		case YELLOW_LIGHT:
			display7SEG(horizontal_counter_high, GPIOA, GPIO_PIN_8,
					GPIO_PIN_9,	GPIO_PIN_10, GPIO_PIN_11);
			display7SEG(horizontal_counter_low, GPIOA, GPIO_PIN_12,
					GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15);

			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, RESET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11|GPIO_PIN_12, SET);

			if(horizontal_counter <= 0) {
				horizontal_counter = red_time;
				horizontal_state = RED_LIGHT;
			}
			break;

		default:
			break;
		}
}

//  reset all pin (GPIO_PIN_SET)
///////////////////////////////////
void resetPin(void){
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|
			GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|
			GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|
			GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, SET);

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|
			GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13, SET);
}



int value_high;
int value_low;
void fsm_input_processing(void){
	// show mode on 7SEG
	display7SEG(mode, GPIOB, GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7);

	switch(mode){
	case 1:		// traffic light
		if(flag_1s) {	//call every 1s
			traffic_func();
			flag_1s = 0;
		}
		break;
	case 2:		// modify red time
		// show value on 7SEG
		value_high = value/10;
		value_low = value%10;
		display7SEG(value_high, GPIOA, GPIO_PIN_8,
				GPIO_PIN_9,	GPIO_PIN_10, GPIO_PIN_11);
		display7SEG(value_low, GPIOA, GPIO_PIN_12,
				GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15);

		// show red time on 7SEG
		int red_time_high = red_time/10;
		int red_time_low  = red_time%10;
		display7SEG(red_time_high, GPIOA, GPIO_PIN_0, GPIO_PIN_1,
				 GPIO_PIN_2, GPIO_PIN_3);
		display7SEG(red_time_low, GPIOA, GPIO_PIN_4, GPIO_PIN_5,
				 GPIO_PIN_6, GPIO_PIN_7);

		// blink red led
		if(flag_500ms){
			flag_500ms = 0;
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_8);
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_11);
		}

		// enter set button
		if(flag_set){
			flag_set = 0;
			red_time = value;
		}
		break;

	case 3:		// modify green time
		// show value on 7SEG
		value_high = value/10;
		value_low = value%10;
		display7SEG(value_high, GPIOA, GPIO_PIN_8,
				GPIO_PIN_9,	GPIO_PIN_10, GPIO_PIN_11);
		display7SEG(value_low, GPIOA, GPIO_PIN_12,
				GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15);

		// show green time on 7SEG
		int green_time_high = green_time/10;
		int green_time_low  = green_time%10;
		display7SEG(green_time_high, GPIOA, GPIO_PIN_0, GPIO_PIN_1,
				 GPIO_PIN_2, GPIO_PIN_3);
		display7SEG(green_time_low, GPIOA, GPIO_PIN_4, GPIO_PIN_5,
				 GPIO_PIN_6, GPIO_PIN_7);

		// blink red led
		if(flag_500ms){
			flag_500ms = 0;
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_9);
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_12);
		}

		// enter set button
		if(flag_set){
			flag_set = 0;
			green_time = value;
		}
		break;

	case 4:		// modify yellow time
		// show value on 7SEG
		value_high = value/10;
		value_low = value%10;
		display7SEG(value_high, GPIOA, GPIO_PIN_8,
				GPIO_PIN_9,	GPIO_PIN_10, GPIO_PIN_11);
		display7SEG(value_low, GPIOA, GPIO_PIN_12,
				GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15);

		// show red time on 7SEG
		int yellow_time_high = yellow_time/10;
		int yellow_time_low  = yellow_time%10;
		display7SEG(yellow_time_high, GPIOA, GPIO_PIN_0, GPIO_PIN_1,
				 GPIO_PIN_2, GPIO_PIN_3);
		display7SEG(yellow_time_low, GPIOA, GPIO_PIN_4, GPIO_PIN_5,
				 GPIO_PIN_6, GPIO_PIN_7);

		// blink red led
		if(flag_500ms){
			flag_500ms = 0;
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_10);
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_13);
		}

		// enter set button
		if(flag_set){
			flag_set = 0;
			yellow_time = value;
		}
		break;
	default:
		break;
	}

}

void setTime(){
	vertical_state = GREEN_LIGHT;
	vertical_counter = green_time;

	horizontal_state = RED_LIGHT;
	horizontal_counter = red_time;
}

void preprocess(){
	mode = 1;
	value = 1;

	red_time = 10;
	green_time = 6;
	yellow_time = 4;

	// begin state of traffic
	setTime();


	flag_set = 0;
	flag_1s = 0;
	flag_500ms = 0;
	flag_reset = 0;

	readButton_preprocess();
}





