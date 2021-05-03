#include "map.h"
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

/* Implementing Map struct*/


 struct Map_t{
    int size;
    KeyData key_data;
    copyMapDataElements copyData;
    copyMapKeyElements copyKey;
    freeMapDataElements freeData;
    freeMapKeyElements freeKey;
    compareMapKeyElements compareKeys;
 };

//newMap -> key_data -> value =  MapOld -> copyData( OldMap -> key_value -> value)
// newMap -> key_data -> key = MapOld -> key_data -> copyKey(OldMap -> key_data -> key)

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
    map->key_data = NULL;
    map->copyData = copyDataElement;
    map->copyKey = copyKeyElement;
    map->freeData = freeDataElement;
    map->freeKey = freeKeyElement;
    map->compareKeys = compareKeyElements;
    return map;

}

/**
* mapDestroy: Deallocates an existing map. Clears all elements by using the
* stored free functions.
*
* @param map - Target map to be deallocated. If map is NULL nothing will be
* 		done
*/
void mapDestroy(Map map);

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
    Map newMap = mapCreate(map->copyData,map->copyKey, map->freeData, map->freeKey, map->compareKeys);
    if(newMap == NULL) {
        mapDestroy(newMap);
        return NULL;
    }

    KeyData ptr_new = malloc(sizeof (KeyData));
    if(ptr_new == NULL) {
        mapDestroy(newMap);
        return NULL;
    }
    newMap->key_data = ptr_new;
    MAP_FOREACH(KeyData , ptr, map){
        ptr_new->key = map->copyKey(ptr->key);
        ptr_new->data = map->copyData(ptr->data);
        if(ptr->next == NULL) // Finished copying successfully.
            break;
        ptr_new->next = malloc(sizeof (KeyData));
        if(ptr_new->next == NULL) {
            mapDestroy(newMap);
            return NULL;
        }
        ptr_new = ptr_new->next;
    }
    ptr_new->next = NULL;
    newMap->size = map->size;
    return newMap;

}

/**
* mapGetSize: Returns the number of elements in a map
* @param map - The map which size is requested
* @return
* 	-1 if a NULL pointer was sent.
* 	Otherwise the number of elements in the map.
*/
int mapGetSize(Map map);

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
    assert(map != NULL);
    Element ptr; //Needed?
/*
    MAP_FOREACH(map->key_data, ptr, map){
        if(map->compareKeys(ptr->key, element) == 0)
            reutrn true;
    }
    return false;
*/

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
MapResult mapPut(Map map, MapKeyElement keyElement, MapDataElement dataElement);

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
    KeyData ptr = map->key_data;
    if(ptr == NULL)
        return NULL;
    while(ptr->next != NULL) {
        if (ptr->key == keyElement)
            return ptr->data;
        ptr = ptr->next;
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
MapResult mapRemove(Map map, MapKeyElement keyElement);

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
    if(map->key_data == NULL)
        return NULL;
    return map->key_data;
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
MapKeyElement mapGetNext(Map map);


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
    if(map->key_data == NULL)
        return MAP_NULL_ARGUMENT;
    KeyData ptr = map->key_data;
    while(ptr->next != NULL){
        map->freeData(ptr->data);
        map->freeKey(ptr->key);

    }
}
