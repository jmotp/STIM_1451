/*
 * NetComm.h
 *
 *  Created on: 14/06/2022
 *      Author: jmotp
 */

#ifndef MODULECOMMUNICATIONS_NETCOMM_H_
#define MODULECOMMUNICATIONS_NETCOMM_H_

#include "Comm.h"

class NetComm:Comm
{
public:
    NetComm();
    virtual ~NetComm();

    //Comm Functions
    virtual UInt16 init()=0;
    virtual UInt16 shutdown() =0;
    virtual UInt16 sleep(TimeDuration duration)=0;
    virtual UInt16 wakeup()=0;
    virtual UInt16 setLocalConfiguration(ArgumentArray params )=0;
    virtual UInt16 getLocalConfiguration( ArgumentArray& params)=0;
    virtual UInt16 sendLocalCommand(UInt8 cmdClassId, UInt8 cmdFunctionId, ArgumentArray inArgs, ArgumentArray& outArgs)=0;
    virtual UInt16 describe( UInt8& logicalType, UInt8& physicalType, String& name)=0;

    //NetComm Functions -> To be overloaded 
    virtual UInt16 open( UInt16 destId, Boolean twoWay, UInt16& maxPayloadLen, UInt16& commId);
    virtual UInt16 openQoS( UInt16 destId, Boolean twoWay, UInt16& maxPayloadLen, UInt16& commId, QosParams& qosParams);
    virtual UInt16 close( UInt16 commId);
    virtual UInt16 readMsg( UInt16 commId, TimeDuration timeout, UInt32 len, OctetArray& payload, Boolean& last);
    virtual UInt16 readRsp(UInt16 commId, TimeDuration timeout, UInt16 msgId, UInt32 maxLen, OctetArray& payload, Boolean& last);
    virtual UInt16 writeMsg(UInt16 commId, TimeDuration timeout, OctetArray payload,  Boolean last, UInt16 msgId);
    virtual UInt16 writeRsp(UInt16 commId, TimeDuration timeout, OctetArray payload,  Boolean last);
    virtual UInt16 flush( UInt16 commId);
    virtual UInt16 readSize( UInt16 commId, UInt32& cacheSize);
    virtual UInt16 setPayloadSize( UInt16 commId, UInt32 size);
    virtual UInt16 abort(UInt16 commId);
    virtual UInt16 commStatus(UInt16 commId, UInt16 msgId, UInt16& statusCode );
    virtual UInt16 discoverDestinations( );
    virtual UInt16 joinGroup( UInt16 groupId, UInt16 destId);
    virtual UInt16 leaveGroup( UInt16 groupId, UInt16 destId);
    virtual UInt16 lookupDestId( UInt16 commId, UInt16& destId);
    virtual UInt16 setRemoteConfiguration( UInt16 commId, TimeDuration timeout, ArgumentArray params);
    virtual UInt16 getRemoteConfiguration( UInt16 commId, TimeDuration timeout, ArgumentArray& params);
    virtual UInt16 sendRemoteCommand(UInt16 commId, TimeDuration timeout, UInt8 cmdClassId, UInt8 cmdFunctionId, ArgumentArray inArgs, ArgumentArray& outArgs);


};

#endif /* MODULECOMMUNICATIONS_NETCOMM_H_ */
