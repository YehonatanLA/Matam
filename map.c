#include "map.h"
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

/* Implementing Map struct*/


 struct Map_t{
    int size;
    KeyData first_node;
    KeyData iterator;
    copyMapDataElements copyData;
    copyMapKeyElements copyKey;
    freeMapDataElements freeData;
    freeMapKeyElements freeKey;
    compareMapKeyElements compareKeys;
 };

/* Implementing Key-Value struct*/
struct Key_Data_t{
    Element key;
    Element data;
    struct Key_Data_t *next;
};
//

/**
* mapCreate: Allocates a new empty map.
*
* @param copyDataElement - Function pointer to be used for copying data elements into
*  	the map or when copying the map.
* @param copyKeyElement - Function pointer to be used for copying key elements into
*  	the map or when copying the map.
* @param freeDataElement - Function pointer to be used for removing data elements from
* 		the map
* @param freeKeyElement - Function pointer to be used for removing key elements from
* 		the map
* @param compareKeyElements - Function pointer to be used for comparing key elements
* 		inside the map. Used to check if new elements already exist in the map.
* @return
* 	NULL - if one of the parameters is NULL or allocations failed.
* 	A new Map in case of success.
*/
Map mapCreate(copyMapDataElements copyDataElement,
              copyMapKeyElements copyKeyElement,
              freeMapDataElements freeDataElement,
              freeMapKeyElements freeKeyElement,
              compareMapKeyElements compareKeyElements){
    if(!copyDataElement || !freeDataElement || !compareKeyElements || !copyKeyElement || !freeKeyElement)
        return NULL;
    Map map = malloc(sizeof (Map));
    if(map == NULL)
        return NULL;
    map->size = 0;
    map->first_node = NULL;
    map->copyData = copyDataElement;
    map->copyKey = copyKeyElement;
    map->freeData = freeDataElement;
    map->freeKey = freeKeyElement;
    map->compareKeys = compareKeyElements;
    map->iterator = map->first_node;
    return map;

}

/**
* mapDestroy: Deallocates an existing map. Clears all elements by using the
* stored free functions.
*
* @param map - Target map to be deallocated. If map is NULL nothing will be
* 		done
*/
void mapDestroy(Map map){
    if(map == NULL)
        return;
    mapClear(map);
    free(map->iterator);
    free(map->first_node);
    free(map);
}

/**
* mapCopy: Creates a copy of target map.
* Iterator values for both maps is undefined after this operation.
*
* @param map - Target map.
* @return
* 	NULL if a NULL was sent or a memory allocation failed.
* 	A Map containing the same elements as map otherwise.
*/
Map mapCopy(Map map){
    if(map == NULL)
        return NULL;
    Map new_map = mapCreate(map->copyData,map->copyKey, map->freeData, map->freeKey, map->compareKeys);
    if(new_map == NULL) {
        mapDestroy(new_map);
        return NULL;
    }

    map->iterator = map->first_node;
    // In case the old map is empty
    if(map->first_node == NULL) {
        new_map->first_node = NULL;
        return new_map;
    }
    MapKeyElement key;
    MapDataElement data;
    while(map->iterator != NULL){
        key = map->copyKey(map->iterator->key);
        data = map->copyData(map->iterator->data);
        if(key == NULL || data == NULL){
            mapDestroy(new_map);
            return NULL;
        }

        MapResult result = mapPut(new_map, key, data);
        if(result == MAP_OUT_OF_MEMORY){
            mapDestroy(new_map);
            return NULL;
        }

        map->iterator = map->iterator->next;
    }

    new_map->size = map->size;
    return new_map;

}


