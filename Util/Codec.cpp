/*
 * Codec.cpp
 *
 *  Created on: 03/06/2022
 *      Author: jmotp
 */

#include "Util/Codec.h"

Codec::Codec()
{
    // TODO Auto-generated constructor stub

}

Codec::~Codec()
{
    // TODO Auto-generated destructor stub
}

UInt16 Codec::encodeCommand(UInt16 channelId, UInt8 cmdClassId, UInt8 cmdFunctionId, ArgumentArray inArgs, OctetArray& payload){

    return 0;
}


UInt16 Codec::encodeResponse(Boolean successFlag,ArgumentArray& outArgs, OctetArray& payload){
    std::stringstream sstream;
    OctetArray buffer;
    sstream.write((const char*)&successFlag,1);
    argumentArray2OctetArray(outArgs,buffer);
    fprintf(stdout,"Octet Array size: %d\n", buffer.size());
    UInt16 len= buffer.size();
    swapByteOrder(len);
    sstream.write((const char *)&len,2);
    sstream.write(buffer.c_str(),buffer.size());
    payload = sstream.str();

    for(char chr: payload){
           fprintf(stdout," %x",chr);
       }
   fprintf(stdout,"\n");
   fflush(stdout);

    return 0;


}



UInt16 Codec::decodeCommand(OctetArray payload, UInt16& channelId, UInt8& cmdClassId, UInt8& cmdFunctionId, ArgumentArray& inArgs){

    channelId =payload[0]*256 + payload[1];
    cmdClassId =payload[2];
    cmdFunctionId =payload[3];

    UInt16 len = payload[4]*256 + payload[5];

    if(len>0){
        octetArray2ArgumentArray(inArgs,payload);
    }

    return 0;
}


 UInt16 Codec::argumentArray2OctetArray(ArgumentArray& inArgs, OctetArray& payload){
    fprintf(stdout,"argumentArray2OctetArray inArgs size %d\n", inArgs.size());
    std::stringstream sstream;
    vector<UInt16> inArgsIndexes;

    inArgs.getIndexes(inArgsIndexes);
    String result;
    Argument arg;
    fprintf(stdout,"argumentArray2OctetArray inArgs size %d\n", inArgs.size());
    for (UInt16 index : inArgsIndexes){
        inArgs.getByIndex(index, arg);
        arg.print();
        arg.write(sstream);
    }

    payload = sstream.str();

    for(char chr: payload){
        fprintf(stdout," %x",chr);
    }
   fprintf(stdout,"\n");
    fflush(stdout);


    return 0;
}

 UInt16 Codec::octetArray2ArgumentArray(ArgumentArray& inArgs, OctetArray payload){
    return 0;
}


