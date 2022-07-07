/*
 * TIM.h
 *
 *  Created on: 07/07/2022
 *      Author: jmotp
 */

#ifndef TRANSDUCERSERVICES_TIM_H_
#define TRANSDUCERSERVICES_TIM_H_

#include <TransducerServices/TransducerChannelManager.h>
#include <TransducerServices/SPITransducerChannel.h>
#include <TransducerServices/CommandCodes.h>
#include <ti/sysbios/knl/Task.h>
#include <config.h>
#include <xdc/runtime/Error.h>
#include"EK_TM4C123GXL.h"
#include "driverlib/timer.h"


#include "ModuleCommunications/Comm.h"
#include "ModuleCommunications/Can.h"

#include "isotp/isotp_user.h"


#include "Util/Codec.h"



class TIM
{
public:
    TIM();
    virtual ~TIM();

    UInt16 handleCommand(Uint16 channelId,UInt8 cmdClassId,UInt8 cmdFunctionId,ArgumentArray inArgs, Boolean& hasResponse,ArgumentArray& outArgs);
    void task();

private:
    OctetArray TEDS;
    Codec codec;
    TransducerChannelManager transducerChannelManager;
};

#endif /* TRANSDUCERSERVICES_TIM_H_ */
