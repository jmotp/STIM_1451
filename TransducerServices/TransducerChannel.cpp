/*
 * TransducerChannel.cpp
 *
 *  Created on: 04/07/2022
 *      Author: jmotp
 */

#include <TransducerServices/TransducerChannel.h>

Codec codec0;

void TransducerChannel::TransducerChannelTask(){

}

OctetArray TransducerChannel::getTransducerChannelTEDS(){
    OctetArray outputTEDS;
    codec0.argumentArray2OctetArray(TransducerChannelTEDS, outputTEDS);

    return outputTEDS;
}


UInt16 TransducerChannel::setTransducerChannelTEDS(OctetArray TEDS){
    codec0.octetArray2ArgumentArray(TransducerChannelTEDS, TEDS);
}

TransducerChannel::TransducerChannel()
{
    
    // TODO Auto-generated constructor stub

}

TransducerChannel::~TransducerChannel()
{
    // TODO Auto-generated destructor stub
}

