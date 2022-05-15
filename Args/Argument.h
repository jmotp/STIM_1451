/*
 * Argument.h
 *
 *  Created on: 12/05/2022
 *      Author: jmotp
 */

#ifndef ARGS_ARGUMENT_H_
#define ARGS_ARGUMENT_H_

#include <String>


#include "TimeDuration.h"
#include "TimeInstance.h"
#include "TimeRepresentation.h"

using namespace std;

typedef uint8_t UInt8;
typedef uint16_t UInt16;
typedef uint32_t UInt32;
typedef int8_t Int8;
typedef int16_t Int16;
typedef int32_t Int32;
typedef float Float32;
typedef double Float64;
typedef string String;
typedef char Octet;
typedef bool Boolean;

typedef std::vector<int8_t> Int8Array;
typedef std::vector<int16_t> Int16Array;
typedef std::vector<int32_t> Int32Array;
typedef std::vector<uint8_t> UInt8Array;
typedef std::vector<uint16_t> UInt16Array;
typedef std::vector<uint32_t> UInt32Array;
typedef std::vector<float> Float32Array;
typedef std::vector<double> Float64Array;
typedef std::vector<string> StringArray;
typedef std::vector<char> OctetArray;
typedef std::vector<bool> BooleanArray;
typedef std::vector<TimeInstance> TimeInstanceArray;
typedef std::vector<TimeDuration> TimeDurationArray;
/*
IDL: union Argument switch (TypeCode) {
case UNKNOWN_TC: Boolean valueError;
case UInt8_TC: UInt8 valueInt8;
case UINT16_TC: UInt16 valueUInt16;
case UINT32_TC: UInt32 valueUInt32;
case FLOAT32_TC: Float32 valueFloat32;
case FLOAT64_TC: Float64 valueFloat64;
case STRING_TC: _String valueString;
case OCTET_TC: _Octet valueOctet;
case BOOLEAN_TC: _Boolean valueBoolean;
case TIME_INSTANCE_TC: TimeInstance valueTimeInstance;
case TIME_DURATION_TC: TimeDuration valueTimeDuration;
case QOS_PARAMS_TC: QosParams valueQosParams;
case UInt8_ARRAY_TC: UInt8Array valueInt8Array;
case UINT16_ARRAY_TC: UInt16Array valueUInt16Array;
case UINT32_ARRAY_TC: UInt32Array valueUInt32Array;
case FLOAT32_ARRAY_TC: Float32Array valueFloat32Array;
case FLOAT64_ARRAY_TC: Float64Array valueFloat64Array;
case STRING_ARRAY_TC: StringArray valueStringArray;
case OCTET_ARRAY_TC: OctetArray valueOctetArray;
case BOOLEAN_ARRAY_TC: BooleanArray valueBooleanArray;
case TIME_INSTANCE_ARRAY_TC: TimeInstanceArray
 valueTimeInstanceArray;
case TIME_DURATION_ARRAY_TC: TimeDurationArray
 valueTimeDurationArray;
};*/

class Argument{
    public:

        Argument();
        Argument(const Argument &);
        ~Argument();

        Argument &operator =(const Argument&);

        void _d (int);
        int _d () const;

        void valueError(const bool&);
        const bool & valueError() const;
        bool & valueError();



};

#endif /* ARGS_ARGUMENT_H_ */
