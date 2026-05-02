#ifndef INC_TIMER_A2_H_
#define INC_TIMER_A2_H_


#include <stdint.h>
#include "msp.h"


// 20ms period
void Timer_A2_PWM_Init();

//5.6  750 - 1500 (1ms - 2ms)
void Timer_A2_Update_Duty_Cycle_1(uint16_t duty_cycle_1);

//5.7
void Timer_A2_Update_Duty_Cycle_2(uint16_t duty_cycle_2);

//6.6 375-1800(0.5ms -2.4ms)
void Timer_A2_Update_Duty_Cycle_3(uint16_t duty_cycle_3);






#endif // INC_TIMER_A2_H_



