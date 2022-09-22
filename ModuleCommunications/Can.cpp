/*
 * Can.cpp
 *
 *  Created on: 14/06/2022
 *      Author: jmotp
 */

#include <ModuleCommunications/Can.h>
#include <cstdio>



uint32_t g_ui32MsgCount = 0;
uint32_t g_ui32MsgBroadcastCount = 0;
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
    //If the interrupt was caused by a STATUS Interrupt
    if(ui32Status == CAN_INT_INTID_STATUS)
    {
        //Save Control Flags Status & clears STATUS Interrupt
        ui32Status = CANStatusGet(CAN0_BASE, CAN_STS_CONTROL);
        //Save which Object raised the interrupt
        origin = CANIntStatus(CAN0_BASE, CAN_INT_STS_CAUSE);
        
        if(origin == 2 && (ui32Status & CAN_STS_TXOK)){
            g_clrToSend = 1; //Message of 1 has been transmitted
            CANIntClear(CAN0_BASE, 2);

        }
        if(origin == 1 && (ui32Status & CAN_STS_RXOK)){
            g_ui32MsgCount++;//Directed message has been received
            CANIntClear(CAN0_BASE, 1);

        }
        if(origin == 3 && (ui32Status & CAN_STS_RXOK)){
            g_ui32MsgBroadcastCount++;//Broadcasted Message has been received
            CANIntClear(CAN0_BASE, 3);

        }
    }
    else if(ui32Status == 1)//Interrupt generated by Message Object 1 update
    {
        g_bErrFlag = 0;
        CANIntClear(CAN0_BASE, 1);
    }
    else if(ui32Status == 2)//Interrupt generated by Message Object 2 update
    {
        g_bErrFlag = 0;
        CANIntClear(CAN0_BASE, 2);
    }
    else if(ui32Status == 3)//Interrupt generated by Message Object 3 update
    {
        g_bErrFlag = 0;
        CANIntClear(CAN0_BASE, 3);
    }
    else
    {
        g_bErrFlag = 1;//Signal Unexpected Interrupt
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
    msgReceive.ui32MsgID = 0x18da0201;
    msgReceive.ui32MsgIDMask = 0x0000FF00;
    msgReceive.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_EXT_FILTER | MSG_OBJ_USE_ID_FILTER | MSG_OBJ_EXTENDED_ID;
    msgReceive.ui32MsgLen = 8; // allow up to 8 bytes
    msgReceive.pui8MsgData = msgReceiveData;


    //msgRSend CAN Object Config
    msgSend.ui32MsgID = 1;
    msgSend.ui32MsgIDMask = 0;
    msgSend.ui32Flags = MSG_OBJ_TX_INT_ENABLE;
    msgSend.ui32MsgLen = 8; // allow up to 8 bytes
    msgSend.pui8MsgData = msgSendData;

    //msgReceiveBroadCast CAN Object Config
    msgReceiveBroadcast.ui32MsgID = 0x18da0201;
    msgReceiveBroadcast.ui32MsgIDMask = 0x0000FFFF;
    msgReceiveBroadcast.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_EXT_FILTER | MSG_OBJ_USE_ID_FILTER | MSG_OBJ_EXTENDED_ID;
    msgReceiveBroadcast.ui32MsgLen = 8; // allow up to 8 bytes
    msgReceiveBroadcast.pui8MsgData = msgReceiveBroadcastData;


    isotp_init_link(&g_linkBroadcast, 0x18da0102, g_isotpSendBuf, sizeof(g_isotpSendBuf), g_isotpRecvBuf, sizeof(g_isotpRecvBuf));


    return 0;
}



