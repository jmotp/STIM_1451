#include<cstdint>
#include <cstdbool>

#include<vector>
#include <String>
#include "Args/TimeDuration.h"
#include "Args/TimeInstance.h"
#include "Args/ArgumentArray.h"
#include "Args/Argument.h"

#define ISOTP_BYTE_ORDER_LITTLE_ENDIAN

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




/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <driverlib/EEPROM.h>

#define TASKSTACKSIZE 2048


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

/*namespace IEEE1451Dot0{
    namespace Args{
       typedef std::vector<int8_t> Int8Array;
       typedef std::vector<int16_t> Int16Array;
       typedef std::vector<int32_t> Int32Array;
       typedef std::vector<uint8_t> UInt8Array;
       typedef std::vector<uint16_t> UInt16Array;
       typedef std::vector<uint32_t> UInt32Array;
       typedef std::vector<float> Float32Array;
       typedef std::vector<double> Float64Array;
       typedef std::vector<string> StringArray;
       typedef std::vector<char> OctetArray;
       typedef std::vector<bool> BooleanArray;
       typedef std::vector<TimeInstance> TimeInstanceArray;
       typedef std::vector<TimeDuration> TimeDurationArray;



    }
}

*/



/**
 * main.c
 */


Can can0;

int mainTask(void){

    EK_TM4C123GXL_initGeneral();

    //initComms();

    ArgumentArray argument;
    UInt32 integer= 33;
    System_printf("Beggining...\n");
    Argument argument_send(Argument::UInt32_TC , &integer) ;
    System_printf("Before putByName: %u \n",argument_send._valueUInt32);



    System_flush();
    String s = "hello\n";

    Argument argument2(Argument::Octet_Array_TC,&s);
    argument2.print();
    System_flush();

    OctetArray test("");
    argument.putByName("Test",argument_send);
    argument_send.print();
    argumentArray2OctetArray(argument,test);



    StringArray stringArray;
        argument.getNames(stringArray);
        for(String out: stringArray){
             System_printf("%s\n", out.c_str());
             System_flush();
        }

    extern uint32_t g_newMessage;
    while(1){
        static OctetArray buffer;
        static uint32_t len;
        static Boolean buf_bool;
        if(g_newMessage){
            can0.readMsg(0, TimeDuration{0,0},len , buffer, buf_bool);
               g_newMessage=0;
            System_printf("String: %s\n", buffer.c_str());
            System_flush();

        }
        Task_yield();



    }





    Argument argument_save;
    argument.getByIndex(0,argument_save);
    return 0;

}

Task_Struct task0Struct,task1Struct;
Char task0Stack[TASKSTACKSIZE],task1Stack[TASKSTACKSIZE];




void CanTaskWrapper() {
    can0.commTask();
}

/*
 *  ======== main ========
 */
Int main()
{

    Clock_tickStart();
    /* Construct BIOS objects */
    Task_Params taskParams,taskParams1;

    /* Construct clock Task thread */
    Task_Params_init(&taskParams);
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task0Stack;
    taskParams.priority = 1;
    Task_construct(&task0Struct, (Task_FuncPtr)mainTask, &taskParams, NULL);


    can0.init();



    /* Construct clock Task thread */
    Task_Params_init(&taskParams1);
    taskParams1.stackSize = TASKSTACKSIZE;
    taskParams1.stack = &task1Stack;
    taskParams1.priority = 1;
    Task_construct(&task1Struct, (Task_FuncPtr)CanTaskWrapper, &taskParams1, NULL);

    BIOS_start();

    return 0;
}
