#include <stdbool.h>
#include <stdlib.h>


typedef struct node_t {
    int x;
    struct node_t *next;
} *Node;
typedef enum {
    SUCCESS=0,
    MEMORY_ERROR,
    UNSORTED_LIST,
    NULL_ARGUMENT,
} ErrorCode;
int getListLength(Node list);
bool isListSorted(Node list);
/* The function receives two ordered lists and return a merged list. */
Node mergeSortedLists(Node list1, Node list2, ErrorCode* error_code);

/* The function receives a list and adds a new node, next to the list head.
 * The return value is based on the success of the memory allocation. */
ErrorCode nodeConcat(Node list);


Node mergeSortedLists(Node list1, Node list2, ErrorCode* error_code) {
    //Checking errors for list1, list2


    if (list1 == NULL || list2 == NULL) {
        *error_code = NULL_ARGUMENT;
        return NULL;
    }
    if (!isListSorted(list2) || !isListSorted(list1)) {
        *error_code = UNSORTED_LIST;
        return NULL;
    }
    Node merged_node = malloc(sizeof(Node));
    if(merged_node == NULL) {
        *(error_code) = MEMORY_ERROR;
        return NULL;
    }
    Node p_list1 = list1, p_list2 = list2,  temp = merged_node;

    while (p_list1!=NULL && p_list2!=NULL) {
        if (p_list1->x >= p_list2->x) {
            temp->x = p_list1->x;
            p_list1 = p_list1->next;
        } else {
            temp->x = p_list2->x;
            p_list2 = p_list2->next;
        }

        if (nodeConcat(temp) == MEMORY_ERROR) {
            *error_code = MEMORY_ERROR;
            return NULL;
        }
        temp = temp->next;
    }

    while (p_list1!=NULL) {
        temp->x = p_list1->x;
        p_list1 = p_list1->next;
        if (nodeConcat(temp) == MEMORY_ERROR) {
            *error_code = MEMORY_ERROR;
            return NULL;
        }
        temp = temp->next;
    }

    while (p_list2!=NULL) {
        temp->x = p_list2->x;
        p_list2 = p_list2->next;
        if (nodeConcat(temp) == MEMORY_ERROR) {
            *error_code=MEMORY_ERROR;
            return NULL;
        }
        temp = temp->next;
    }
    return merged_node;
}


ErrorCode nodeConcat(Node list){
    list->next = (Node) malloc(sizeof(Node));
    if(list->next == NULL)
        return MEMORY_ERROR;
    return SUCCESS;

}
