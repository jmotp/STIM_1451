/*
 * ArgumentArray.cpp
 *
 *  Created on: 12/05/2022
 *      Author: jmotp
 */

#include "Args/ArgumentArray.h"

#define xdc__nolocalstring
extern "C" {
#include <xdc/std.h>
#include <xdc/runtime/System.h>
}

ArgumentArray::ArgumentArray()
{
    // TODO Auto-generated constructor stub

}

ArgumentArray::~ArgumentArray()
{
    // TODO Auto-generated destructor stub
}

uint16_t ArgumentArray::getByName(string name, Argument& argument){
    return 0;
};

uint16_t ArgumentArray::getByIndex(uint16_t index, Argument& argument){
    argument = get<1>(argumentArray[index]);
    return 0;
}

uint16_t ArgumentArray::putByName(string name, Argument& value){
    argumentArray.push_back(tuple<string,Argument&>(name,value));
    return 0;
}

uint16_t ArgumentArray::putByIndex(uint16_t index, Argument value){

    vector<tuple<string, Argument&>>::iterator it = argumentArray.begin();
    argumentArray.insert (it+index,tuple<string,Argument&>("",value));

    return 0;
};

uint16_t ArgumentArray::getNames(string* names){
    return 0;
};

uint16_t ArgumentArray::getIndexes(int *indexes){
    return 0;
};

