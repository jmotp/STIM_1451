/*
 * TIM.cpp
 *
 *  Created on: 07/07/2022
 *      Author: jmotp
 */

#include <TransducerServices/TIM.h>

TIM::TIM()
{
    vector<unsigned char> vec = {0x00,0x00,0x00,0x24,0x03,0x04,0x00,0x01,0x01,0x01,0x04,0x0a,0x81,0xc0,0xf9,0x74,0x48,0x81,0xf5,0x62,0x2e,0x78,0x0a,0x04,0x3f,0x00,0x00,0x00,0x0c,0x04,0xc0,0xa0,0x00,0x00,0x0d,0x02,0x00,0x01,0xf8,0x82};
    TEDS.append(vec.begin(),vec.end());
    SPITransducerChannel * tchannel = new SPITransducerChannel();
    transducerChannelManager.registerTransducerChannel(*tchannel);



}

TIM::~TIM()
{
    // TODO Auto-generated destructor stub
}

UInt16 TIM::handleCommand(UInt16 channelId,UInt8 cmdClassId,UInt8 cmdFunctionId,ArgumentArray inArgs, Boolean& hasResponse,ArgumentArray& outArgs){
    TransducerChannel* transducerChannel = NULL;
//    System_printf("Channel Id %d\n", channelId);

    if(channelId != 0){
        transducerChannel = transducerChannelManager.getTransducerChannel(channelId);
    }

    if(cmdClassId == COMMON_CMD){
        if(cmdFunctionId == READ_TEDS_SEGMENT){

            Argument TEDSAccessCode_arg;
            inArgs.getByIndex(0,TEDSAccessCode_arg);
            UInt8 TEDSAccessCode = TEDSAccessCode_arg._valueUInt8;
            UInt32 offset = 0;
            outArgs.putByIndex(0, Argument(Argument::UInt32_TC,(void*)&offset));
            switch(TEDSAccessCode){
                case 1:
                    outArgs.putByIndex(1, Argument(Argument::Octet_Array_TC, (void *)&TEDS));
                    break;
                case 3:
                    if(channelId == 0 ){
//                        System_printf("Channel Id == 0\n");
                        System_flush();
                    }else{
                        OctetArray TransducerChannelTEDS = transducerChannel->getTransducerChannelTEDS();
                        outArgs.putByIndex(1, Argument(Argument::Octet_Array_TC, (void *)&TransducerChannelTEDS));
                    }
                    break;

            }
            hasResponse = 1;

            
        }
    }else if(cmdClassId == XDCR_OPERATE){
        if(cmdFunctionId == READ_TRANSDUCERCHANNEL_DATA){
            hasResponse=1;
            Argument arg;
            if(channelId==0){

            }else{
                transducerChannel->getDataSet(arg);
                UInt32 offset = 0;
                outArgs.putByIndex(0, Argument(Argument::UInt32_TC,(void*)&offset));
                outArgs.putByIndex(1, arg);
            }

        }
    }else if(cmdClassId == REGISTER_CMD){
            if(cmdFunctionId == DISCOVERY){
                hasResponse=1;
                extern Can can0;
                UInt8 canId= can0.getId();
                System_printf("Discovery from NCAP: sending Id %d", canId);
                outArgs.putByIndex(0, Argument(Argument::UInt8_TC,(void*)&canId));
            }
        }
    return 0;
}

void TIM::task(){
    extern Can can0;
    extern NetReceive netReceive;
    SPITransducerChannel * tchannel = new SPITransducerChannel();
    transducerChannelManager.registerTransducerChannel(*tchannel);
    ArgumentArray outArgs;
    UInt32 ui32value;
    ADCSequenceDataGet(ADC0_BASE, 0, &ui32value);

    UInt32 random= (UInt8)ui32value;//TimerValueGet (TIMER5_BASE,TIMER_A);
    outArgs.putByIndex(0, Argument(Argument::UInt32_TC,&random));
    OctetArray payload;
    codec.encodeHandshakeMessage(0x00, 0x80, 0x01, (UInt8)random, payload);
    UInt16 maxPayloadLen;
    UInt16 commId = 0;
    can0.open(0x00,true,maxPayloadLen,commId);
    Boolean registerProcessFinished = false;
    fprintf(stdout,"Tim initiated\n");
    fflush(stdout);

    Argument argforread;
//    extern uint32_t micros;
//    UInt32 time=micros;
//
//    for(int i=0;i<100;i++){
//        time=micros;
//        tchannel->getDataSet(argforread);
//        time = micros-time;
//        fprintf(stdout,"%lu\n",time);
//    }
//
//    fflush(stdout);

    int counter = 0;
    while(!registerProcessFinished){

        can0.writeMsg(commId, TimeDuration{1,0}, payload, true, 0x1001);
        System_printf("Trying Registration\n");
        extern UInt32 millis;
        UInt32 time = millis;
        while(1){
            OctetArray buffer("");
            static uint32_t len;
            static Boolean buf_bool;
            if(netReceive.responseAvailable()){
                ResponseIncomingInfo message = netReceive.getResponseIncomingInfo();
                can0.readRsp(message.rcvCommId, TimeDuration{0,0}, message.msgId, len , buffer, buf_bool);
                Boolean successFlag;
                UInt8 random_from_ncap, id;
                codec.decodeHandshakeResponse(buffer, successFlag, random_from_ncap,id);
                //printf("Received random from ncap %x %x\n", random, random_from_ncap);
                if((UInt8)random == random_from_ncap){

                    can0.setId(id);
                    registerProcessFinished = true;
//                    extern uint32_t time_u;
//                    extern uint32_t micros;
//                    time_u= micros-time_u;
//                    fprintf(stdout,"%lu\n",time_u);
//                    fflush(stdout);

//                    if(netReceive.messageAvailable()){
//                        MessageIncomingInfo message = netReceive.getMessageIncomingInfo();
//                        can0.readMsg(message.rcvCommId, TimeDuration{0,0},len , buffer, buf_bool);
//                    }
                    break;
                }else{

                }
                

            }
            if(millis - time > TIMEOUT_REGISTRATION){
                break;
            }
        }
    }

    System_printf("Registering...\n");
    System_printf("Finished Registration -> Listening @ Id %d\n", can0.getId());

    while(1){
        OctetArray buffer("");
        static uint32_t len;
        static Boolean buf_bool;


        if(netReceive.messageAvailable()){
            MessageIncomingInfo message = netReceive.getMessageIncomingInfo();
            can0.readMsg(message.rcvCommId, TimeDuration{0,0},len , buffer, buf_bool);
            UInt16 channelId;
            UInt8 cmdFunctionId;
            UInt8 cmdClassId;
            ArgumentArray inArgs;
            codec.decodeCommand(buffer, channelId, cmdClassId, cmdFunctionId, inArgs);
//            System_printf("Command: Channel %d Cmd %d Function %d\n",channelId,cmdClassId,cmdFunctionId);
            ArgumentArray outArgs;
            Boolean hasResponse =0;
            handleCommand(channelId, cmdClassId, cmdFunctionId, inArgs, hasResponse, outArgs);
            if(hasResponse){
                OctetArray payload;
                Boolean last = 1;
                TimeDuration time{{1,0}};
                codec.encodeResponse(1, outArgs, payload);
                can0.writeRsp(message.rcvCommId, time, payload, last);
            }

        }
        System_flush();


        //Task_yield();
    }



}
