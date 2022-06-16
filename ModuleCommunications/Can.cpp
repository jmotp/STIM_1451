/*
 * Can.cpp
 *
 *  Created on: 14/06/2022
 *      Author: jmotp
 */

#include <ModuleCommunications/Can.h>

    #include <isotp/isotp.h>
#include <ti/sysbios/knl/Task.h>


uint32_t g_ui32MsgCount = 0;
uint32_t g_bErrFlag = 0;
uint32_t g_clrToSend = 1;
uint32_t g_sendFlag = 0;
uint32_t g_newMessage = 0;
uint32_t g_messageSent=0;

//Interrupt handler for CAN communication
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
    else if(ui32Status == 2)
    {
        g_bErrFlag = 0;
        CANIntClear(CAN0_BASE, 2);
        g_clrToSend = 1;
    }
    else
    {
    }
}

Can::Can()
{
    // TODO Auto-generated constructor stub

}

Can::~Can()
{
    // TODO Auto-generated destructor stub
}

UInt16 Can::init(){

    //System_printf("Can initiated\n");
    //System_flush();

    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

    //CAN using pins PE4_RX PE5_TX
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    GPIOPinConfigure(GPIO_PE4_CAN0RX);
    GPIOPinConfigure(GPIO_PE5_CAN0TX);
    GPIOPinTypeCAN(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);
    CANInit(CAN0_BASE);

    //CAN bitrate 500kbps
    CANBitRateSet(CAN0_BASE, SysCtlClockGet(), 500000);
    CANIntRegister(CAN0_BASE, CANIntHandler);
    CANIntEnable(CAN0_BASE, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);
    IntEnable(INT_CAN0);
    CANEnable(CAN0_BASE);


    //msgReceive CAN Object Config
    msgReceive.ui32MsgID = 0;
    msgReceive.ui32MsgIDMask = 0;
    msgReceive.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;
    msgReceive.ui32MsgLen = 8; // allow up to 8 bytes
    msgReceive.pui8MsgData = msgReceiveData;


    //msgRSend CAN Object Config
    msgSend.ui32MsgID = 1;
    msgSend.ui32MsgIDMask = 0;
    msgSend.ui32Flags = MSG_OBJ_TX_INT_ENABLE;
    msgSend.ui32MsgLen = 8; // allow up to 8 bytes
    msgSend.pui8MsgData = msgSendData;

    isotp_init_link(&g_link, 0x18daaa55, g_isotpSendBuf, sizeof(g_isotpSendBuf), g_isotpRecvBuf, sizeof(g_isotpRecvBuf));


    return 0;
}



void Can::commTask(){
    SysTickbegin();
    CANMessageSet(CAN0_BASE, 1, &msgReceive, MSG_OBJ_TYPE_RX);
    //System_printf("Can running\n");
    //System_flush();

    uint16_t out_size;
    uint32_t ret;
    while(true){
        //System_printf("Can running\n");
        //System_flush();
        if(g_clrToSend && g_sendFlag){
            System_printf("Message Sent to id %d\n", msgSend.ui32MsgID);
            for(int i = 0 ; i < 8;i++){
                    System_printf("%x ",msgSendData[i]);
            }
            System_printf("\n");
            System_flush();
            CANMessageSet(CAN0_BASE, 2, &msgSend, MSG_OBJ_TYPE_TX); // send as msg object 2

            g_sendFlag=0;
            g_clrToSend = 0;

        }

        if(g_ui32MsgCount==1){

            CANMessageGet(CAN0_BASE, 1, &msgReceive, 0); // receive as msg object 1
            System_printf("Received_byte\n");
            for(int i = 0 ; i < 8;i++){
                    System_printf("%x ",msgReceiveData[i]);
            }
            System_printf("\n");
            System_flush();
            isotp_on_can_message(&g_link, msgReceiveData, msgReceive.ui32MsgLen);





            g_ui32MsgCount=0;

        }

        isotp_poll(&g_link);

        ret = isotp_receive(&g_link, (uint8_t*) receiveBuffer, 512, &out_size);
        if(ISOTP_RET_OK == ret){
            g_newMessage = 1;
            System_printf("Message acquired\n");
            System_flush();
        }

        Task_yield();
    }
}


UInt16 Can::sendMessage(const uint32_t arbitration_id, const uint8_t* data, const uint8_t size)
{
    memcpy(msgSendData,data,size * sizeof(char));
    msgSend.ui32MsgID = arbitration_id;
    g_sendFlag=1;
    return ISOTP_RET_OK;
}

UInt16 Can::readMsg( UInt16 commId, TimeDuration timeout, UInt32& len, OctetArray& payload, Boolean& last){
    len = receiveSize;
    payload = string((const char *)receiveBuffer,(size_t)receiveSize);

    return 0;
}


UInt16 Can::shutdown(){}
UInt16 Can::sleep(TimeDuration duration){}
UInt16 Can::wakeup(){}
UInt16 Can::setLocalConfiguration(ArgumentArray params ){}
UInt16 Can::getLocalConfiguration( ArgumentArray& params){}
UInt16 Can::sendLocalCommand(UInt8 cmdClassId, UInt8 cmdFunctionId, ArgumentArray inArgs, ArgumentArray& outArgs){}
UInt16 Can::describe( UInt8& logicalType, UInt8& physicalType, String& name){}

UInt16 Can::writeMsg(UInt16 commId, TimeDuration timeout, OctetArray payload, Boolean last, UInt16 msgId){
    extern uint32_t millis;
    uint32_t time = millis;
    while(millis-time < timeout.timeRepresentation.Secs/1000){
        UInt32 ret =  isotp_send(&g_link,(uint8_t* )payload.c_str(),payload.size());
        if (ISOTP_RET_OK == ret) {
            break;
        }        
    }

    return 0;


}

UInt16 Can::open( UInt16 destId, Boolean twoWay, UInt16& maxPayloadLen, UInt16& commId){

}

