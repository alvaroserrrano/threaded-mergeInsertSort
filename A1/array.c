/*
Alvaro Serrano
CS3240
1/13/2019
Assignment 1
*/

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include "apue.h"
#include "array.h"
#include <time.h>
#include "random.h"
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

#define BREAK_SIZE_ARR 1000 //Parent divides in half and spawn 2 threads recursively until size <= 1000

// unsigned int usecs; //the type useseconds_t is an unsigned int that can hold ints ranging [0, 1000000]. It is not used here for portability issues

//Generate a random collection of doubles btw 10000 and 12000 elements
double *create_array(int *size)
{
    int i;
    //Random size btw 10000 and 12000 elements
    *size = make_int(10000, 12000);
    //Malloc array
    double *array = malloc((*size) * sizeof(double));
    //Fill array
    for (i = 0; i < *size; i++)
    {
        array[i] = make_double(1, 100000);
    }
    return array;
}

//Thread to implement merge_insert_sort, write array to output file ("array.csv"), and free memory location
int sort_arr(double *arr, int size)
{
    //Var init
    int retval = 0;
    pthread_t thread;
    struct timespec start, finish;
    struct thread_arg_arr thread_args = {0, arr, size};
    long s;  //SECONDS
    long ns; //NANOSECONDS

    printf("in array: %d elements\n", size);

    //init timer
    clock_gettime(CLOCK_REALTIME, &start);

    //Wait for the thread to complete
    pthread_create(&(thread), NULL, &merge_insert_sort_arr, (void *)&(thread_args));
    while (!thread_args.done)
        usleep(1);
    //terminate thread
    pthread_join(thread, NULL);

    //stop timer
    clock_gettime(CLOCK_REALTIME, &finish);
    s = finish.tv_sec - start.tv_sec;
    ns = finish.tv_nsec - start.tv_nsec;
    if (start.tv_nsec > finish.tv_nsec)
    { // clock underflow
        --s;
        ns += 1000000000;
    }
    printf("%s\n", "Sorting done...");
    // printf("Seconds without ns: %ld\n", s);
    printf("Runtime in ns: %ld nanoseconds\n", ns);
    printf("Runtime in s: %e seconds\n", (double)s + (double)ns / (double)1000000000);
    printf("Runtime in ms: %ldms miliseconds\n", ns / 1000000);

    //top parent opens and writes csv file
    if (!write_output_csv(arr, size, "array.csv"))
    {
        retval = -1;
    }

    //top parent frees array
    printf("---------------------------------\n");
    return retval;
}

//a thread is spawned here and merge_insert_sort is implemented
void *merge_insert_sort_arr(void *thread_args)
{
    int i, j;
    double key;
    struct thread_arg_arr *args = thread_args;

    //BASE CASE
    if (args->size <= BREAK_SIZE_ARR)
    {
        // do insertion sort
        for (i = 1; i < args->size; i++)
        {
            key = args->arr[i];
            j = i - 1;
            // Move elements of arr[0..i-1], that are greater than key, to one position ahead of their current position
            while (j >= 0 && args->arr[j] > key)
            {
                args->arr[j + 1] = args->arr[j];
                j = j - 1;
            }
            args->arr[j + 1] = key;
        }
    }
    //RECURSIVE CASE
    else
    {
        pthread_t thread_1, thread_2;
        //Group into  at least n/2 pairs of elements, leaving one unpaired if the size of the array is odd
        int sub_arr_1_size = args->size / 2;
        int sub_arr_2_size = (args->size / 2) + (args->size % 2);

        //malloc temp arrays
        double *sub_arr_1 = malloc(sub_arr_1_size * sizeof(double));
        double *sub_arr_2 = malloc(sub_arr_2_size * sizeof(double));

        //build temp arrays
        for (i = 0; i < sub_arr_1_size; i++)
        {
            sub_arr_1[i] = args->arr[i];
        }
        for (i = 0; i < sub_arr_2_size; i++)
        {
            sub_arr_2[i] = args->arr[i + sub_arr_1_size];
        }

        //Sort both subarrays recursively in ascending order. Span 2 threads (1 for each half)
        struct thread_arg_arr thread_1_args = {0, sub_arr_1, sub_arr_1_size};
        struct thread_arg_arr thread_2_args = {0, sub_arr_2, sub_arr_2_size};
        pthread_create(&(thread_1), NULL, &merge_insert_sort_arr, (void *)&(thread_1_args));
        pthread_create(&(thread_2), NULL, &merge_insert_sort_arr, (void *)&(thread_2_args));
        //Wait for thread to complete
        while (!thread_1_args.done && !thread_2_args.done)
            usleep(1);
        //Finish thread
        pthread_join(thread_1, NULL);
        pthread_join(thread_2, NULL);

        //Merge subarrays
        merge_arr(sub_arr_1, sub_arr_1_size, sub_arr_2, sub_arr_2_size, args->arr);

        //Free memory in reverse order than it was allocated
        free(sub_arr_2);
        sub_arr_2 = NULL;
        free(sub_arr_1);
        sub_arr_2 = NULL;
    }

    //Mark thread as completed
    args->done = 1;

    //terminate calling thread
    pthread_exit(0);
}

// //Insert double into a sorted array of doubles
// void insert_element_arr(double *arr, double val, int size)
// {
//     int i;
//     //shift elements forward
//     for (i = size - 1; i >= 0 && arr[i] > val; i--)
//     {
//         arr[i + 1] = arr[i];
//     }
//     arr[i + 1] = val;
// }

//Merge 2 subarrays
void merge_arr(double *sub_arr_1, int sub_arr_1_size, double *sub_arr_2, int sub_arr_2_size, double *merged_array)
{
    int i = 0, l = 0, r = 0;
    int n = sub_arr_1_size + sub_arr_2_size; //size of merged_array
    for (i = 0; i < n; i++)
    {
        if (l < sub_arr_1_size && r < sub_arr_2_size)
        {
            if (sub_arr_1[l] < sub_arr_2[r])
            {
                merged_array[i] = sub_arr_1[l];
                l++;
            }
            else
            {
                merged_array[i] = sub_arr_2[r];
                r++;
            }
        }
        else if (l == sub_arr_1_size)
        { //reached end of sub_arr_1
            // copy remaining elements in sub_arr_2
            while (i < n)
            {
                merged_array[i] = sub_arr_2[r];
                i++;
                r++;
            }
        }
        else
        { //reached end of sub_arr_2
            //copy remaining elements in sub_arr_2
            while (i < n)
            {
                merged_array[i] = sub_arr_1[l];
                i++;
                l++;
            }
        }
    }
}

int write_output_csv(double *arr, int size, char *outfilename)
{
    int i, errnum, retval = 0;
    FILE *outfile = fopen(outfilename, "w");
    if (outfile == NULL)
    {
        errnum = errno;
        err_msg("Error opening file: %s\n", strerror(errnum));
        retval = -1;
    }
    else
    {
        for (i = 0; i < size; i++)
        {
            if (!fprintf(outfile, "%f, ", arr[i]))
            {
                errnum = errno;
                err_msg("Error writing to file: %s\n", strerror(errnum));
                retval = -1;
            }
            else
            {
                if ((i + 1) % 10 == 0)
                {
                    if (!fprintf(outfile, "\n"))
                    {
                        errnum = errno;
                        err_msg("Error writing to file: %s\n", strerror(errnum));
                        retval = -1;
                    }
                }
            }
        }
        fclose(outfile);
    }
    return retval;
}

void print_arr(double *arr, int size)
{
    int i;
    for (i = 0; i < size; i++)
    {
        printf(">%f<\n", arr[i]);
    }
}
