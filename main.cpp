#include<cstdint>
#include <cstdbool>

#include<vector>
#include <String>
#include "Args/TimeDuration.h"
#include "Args/TimeInstance.h"
#include "Args/ArgumentArray.h"
#include "Args/Argument.h"

#define xdc__nolocalstring
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


/* BIOS Header files */
//#include <ti/sysbios/BIOS.h>
//#include <ti/sysbios/knl/Task.h>

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

volatile uint32_t g_ui32MsgCount = 0;
volatile uint32_t g_bErrFlag=0;


void
CANIntHandler(void)
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
        CANIntClear(CAN0_BASE, 1);

        g_ui32MsgCount++;

        g_bErrFlag = 0;
    }
    else
    {

    }
}

int main(void)
{
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
    ArgumentArray argument;
    UInt32 integer= 33;
    System_printf("Beggining...\n");
    Argument argument_send(Argument::UInt32_TC , &integer) ;
    System_printf("Before putByName: %u \n",argument_send._valueUInt32);


    tCANMsgObject msg;
    unsigned char msgData[8];
    System_flush();

    while(true){
        System_printf("Before putByName: %u \n",g_ui32MsgCount);
        System_flush();
        if(g_ui32MsgCount>0){
            System_printf("Before putByName: %u \n",g_ui32MsgCount);
            msg.pui8MsgData = msgData; // set pointer to rx buffer
            CANMessageGet(CAN0_BASE, 1, &msg, 0);
            g_ui32MsgCount = 0;
        }

    }
    argument.putByName("Test",argument_send);
    Argument argument_save;
    argument.getByIndex(0,argument_save);


    return 0;
}
