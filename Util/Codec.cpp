/*
 * Codec.cpp
 *
 *  Created on: 03/06/2022
 *      Author: jmotp
 */

#include <Util/Codec.h>

Codec::Codec()
{
    // TODO Auto-generated constructor stub

}

Codec::~Codec()
{
    // TODO Auto-generated destructor stub
}

UInt16 Codec::encodeCommand(UInt16 destId, UInt16 channelId, UInt8 cmdClassId, UInt8 cmdFunctionId, ArgumentArray inArgs, OctetArray& payload){

    return 0;
}



UInt16 Codec::decodeCommand(OctetArray payload, UInt16& destId, UInt16& channelId, UInt8& cmdClassId, UInt8& cmdFunctionId, ArgumentArray& inArgs){

    return 0;
}


 UInt16 argumentArray2OctetArray(ArgumentArray inArgs, OctetArray& payload){

    std::stringstream sstream;
    String _payload("");
    vector<UInt16> inArgsIndexes;

    inArgs.getIndexes(inArgsIndexes);
    String result;
    Argument arg;
    for (UInt16 index : inArgsIndexes){
        inArgs.getByIndex(index, arg);
        arg.write(sstream);
    }

    sstream>>_payload;

    for(char chr: _payload){
        System_printf(" %x",chr);
    }
    System_printf("\n");
    System_flush();


    return 0;
}

