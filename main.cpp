#include<cstdint>
#include <cstdbool>
#include <cstdio>

#include<vector>
#include <String>
#include "Args/TimeDuration.h"
#include "Args/TimeInstance.h"
#include "Args/ArgumentArray.h"
#include "Args/Argument.h"

#include <config.h>
#define xdc_nolocalstring
extern "C"{
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
}

#include "driverlib/systick.h"

#include"EK_TM4C123GXL.h"

#include "ModuleCommunications/Comm.h"
#include "ModuleCommunications/Can.h"

#include "isotp/isotp_user.h"


#include "Util/Codec.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

#include <ti/sysbios/knl/Clock.h>

#include <TransducerServices/Handler.h>

#include <xdc/runtime/Error.h>


///* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <driverlib/EEPROM.h>

#include <TransducerServices/SPITransducerChannel.h>
#include <TransducerServices/TransducerChannelManager.h>

/* TI-RTOS Header files */
//#include <ti/drivers/GPIO.h>
// #include <ti/drivers/I2C.h>
// #include <ti/drivers/SDSPI.h>
// #include <ti/drivers/SPI.h>
// #include <ti/drivers/UART.h>
// #include <ti/drivers/Watchdog.h>
// #include <ti/drivers/WiFi.h>

/* Board Header file */
//#include "Board.h"


// ERROR ENCODING
#define NO_ERROR 0
#define INVALID_COMMID 1
#define UNKNOWN_DESTID 2
#define TIMEOUT 3
#define NETWORK_FAILURE 4
#define NETWORK_CORRUPTION 5
#define MEMORY 6
#define QOS_FAILURE 7
#define MCAST_NOT_SUPPORTED 8
#define UNKNOWN_GROUPID 9
#define UNKNOWN_MODULEID 10
#define UNKNOWN_MSGID 11
#define NOT_GROUP_MEMBER 12
#define ILLEGAL_MODE 13
#define LOCKED_RESOURCE 14
#define FATAL_TEDS_ERROR 15
#define NON_FATAL_TEDS_ERROR  16
#define CLOSE_ON_LOCKED_RESOURCE 17
#define LOCK_BROKEN 18
#define NETWORK_RESOURCE_EXCEEDED 19
#define MEMORY_RESOURCE_EXCEEDED 20

using namespace std;

/**
 * main.c
 */


Can can0;
Codec codec;
Handler handler;
NetReceive netReceive;
SPITransducerChannel tchannel;
TransducerChannelManager transducerChannelManager;
int mainTask(void){


    EK_TM4C123GXL_initGeneral();



    transducerChannelManager.registerTransducerChannel(tchannel);

    printf("Beggining...\n");



    extern uint32_t g_newMessage;
    while(1){
        OctetArray buffer("");
        static uint32_t len;
        static Boolean buf_bool;
        System_flush();

        if(netReceive.messageAvailable()){
            MessageIncomingInfo message = netReceive.getMessageIncomingInfo();
            can0.readMsg(message.rcvCommId, TimeDuration{0,0},len , buffer, buf_bool);
            g_newMessage=0;
            UInt16 channelId;
            UInt8 cmdFunctionId;
            UInt8 cmdClassId;
            ArgumentArray inArgs;
            codec.decodeCommand(buffer, channelId, cmdClassId, cmdFunctionId, inArgs);
            System_printf("Command: Channel %d Cmd %d Function %d\n",channelId,cmdClassId,cmdFunctionId);
            ArgumentArray outArgs;
            Boolean hasResponse =0;
            handler.handleCommand(cmdClassId, cmdFunctionId, inArgs, hasResponse, outArgs);

            if(hasResponse){
                OctetArray payload;
                Boolean last = 1;
                TimeDuration time{{1,0}};
                codec.encodeResponse(1, outArgs, payload);
                can0.writeRsp(message.rcvCommId, time, payload, last);
            }
//            System_flush();

        }


        //Task_yield();
    }





    //Argument argument_save;
    //argument.getByIndex(0,argument_save);
    return 0;

}

Task_Struct task0Struct,task1Struct;
Char task0Stack[TASKSTACKSIZE],task1Stack[TASKSTACKSIZE];




void CanTaskWrapper() {
    can0.commTask();
}


void clockHandler1(){


    //Task_yield();
}

/*
 *  ======== main ========
 */
Int main()
{
    Error_Block eb;


    Clock_Params clockParams;
    Clock_Params_init(&clockParams);
     clockParams.period = 1000;/* every 4 Clock ticks */
     clockParams.startFlag = TRUE;/* start immediately */
     Clock_Handle myClk0 = Clock_create((Clock_FuncPtr)clockHandler1, 2000, &clockParams, &eb);
     if (myClk0 == NULL) {
     System_abort("Clock0 create failed");
     }
     Clock_tickStart();
    /* Construct BIOS objects */
    Task_Params taskParams,taskParams1;

    /* Construct clock Task thread */
    Task_Params_init(&taskParams);
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task0Stack;
    taskParams.priority = 1;
    Task_construct(&task0Struct, (Task_FuncPtr)mainTask, &taskParams, &eb);

    can0.init();

    can0.registerNetReceive(&netReceive);

    Task_Params_init(&taskParams1);
    taskParams1.stackSize = TASKSTACKSIZE;
    taskParams1.stack = &task1Stack;
    taskParams1.priority = 2;

    Task_construct(&task1Struct, (Task_FuncPtr)CanTaskWrapper, &taskParams1, &eb);


    BIOS_start();

    return 0;
}
