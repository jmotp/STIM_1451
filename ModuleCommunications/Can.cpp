/*
 * Can.cpp
 *
 *  Created on: 14/06/2022
 *      Author: jmotp
 */

#include <ModuleCommunications/Can.h>
#include <cstdio>



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
    uint32_t origin;
    ui32Status = CANIntStatus(CAN0_BASE, CAN_INT_STS_CAUSE);

    if(ui32Status == CAN_INT_INTID_STATUS)
    {

        ui32Status = CANStatusGet(CAN0_BASE, CAN_STS_CONTROL);
        
        origin = CANIntStatus(CAN0_BASE, CAN_INT_STS_CAUSE);
        //printf("Status %x\n", origin);
        if(origin == 2 && (ui32Status & CAN_STS_TXOK)){
            g_clrToSend = 1;
            System_printf("Sent Message\n");
        }
        if(origin == 1 && (ui32Status & CAN_STS_RXOK)){
            g_ui32MsgCount++;
        }
        g_bErrFlag = 1;
    }
    else if(ui32Status == 1)
    {
        g_bErrFlag = 0;
        CANIntClear(CAN0_BASE, 1);
        //g_ui32MsgCount++;

    }
    else if(ui32Status == 2)
    {
        g_bErrFlag = 0;
        CANIntClear(CAN0_BASE, 2);
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





    //SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

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

    isotp_init_link(&g_link, 0x18da0102, g_isotpSendBuf, sizeof(g_isotpSendBuf), g_isotpRecvBuf, sizeof(g_isotpRecvBuf));

    /* Construct clock Task thread */

    return 0;
}



void Can::commTask(){
    SysTickbegin();
    CANMessageSet(CAN0_BASE, 1, &msgReceive, MSG_OBJ_TYPE_RX);
    //CANMessageSet(CAN0_BASE, 2, &msgSend, MSG_OBJ_TYPE_TX);
    uint32_t ret;
    UInt32 receiving_ret;
    while(true){

        //System_printf("Polled\n");
        //printf("HERE\n");
        UInt32 canStatus = g_ui32MsgCount;
//        printf(" CanSTATUS %x\n", canStatus);
        if(g_ui32MsgCount){
            CANMessageGet(CAN0_BASE, 1, &msgReceive, 0);
            //isotp_poll(&g_link);
            g_ui32MsgCount--;
           System_printf("Message Received\n");
           for(int i = 0 ; i < 8;i++){
                   System_printf("%x ",msgReceiveData[i]);
           }
           System_printf("\n");
           System_flush();

           isotp_on_can_message(&g_link, msgReceiveData, msgReceive.ui32MsgLen);

        }
        
        isotp_poll(&g_link);
              if(g_link.send_status == ISOTP_SEND_STATUS_INPROGRESS){
//                  System_printf("Polled");
              }


         
        receiving_ret = isotp_receive(&g_link, (uint8_t*) receiveBuffer, 512, &receiveSize);
        if(receivingCommState == RECEIVE_IDLE && canStatus){
            nextReceivingCommState = RECEIVING;
        }else if(receivingCommState == RECEIVING && receiving_ret==ISOTP_RET_OK){
            nextReceivingCommState = RECEIVE_IDLE;
        } if(receivingCommState == RECEIVING && canStatus){
            nextReceivingCommState = RECEIVING;
        }

        if(receiving_ret==ISOTP_RET_OK||(receivingCommState==RECEIVING&&nextReceivingCommState==RECEIVE_IDLE)){
            CanMessage buffer;

            buffer.message = string((const char*)receiveBuffer+2,(size_t)receiveSize-2);
            //buffer.destId = msgReceive.ui32MsgID;
            UInt16 msgId;
            memcpy((void *)&msgId,receiveBuffer,2);
            swapByteOrder(msgId);
            buffer.msgId = msgId;
            printf("msgId @Reception %x\n",msgId);

//            printf("msgId @Reception %x\n",nextCommId);

            CanReceiveArray.insert(std::pair<int,CanMessage>(nextCommId,buffer));
            netReceive->notifyMsg(nextCommId,msgId!=0,buffer.message.size(),0,0,0);
//            printf("msgId @Reception %x\n",msgId);
            nextCommId++;
        }

        receivingCommState = nextReceivingCommState;



        //System_printf("Can running\n");
//        System_flush();
//         if(g_clrToSend && g_sendFlag){
// //            printf("Message Sent to id %d\n", msgSend.ui32MsgID);
// //            for(int i = 0 ; i < 8;i++){
// //                    printf("%x ",msgSendData[i]);
// //            }
// //            printf("\n");
// //            System_flush();
//             CANMessageSet(CAN0_BASE, 2, &msgSend, MSG_OBJ_TYPE_TX); // send as msg object 2

//             g_sendFlag=0;
//             g_clrToSend = 0;

//         }


            //isotp_on_can_message(&g_link, msgReceiveData, msgReceive.ui32MsgLen);










//
//        if(ISOTP_RET_OK == receiving_ret){
//
//            CanMessage buffer;
//
//            buffer.message = string((const char*)receiveBuffer+2,(size_t)receiveSize-2);
//            //buffer.destId = msgReceive.ui32MsgID;
//            UInt16 msgId;
//            memcpy((void *)&msgId,receiveBuffer,2);
//            swapByteOrder(msgId);
//            buffer.msgId = msgId;
////            printf("msgId @Reception %x\n",msgId);
//
////            printf("msgId @Reception %x\n",nextCommId);
//
//            CanReceiveArray.insert(std::pair<int,CanMessage>(nextCommId,buffer));
//            netReceive->notifyMsg(nextCommId,msgId!=0,buffer.message.size(),0,0,0);
////            printf("msgId @Reception %x\n",msgId);
//            nextCommId++;
//
//
//        }


        //System_flush();
        Task_sleep(10);
    }
}


