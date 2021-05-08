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
    MapKeyElement key;
    MapDataElement data;
    struct Key_Data_t *next;
};
//

/**
* Helper function addNode: adds a new node to the KeyData linked list.
* @param map - The map for which to add the KeyData element
* @param keyElement - The key element which need to be reassigned
* @param dataElement - The new data element to associate with the given key.
* @param iterator - The KeyData element which the new KeyData element is added after.

* @return
* 	MAP_NULL_ARGUMENT if a NULL was sent as map or as iterator
* 	MAP_OUT_OF_MEMORY if an allocation failed (Meaning the function for copying
* 	an element failed)
* 	MAP_SUCCESS the KeyData element had been inserted successfully
*/

static MapResult addNode(Map map, KeyData iterator, MapKeyElement keyElement, MapDataElement dataElement);


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


void mapDestroy(Map map){
    if(map == NULL)
        return;
    mapClear(map);
    free(map->iterator);
    free(map->first_node);
    free(map);
}


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
        if(key == NULL){
            mapDestroy(new_map);
            return NULL;
        }
        data = map->copyData(map->iterator->data);
        if(data == NULL){
            new_map->freeKey(key);
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


int mapGetSize(Map map){
    if(map == NULL)
        return -1;
    return map->size;
}


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


MapResult mapPut(Map map, MapKeyElement keyElement, MapDataElement dataElement) {
    if (map == NULL) {
        return MAP_NULL_ARGUMENT;
    }
    if(map->first_node == NULL){
        return MAP_NULL_ARGUMENT;
    }

    map->iterator = map->first_node;
    // Run to relevant node
    while (map->iterator->next && map->compareKeys(map->iterator->next->key, keyElement) < 0) {
        map->iterator = map->iterator->next;
    }
    //Insert KeyData element after iterator.
    if (map->iterator->next == NULL || map->compareKeys(map->iterator->next->key, keyElement) > 0) {
        addNode(map, map->iterator, keyElement, dataElement);
    }

    //Exists. hence, update.
    if (map->compareKeys(map->iterator->next->key, keyElement) == 0) {
        // Moving to the right KeyData element needed to update.
        map->iterator = map->iterator->next;
        MapDataElement tmp = map->copyData(dataElement);
        if(tmp == NULL){
            return MAP_OUT_OF_MEMORY;
        }
        map->freeData(map->iterator->next->data);
        map->iterator->data = tmp;
    }
    return MAP_SUCCESS;
}


MapDataElement mapGet(Map map, MapKeyElement keyElement){
    if(map == NULL)
        return NULL;
    KeyData ptr = map->first_node;
    if(ptr == NULL)
        return NULL;
    while(ptr != NULL) {
        if (map->compareKeys(ptr->key, keyElement) == 0){
            return ptr->data;
        }
        if(map->compareKeys(ptr->key, keyElement) > 0) {
            return NULL;
        }
        ptr = ptr->next;
    }
    //Didn't find the key;
    return NULL;
}


MapResult mapRemove(Map map, MapKeyElement keyElement){
    if(map == NULL || keyElement == NULL)
        return MAP_NULL_ARGUMENT;
    map->iterator = map->first_node;
    if(map->iterator == NULL)
        return MAP_ITEM_ALREADY_EXISTS;
    while (map->iterator->next != NULL) {
        if (map->compareKeys(map->iterator->next->key, keyElement) == 0) {
            KeyData tmp = map->iterator->next;
            map->iterator->next = (map->iterator->next->next == NULL)? map->iterator->next->next : NULL;

            map->freeKey(tmp->key);
            map->freeData(tmp->data);
            free(tmp);
            map->size--;
            return MAP_SUCCESS;
        }
        if (map->compareKeys(map->iterator->key, keyElement) > 0)
            break;
    }
    return MAP_ITEM_DOES_NOT_EXIST;

}

MapKeyElement mapGetFirst(Map map){
    if(map == NULL || map->first_node == NULL)
        return NULL;
    return map->first_node->key;
}


MapKeyElement mapGetNext(Map map){
    if(map == NULL || map->iterator == NULL)
        return NULL;
    map->iterator=map->iterator->next;
    if(map->iterator == NULL)
        return NULL;
    return map->iterator->key;
}


MapResult mapClear(Map map){
    if(map == NULL)
        return MAP_NULL_ARGUMENT;
    if(map->first_node == NULL)
        return MAP_SUCCESS;
    KeyData temp;
    while(map->iterator != NULL){
        temp = map->iterator;
        map->iterator = map->iterator->next;
        map->freeData(temp->data);
        map->freeKey(temp->key);
        free(temp);
    }
    map->first_node = NULL;
    map->size = 0;
    return MAP_SUCCESS;
}


static MapResult addNode(Map map, KeyData iterator, MapKeyElement keyElement, MapDataElement dataElement){
    if(map == NULL || iterator == NULL){
        return MAP_NULL_ARGUMENT;
    }
    KeyData tmp = malloc(sizeof(KeyData));
    if (tmp == NULL)
        return MAP_OUT_OF_MEMORY;
    tmp->key = map->copyKey(keyElement);
    if(tmp->key == NULL){
        free(tmp);
        return MAP_OUT_OF_MEMORY;
    }
    tmp->data = map->copyData(dataElement);
    if(tmp->data == NULL) {
        free(tmp->key);
        free(tmp);
        return MAP_OUT_OF_MEMORY;
    }

    tmp->next = map->iterator->next;
    map->iterator->next = tmp;
    map->size++;
}
