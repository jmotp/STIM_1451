/*
 * TransducerChannel.h
 *
 *  Created on: 04/07/2022
 *      Author: jmotp
 */

#ifndef TRANSDUCERSERVICES_TRANSDUCERCHANNEL_H_
#define TRANSDUCERSERVICES_TRANSDUCERCHANNEL_H_

#include <Args/Types.h>
#include <Args/Argument.h>
#include <Args/ArgumentArray.h>
#include <Util/Codec.h>


class TransducerChannel
{
private:
    OctetArray TransducerChannelTEDS;
public:
    void TransducerChannelTask();
    OctetArray getTransducerChannelTEDS();
    UInt16 setTransducerChannelTEDS(OctetArray TEDS);
     virtual UInt16 getDataSet(Argument &dataSet)=0;
    TransducerChannel();
    virtual ~TransducerChannel();
};

#endif /* TRANSDUCERSERVICES_TRANSDUCERCHANNEL_H_ */