UInt16 Can::sendMessage(const uint32_t arbitration_id, const uint8_t* data, const uint8_t size)
{
    while(g_sendFlag);
    g_sendFlag=1;

    //while(!g_clrToSend);

    while(!(g_clrToSend));
    memcpy(msgSendData,data,size * sizeof(char));
    msgSend.ui32MsgID = arbitration_id;
    g_clrToSend = 0;
    CANMessageSet(CAN0_BASE, 2, &msgSend, MSG_OBJ_TYPE_TX); // send as msg object 2
    
    //g_sendFlag=0;


    while(!g_clrToSend);

    g_sendFlag=0;

    System_printf("Message Sent to id %d\n", msgSend.ui32MsgID);
    for(int i = 0 ; i < 8;i++){
            System_printf("%x ",msgSendData[i]);
    }
    System_printf("\n");

    System_flush();
    
    return ISOTP_RET_OK;
}

UInt16 Can::readMsg( UInt16 commId, TimeDuration timeout, UInt32& len, OctetArray& payload, Boolean& last){
    payload = CanReceiveArray[commId].message;
    last = 0;
    len = payload.size();
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
        if (ISOTP_RET_OK == ret && g_link.send_status==ISOTP_SEND_STATUS_IDLE) {
            System_flush();
            break;
        }      
    }
    System_printf("Timeout\n");
    System_flush();

    return 0;


}

UInt16 Can::writeRsp(UInt16 commId, TimeDuration timeout, OctetArray payload,  Boolean last){

    CanMessage canMessage;
    canMessage.destId = commId;
    canMessage.message = payload;
    //canMessage.size = payload.size();
    CanQueueSend.push(canMessage);
    extern uint32_t millis;
    uint32_t time = millis;
    Boolean isTimeout = 1;
    UInt16 msgId = CanReceiveArray[commId].msgId;
    CanReceiveArray.erase(CanReceiveArray.find(commId));
    //printf("%x\n",msgId);
    swapByteOrder(msgId);
    //printf("%x\n",msgId);
    payload.insert(0,(const char *)&msgId,2);
    //payload.insert(payload.begin(),(char)1);
    //payload.insert(payload.begin(),(char)0);

    UInt32 ret =  isotp_send(&g_link,(uint8_t* )payload.c_str(),payload.size());
    while(millis-time < timeout.timeRepresentation.Secs*1000){
        Task_yield();
        if (ISOTP_RET_OK == ret && g_link.send_status==ISOTP_SEND_STATUS_IDLE) {
            System_printf("WriteRsp %d\n", ret);
            System_flush();
            isTimeout=0;
            break;
        }
    }
    if(isTimeout){
        System_printf("Timeout\n");
        System_flush();

    }



    return 0;
}


UInt16 Can::open( UInt16 destId, Boolean twoWay, UInt16& maxPayloadLen, UInt16& commId){}

UInt16 Can::openQoS( UInt16 destId, Boolean twoWay, UInt16& maxPayloadLen, UInt16& commId, QosParams& qosParams){}
UInt16 Can::close( UInt16 commId){}

UInt16 Can::flush( UInt16 commId){}
    UInt16 Can::readSize( UInt16 commId, UInt32& cacheSize){}
    UInt16 Can::setPayloadSize( UInt16 commId, UInt32 size){}
    UInt16 Can::abort(UInt16 commId){}
    UInt16 Can::commStatus(UInt16 commId, UInt16 msgId, UInt16& statusCode ){}
    UInt16 Can::discoverDestinations(){}
    UInt16 Can::joinGroup( UInt16 groupId, UInt16 destId){}
    UInt16 Can::leaveGroup( UInt16 groupId, UInt16 destId){}
    UInt16 Can::lookupDestId( UInt16 commId, UInt16& destId){}
    UInt16 Can::setRemoteConfiguration( UInt16 commId, TimeDuration timeout, ArgumentArray params){}
    UInt16 Can::getRemoteConfiguration( UInt16 commId, TimeDuration timeout, ArgumentArray& params){}
    UInt16 Can::sendRemoteCommand(UInt16 commId, TimeDuration timeout, UInt8 cmdClassId, UInt8 cmdFunctionId, ArgumentArray inArgs, ArgumentArray& outArgs){}

    UInt16 Can::readRsp(UInt16 commId, TimeDuration timeout, UInt16 msgId, UInt32 maxLen, OctetArray& payload, Boolean& last){}
