#include<cstdint>
#include<vector>
#include<String>
#include"Args/TimeDuration.h"
#include"Args/TimeInstance"
// ERROR ENCODING
#define NO_ERROR 0
#define INVALID_COMMID 1
#define UNKNOWN_DESTID 2
#define TIMEOUT 3
#define NETWORK_FAILURE 4
#define NETWORK_CORRUPTION 5
#define MEMORY 6
#define QOS_FAILURE 7
#define MCAST_NOT_SUPPORTED 8
#define UNKNOWN_GROUPID 9
#define UNKNOWN_MODULEID 10
#define UNKNOWN_MSGID 11
#define NOT_GROUP_MEMBER 12
#define ILLEGAL_MODE 13
#define LOCKED_RESOURCE 14
#define FATAL_TEDS_ERROR 15
#define NON_FATAL_TEDS_ERROR  16
#define CLOSE_ON_LOCKED_RESOURCE 17
#define LOCK_BROKEN 18
#define NETWORK_RESOURCE_EXCEEDED 19
#define MEMORY_RESOURCE_EXCEEDED 20

using namespace std;

namespace IEEE1451Dot0{
    namespace Args{
       typedef std::vector<int8_t> Int8Array;
       typedef std::vector<int16_t> Int16Array;
       typedef std::vector<int32_t> Int32Array;
       typedef std::vector<uint8_t> UInt8Array;
       typedef std::vector<uint16_t> UInt16Array;
       typedef std::vector<uint32_t> UInt32Array;
       typedef std::vector<float> Float32Array;
       typedef std::vector<double> Float64Array;
       typedef std::vector<String> StringArray;
       typedef std::vector<char> OctetArray;
       typedef std::vector<bool> BooleanArray;
       typedef std::vector<TimeInstance> TimeInstanceArray;
       typedef std::vector<TimeDuration> TimeDurationArray;



    }
}





/**
 * main.c
 */

int main(void)
{
	return 0;
}
