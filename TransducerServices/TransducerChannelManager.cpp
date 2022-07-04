/*
 * TransducerChannelManager.cpp
 *
 *  Created on: 04/07/2022
 *      Author: jmotp
 */

#include <TransducerServices/TransducerChannelManager.h>

TransducerChannelManager::TransducerChannelManager()
{
    // TODO Auto-generated constructor stub

}

TransducerChannelManager::~TransducerChannelManager()
{
    // TODO Auto-generated destructor stub
}

UInt16 TransducerChannelManager::registerTransducerChannel(const TransducerChannel& transducerChannel){
    const TransducerChannel & transducerChannelRef = transducerChannel;
    transducerChannelArray.insert(std::pair<int,TransducerChannel>(nextChannelId,transducerChannel));
    return 0;
}

TransducerChannel & TransducerChannelManager::getTransducerChannel(UInt16 transducerChannelID){
    return transducerChannelArray.at(transducerChannelID);
}

