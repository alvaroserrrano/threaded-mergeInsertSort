/*
Alvaro Serrano
CS3240
1/13/2019
Assignment 1
*/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "random.h"
#include "apue.h"

//Generate a random integer; i.e. to determine size of the array
int make_int(int low, int high)
{
    struct timespec spec_struct;
    clock_gettime(CLOCK_REALTIME, &spec_struct);
    // Initialize random number generator
    srand((unsigned long)spec_struct.tv_nsec); //current time as seed
    return (rand() % (high - low)) + low;
}

double make_double(int low, int high)
{
    const int decimals = 1.0e4; //4 decimal places
    return ((double)(make_int(low * decimals, high * decimals)) / decimals);
}
