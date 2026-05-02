/**
 * @file main.c
 *
 * @brief Main source code for the Line_Follower program.
 *
 * This file contains the main entry point for the Line_Follower program.
 * The main controller demonstrates a Line Follower robot without using an advanced algorithm.
 *
 * It interfaces the following peripherals using GPIO to demonstrate line following:
 *  - 8-Channel QTRX Sensor Array module
 *
 * Timers are used in this lab:
 *  - SysTick:  Used to generate periodic interrupts at a specified rate (1 kHz)
 *  - Timer A0: Used to generate PWM signals that will be used to drive the DC motors
 *  - Timer A1: Used to generate periodic interrupts at a specified rate (1 kHz)
 *
 * @note For more information regarding the 8-Channel QTRX Sensor Array module,
 * refer to the product page: https://www.pololu.com/product/3672
 *
 * @author
 *
 */
#include "msp.h"
#include "inc/Clock.h"
#include "inc/CortexM.h"
#include "inc/GPIO.h"
#include "inc/EUSCI_A0_UART.h"
#include "inc/Motor.h"
#include "inc/Timer_A1_Interrupt.h"
#include "inc/SysTick_Interrupt.h"
#include "inc/Reflectance_Sensor.h"
#include "inc/BLE_UART.h"
#include "inc/Timer_A2.h"
#include "inc/BLE_UART.h"

#define PWM_NOMINAL 3000

static uint32_t Timer_A1_ms_elapsed = 0;

typedef enum
{
    CENTER              = 0b00011000,
    SLIGHT_LEFT_CENTER  = 0b00011100,
    SLIGHT_RIGHT_CENTER = 0b00111000,
    FAR_LEFT            = 0b00000001,
    FAR_RIGHT           = 0b10000000,
    LEFT_TURN           = 0b11111000,
    RIGHT_TURN          = 0b00011111,
    T_INTERSECTION      = 0b11111111,
    DEAD_END            = 0b00000000,
} Line_Position;

void Detect_Line_Position(uint8_t reflectance_sensor_data) {
    switch (reflectance_sensor_data) {
    case CENTER:
        LED1_Output(RED_LED_OFF);
        LED2_Output(RGB_LED_GREEN);
        break;
    case SLIGHT_LEFT_CENTER:
        LED1_Output(RED_LED_OFF);
        LED2_Output(RGB_LED_YELLOW);
        break;
    case SLIGHT_RIGHT_CENTER:
        LED1_Output(RED_LED_OFF);
        LED2_Output(RGB_LED_PINK);
        break;
    case FAR_LEFT:
        LED1_Output(RED_LED_OFF);
        LED2_Output(RGB_LED_WHITE);
        break;
    case FAR_RIGHT:
        LED1_Output(RED_LED_OFF);
        LED2_Output(RGB_LED_SKY_BLUE);
        break;
    case LEFT_TURN:
        LED1_Output(RED_LED_ON);
        LED2_Output(RGB_LED_WHITE);
        break;
    case RIGHT_TURN:
        LED1_Output(RED_LED_ON);
        LED2_Output(RGB_LED_SKY_BLUE);
        break;
    case T_INTERSECTION:
        LED1_Output(RED_LED_OFF);
        LED2_Output(RGB_LED_BLUE);
        break;
    case DEAD_END:
        LED1_Output(RED_LED_OFF);
        LED2_Output(RGB_LED_RED);
        break;
    default:
        LED1_Output(RED_LED_OFF);
        LED2_Output(RGB_LED_OFF);
        break;
    }
}

typedef enum
{
    STRAIGHT,
    LEFT,
    RIGHT,
    HARD_LEFT,
    HARD_RIGHT,
    TURN_AROUND,
    STATE_T_INTERSECTION




} State;

uint8_t state = STRAIGHT;


