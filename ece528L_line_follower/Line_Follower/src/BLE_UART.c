/**
 * @file BLE_UART.c
 *
 * @brief Source code for the BLE_UART driver.
 *
 * This file contains the function definitions for the BLE_UART driver.
 *
 * It interfaces with the Adafruit Bluefruit LE UART Friend Bluetooth Low Energy (BLE) module, which uses the UART communication protocol.
 *  - Product Link: https://www.adafruit.com/product/2479
 *
 * The following connections must be made:
 *  - BLE UART MOD  (Pin 1)     <-->  MSP432 LaunchPad Pin P1.6
 *  - BLE UART CTS  (Pin 2)     <-->  MSP432 LaunchPad GND
 *  - BLE UART TXO  (Pin 3)     <-->  MSP432 LaunchPad Pin P9.6 (PM_UCA3RXD)
 *  - BLE UART RXI  (Pin 4)     <-->  MSP432 LaunchPad Pin P9.7 (PM_UCA3TXD)
 *  - BLE UART VIN  (Pin 5)     <-->  MSP432 LaunchPad VCC (3.3V)
 *  - BLE UART RTS  (Pin 6)     <-->  Not Connected
 *  - BLE UART GND  (Pin 7)     <-->  MSP432 LaunchPad GND
 *  - BLE UART DFU  (Pin 8)     <-->  Not Connected
 *
 * @note For more information regarding the Enhanced Universal Serial Communication Interface (eUSCI),
 * refer to the MSP432Pxx Microcontrollers Technical Reference Manual
 *
 * @author
 *
 */

#include "../inc/BLE_UART.h"

void BLE_UART_Init()
{
    P9->SEL0 |= 0xC0;
    P9->SEL1 &= ~0xC0;

    P1-> SEL0 &= ~0x40;
    P1-> SEL1 &= ~0x40;
    P1-> DIR |= 0x40;

    EUSCI_A3->CTLW0 |= 0x01;
    EUSCI_A3->MCTLW &= ~0xFF;
    EUSCI_A3->CTLW0 &= ~0x8000;
    EUSCI_A3->CTLW0 &= ~0x4000;
    EUSCI_A3->CTLW0 &= ~0x2000;
    EUSCI_A3->CTLW0 &= ~0x1000;
    EUSCI_A3->CTLW0 &= ~0x0800;
    EUSCI_A3->CTLW0 &= ~0x0600;
    EUSCI_A3->CTLW0 &= ~0x0100;
    EUSCI_A3->CTLW0 |= 0x00C0;
    EUSCI_A3->BRW = 1250;
    EUSCI_A3->IE &= ~0x0C;
    EUSCI_A3->IE |= 0x03;
    EUSCI_A3->CTLW0 &= ~0x01;

}

uint8_t BLE_UART_InChar()
{
    // Check the receive interrupt flag (UCRXIFG , Bit 0)
    // in the IFG register and wait if the flag is not set
    // if the UCRXIFG is set , then the receive buffer (UCAxRXBUF) has
    // Received a complete character
    while((EUSCI_A3->IFG & 0x01)==0);

    //Return the data from the Receive buffer(UCAxRXBUF)
    // Reading the UCAxRXBUF will reset the UCRXIFG flag
    return  EUSCI_A3->RXBUF;
}

void BLE_UART_OutChar(uint8_t data)
{
    //Check the Transmit Interrupt flag (UCTXIFG, Bit 1)
    // in the IFG register and wait if the flag is not set
    //if the UCTXIFG is set , then the transmit buffer (UCAxTXBUF) is empty

    while((EUSCI_A3->IFG & 0x02) == 0);

    // WRITE the data to  the transmit Buffer(UCAxTXBUF)
    // WRITE to the UCAxTXBUF will clear the UCTXIFG flag
    EUSCI_A3->TXBUF = data;

}

int BLE_UART_InString(char *buffer_pointer, uint16_t buffer_size)
{
    int length = 0;
    int string_size = 0;

    // Read the last received data from the UART receive Buffer
    char character = BLE_UART_InChar();
    length++; //


    //Check if the received character is a carriage return. Otherwise,
    // If it is a valid character , then store it in Barcode_Scanner_Buffer.
    // for each valid character, increment the string_Size variable which will
    // indicate how many characters have been detected from the Barcode Scanner module.
    while(character != LF)
    {
        // Remove the character from the buffer is the received character is a backspace character
        if(character == BS)
        {
            if (length)
            {
                buffer_pointer--;
                length--;
                BLE_UART_OutChar(BS);
            }
        }

        // Otherwise, if there are more characters to be read, store them in the buffer
        else if (length < buffer_size)
        {
            *buffer_pointer = character;
            buffer_pointer++;
            length++;
            string_size++;
        } else {
            break;
        }
        character = BLE_UART_InChar();
    }
    *buffer_pointer = 0;

    return string_size;
}

void BLE_UART_OutString(char *pt)
{
    while(*pt)
    {
        BLE_UART_OutChar(*pt);
        pt++;
    }

}

uint8_t Check_BLE_UART_Data(char BLE_UART_Data_Buffer[], char *data_string)
{
    if (strstr(BLE_UART_Data_Buffer, data_string) != NULL)
    {
        return 0x01;
    }
    else
    {
        return 0x00;
    }
}

void BLE_UART_Reset()
{
    // Switch to CMD mode by setting the MOD pin (p1.6) to 1
    P1->OUT |= 0x40;
    Clock_Delay1ms(1000);

    //send the system reset command by sending the "ATZ" String
    // to the BLE UART Module
    BLE_UART_OutString("ATZ\r\n");
    Clock_Delay1ms(3000);
    P1->OUT &= ~0x40;

}
