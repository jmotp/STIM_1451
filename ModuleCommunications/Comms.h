/*
 * Comms.h
 *
 *  Created on: 06/06/2022
 *      Author: jmotp
 */

#ifndef MODULECOMMUNICATIONS_COMMS_H_
#define MODULECOMMUNICATIONS_COMMS_H_

#define xdc__nolocalstring
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include "driverlib/can.h"
#include "driverlib/interrupt.h"
#include "inc/hw_can.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "inc/hw_ints.h"
#include "driverlib/uart.h"
#include <utils/uartstdio.h>



 uint32_t g_ui32MsgCount = 0;
 uint32_t g_bErrFlag=0;
tCANBitClkParms _tCANBitClkParms;



void CANIntHandler(void)
{
    uint32_t ui32Status;

    ui32Status = CANIntStatus(CAN0_BASE, CAN_INT_STS_CAUSE);

    if(ui32Status == CAN_INT_INTID_STATUS)
    {
        ui32Status = CANStatusGet(CAN0_BASE, CAN_STS_CONTROL);
        g_bErrFlag = 1;
    }
    else if(ui32Status == 1)
    {
        g_bErrFlag = 0;
        CANIntClear(CAN0_BASE, 1);
        g_ui32MsgCount++;

    }
    else
    {
        //UARTprintf("Unexpected CAN bus interrupt\n");
    }
}


void initComms(){

    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    GPIOPinConfigure(GPIO_PE4_CAN0RX);
    GPIOPinConfigure(GPIO_PE5_CAN0TX);
    GPIOPinTypeCAN(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);
    CANInit(CAN0_BASE);
    CANBitRateSet(CAN0_BASE, SysCtlClockGet(), 500000);
    CANIntRegister(CAN0_BASE, CANIntHandler); // use dynamic vector table allocation
    CANIntEnable(CAN0_BASE, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);
    IntEnable(INT_CAN0);
    CANEnable(CAN0_BASE);


}

uint16_t c_count = 0;

void commTask(){
    tCANMsgObject msg;
    unsigned char msgData[8];


    //System_printf("Alive %d\n",g_ui32MsgCount);
    // Use ID and mask 0 to recieved messages with any CAN ID
    msg.ui32MsgID = 0;
    msg.ui32MsgIDMask = 0;
    msg.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;
    msg.ui32MsgLen = 8; // allow up to 8 bytes
    System_flush();
    CANMessageSet(CAN0_BASE, 1, &msg, MSG_OBJ_TYPE_RX);
    while(true){
        //System_printf("Here %d\n",g_ui32MsgCount);
        if(g_ui32MsgCount>0){
           c_count++;
           System_printf("Received Byte\n",g_ui32MsgCount);
           msg.pui8MsgData = msgData; // set pointer to rx buffer
           CANMessageGet(CAN0_BASE, 1, &msg, 0);

           for(int i =0; i<8;i++){
               System_printf("%x ",msgData[i]);
           }


           System_printf("\n");
           System_flush();
           g_ui32MsgCount = 0;

        }
        //System_printf("Alive\n");
        System_flush();


     }
}

#endif /* MODULECOMMUNICATIONS_COMMS_H_ */
