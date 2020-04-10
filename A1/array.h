/*
Alvaro Serrano
CS3240
1/13/2019
Assignment 1
*/

#ifndef ARRAY_H
#define ARRAY_H

//thread arguments
struct thread_arg_arr
{
    int done;
    double *arr;
    int size;
};

extern double *
create_array(int *size);
extern int sort_arr(double *arr, int size);
extern void *merge_insert_sort_arr(void *thread_args);
// extern void insert_element_arr(double *arr, double val, int size);
extern void merge_arr(double *sub_arr_1, int sub_arr_size_1, double *sub_arr_2, int size_sub_arr_2, double *target);
extern int write_output_csv(double *arr, int size, char *outfilename);
extern void print_arr(double *arr, int size);

#endif