void FSM(uint8_t reflectance_sensor_data)
{
    switch(state)
    {
    case STRAIGHT :
        Motor_Forward(PWM_NOMINAL, PWM_NOMINAL );

        if(reflectance_sensor_data == SLIGHT_RIGHT_CENTER) state = LEFT;
        if(reflectance_sensor_data == SLIGHT_LEFT_CENTER) state = RIGHT;
        if(reflectance_sensor_data == LEFT_TURN) state = HARD_LEFT;
        if(reflectance_sensor_data == RIGHT_TURN) state = HARD_RIGHT;
        if(reflectance_sensor_data == DEAD_END) state = TURN_AROUND;
        if(reflectance_sensor_data == T_INTERSECTION) state = STATE_T_INTERSECTION;
        break;
    case LEFT :
        Motor_Left(0, PWM_NOMINAL); // stop the other motor, while the other is on

        if(reflectance_sensor_data == CENTER) state = STRAIGHT;
        //if(reflectance_sensor_data == SLIGHT_RIGHT_CENTER) state = LEFT;
        //if(reflectance_sensor_data == SLIGHT_LEFT_CENTER) state = RIGHT;
        if(reflectance_sensor_data == LEFT_TURN) state = HARD_LEFT;
        if(reflectance_sensor_data == RIGHT_TURN) state = HARD_RIGHT;
        if(reflectance_sensor_data == DEAD_END) state = TURN_AROUND;
        if(reflectance_sensor_data == T_INTERSECTION) state = STATE_T_INTERSECTION;
        break;

    case RIGHT:

        Motor_Right(PWM_NOMINAL, 0);
        if(reflectance_sensor_data == CENTER) state = STRAIGHT;
        //if(reflectance_sensor_data == SLIGHT_RIGHT_CENTER) state = LEFT;
        //if(reflectance_sensor_data == SLIGHT_LEFT_CENTER) state = RIGHT;
        if(reflectance_sensor_data == LEFT_TURN) state = HARD_LEFT;
        if(reflectance_sensor_data == RIGHT_TURN) state = HARD_RIGHT;
        if(reflectance_sensor_data == DEAD_END) state = TURN_AROUND;
        if(reflectance_sensor_data == T_INTERSECTION) state = STATE_T_INTERSECTION;
        break;

    case HARD_LEFT:
        Motor_Left(0.8*PWM_NOMINAL, 1.2*PWM_NOMINAL); // LEFT WHEEL is going about half speed backwards, right wheel is going about 50 percent faster

        if(reflectance_sensor_data == CENTER) state = STRAIGHT;
        if(reflectance_sensor_data == SLIGHT_RIGHT_CENTER) state = LEFT;
        if(reflectance_sensor_data == SLIGHT_LEFT_CENTER) state = RIGHT;

        break;

    case HARD_RIGHT:
        Motor_Right(1.2*PWM_NOMINAL, 0.8*PWM_NOMINAL); // LEFT WHEEL is going about half speed backwards, right wheel is going about 50 percent faster

        if(reflectance_sensor_data == CENTER) state = STRAIGHT;
        if(reflectance_sensor_data == SLIGHT_RIGHT_CENTER) state = LEFT;
        if(reflectance_sensor_data == SLIGHT_LEFT_CENTER) state = RIGHT;

        break;
    case TURN_AROUND:
        Motor_Left(PWM_NOMINAL, PWM_NOMINAL);

        if(reflectance_sensor_data == CENTER) state = STRAIGHT;
        if(reflectance_sensor_data == SLIGHT_RIGHT_CENTER) state = LEFT;
        if(reflectance_sensor_data == SLIGHT_LEFT_CENTER) state = RIGHT;

        break;

    case STATE_T_INTERSECTION:

        Motor_Stop();
        //state = HARD_LEFT;

        break;



    }
}



