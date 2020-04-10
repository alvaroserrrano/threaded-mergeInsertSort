/*
Alvaro Serrano
CS3240
1/13/2019
Assignment 1
*/

#ifndef LINKED_H
#define LINKED_H

//Define node as pointer of data type struct LinkedList
typedef struct node_struct
{
    struct node_struct *next;
    double val;
} node;

/* arguments for a thread */
struct thread_arg_linked
{
    int done;
    node *linked_list;
    int size;
};

// extern node *initialize_linked(int *size);
extern node *initialize_linked(int size, double *arr);
extern node *create_node(int size);
extern int sort_linked(node *innode, int size);
extern void *merge_insert_sort_linked(void *thread_args);
extern void insert_element_linked(node **head, node *innode);
extern void merge_linked(node *head_1, node *head_2, node **target);
extern void swap(node **node_1, node **node_2);
extern int write_linked(node *linked, int size, char *outfilename);
extern void print_linked(node *head);
#endif // !LINKED_H
