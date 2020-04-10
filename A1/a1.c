/*
Alvaro Serrano
CS3240
1/13/2019
Assignment 1


1.	A random collection of doubles of random length between 10 000 and 12 000 elements is generated.

		2.1.	Parent divides in half and spawns two threads recursively until size <=1000.
		2.2.	The <=1000 elements collection is sorted (Insertion Sort).
3.	The sorted collection is written to "array.csv" with 10 doubles per line separated by commas.
*/

#include "array.h"
#include "linked.h"
#include <stdlib.h>
#include <stdio.h>
// #include "apue.h"

int main(int argc, char *argv[])
{
	int size;
	double *arr = create_array(&size);
	sort_arr(arr, size);
	node *linked_list = initialize_linked(size, arr);
	// node *linked_list = initialize_linked(&size);
	sort_linked(linked_list, size);
	free(arr);
	arr = NULL;
	free(linked_list);
	linked_list = NULL;
	exit(0);
} //main