void Timer_A1_Periodic_Task(void)
{
    Timer_A1_ms_elapsed++;

    if((Timer_A1_ms_elapsed % 10)==0)
    {
        Reflectance_Sensor_Start();
    }

    if((Timer_A1_ms_elapsed % 10) == 1)
    {
        uint8_t Reflectance_Sensor_Data = Reflectance_Sensor_End();
        Detect_Line_Position(Reflectance_Sensor_Data);
        FSM(Reflectance_Sensor_Data);
    }
}

//void Timer_A1_Periodic_Task(void)
//{
//    Timer_A1_ms_elapsed++;
//
//    uint8_t Reflectance_Sensor_Data = Reflectance_Sensor_End();
//
//    if((Timer_A1_ms_elapsed % 10)==0)
//    {
//        Reflectance_Sensor_Data =Reflectance_Sensor_Read(850);
//    }
//
//    if((Timer_A1_ms_elapsed % 10) == 1)
//    {
//        Detect_Line_Position(Reflectance_Sensor_Data);
//        FSM(Reflectance_Sensor_Data);
//    }
//}

uint16_t dutyCycle1 = 1000;
uint16_t dutyCycle2 = 1000;
uint16_t dutyCycle3 = 1800;
void Decode_Controller_Packet(char BLE_UART_Buffer[])
{
    EUSCI_A0_UART_OutString("Decode packet\n");

        switch(BLE_UART_Buffer[2])
        {

            case '1': // close claw
                Timer_A2_Update_Duty_Cycle_3(1800);
                break;

            case '2': // open claw
                Timer_A2_Update_Duty_Cycle_3(375);
                break;

            case '3':
                state = TURN_AROUND;
                break;

            case '4':
                state = STATE_T_INTERSECTION;
                Motor_Stop();
                break;


            case '5':// UP ARROW
                if(dutyCycle1 < 1500) dutyCycle1 += 100;
                Timer_A2_Update_Duty_Cycle_1(dutyCycle1);

                break;

            case '6': // DOWN ARROW
                if(dutyCycle1 > 800) dutyCycle1 -= 100;
                Timer_A2_Update_Duty_Cycle_1(dutyCycle1);

                break;


            case '7': // LEFT ARROW claw wrist opens up
                if (dutyCycle2 < 1500) dutyCycle2 +=100;
                Timer_A2_Update_Duty_Cycle_2(dutyCycle2);
                break;

            case '8': // Right ARROW claw wrist opens down
                if(dutyCycle2 > 800) dutyCycle2 -=100;
                Timer_A2_Update_Duty_Cycle_2(dutyCycle2);
                break;

            default:
                break;

        }



}



int main(void)
{
    DisableInterrupts();

    Motor_Init();
    DisableInterrupts();
    Clock_Init48MHz();
    LED1_Init();
    LED2_Init();

    EUSCI_A0_UART_Init_Printf();

    Reflectance_Sensor_Init();
    Timer_A2_PWM_Init();


    Timer_A1_Interrupt_Init(&Timer_A1_Periodic_Task,TIMER_A1_INT_CCR0_VALUE);
    BLE_UART_Init();
    char BLE_UART_Buffer[BLE_UART_BUFFER_SIZE] = {0};


    EnableInterrupts();

    Timer_A2_Update_Duty_Cycle_1(dutyCycle1);
    Timer_A2_Update_Duty_Cycle_2(dutyCycle2);
    Timer_A2_Update_Duty_Cycle_3(dutyCycle3);

    Clock_Delay1ms(1000);
    BLE_UART_Reset();

    //Send a message to the BLE Module to check if the connection is stable
    BLE_UART_OutString("BLE UART Active\r\n");
    Clock_Delay1ms(1000);

    while(1)
    {
//        // range 750-1500
//        Timer_A2_Update_Duty_Cycle_1(750); //15000*0.05
//        Clock_Delay1ms(1000);
//        Timer_A2_Update_Duty_Cycle_1(1500);
//        Clock_Delay1ms(1000);

        int string_size  = BLE_UART_InString(BLE_UART_Buffer, 5);
        Decode_Controller_Packet(BLE_UART_Buffer);


    }
}