/**
* mapGetSize: Returns the number of elements in a map
* @param map - The map which size is requested
* @return
* 	-1 if a NULL pointer was sent.
* 	Otherwise the number of elements in the map.
*/
int mapGetSize(Map map){
    if(map == NULL)
        return -1;
    return map->size;
}
/**
* mapContains: Checks if a key element exists in the map. The key element will be
* considered in the map if one of the key elements in the map it determined equal
* using the comparison function used to initialize the map.
*
* @param map - The map to search in
* @param element - The element to look for. Will be compared using the
* 		comparison function.
* @return
* 	false - if one or more of the inputs is null, or if the key element was not found.
* 	true - if the key element was found in the map.
*/
bool mapContains(Map map, MapKeyElement element){
    if(map == NULL || element == NULL)
        return false;
    KeyData ptr = map->first_node;
    if(ptr == NULL)
        return false;
    while(ptr != NULL){
        if(map->compareKeys(ptr->key, element) == 0)
            return true;
        ptr = ptr->next;
    }
    //Didn't find the key;
    return false;


}

/**
*	mapPut: Gives a specified key a specific value.
*  Iterator's value is undefined after this operation.
*
* @param map - The map for which to reassign the data element
* @param keyElement - The key element which need to be reassigned
* @param dataElement - The new data element to associate with the given key.
*      A copy of the element will be inserted as supplied by the copying function
*      which is given at initialization and old data memory would be
*      deleted using the free function given at initialization.
* @return
* 	MAP_NULL_ARGUMENT if a NULL was sent as map
* 	MAP_OUT_OF_MEMORY if an allocation failed (Meaning the function for copying
* 	an element failed)
* 	MAP_SUCCESS the paired elements had been inserted successfully
*/
MapResult mapPut(Map map, MapKeyElement keyElement, MapDataElement dataElement){
    if(map == NULL)
        return MAP_NULL_ARGUMENT;
    map->iterator = map->first_node;
    while(map->iterator->next && map->compareKeys(map->iterator->next->key, keyElement) > 0) //run to relevant node
        map->iterator = map->iterator->next;

    if (map->iterator->next == NULL){ //END. hence, insert.
        KeyData tmp = malloc(sizeof (KeyData));
        if(tmp == NULL)
            return MAP_OUT_OF_MEMORY; ///that's it? destroy?
        map->iterator->next=tmp;
        tmp->next = NULL;
        tmp->key=map->copyKey(keyElement); /// If failed return MAP_OUT_OF_MEMORY. How?
        tmp->data=map->copyData(dataElement); /// If failed return MAP_OUT_OF_MEMORY. How?
        map->size++;
    }

    if(map->compareKeys(map->iterator->next->key, keyElement) < 0){ //SKIP. hence, insert.
        KeyData tmp = malloc(sizeof(KeyData));
        if(tmp == NULL)
            return MAP_OUT_OF_MEMORY; ///that's it? destroy?
        tmp->next = map->iterator->next;
        map->iterator->next=tmp;
        tmp->key=map->copyKey(keyElement); /// If failed return MAP_OUT_OF_MEMORY. How?
        tmp->data=map->copyData(dataElement); /// If failed return MAP_OUT_OF_MEMORY. How?
        map->size++;
    }

    if(map->compareKeys(map->iterator->next->key, keyElement) == 0){ //Exists. hence, update.
        map->freeKey(map->iterator->next->key);
        map->freeData(map->iterator->next->data);
        map->iterator->next->key = map->copyKey(keyElement); /// If failed return MAP_OUT_OF_MEMORY. How?
        map->iterator->next->data = map->copyData(dataElement); /// If failed return MAP_OUT_OF_MEMORY. How?

    }
    return MAP_SUCCESS;

/**
*	mapGet: Returns the data associated with a specific key in the map.
*			Iterator status unchanged
*
* @param map - The map for which to get the data element from.
* @param keyElement - The key element which need to be found and whos data
we want to get.
* @return
*  NULL if a NULL pointer was sent or if the map does not contain the requested key.
* 	The data element associated with the key otherwise.
*/
MapDataElement mapGet(Map map, MapKeyElement keyElement){
    if(map == NULL)
        return NULL;
    KeyData ptr = map->first_node;
    if(ptr == NULL)
        return NULL;
    while(ptr != NULL) {
        if (ptr->key == keyElement)
            return ptr->data;
        else {
            ptr = ptr->next;
        }
    }
    //Didn't find the key;
     return NULL;
}


/**
* 	mapRemove: Removes a pair of key and data elements from the map. The elements
*  are found using the comparison function given at initialization. Once found,
*  the elements are removed and deallocated using the free functions
*  supplied at initialization.
*  Iterator's value is undefined after this operation.
*
* @param map -
* 	The map to remove the elements from.
* @param keyElement
* 	The key element to find and remove from the map. The element will be freed using the
* 	free function given at initialization. The data element associated with this key
*  will also be freed using the free function given at initialization.
* @return
* 	MAP_NULL_ARGUMENT if a NULL was sent to the function
*  MAP_ITEM_DOES_NOT_EXIST if an equal key item does not already exists in the map
* 	MAP_SUCCESS the paired elements had been removed successfully
*/
MapResult mapRemove(Map map, MapKeyElement keyElement){
    if(map == NULL || keyElement == NULL)
        return MAP_NULL_ARGUMENT;
    while (map->iterator->next != NULL) {
        if (map->compareKeys(map->iterator->next->key, keyElement) == 0) {
            KeyData tmp = map->iterator->next;
         if(map->iterator->next->next == NULL){
            map->iterator->next = NULL
         }
            map->iterator->next = map->iterator->next->next; ///if NULL?
            map->freeKey(tmp);
            map->freeData(tmp);
            free(tmp);
            map->size--;
            return MAP_SUCCESS;
        }
        if (map->compareKeys(map->iterator->key, keyElement) > 0)
            break;
    }
    return MAP_ITEM_DOES_NOT_EXIST;

}
/**
*	mapGetFirst: Sets the internal iterator (also called current key element) to
*	the first key element in the map. There doesn't need to be an internal order
*  of the keys so the "first" key element is any key element.
*	Use this to start iterating over the map.
*	To continue iteration use mapGetNext
*
* @param map - The map for which to set the iterator and return the first
* 		key element.
* @return
* 	NULL if a NULL pointer was sent or the map is empty.
* 	The first key element of the map otherwise
*/
MapKeyElement mapGetFirst(Map map){
    if(map == NULL)
        return NULL;
    if(map->first_node == NULL)
        return NULL;
    return map->first_node->key;
}

/**
*	mapGetNext: Advances the map iterator to the next key element and returns it.
*	The next key element is any key element not previously returned by the iterator.
* @param map - The map for which to advance the iterator
* @return
* 	NULL if reached the end of the map, or the iterator is at an invalid state
* 	or a NULL sent as argument
* 	The next key element on the map in case of success
*/
MapKeyElement mapGetNext(Map map){
    if(map == NULL || map->iterator == NULL)
        return NULL;
    map->iterator=map->iterator->next;
    if(map->iterator == NULL)
        return NULL;
    return map->iterator->key;
}

/**
* mapClear: Removes all key and data elements from target map.
* The elements are deallocated using the stored free functions.
* @param map
* 	Target map to remove all element from.
* @return
* 	MAP_NULL_ARGUMENT - if a NULL pointer was sent.
* 	MAP_SUCCESS - Otherwise.
*/
MapResult mapClear(Map map){
    if(map == NULL)
        return MAP_NULL_ARGUMENT;
    if(map->first_node == NULL)
        return MAP_SUCCESS;
    KeyData temp;
    while(map->iterator != NULL){
        temp = map->iterator;
        map->freeData(map->iterator->data);
        map->freeKey(map->iterator->key);
        map->iterator = map->iterator->next;
        free(temp); // Free function for Keydata.c?
    }
    map->first_node = map->iterator;
    map->size = 0;
    return MAP_SUCCESS;
}
