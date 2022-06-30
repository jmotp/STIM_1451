/*
 * Codec.h
 *
 *  Created on: 03/06/2022
 *      Author: jmotp
 */

#ifndef UTIL_CODEC_H_
#define UTIL_CODEC_H_

#include "../Args/Argument.h"
#include "../Args/ArgumentArray.h"
#include <istream>
#include <sstream>

class Codec
{
public:
    Codec();
    virtual ~Codec();

    UInt16 encodeCommand(UInt16 channelId, UInt8 cmdClassId, UInt8 cmdFunctionId, ArgumentArray inArgs, OctetArray& payload);

    UInt16 decodeCommand(OctetArray payload, UInt16& channelId, UInt8& cmdClassId, UInt8& cmdFunctionId, ArgumentArray& inArgs);

    UInt16 encodeResponse(Boolean successFlag,ArgumentArray& outArgs, OctetArray& payload);

    UInt16 argumentArray2OctetArray(ArgumentArray& inArgs, OctetArray& payload);

    UInt16 octetArray2ArgumentArray(ArgumentArray& inArgs, OctetArray payload);

};



#endif /* UTIL_CODEC_H_ */
