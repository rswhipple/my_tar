#include "header.h"

void custom_exit(flag_t *flag, arg_t *arg) 
{
    free(flag);
    free(arg->name);
    free_array(arg->files, arg->size);
    free(arg);
}


void free_array(char **array, int size) 
{
    int i = 0;

    while (i < size) {
        free(array[i]);
        i += 1;
    }

    free(array);
}


void free_tar_linklist(tar_node_t* head)
{
    tar_node_t* temp = NULL;

    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp->name);
        free(temp);
    }
}
