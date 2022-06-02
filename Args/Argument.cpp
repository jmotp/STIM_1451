/*
 * Argument.cpp
 *
 *  Created on: 12/05/2022
 *      Author: jmotp
 */

#include "Args/Argument.h"
#define xdc__nolocalstring
extern "C" {
#include <xdc/std.h>
#include <xdc/runtime/System.h>
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
    }

    System_printf("%u", *(long *)value_ref);



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
        }

    return *this;
    ;
}


Argument::Argument(const Argument &)
{
    // TODO Auto-generated constructor stub

};

Argument::~Argument()
{
    // TODO Auto-generated destructor stub
};