void Can::commTask(){
    SysTickbegin();
    CANMessageSet(CAN0_BASE, 3, &msgReceiveBroadcast, MSG_OBJ_TYPE_RX);
    //CANMessageSet(CAN0_BASE, 2, &msgSend, MSG_OBJ_TYPE_TX);
    UInt32 receiving_ret;
    UInt32 receivingBroadcast_ret;
    while(true){

        //System_printf("Polled\n");
        //printf("HERE\n");
        UInt32 canStatus = g_ui32MsgCount;
//        printf(" CanSTATUS %x\n", canStatus);
        if(g_ui32MsgCount&&glinkinit){
            CANMessageGet(CAN0_BASE, 1, &msgReceive, 0);
            //isotp_poll(&g_link);
            g_ui32MsgCount--;
//          System_printf("Message Received\n");
//          for(int i = 0 ; i < 8;i++){
//                  System_printf("%x ",msgReceiveData[i]);
//          }
//          System_printf("\n");
//          //System_flush();

           isotp_on_can_message(&g_link, msgReceiveData, msgReceive.ui32MsgLen);

        }
        
        if(g_ui32MsgBroadcastCount){
            CANMessageGet(CAN0_BASE, 3, &msgReceiveBroadcast, 0);
            //isotp_poll(&g_link);
            g_ui32MsgBroadcastCount--;
//          System_printf("Message Broadcast Received\n");
//          for(int i = 0 ; i < 8;i++){
//                  System_printf("%x ",msgReceiveBroadcastData[i]);
//          }
//          System_printf("\n");
//          //System_flush();


           isotp_on_can_message(&g_linkBroadcast, msgReceiveBroadcastData, msgReceiveBroadcast.ui32MsgLen);

        }
        if(glinkinit){
            isotp_poll(&g_link);
            receiving_ret = isotp_receive(&g_link, (uint8_t*) receiveBuffer, 512, &receiveSize);
        }

        isotp_poll(&g_linkBroadcast);
        receivingBroadcast_ret = isotp_receive(&g_linkBroadcast, (uint8_t*) receiveBroadcastBuffer, 32, &receiveBroadcastSize);

        if(receiving_ret==ISOTP_RET_OK){

            addMessage(0x01, (const char * ) receiveBuffer , (size_t) receiveSize);
        }
         if(receivingBroadcast_ret==ISOTP_RET_OK){
             addMessage(0x00, (const char * ) receiveBroadcastBuffer , (size_t) receiveBroadcastSize);
        }
        Task_sleep(1);

    }
}

UInt16 Can::addMessage(UInt8 srcId, const char * receiveBuffer, size_t bufferSize){
    CanMessage buffer;

    buffer.message = string((const char*)receiveBuffer+2,(size_t)bufferSize-2);
    UInt16 msgId;
    memcpy((void *)&msgId,receiveBuffer,2);
    swapByteOrder(msgId);
    buffer.msgId = msgId;
    buffer.srcId = srcId;
//    fprintf(stdout,"msgId @Reception %d\n",msgId);
//    fprintf(stdout,"Can Message Size: %d\n", buffer.message.size());
//    fprintf(stdout,"Message being added: ");
//    for(char chr: buffer.message){
//        fprintf(stdout," %x",chr);
//    }
//   fprintf(stdout,"\n");
    if(!(pendingResponse.find(msgId) == pendingResponse.end())){
        buffer.destId = CanReceiveArray[pendingResponse[msgId]].destId;
        CanReceiveArray[pendingResponse[msgId]] = buffer;
//        System_printf("Can Message Size: %d\n", CanReceiveArray[pendingResponse[msgId]].message.size());
        netReceive->notifyRsp(pendingResponse[msgId],msgId,buffer.message.size(),0,0,0);
    }else{
        netReceive->notifyMsg(nextCommId,msgId!=0,buffer.message.size(),0,0,0);
        CanReceiveArray.insert(std::pair<int,CanMessage>(nextCommId,buffer));
        nextCommId++;
    }
}

UInt16 Can::setId(UInt8 selfId){


    msgReceiveBroadcast.ui32MsgID = 0x18da0001;
    g_linkBroadcast.send_arbitration_id = 0x18da0100|selfId;
    msgReceive.ui32MsgID &= 0xFFFF00FF;
    msgReceive.ui32MsgID |= selfId << 8;
    CANMessageSet(CAN0_BASE, 3, &msgReceiveBroadcast, MSG_OBJ_TYPE_RX);
    CANMessageSet(CAN0_BASE, 1, &msgReceive, MSG_OBJ_TYPE_RX);
    isotp_init_link(&g_link, (0x18da0100 | (UInt32) selfId), g_isotpSendBuf, sizeof(g_isotpSendBuf), g_isotpRecvBuf, sizeof(g_isotpRecvBuf));
    glinkinit = true;

}

UInt8 Can::getId(void){
   
    return (UInt8)((msgReceive.ui32MsgID & 0x0000FF00) >> 8);


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

//    System_printf("Message Sent to id %x\n", (msgSend.ui32MsgID & 0x0000FF00)>>8);
//    for(int i = 0 ; i < 8;i++){
//            System_printf("%x ",msgSendData[i]);
//    }
//    System_printf("\n");
//
//    System_flush();
    
    return ISOTP_RET_OK;
}

