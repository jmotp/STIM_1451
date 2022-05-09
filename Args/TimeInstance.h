/*
 * TimeInstance.h
 *
 *  Created on: 19/04/2022
 *      Author: jmotp
 */

#ifndef TIMEINSTANCE_H_
#define TIMEINSTANCE_H_

#include<cstdint>

class TimeInstance :public TimeRepresentation
{
public:
    uint8_t epoch;
    TimeInstance();
    virtual ~TimeInstance();
};

#endif /* TIMEINSTANCE_H_ */
