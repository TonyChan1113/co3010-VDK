/*
 * Scheduler.h
 *
 *  Created on: Oct 12, 2021
 *      Author: ngocc
 */

#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_

#include "main.h"
#include <stdio.h>

//SCHEDULER SECTION----------------------------------------------------------------

struct SCH_Task {
	//Pointer to the task (must be a 'void (void)' function)
	void (*pFunc)(void);
	//Delay (ticks) until the function will (next) be run
	int delay;
	//Interval (ticks) between subsequent runs
	int period;
	//Incremented (by scheduler) when task is due to execute
	int ready;
	//This is a hint to solve the question below
	uint32_t TaskID;
};

#define MAX_TASK 	5
#define NO_TASK_ID	0
#define TICK_TIME 	10

#define RETURN_ERROR	0
#define RETURN_NORMAL	1

void SCH_Init();
uint32_t SCH_Add_Task(void(*pFunction)(), uint32_t DELAY, uint32_t PERIOD);
uint8_t SCH_Delete_Task(uint32_t taskID);
void SCH_Update(void);
void SCH_Dispactch_Tasks(void);
void SCH_Check_Ready_Task();

//----------------------------------------------------------------------------------

//ERROR SECTION---------------------------------------------------------------------

#define ERROR_SCH_CANNOT_DELETE_TASK 0x01
#define ERROR_SCH_TOO_MANY_TASKS	 0x02

extern unsigned char Error_code_G;

void SCH_Report_Status(void);

//----------------------------------------------------------------------------------

#endif /* INC_SCHEDULER_H_ */
