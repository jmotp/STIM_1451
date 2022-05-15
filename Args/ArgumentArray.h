/*
 * ArgumentArray.h
 *
 *  Created on: 12/05/2022
 *      Author: jmotp
 */

#ifndef ARGS_ARGUMENTARRAY_H_
#define ARGS_ARGUMENTARRAY_H_

#include <String>
#include "Argument.h"
#include <cstdint>


using namespace std;
class ArgumentArray
{
public:
    ArgumentArray();
    virtual ~ArgumentArray();

    uint16_t getByName(string name, Argument* argument);

    uint16_t getByIndex(uint16_t index, Argument* argument);

    uint16_t putByName(string name, Argument value);

    uint16_t putByIndex(uint16_t index, Argument value);

    uint16_t getNames(string* names);

    uint16_t getIndexes(int *indexes);


};

#endif /* ARGS_ARGUMENTARRAY_H_ */
