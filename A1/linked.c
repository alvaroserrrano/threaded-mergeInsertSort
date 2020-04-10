/*
Alvaro Serrano
CS3240
1/13/2019
Assignment 1
*/

#include "apue.h"
#include "linked.h"
#include "random.h"
#include "array.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

#define BREAK_SIZE_LINKED 1000 //Parent divides in half and spawn 2 threads recursively until size <= 1000

// //Create node
// node *create_node()
// {
//     //declare a node
//     node *cur;
//     cur = malloc(sizeof(node));
//     cur->next = NULL;
//     cur->val = 0;
//     return cur;
// }

//Fill node values with random doubles
node *initialize_linked(int size, double *arr)
{
    int i;
    node *head, *cur;
    head = create_node(size);
    cur = head;
    for (i = 0; i < size; i++)
    {
        cur->val = arr[i];
        cur = cur->next;
    }
    return head;
}

// node *initialize_linked(int *size)
// {
//     int i;
//     *size = make_int(10000, 12000);
//     node *head, *cur;
//     head = create_node(*size);
//     cur = head;
//     for (i = 0; i < *size; i++)
//     {
//         cur->val = make_double(1, 100000);
//         cur = cur->next;
//     }
//     return head;
// }

//Create linked list and allocate memory
node *create_node(int size)
{
    int i;
    node *head, *cur;
    head = malloc(sizeof(node));
    head->next = NULL;
    head->val = 0.0;
    cur = head; //point to the head

    //create subsequent nodes
    for (i = 0; i < size - 1; i++)
    {
        node *new_node = malloc(sizeof(node));
        cur->next = new_node;
        cur->val = 0.0;
        cur = cur->next;
    }
    //tail points to NULL
    cur->next = NULL;
    return head;
}

int sort_linked(node *innode, int size)
{
    int retval = 0;
    pthread_t thread;
    struct timespec start, finish;
    struct thread_arg_linked thread_args = {0, innode, size};
    long s;  //SECONDS
    long ns; //NANOSECONDS

    printf("in linked list: %d elements\n", size);

    //start timer
    clock_gettime(CLOCK_REALTIME, &start);

    //Wait for thread completion
    pthread_create(&(thread), NULL, &merge_insert_sort_linked, (void *)&(thread_args));

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
    printf("Runtime in ns: %ld nanoseconds\n", ns);
    printf("Runtime in s: %e seconds\n", (double)s + (double)ns / (double)1000000000);
    printf("Runtime in ms: %ldms miliseconds\n", ns / 1000000);

    if (!write_linked(thread_args.linked_list, thread_args.size, "linked.csv"))
    {
        retval = -1;
    }
    // free(innode);
    // innode = NULL;
    printf("---------------------------------\n");
    return retval;
}

//a thread is spawned here and merge_insert_sort is implemented
void *merge_insert_sort_linked(void *thread_args)
{
    struct thread_arg_linked *args = thread_args;
    if (args->size <= BREAK_SIZE_LINKED)
    { //Insertion sort
        node *cur, *next;
        //create result list
        node *res = create_node(1);
        //traverse given list and insert current node in sorted way in result list
        res->val = args->linked_list->val;
        args->linked_list = args->linked_list->next;
        cur = args->linked_list;
        while (cur != NULL)
        {
            next = cur->next;
            insert_element_linked(&res, cur);
            cur = next;
        }
        args->linked_list = res;
    }
    else
    {
        pthread_t thread_1, thread_2;
        int sub_linked_1_size = args->size / 2;
        int sub_linked_2_size = (args->size / 2) + (args->size % 2);

        node *head_1 = create_node(sub_linked_1_size);
        node *head_2 = create_node(sub_linked_2_size);

        node *cur;

        for (cur = head_1; cur != NULL; cur = cur->next)
        {
            cur->val = args->linked_list->val;
            args->linked_list = args->linked_list->next;
        }
        for (cur = head_2; cur != NULL; cur = cur->next)
        {
            cur->val = args->linked_list->val;
            args->linked_list = args->linked_list->next;
        }
        //Sort both sublists recursively in ascending order. Spawn 2 threads
        struct thread_arg_linked thread_1_args = {0, head_1, sub_linked_1_size};
        struct thread_arg_linked thread_2_args = {0, head_2, sub_linked_2_size};

        //init threads
        pthread_create(&(thread_1), NULL, &merge_insert_sort_linked, (void *)&(thread_1_args));
        pthread_create(&(thread_2), NULL, &merge_insert_sort_linked, (void *)&(thread_2_args));

        //wait for threads
        while (!thread_1_args.done && !thread_2_args.done)
        {
            usleep(1);
        }

        //finish threads
        pthread_join(thread_1, NULL);
        pthread_join(thread_2, NULL);

        //MERGE sublists
        merge_linked(thread_1_args.linked_list, thread_2_args.linked_list, &(args->linked_list));

        //free memory in reverse order as allocated
        free(head_2);
        head_2 = NULL;
        free(head_1);
        head_1 = NULL;
    }
    args->done = 1;
    pthread_exit(0);
}

//Insert  node in sorted way in a result linked list. This function expects a pointer to the head as it can modify the head of the input linked list
void insert_element_linked(node **head, node *innode)
{
    node *cur; //cursor node
    //check if list is empty or node's value is less than head's value
    if (*head == NULL || (innode)->val <= (*head)->val)
    {
        innode->next = *head;
        *head = innode; //inserted node becomes head
    }
    else
    {
        cur = *head;
        while (cur->next != NULL && cur->next->val < innode->val)
        {
            cur = cur->next;
        }
        innode->next = cur->next;
        cur->next = innode;
    }
}

void merge_linked(node *head_1, node *head_2, node **target)
{
    node temp;
    node *cur = &temp;
    temp.next = NULL;

    while (!(head_1 == NULL || head_2 == NULL))
    {
        if (head_1->val <= head_2->val)
        {
            swap(&(cur->next), &head_1);
        }
        else
        {
            swap(&(cur->next), &head_2);
        }
        cur = cur->next;
    }
    if (head_1 == NULL)
    {
        cur->next = head_2;
    }
    else if (head_2 == NULL)
    {
        cur->next = head_1;
    }
    (*target) = temp.next;
}

//Move node_1 to node_2 and node_2 to node_2->next
void swap(node **node_1, node **node_2)
{
    node *temp = *node_2; //temp = sublist_x
    *node_2 = temp->next;
    temp->next = *node_1;
    *node_1 = temp;
}

void print_linked(node *head)
{
    node *cur;
    for (cur = head; cur != NULL; cur = cur->next)
    {
        printf(">%f\n", cur->val);
    }
}

int write_linked(node *head, int size, char *outfilename)
{
    int i = 0, retval = 0;
    node *cur;
    double *buffer = malloc(size * sizeof(double));
    for (cur = head; cur != NULL; cur = cur->next)
    {
        buffer[i] = cur->val;
        i++;
    }
    retval = write_output_csv(buffer, size, outfilename);
    free(buffer);
    buffer = NULL;
    return retval;
}
