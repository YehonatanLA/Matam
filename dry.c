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

/** The function receives two ordered lists and return a merged list. \
 *
 * @param list1 - target list to be merged. If list1 is NULL nothing will be done.
 * @param list2 - target list to be merged. If list2 is NULL nothing will be done.
 * @param error_code - stores any errors the function made' or success if the function merged successfully.

 * */
Node mergeSortedLists(Node list1, Node list2, ErrorCode* error_code);

/** The function receives a list and adds a new node, next to the list head.
 * 
 * @return-
 * MEMORY_ERROR if the allocation wasn't successful.
 * SUCCESS if otherwise.
 * */
ErrorCode nodeConcat(Node list);


/** The function receives a linked list and frees it
 * @param list - the linked list to be freed. */
void freeNode(Node list);


void addFinalNodes(Node ptr_list_src, Node ptr_list_dest, ErrorCode* error_code);


Node mergeSortedLists(Node list1, Node list2, ErrorCode* error_code) {

    if (list1 == NULL || list2 == NULL) {
        *error_code = NULL_ARGUMENT;
        return NULL;
    }
    if (!isListSorted(list2) || !isListSorted(list1)) {
        *error_code = UNSORTED_LIST;
        return NULL;
    }
    Node merged_list_head = malloc(sizeof(Node));
    if(merged_list_head == NULL) {
        *(error_code) = MEMORY_ERROR;
        return NULL;
    }
    Node p_list1 = list1, p_list2 = list2,  temp = merged_list_head;

    while (p_list1!=NULL && p_list2!=NULL) {
        if (p_list1->x >= p_list2->x) {
            temp->x = p_list1->x;
            p_list1 = p_list1->next;
        }
        else {
            temp->x = p_list2->x;
            p_list2 = p_list2->next;
        }

        if (nodeConcat(temp) == MEMORY_ERROR) {
            *error_code = MEMORY_ERROR;
            freeNode(merged_list_head);
            return NULL;
        }
        temp = temp->next;
    }

    addFinalNodes(p_list1, temp, error_code);
    if(*error_code == MEMORY_ERROR)
        return NULL;

    addFinalNodes(p_list2, temp, error_code);
    if(*error_code == MEMORY_ERROR)
        return NULL;


    *error_code = SUCCESS;
    return merged_list_head;
}


ErrorCode nodeConcat(Node list){
    Node  new_node = (Node) malloc(sizeof(Node));
    if(new_node == NULL)
        return MEMORY_ERROR;

    list->next = new_node;
    return SUCCESS;

}

void freeNode(Node list){
    Node temp;
    while(list != NULL) {
        temp = list;
        list = list->next;
        free(temp);
    }
}

void addFinalNodes(Node ptr_list_src, Node ptr_list_dest, ErrorCode* error_code) {
    while (ptr_list_src != NULL) {
        ptr_list_dest->x = ptr_list_src->x;
        ptr_list_src = ptr_list_src->next;
        if (nodeConcat(ptr_list_dest) == MEMORY_ERROR) {
            *error_code = MEMORY_ERROR;
            freeNode(ptr_list_dest);
            return;
        }
    }
}