/*
 * Argument.cpp
 *
 *  Created on: 12/05/2022
 *      Author: jmotp
 */

#include "Args/Argument.h"
#include <cstdio>
#include <variant>

void swapByteOrder(uint16_t& us)
{
    us = (us >> 8) |
         (us << 8);
}

void swapByteOrder(uint32_t& ui)
{
    ui = (ui >> 24) |
         ((ui<<8) & 0x00FF0000) |
         ((ui>>8) & 0x0000FF00) |
         (ui << 24);
}

Argument::Argument()
{

    // TODO Auto-generated constructor stub

};


/*
enum TypeCode{
         UNKNOWN_TC,
         // Simple types
         UInt8_TC, UINT16_TC, UINT32_TC,
         FLOAT32_TC, FLOAT64TC, STRING_TC,
         OCTET_TC, BOOLEAN_TC,
         TIME_INSTANCE_TC, TIME_DURATION_TC,
         QOS_PARAMS_TC,
         // Arrays of simple types. Note no QOS array
         UInt8_ARRAY_TC, UINT16_ARRAY_TC, UINT32_ARRAY_TC,
         FLOAT32_ARRAY_TC, FLOAT64_ARRAY_TC, STRING_ARRAY_TC,
         OCTET_ARRAY_TC, BOOLEAN_ARRAY_TC,
        TIME_INSTANCE_ARRAY_TC, TIME_DURATION_ARRAY_TC
        } type;
*/

Argument::Argument(TypeCode _type, void * value_ref){
    type = _type;

    switch(type){
        case UInt8_TC:
            _valueUInt8 = *(uint8_t *)value_ref;
            break;
        case UInt32_TC:
            _valueUInt32 = *(uint32_t *)value_ref;
            break;

        case Octet_Array_TC:
            System_printf("%s",((string)*(string*)(value_ref)).c_str());
            System_flush();
            new(&this->_valueOctetArray) string(*(string*)value_ref);
            break;
        }

    this->print();



}



Argument& Argument::operator= (const Argument& argument){

    this->type = argument.type;

    switch(argument.type){
        case UInt8_TC:
            this->_valueUInt8 = argument._valueUInt8;
            break;
        case UInt16_TC:
            this->_valueUInt16 = argument._valueUInt16;
            break;
        case UInt32_TC:
            this->_valueUInt32 = argument._valueUInt32;
            break;
        case Octet_Array_TC:
//            System_printf("String Pointer: %d\n", argument._valueOctetArray);
//            System_flush();

            new(&this->_valueOctetArray) string(argument._valueOctetArray);
        }

    return *this;
    ;
}

UInt16 Argument::print(){
    switch(this->type){
            case UInt8_TC:
                System_printf("%u", this->_valueUInt8);
                break;
            case UInt16_TC:
                System_printf("%u", this->_valueUInt16);
                break;
            case UInt32_TC:
                System_printf("Argument long %u\n", this->_valueUInt32);
                break;
            case Octet_Array_TC:
                System_printf("Argument %s\n", this->_valueOctetArray.c_str());
                break;
    }
    System_flush();
    return 0;

}

UInt16 Argument::toString(String& result){
    switch(this->type){
                case UInt8_TC:
                    result = this->_valueUInt8;
                    break;
                case UInt16_TC:
                    result = this->_valueUInt16;
                    break;
                case UInt32_TC:
                    result = this->_valueUInt32;
                    break;
                case Octet_Array_TC:
                    result = this->_valueOctetArray;
                    break;
        }
        System_flush();
        return 0;

}

UInt16 Argument::write(stringstream& ss){
    uint8_t size;
    uint16_t arg_size;
    ss.write((const char*)&type,sizeof(uint8_t));
    switch(this->type){
        
                    case UInt8_TC:
                        ss.write((const char*)&(this->_valueUInt8),sizeof(uint8_t));
                        break;
                    case UInt16_TC:
                        uint16_t buffer_ = this->_valueUInt16;
                        swapByteOrder(buffer_);
                        size=2;
                        ss.write((const char*)&(buffer_),sizeof(uint8_t));
                        break;
                    case UInt32_TC:


                        uint32_t buffer = this->_valueUInt32;
                        swapByteOrder(buffer);
                        size=4;
                        arg_size = 4;
                        swapByteOrder(arg_size);
                        ss.write((const char*)(&arg_size),2);
                        ss.write((const char*)&(buffer),sizeof(uint32_t));
                        break;
                    case Octet_Array_TC:
                        size = this->_valueOctetArray.size();
//                        System_printf("Size: %d\n", size);
//                        System_flush();
                        arg_size=size;
                        swapByteOrder(arg_size);
                        ss.write((const char*)(&arg_size),2);
                        ss.write((const char*)(this->_valueOctetArray.data()),size);
                        break;
            }
            return 0;

}


Argument::Argument(const Argument & arg)
{
    this->type = arg.type;
    switch(this->type){
        case UInt32_TC: this->_valueUInt32 = arg._valueUInt32;
        break;
        case Octet_Array_TC: new(&this->_valueOctetArray) string(arg._valueOctetArray);
     }
    // TODO Auto-generated constructor stub

};

Argument::~Argument()
{
    switch(type){
           this->print();
           case Octet_Array_TC: delete(((string*)&(this->_valueOctetArray)));
        }
    // TODO Auto-generated destructor stub
};
