/*
 * Scheduler.c
 *
 *  Created on: Oct 12, 2021
 *      Author: ngocc
 */
//REJECT HUMANITY, RETURN TO MONNKEY
//THIS WON'T ACT AS A QUEUE, BUT THE COMPLEXITY OF THE CODE IS REDUCED
//DELETE TASK NO LONGER O(N^2), BUT O(1)
//ADD TASK IS O(N), UPDATE TOO (but better).
#include "Scheduler.h"
#include "string.h"		//needed for memmove
#include "main.h"


//SCHEDULE SECTION------------------------------------------------------------------------
static struct SCH_Task sch_Task[MAX_TASK];
static uint32_t tick_time = 0; //TICK_TIME
static uint32_t min_delay = 0xffffffff;
UART_HandleTypeDef huart1;		//NEEDED to use UART
IWDG_HandleTypeDef hiwdg;		//NEEDED for watchdog Timer

unsigned char Error_code_G;
unsigned char Last_error_code_G;

void SCH_Init(void) {
	for(int i = 0; i < MAX_TASK; i++) {
		sch_Task[i].delay = 0;
		sch_Task[i].pFunc = NULL;
		sch_Task[i].period = 0;
		sch_Task[i].ready = 0;
	}

	//init Error code
	Error_code_G = 0;
	//START A WATCHDOG TIMER. THIS IS PUT AT THE START (INIT) OF THE SCHEDULER.
	//WATCHDOG TIMER IS A TYPE OF SOFTWARE TIMER THAT CAN RESET THE ENTIRE CHIP IF
	//THE SYSTEM IS STUCK, ONLY AFTER A timeout. We can specify the timeout in .ioc, IWDG.
	//timeout = (prescaller divider) * (reload value) / LSI frequency
	HAL_IWDG_Init(&hiwdg);
	//Since the timer is start, it will start counting down. If it reach 0, the microprocessor will be reset.
	//Then how to use it? If we let it count down to 0, then that mean the system is hang, then the system won't run at all,
	//it can't even call function. That mean, if the function is called regularly, then the system is fine. this function will be our SCH_update which is
	//called every 10ms. in this function, we will always reset the watchdog timer (to timeout) and let it count down again. This way, if the system run
	//normally, watchdog timer will always be reset, and it won't reset the microprocessor. BUT, in case the system is hang, SCH_update won't be call at all
	//and the time elapsed pass the timeout -> watchdog timer reach 0 -> reset the microprocessor.
}



//in this add_task function, to make sure that min_delay is working, we will:
// + if tick_time (current time) + DELAY < min_delay, update min_delay
// + newtask.delay of the new task will be tick_time + DELAY, no matter this value is greater or lesser than current min_delay.
// 		because, if the value is greater, it will be subtract by SCH_Update when tick_time == min_delay.
//		or if value is lesser, this will be our new min_delay, and with SCH_Update, tick_time will soon = min_delay = newtask.delay, and task will be
// 		executed.
uint32_t SCH_Add_Task(void(*pFunction)(), uint32_t DELAY, uint32_t PERIOD) {

	//find an empty slot to put the task in.
	uint32_t index = 0;
	while ((sch_Task[index].pFunc != NULL) && (index < MAX_TASK)) {
		index++;
	}
	//if the queue is full
	if (index >= MAX_TASK) {
		//Task list is full
		// Set the global error variable
		Error_code_G = ERROR_SCH_TOO_MANY_TASKS;
		//also return an error code
		return MAX_TASK;
	}
	//if a slot is found (queue still have some space)
	if (tick_time + DELAY < min_delay) {
		min_delay = tick_time + DELAY;
	}
	sch_Task[index].delay = tick_time + DELAY;
	sch_Task[index].pFunc = pFunction;
	sch_Task[index].period = PERIOD;
	sch_Task[index].ready = 0;

	return (index);
}

