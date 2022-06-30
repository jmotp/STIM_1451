/*
 * Can.h
 *
 *  Created on: 14/06/2022
 *      Author: jmotp
 */

#ifndef MODULECOMMUNICATIONS_CAN_H_
#define MODULECOMMUNICATIONS_CAN_H_

#include "NetComm.h"

    #include <isotp/isotp.h>

#include <ti/sysbios/knl/Task.h>
#include <queue>
#include <vector>
#include <isotp/isotp.h>
#include <config.h>

/* Alloc IsoTpLink statically in RAM */
    static IsoTpLink g_link;

    /* Alloc send and receive buffer statically in RAM */
    static uint8_t g_isotpRecvBuf[ISOTP_BUFSIZE];
    static uint8_t g_isotpSendBuf[ISOTP_BUFSIZE];


typedef struct CanMessage_t{
        UInt16 msgId;
        UInt16 destId;
        OctetArray message;
  }CanMessage;


class Can : public NetComm
{
public:

    Can();
    virtual ~Can();
    void commTask();
    //static void * commTaskWrapper(void * can_object);
    UInt16 init();
    UInt16 shutdown() ;
    UInt16 sleep(TimeDuration duration) ;
    UInt16 wakeup();
    UInt16 setLocalConfiguration(ArgumentArray params );
    UInt16 getLocalConfiguration( ArgumentArray& params);
    UInt16 sendLocalCommand(UInt8 cmdClassId, UInt8 cmdFunctionId, ArgumentArray inArgs, ArgumentArray& outArgs);
    UInt16 describe( UInt8& logicalType, UInt8& physicalType, String& name);

    UInt16 sendMessage(const uint32_t arbitration_id, const uint8_t* data, const uint8_t size);

    UInt16 open( UInt16 destId, Boolean twoWay, UInt16& maxPayloadLen, UInt16& commId);
    UInt16 openQoS( UInt16 destId, Boolean twoWay, UInt16& maxPayloadLen, UInt16& commId, QosParams& qosParams);
    UInt16 close( UInt16 commId);
    UInt16 readMsg( UInt16 commId, TimeDuration timeout, UInt32& len, OctetArray& payload, Boolean& last);

    UInt16 readRsp(UInt16 commId, TimeDuration timeout, UInt16 msgId, UInt32 maxLen, OctetArray& payload, Boolean& last);
    UInt16 writeMsg(UInt16 commId, TimeDuration timeout, OctetArray payload,  Boolean last, UInt16 msgId);
    UInt16 writeRsp(UInt16 commId, TimeDuration timeout, OctetArray payload,  Boolean last);
    UInt16 flush( UInt16 commId);
    UInt16 readSize( UInt16 commId, UInt32& cacheSize);
    UInt16 setPayloadSize( UInt16 commId, UInt32 size);
    UInt16 abort(UInt16 commId);
    UInt16 commStatus(UInt16 commId, UInt16 msgId, UInt16& statusCode );
    UInt16 discoverDestinations();
    UInt16 joinGroup( UInt16 groupId, UInt16 destId);
    UInt16 leaveGroup( UInt16 groupId, UInt16 destId);
    UInt16 lookupDestId( UInt16 commId, UInt16& destId);
    UInt16 setRemoteConfiguration( UInt16 commId, TimeDuration timeout, ArgumentArray params);
    UInt16 getRemoteConfiguration( UInt16 commId, TimeDuration timeout, ArgumentArray& params);
    UInt16 sendRemoteCommand(UInt16 commId, TimeDuration timeout, UInt8 cmdClassId, UInt8 cmdFunctionId, ArgumentArray inArgs, ArgumentArray& outArgs);

   // virtual UInt16 registerNetReceive(NetReceive &netReceive);



private:
    //NetReceive netReceive;

    //the CAN RECEIVE message Object
    tCANMsgObject msgReceive;
    unsigned char msgReceiveData[8];
    
    std::queue<CanMessage> CanQueueSend;

    std::vector<CanMessage> CanReceiveArray;

    //the CAN SEND message Object
    tCANMsgObject msgSend;
    unsigned char msgSendData[8];

    unsigned char sendBuffer[512];
    unsigned char receiveBuffer[512];
    UInt16 receiveSize;
    UInt32 sendSize;

    UInt16 nextCommId=0;


    void delay(unsigned int milliseconds) {
        SysCtlDelay((50000000 / 3) * (milliseconds / 1000.0f));
    }





};

#endif /* MODULECOMMUNICATIONS_CAN_H_ */