UInt16 Can::readRsp(UInt16 commId, TimeDuration timeout, UInt16 msgId, UInt32 maxLen, OctetArray& payload, Boolean& last){
//    fprintf(stdout,"Response being read: ");
//        for(char chr:  CanReceiveArray[commId].message){
//            fprintf(stdout," %x",chr);
//        }
//       fprintf(stdout,"\n");
//        fflush(stdout);
    CanMessage canMessage= CanReceiveArray.at(commId);
    payload = canMessage.message;
    last = 0;
    return 0;
}

UInt16 Can::readMsg( UInt16 commId, TimeDuration timeout, UInt32& len, OctetArray& payload, Boolean& last){
//    fprintf(stdout,"Message being read: ");
//    for(char chr:  CanReceiveArray[commId].message){
//        fprintf(stdout," %x",chr);
//    }
//    fprintf(stdout,"\n");
//    fflush(stdout);
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
    Boolean isTimeout = 1;
    CanMessage canMessage =  CanReceiveArray[commId];
//    System_printf("WriteMsg %d\n", msgId);

    canMessage.msgId = msgId;
    swapByteOrder(msgId);

    payload.insert(0,(const char *)&msgId,2);
    swapByteOrder(msgId);
    if(canMessage.destId == 0x00){
        UInt32 ret =  isotp_send(&g_linkBroadcast,(uint8_t* )payload.c_str(),payload.size());
        while(millis-time < timeout.timeRepresentation.Secs*1000){
            Task_yield();
            if (ISOTP_RET_OK == ret && g_linkBroadcast.send_status==ISOTP_SEND_STATUS_IDLE) {
//                System_printf("WriteMsg %d\n", ret);
//                System_flush();
                isTimeout=0;
                break;
            }
        }
    }else
    {
        UInt32 ret =  isotp_send(&g_link,(uint8_t* )payload.c_str(),payload.size());
        while(millis-time < timeout.timeRepresentation.Secs*1000){
            Task_yield();
            if (ISOTP_RET_OK == ret && g_link.send_status==ISOTP_SEND_STATUS_IDLE) {
//                System_printf("WriteMsg %d\n", ret);
//                System_flush();
                isTimeout=0;
                break;
            }
        }
    }

    CanReceiveArray.erase(CanReceiveArray.find(commId));


    if(msgId==0x00){
    }else{
//        System_printf("Inserted msgId %d commId %d\n",msgId,commId);
        pendingResponse.insert(std::pair<int,int>(msgId,commId));
    }

    if(isTimeout){
        System_printf("Timeout\n");
        System_flush();

    }



}

UInt16 Can::writeRsp(UInt16 commId, TimeDuration timeout, OctetArray payload,  Boolean last){

    extern uint32_t millis;
    uint32_t time = millis;
    Boolean isTimeout = 1;
    CanMessage canMessage =  CanReceiveArray[commId];
    UInt16 msgId = canMessage.msgId;
    swapByteOrder(msgId);
    payload.insert(0,(const char *)&msgId,2);
//    if(canMessage.srcId == 0x00){
//        UInt32 ret =  isotp_send(&g_linkBroadcast,(uint8_t* )payload.c_str(),payload.size());
//        while(millis-time < timeout.timeRepresentation.Secs*1000){
//            Task_yield();
//            if (ISOTP_RET_OK == ret && g_link.send_status==ISOTP_SEND_STATUS_IDLE) {
////                System_printf("WriteRsp %d\n", ret);
////                System_flush();
//                isTimeout=0;
//                break;
//            }
//        }
//    }else
    {


        UInt32 ret =  isotp_send(&g_link,(uint8_t* )payload.c_str(),payload.size());
        while(millis-time < timeout.timeRepresentation.Secs*1000){
            Task_yield();
            if (ISOTP_RET_OK == ret && g_link.send_status==ISOTP_SEND_STATUS_IDLE) {
//                System_printf("WriteRsp %d\n", ret);
//                System_flush();
                isTimeout=0;
                break;
            }
        }
    }

    CanReceiveArray.erase(CanReceiveArray.find(commId));

    if(isTimeout){
        System_printf("Timeout\n");
        System_flush();

    }



    return 0;
}


UInt16 Can::open( UInt16 destId, Boolean twoWay, UInt16& maxPayloadLen, UInt16& commId){
    commId = nextCommId++;
    CanMessage canMessage;
    canMessage.destId = destId;
    canMessage.msgId = twoWay? nextOutmsgId++: 0;
    CanReceiveArray.insert(std::pair<int,CanMessage>(commId,canMessage));
}

UInt16 Can::openQoS( UInt16 destId, Boolean twoWay, UInt16& maxPayloadLen, UInt16& commId, QosParams& qosParams){}
UInt16 Can::close( UInt16 commId){
    CanReceiveArray.erase(commId);

}
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


