#include "../inc/Timer_A2.h"



void Timer_A2_PWM_Init()
{

    // set P5.6, 5.7 as PWM Output
    P5->SEL0 |= 0xC0;
    P5->SEL1 &= ~ 0xC0;

    P5->DIR |= 0xC0;

    //set P6.6 as PWM Output
    P6->SEL0 |= 0x40;
    P6->SEL1 &= ~0x40;

    P6->DIR |= 0x40;

    // 20ms period
    TIMER_A2->CCR[0] = 15000;
    TIMER_A2->EX0 = 0x0000;

    // pg 130 data sheet using CCR1-CCR3 bcus ccr0 is being used for Timer_A0
    TIMER_A2->CCTL[1] |= 0x0040;
    TIMER_A2->CCR[1] = 0;

    TIMER_A2->CCTL[2] |= 0x0040;
    TIMER_A2->CCR[2] = 0;

    TIMER_A2->CCTL[3] |= 0x0040;
    TIMER_A2->CCR[3] = 0;


    TIMER_A2->CTL |= 0x02F0;


}

void Timer_A2_Update_Duty_Cycle_1(uint16_t duty_cycle_1)
{
    if (duty_cycle_1 >= TIMER_A2->CCR[0]) return;
    TIMER_A2->CCR[1] = duty_cycle_1;
}

void Timer_A2_Update_Duty_Cycle_2(uint16_t duty_cycle_2)
{
    if (duty_cycle_2 >= TIMER_A2->CCR[0]) return;
       TIMER_A2->CCR[2] = duty_cycle_2;
}
void Timer_A2_Update_Duty_Cycle_3(uint16_t duty_cycle_3)
{
    if (duty_cycle_3 >= TIMER_A2->CCR[0]) return;
    TIMER_A2->CCR[3] = duty_cycle_3;
}