uint8_t SCH_Delete_Task(uint32_t taskID) {
	unsigned char Return_code;
	if (taskID < 0 || taskID >= MAX_TASK || sch_Task[taskID].pFunc == NULL) {
		// No task at this location...
		//
		// Set the global error variable
		Error_code_G = ERROR_SCH_CANNOT_DELETE_TASK;

		// ...also return an error code
		Return_code = RETURN_ERROR;
	} else {
		Return_code = RETURN_NORMAL;
	}

	sch_Task[taskID].delay = 0;
	sch_Task[taskID].pFunc = NULL;
	sch_Task[taskID].period = 0;
	sch_Task[taskID].ready = 0;

	return Return_code;
}

//in this function, every time it's called, it will increase tick_time by TICK_TIME.


void SCH_Update(void) {		//tai sao minh lai co khien ham SCH_update khong O(n)?
	//boi vi SCH_update duoc dat trong timer. ta khong duoc de SCH_Update chay qua lau gay tran timer
	//co the khien he thong bi treo
	tick_time += TICK_TIME;
//	this 3 lines printout min_time and tick_time.
//	static char timeFormat[30];
//	int strlength = sprintf(timeFormat, "min: %ld, tick: %ld\r\n", min_delay, tick_time);
//	HAL_UART_Transmit_IT(&huart1, (uint8_t*)timeFormat, strlength);
	HAL_IWDG_Refresh(&hiwdg);
}

//if tick_time == min_delay, we search through the array and subtract each delay by
//the current tick_time. While searching, we also find the new min_delay.
//after update all the delay value, we have a new min_delay, and tick_time is set to 0.
//this tick_time will continue to count up till reach the min_delay, and then we go update everything again.
//---------------------------------------------------------------------------------------------------------
//example:
//if somehow, min_delay is 500 and tick_time is 600, and the next task will execute in 1000. these two task's period is 0
//current task delay: 500 - 600 = -100 -> ready++;
//next task delay: 1000 - 600 = 400. because there's no other task, this will be our new min_delay
//tick_time is set back to 0. it will then count up to 400 and next task will be execute at the right time (1000)
//remember, we only update the new_min delay value if the delay value is > 0.
//-----------------------------------------------------------------------------------------------------------
//if tick_time >= min_delay, there must be at least one task will be executed.s
//if tick_time > min_delay, this mean there's one task that run too long in dispact_task.
//then, when this function is called, tick_time is way larger than min_delay. in that case,
//we have to decrease all task delay by tick_time. If we're lucky, new delay is > 0, and the
//scheduler run normally. but, if the delay is < 0, mean that the task is supposed to run while dispact_task was running.
//so we increase the ready value by one. If that task with negative delay value have period, we increase the delay with period.
//if even after we increase the delay, it's still < 0, that mean the task is scheduled to run multiple times while we stuck at dispact_task, not just once.
//So, we'll check for the delay, continuously add that task delay with period and increase ready if that task delay is still <= 0
//By doing like this, even if some tasks is scheduled to run while we stuck at dispact_task, these tasks won't be skipped;
//the scheduler will run these tasks, eventually.
void SCH_Check_Ready_Task() {


	if (tick_time >= min_delay) {
		static char timeformat[100];
			//		uint32_t time_ms = SCH_tasks_G[Index].Delay;
		int strlength = sprintf(timeformat, "before: array: %d %d %d %d %d\r\n", sch_Task[0].delay, sch_Task[1].delay,
				sch_Task[2].delay, sch_Task[3].delay,sch_Task[4].delay);
		HAL_UART_Transmit(&huart1, (uint8_t*)timeformat, strlength, 10*strlength);
//			static char timeFormat[30];
//			int strlength = sprintf(timeFormat, "min: %ld, tick: %ld\r\n", min_delay, tick_time);
//			HAL_UART_Transmit_IT(&huart1, (uint8_t*)timeFormat, strlength);
		uint32_t new_min = 0xffffffff;
		for(int i = 0; i < MAX_TASK; i++) {
			if (sch_Task[i].pFunc != 0) {
				sch_Task[i].delay -= tick_time;
				if (sch_Task[i].delay <= 0) {
					//schedule the task to run
					sch_Task[i].ready += 1;
					if (sch_Task[i].period != 0) {
						sch_Task[i].delay += sch_Task[i].period;
						if (sch_Task[i].delay <= 0) {
							//make the task wait for "period" and run again
							int temp = sch_Task[i].delay * (-1);
							int distance = temp / sch_Task[i].period;
							sch_Task[i].delay += sch_Task[i].period * (distance + 1);
							sch_Task[i].ready += distance;
						}
					} //if period == 0, we don't want it to execute again, so we do nothing here
				}
				if (sch_Task[i].delay < new_min && sch_Task[i].delay > 0) {
					new_min = sch_Task[i].delay;
				}
			}
		}
		min_delay = new_min;
		tick_time = 0;
		//after changing
		strlength = sprintf(timeformat, "after: array: %d %d %d %d %d\r\n", sch_Task[0].delay, sch_Task[1].delay,
				sch_Task[2].delay, sch_Task[3].delay,sch_Task[4].delay);
		HAL_UART_Transmit(&huart1, (uint8_t*)timeformat, strlength, 10*strlength);
	}
}

