/*
 * Handler.cpp
 *
 *  Created on: 21/06/2022
 *      Author: jmotp
 */

#include <TransducerServices/Handler.h>
#include <memory>

Handler::Handler()
{
    vector<char> vec = {0x00,0x00,0x00,0x24,0x03,0x04,0x00,0x01,0x01,0x01,0x04,0x0a,0x81,0xc0,0xf9,0x74,0x48,0x81,0xf5,0x62,0x2e,0x78,0x0a,0x04,0x3f,0x00,0x00,0x00,0x0c,0x04,0xc0,0xa0,0x00,0x00,0x0d,0x02,0x00,0x01,0xf8,0x82};
    TEDS.append(vec.begin(),vec.end());
    // TODO Auto-generated constructor stub

}

Handler::~Handler()
{
    // TODO Auto-generated destructor stub
}

UInt16 Handler::handleCommand(UInt8 cmdClassId,UInt8 cmdFunctionId,ArgumentArray inArgs, Boolean& hasResponse,ArgumentArray& outArgs){
    if(cmdClassId == COMMON_CMD){
        if(cmdFunctionId == READ_TEDS_SEGMENT){
            hasResponse = 1;
//            System_printf("TEDS_arg type: %d\n", TEDS_arg->type);
            UInt32 offset = 0;
             Argument TEDS_offset(Argument::UInt32_TC,(void*)&offset);
             static Argument TEDS_arg(Argument::Octet_Array_TC, (void *)&TEDS);

            outArgs.putByIndex(0,TEDS_offset);
            outArgs.putByIndex(1, TEDS_arg);

            System_printf("outArgs size: %d\n", outArgs.size());
            System_flush();



        }
    }
    return 0;
}