void SCH_Dispactch_Tasks(void) {
	//for loop: present FIFO, the first task will execute first.
	for(int i = 0; i < MAX_TASK; i++) {
		if (sch_Task[i].pFunc != NULL && sch_Task[i].ready > 0) {
			//run the task
			//dereference it first, and then run.
			(*sch_Task[i].pFunc)();
			//----------------------------------send data to terminal----------------------------------------------
			//use static so the buffer will live longer, long enough to send all data to the UART.
//			static char timeFormat[30];
//			uint32_t time_ms = HAL_GetTick();
//			int strlength = sprintf(timeFormat, "%ld :task done\r\n", time_ms);
//			HAL_UART_Transmit_IT(&huart1, (uint8_t*)timeFormat, strlength);
			//call, and then return. This function does not "block" as HAL_UART_Transmit that wait for all byte to send.
			//since _IT will end quickly, if the buffer is not static, buffer might be destroyed before all the data
			//has been send.
			//------------------------------------------------------------------------------------------------------
			sch_Task[i].ready -= 1;
			//in case the task only run once, if it's scheduled to run, we need to run it first
			//(keep the function there to run) and then only after it has done, delete the task.
			//if period = 0, we don't want it to execute again
			if (sch_Task[i].period == 0) {
				SCH_Delete_Task(i);
			}
		}
	}
	// Report system status
	SCH_Report_Status();
}
//---------------------------------------------------------------------------------------




//ERROR SECTION--------------------------------------------------------------------------

static uint32_t Error_tick_count_G;

//the funny part is, this function is called inside Dispact_Task, and Dispact_Task is called
//inside while loop. then how much "tick" is need, exactly?
void SCH_Report_Status(void) {
#ifdef SCH_REPORT_ERRORS
	//ONLY APPLIES IF WE ARE REPORTING ERRORS
	// Check for a new error code
	if (Error_code_G != Last_error_code_G) {
		//B(SET)RR: 32 BIT, 16bit low can use to set a pin to high if a bit is 1.
		Error_port->BSRR = ((uint16_t)Error_code_G << 8);
		Last_error_code_G = Error_code_G;
		if (Error_code_G != 0) {
			Error_tick_count_G = 60000;
		} else {
			Error_tick_count_G = 0;
			Error_port->BRR = ((uint16_t)0xff << 8); 		//have to manually RESET error pin.
		}
	} else {
		if (Error_tick_count_G != 0) {
			if (--Error_tick_count_G == 0) {
				Error_code_G = 0; //Reset error code.
			}
		}
	}
#endif
}



