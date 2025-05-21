#include "film.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

// Parse input from CSVEntry
void film_parse(tFilm* data, tCSVEntry entry) {
    // Check input data
    assert(data != NULL);
    assert(csv_numFields(entry) == NUM_FIELDS_FILM);

    int pos = 0;

    // Name
    const char* name = entry.fields[pos++];
    assert(name != NULL);

    // Duration
    assert(strlen(entry.fields[pos]) == TIME_LENGTH);
    tTime duration;
    int itemsRead = sscanf(entry.fields[pos++], "%d:%d", &duration.hour, &duration.minutes);
    assert(itemsRead == 2);

    // Genre
    int genreValue = csv_getAsInteger(entry, pos++);
    assert(genreValue >= GENRE_FIRST && genreValue < GENRE_END);
    tFilmGenre genre = (tFilmGenre)genreValue;

    // Release date
    assert(strlen(entry.fields[pos]) == DATE_LENGTH);
    tDate release;
    itemsRead = sscanf(entry.fields[pos++], "%d/%d/%d", &release.day, &release.month, &release.year);
    assert(itemsRead == 3);

    // Rating
    float rating = csv_getAsReal(entry, pos++);
    assert(rating >= RATING_MIN && rating <= RATING_MAX);

    // isFree
    int isFree = csv_getAsInteger(entry, pos++);
    assert(isFree == 0 || isFree == 1);

    // Call film_init with the parsed data
    film_init(data, name, duration, genre, release, rating, (bool)isFree);
}

// Initialize a film
void film_init(tFilm* data, const char* name, tTime duration, tFilmGenre genre, tDate release, float rating, bool isFree) {
    // Check preconditions
    assert(data != NULL);
    assert(name != NULL);
    
    // Name
    data->name = (char*) malloc((strlen(name) + 1) * sizeof(char));
    assert(data->name != NULL);
    strcpy(data->name, name);
    
    // Duration
    time_cpy(&data->duration, duration);
    
    // Genre
    data->genre = genre;
    
    // Release
    date_cpy(&data->release, release);
    
    // Rating
    data->rating = rating;
    
    // isFree
    data->isFree = isFree;
}

// Copy a film from src to dst
void film_cpy(tFilm* dst, tFilm src) {
    // Check preconditions
    assert(dst != NULL);
    
    film_init(dst, src.name, src.duration, src.genre, src.release, src.rating, src.isFree);
}

// Get film data using a string
void film_get(tFilm data, char* buffer) {
    // Print all data at same time
    sprintf(buffer,"%s;%02d:%02d;%d;%02d/%02d/%04d;%.1f;%d",
        data.name,
        data.duration.hour, data.duration.minutes,
        data.genre,
        data.release.day, data.release.month, data.release.year,
        data.rating,
        data.isFree);
}

// Remove the data from a film
void film_free(tFilm* data) {
    // Check preconditions
    assert(data != NULL);
    
    if (data->name != NULL)
    {
        free(data->name);
        data->name = NULL;
    }
}

// Returns true if two films are considered equal (same name)
bool film_equals(tFilm a, tFilm b) {
    if (a.name == NULL || b.name == NULL) {
        return false;
    }
    return strcmp(a.name, b.name) == 0;
}

// Initialize the films list
tApiError filmList_init(tFilmList* list) {
    // Check preconditions
    assert(list != NULL);
    
    list->first = NULL;
    list->last = NULL;
    list->count = 0;
    
    return E_SUCCESS;
}

// Add a new film to the list
tApiError filmList_add(tFilmList* list, tFilm film) {
    // Check preconditions
    assert(list != NULL);
    
    tFilmListNode *node;
    
    // Check if the film is already in the list
    if (filmList_find(*list, film.name) != NULL)
        return E_FILM_DUPLICATED;
    
    // Create the node
    node = (tFilmListNode*)malloc(sizeof(tFilmListNode));
    assert(node != NULL);
    
    // Assign the properties of the nodes
    film_cpy(&node->elem, film);
    node->next = NULL;
    
    // Link the new node to the end of the list
    if (list->first == NULL)
        list->first = node;
    else
        list->last->next = node;
    
    list->last = node;
    list->count++;
    
    return E_SUCCESS;
}

// Remove a film from the list
tApiError filmList_del(tFilmList* list, const char* name) {
    // Check preconditions
    assert(list != NULL);
    assert(name != NULL);
    
    tFilmListNode *node, *prev;
    
    // Iterate until the node and remove it
    node = list->first;
    prev = NULL;
    
    while (node != NULL)
    {
        if (strcmp(node->elem.name, name) == 0)
            break;
        
        prev = node;
        node = node->next;
    }
    
    // If node does not exist, return an error
    if (node == NULL)
        return E_FILM_NOT_FOUND;
    
    // Link the list without the node to remove
    if (prev == NULL)
        list->first = node->next;
    else
        prev->next = node->next;
    
    if (list->last == node)
        list->last = prev;
    
    list->count--;
    
    film_free(&(node->elem));
    free(node);
    
    return E_SUCCESS;
}

// Return a pointer to the film
tFilm* filmList_find(tFilmList list, const char* name) {
    // Check preconditions
    assert(name != NULL);
    
    tFilmListNode *node;
    node = list.first;
    
    while (node != NULL)
    {
        if (strcmp(node->elem.name, name) == 0)
            return &(node->elem);
        
        node = node->next;
    }
    
    return NULL;
}

// Return a pointer to the longest film of the list
tFilm* filmList_longestFind(tFilmList list) {
    printf("[DEBUG: EX1_2/3] Entering filmList_longestFind\n");
    
    if (list.first == NULL) {
        printf("[DEBUG: EX1_2/3] Empty film list\n");
        return NULL;
    }
    
    tFilmListNode* current = list.first;
    tFilm* longest = &(current->elem);
    int maxDuration = current->elem.duration.hour * 60 + current->elem.duration.minutes;
    
    printf("[DEBUG: EX1_2/3] Initial longest film: %s, duration: %d:%d\n", 
           longest->name, longest->duration.hour, longest->duration.minutes);
    
    current = current->next; // Move to the next film
    
    while (current != NULL) {
        int currentDuration = current->elem.duration.hour * 60 + current->elem.duration.minutes;
        printf("[DEBUG: EX1_2/3] Checking film: %s, duration: %d:%d\n", 
               current->elem.name, current->elem.duration.hour, current->elem.duration.minutes);
        
        if (currentDuration > maxDuration) {
            longest = &(current->elem);
            maxDuration = currentDuration;
            printf("[DEBUG: EX1_2/3] New longest film: %s, duration: %d:%d\n", 
                   longest->name, longest->duration.hour, longest->duration.minutes);
        }
        
        current = current->next;
    }
    
    printf("[DEBUG: EX1_2/3] Final longest film: %s, duration: %d:%d\n", 
           longest->name, longest->duration.hour, longest->duration.minutes);
    return longest;
}

// Return a pointer to the longest film of the list
tFilm* freeFilmList_longestFind(tFreeFilmList list) {
    printf("[DEBUG: EX1_2/3] Entering freeFilmList_longestFind\n");
    
    if (list.first == NULL) {
        printf("[DEBUG: EX1_2/3] Empty free film list\n");
        return NULL;
    }
    
    tFreeFilmListNode* current = list.first;
    tFilm* longest = current->elem;
    int maxDuration = current->elem->duration.hour * 60 + current->elem->duration.minutes;
    
    printf("[DEBUG: EX1_2/3] Initial longest free film: %s, duration: %d:%d\n", 
           longest->name, longest->duration.hour, longest->duration.minutes);
    
    current = current->next; // Move to the next film
    
    while (current != NULL) {
        int currentDuration = current->elem->duration.hour * 60 + current->elem->duration.minutes;
        printf("[DEBUG: EX1_2/3] Checking free film: %s, duration: %d:%d\n", 
               current->elem->name, current->elem->duration.hour, current->elem->duration.minutes);
        
        if (currentDuration > maxDuration) {
            longest = current->elem;
            maxDuration = currentDuration;
            printf("[DEBUG: EX1_2/3] New longest free film: %s, duration: %d:%d\n", 
                   longest->name, longest->duration.hour, longest->duration.minutes);
        }
        
        current = current->next;
    }
    
    printf("[DEBUG: EX1_2/3] Final longest free film: %s, duration: %d:%d\n", 
           longest->name, longest->duration.hour, longest->duration.minutes);
    return longest;
}

// Sort a list of films by year
tApiError filmList_SortByYear_Bubble(tFilmList* list) {
    printf("[DEBUG: EX1_6/7] Entering filmList_SortByYear_Bubble\n");
    
    if (list == NULL) {
        printf("[DEBUG: EX1_6/7] List is NULL\n");
        return E_INVALID_ENTRY_TYPE;
    }
    
    if (list->first == NULL || list->first->next == NULL) {
        printf("[DEBUG: EX1_6/7] List is empty or has only one element\n");
        return E_SUCCESS;
    }
    
    int swapped;
    tFilmListNode* ptr1;
    tFilmListNode* lptr = NULL;
    
    do {
        swapped = 0;
        ptr1 = list->first;
        
        while (ptr1->next != lptr) {
            printf("[DEBUG: EX1_6/7] Comparing %s (%d) with %s (%d)\n", 
                   ptr1->elem.name, ptr1->elem.release.year,
                   ptr1->next->elem.name, ptr1->next->elem.release.year);
                   
            // Compare release dates
            if (ptr1->elem.release.year > ptr1->next->elem.release.year || 
                (ptr1->elem.release.year == ptr1->next->elem.release.year && 
                 ptr1->elem.release.month > ptr1->next->elem.release.month) ||
                (ptr1->elem.release.year == ptr1->next->elem.release.year && 
                 ptr1->elem.release.month == ptr1->next->elem.release.month &&
                 ptr1->elem.release.day > ptr1->next->elem.release.day)) {
                // Swap the elements
                tFilm temp;
                film_cpy(&temp, ptr1->elem);
                film_cpy(&ptr1->elem, ptr1->next->elem);
                film_cpy(&ptr1->next->elem, temp);
                
                swapped = 1;
                
                printf("[DEBUG: EX1_6/7] Swapped %s and %s\n", 
                       ptr1->elem.name, ptr1->next->elem.name);
            }
            
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);
    
    printf("[DEBUG: EX1_6/7] Sorting complete\n");
    return E_SUCCESS;
}

// Sort a list of free films by year
tApiError freeFilmList_SortByYear_Bubble(tFreeFilmList* list) {
    printf("[DEBUG: EX1_8/9] Entering freeFilmList_SortByYear_Bubble\n");
    
    if (list == NULL) {
        printf("[DEBUG: EX1_8/9] List is NULL\n");
        return E_INVALID_ENTRY_TYPE;
    }
    
    if (list->first == NULL || list->first->next == NULL) {
        printf("[DEBUG: EX1_8/9] List is empty or has only one element\n");
        return E_SUCCESS;
    }
    
    int swapped;
    tFreeFilmListNode* ptr1;
    tFreeFilmListNode* lptr = NULL;
    
    do {
        swapped = 0;
        ptr1 = list->first;
        
        while (ptr1->next != lptr) {
            printf("[DEBUG: EX1_8/9] Comparing free films %s (%d) with %s (%d)\n", 
                   ptr1->elem->name, ptr1->elem->release.year,
                   ptr1->next->elem->name, ptr1->next->elem->release.year);
                   
            // Compare release dates
            if (ptr1->elem->release.year > ptr1->next->elem->release.year || 
                (ptr1->elem->release.year == ptr1->next->elem->release.year && 
                 ptr1->elem->release.month > ptr1->next->elem->release.month) ||
                (ptr1->elem->release.year == ptr1->next->elem->release.year && 
                 ptr1->elem->release.month == ptr1->next->elem->release.month &&
                 ptr1->elem->release.day > ptr1->next->elem->release.day)) {
                // Swap the elements (pointers)
                tFilm* temp = ptr1->elem;
                ptr1->elem = ptr1->next->elem;
                ptr1->next->elem = temp;
                
                swapped = 1;
                
                printf("[DEBUG: EX1_8/9] Swapped free films %s and %s\n", 
                       ptr1->elem->name, ptr1->next->elem->name);
            }
            
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);
    
    printf("[DEBUG: EX1_8/9] Free film sorting complete\n");
    return E_SUCCESS;
}

// Sort a catalog of films by date
tApiError filmCatalog_SortByYear(tFilmCatalog* catalog) {
    printf("[DEBUG: EX1_10/11] Entering filmCatalog_SortByYear\n");
    
    if (catalog == NULL) {
        printf("[DEBUG: EX1_10/11] Catalog is NULL\n");
        return E_INVALID_ENTRY_TYPE;
    }
    
    // Sort both film lists
    tApiError error1 = filmList_SortByYear_Bubble(&catalog->filmList);
    tApiError error2 = freeFilmList_SortByYear_Bubble(&catalog->freeFilmList);
    
    if (error1 != E_SUCCESS || error2 != E_SUCCESS) {
        printf("[DEBUG: EX1_10/11] Error sorting catalog\n");
        return (error1 != E_SUCCESS) ? error1 : error2;
    }
    
    // Mark the catalog as sorted by date
    catalog->sortedByDate = true;
    
    printf("[DEBUG: EX1_10/11] Catalog sorted by date\n");
    return E_SUCCESS;
}

// Return a pointer to the oldest film of the catalog
tFilm* filmCatalog_OldestFind (tFilmCatalog catalog, bool free) {
    printf("[DEBUG: EX1_16-21] Entering filmCatalog_OldestFind (free=%d)\n", free);
    
    if (free) {
        // Find oldest free film
        if (catalog.freeFilmList.first == NULL) {
            printf("[DEBUG: EX1_16-21] Empty free film list\n");
            return NULL;
        }
        
        if (catalog.sortedByDate) {
            // If the catalog is sorted by date, the first film is the oldest
            printf("[DEBUG: EX1_16-21] Catalog is sorted, returning first free film: %s\n", 
                   catalog.freeFilmList.first->elem->name);
            return catalog.freeFilmList.first->elem;
        } else {
            // Need to find the oldest film
            tFreeFilmListNode* current = catalog.freeFilmList.first;
            tFilm* oldest = current->elem;
            
            printf("[DEBUG: EX1_16-21] Initial oldest free film: %s, date: %d/%d/%d\n", 
                   oldest->name, oldest->release.day, oldest->release.month, oldest->release.year);
            
            current = current->next;
            
            while (current != NULL) {
                printf("[DEBUG: EX1_16-21] Checking free film: %s, date: %d/%d/%d\n", 
                       current->elem->name, current->elem->release.day, 
                       current->elem->release.month, current->elem->release.year);
                
                if (current->elem->release.year < oldest->release.year || 
                    (current->elem->release.year == oldest->release.year && 
                     current->elem->release.month < oldest->release.month) ||
                    (current->elem->release.year == oldest->release.year && 
                     current->elem->release.month == oldest->release.month &&
                     current->elem->release.day < oldest->release.day)) {
                    oldest = current->elem;
                    printf("[DEBUG: EX1_16-21] New oldest free film: %s, date: %d/%d/%d\n", 
                           oldest->name, oldest->release.day, oldest->release.month, oldest->release.year);
                }
                
                current = current->next;
            }
            
            printf("[DEBUG: EX1_16-21] Final oldest free film: %s, date: %d/%d/%d\n", 
                   oldest->name, oldest->release.day, oldest->release.month, oldest->release.year);
            return oldest;
        }
    } else {
        // Find oldest regular film
        if (catalog.filmList.first == NULL) {
            printf("[DEBUG: EX1_16-21] Empty film list\n");
            return NULL;
        }
        
        if (catalog.sortedByDate) {
            // If the catalog is sorted by date, the first film is the oldest
            printf("[DEBUG: EX1_16-21] Catalog is sorted, returning first film: %s\n", 
                   catalog.filmList.first->elem.name);
            return &catalog.filmList.first->elem;
        } else {
            // Need to find the oldest film
            tFilmListNode* current = catalog.filmList.first;
            tFilm* oldest = &current->elem;
            
            printf("[DEBUG: EX1_16-21] Initial oldest film: %s, date: %d/%d/%d\n", 
                   oldest->name, oldest->release.day, oldest->release.month, oldest->release.year);
            
            current = current->next;
            
            while (current != NULL) {
                printf("[DEBUG: EX1_16-21] Checking film: %s, date: %d/%d/%d\n", 
                       current->elem.name, current->elem.release.day, 
                       current->elem.release.month, current->elem.release.year);
                
                if (current->elem.release.year < oldest->release.year || 
                    (current->elem.release.year == oldest->release.year && 
                     current->elem.release.month < oldest->release.month) ||
                    (current->elem.release.year == oldest->release.year && 
                     current->elem.release.month == oldest->release.month &&
                     current->elem.release.day < oldest->release.day)) {
                    oldest = &current->elem;
                    printf("[DEBUG: EX1_16-21] New oldest film: %s, date: %d/%d/%d\n", 
                           oldest->name, oldest->release.day, oldest->release.month, oldest->release.year);
                }
                
                current = current->next;
            }
            
            printf("[DEBUG: EX1_16-21] Final oldest film: %s, date: %d/%d/%d\n", 
                   oldest->name, oldest->release.day, oldest->release.month, oldest->release.year);
            return oldest;
        }
    }
}

// Sort a catalog of films by rating, higher to lower
tApiError filmCatalog_SortByRating(tFilmCatalog* catalog) {
    printf("[DEBUG: EX1_22/23] Entering filmCatalog_SortByRating\n");
    
    if (catalog == NULL) {
        printf("[DEBUG: EX1_22/23] Catalog is NULL\n");
        return E_INVALID_ENTRY_TYPE;
    }
    
    if (catalog->filmList.first == NULL || catalog->filmList.first->next == NULL) {
        printf("[DEBUG: EX1_22/23] Film list is empty or has only one element\n");
        return E_SUCCESS;
    }
    
    // Sort regular films by rating (descending)
    int swapped;
    tFilmListNode* ptr1;
    tFilmListNode* lptr = NULL;
    
    do {
        swapped = 0;
        ptr1 = catalog->filmList.first;
        
        while (ptr1->next != lptr) {
            printf("[DEBUG: EX1_22/23] Comparing %s (%.1f) with %s (%.1f)\n", 
                   ptr1->elem.name, ptr1->elem.rating,
                   ptr1->next->elem.name, ptr1->next->elem.rating);
                   
            // Compare ratings (descending order)
            if (ptr1->elem.rating < ptr1->next->elem.rating) {
                // Swap the elements
                tFilm temp;
                film_cpy(&temp, ptr1->elem);
                film_cpy(&ptr1->elem, ptr1->next->elem);
                film_cpy(&ptr1->next->elem, temp);
                
                swapped = 1;
                
                printf("[DEBUG: EX1_22/23] Swapped %s and %s\n", 
                       ptr1->elem.name, ptr1->next->elem.name);
            }
            
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);
    
    // Sort free films by rating (descending)
    if (catalog->freeFilmList.first != NULL && catalog->freeFilmList.first->next != NULL) {
        tFreeFilmListNode* ptr1;
        tFreeFilmListNode* lptr = NULL;
        
        do {
            swapped = 0;
            ptr1 = catalog->freeFilmList.first;
            
            while (ptr1->next != lptr) {
                printf("[DEBUG: EX1_22/23] Comparing free films %s (%.1f) with %s (%.1f)\n", 
                       ptr1->elem->name, ptr1->elem->rating,
                       ptr1->next->elem->name, ptr1->next->elem->rating);
                       
                // Compare ratings (descending order)
                if (ptr1->elem->rating < ptr1->next->elem->rating) {
                    // Swap the elements (pointers)
                    tFilm* temp = ptr1->elem;
                    ptr1->elem = ptr1->next->elem;
                    ptr1->next->elem = temp;
                    
                    swapped = 1;
                    
                    printf("[DEBUG: EX1_22/23] Swapped free films %s and %s\n", 
                           ptr1->elem->name, ptr1->next->elem->name);
                }
                
                ptr1 = ptr1->next;
            }
            lptr = ptr1;
        } while (swapped);
    }
    
    // Mark the catalog as not sorted by date
    catalog->sortedByDate = false;
    
    printf("[DEBUG: EX1_22/23] Catalog sorted by rating\n");
    return E_SUCCESS;
}

// Remove the films from the list
tApiError filmList_free(tFilmList* list) {
    // Check preconditions
    assert(list != NULL);
    
    tFilmListNode *node, *auxNode;
    
    node = list->first;
    auxNode = NULL;
    
    while (node != NULL)
    {
        auxNode = node->next;
        
        film_free(&(node->elem));
        free(node);
        
        node = auxNode;
    }
    
    filmList_init(list);
    
    return E_SUCCESS;
}

// Initialize the free films list
tApiError freeFilmList_init(tFreeFilmList* list) {
    // Check preconditions
    assert(list != NULL);
    
    list->first = NULL;
    list->last = NULL;
    list->count = 0;
    
    return E_SUCCESS;
}

// Add a new free film to the list
tApiError freeFilmList_add(tFreeFilmList* list, tFilm* film) {
    // Check preconditions
    assert(list != NULL);
    assert(film != NULL);

    if (freeFilmList_find(*list, film->name) != NULL)
        return E_FILM_DUPLICATED;

    tFreeFilmListNode* node = (tFreeFilmListNode*)malloc(sizeof(tFreeFilmListNode));
    assert(node != NULL);

    node->elem = film; // Store the reference
    node->next = NULL;

    if (list->first == NULL)
        list->first = node;
    else
        list->last->next = node;

    list->last = node;
    list->count++;

    return E_SUCCESS;
}

// Remove a free film from the list
tApiError freeFilmList_del(tFreeFilmList* list, const char* name) {
    // Check preconditions
    assert(list != NULL);
    assert(name != NULL);

    tFreeFilmListNode *node = list->first, *prev = NULL;

    while (node != NULL)
    {
        if (strcmp(node->elem->name, name) == 0)
            break;
        prev = node;
        node = node->next;
    }

    if (node == NULL)
        return E_FILM_NOT_FOUND;

    if (prev == NULL)
        list->first = node->next;
    else
        prev->next = node->next;

    if (list->last == node)
        list->last = prev;

    free(node);
    list->count--;

    return E_SUCCESS;
}

// Return a pointer to the free film
tFilm* freeFilmList_find(tFreeFilmList list, const char* name) {
    // Check preconditions
    assert(name != NULL);
    
    tFreeFilmListNode *node;
    node = list.first;
    
    while (node != NULL)
    {
        if (strcmp(node->elem->name, name) == 0)
            return node->elem;
            
        node = node->next;
    }
    
    return NULL;
}

// Remove the free films from the list
tApiError freeFilmsList_free(tFreeFilmList* list) {
    // Check preconditions
    assert(list != NULL);
    
    tFreeFilmListNode *node, *auxNode;
    
    node = list->first;
    auxNode = NULL;
    
    while (node != NULL)
    {
        auxNode = node->next;
        free(node);
        node = auxNode;
    }
    
    freeFilmList_init(list);
    
    return E_SUCCESS;
}

// Initialize the films catalog
tApiError film_catalog_init(tFilmCatalog* catalog) {
    /////////////////////////////////
    // Ex1 PR1 2a
    /////////////////////////////////
    // Check preconditions
    assert(catalog != NULL);
    
    filmList_init(&(catalog->filmList));
    freeFilmList_init(&(catalog->freeFilmList));
    
    /////////////////////////////////
    // PR3_1e
    /////////////////////////////////
    
    return E_SUCCESS;
    /////////////////////////////////
    // return E_NOT_IMPLEMENTED;
}

// Add a new film to the catalog
tApiError film_catalog_add(tFilmCatalog* catalog, tFilm film) {
    /////////////////////////////////
    // Ex1 PR1 2b
    /////////////////////////////////
    tApiError error;
    tFilm *auxFilm;
    
    // Check preconditions
    assert(catalog != NULL);
    
    // Try to add the film to the catalog
    error = filmList_add(&(catalog->filmList), film);
    
    // Get the film from the list if exist and if it is free
    if (film.isFree && error == E_SUCCESS && (auxFilm = filmList_find(catalog->filmList, film.name)) != NULL)
    {
        error = freeFilmList_add(&(catalog->freeFilmList), auxFilm);
        
        // Revert if freeFilmList_add failed
        if (error != E_SUCCESS)
        {
            filmList_del(&(catalog->filmList), film.name);
        }
    }
    
    /////////////////////////////////
    // PR3_1e
    /////////////////////////////////
    
    return error;
    /////////////////////////////////
    // return E_NOT_IMPLEMENTED;
}

// Remove a film from the catalog
tApiError film_catalog_del(tFilmCatalog* catalog, const char* name) {
    /////////////////////////////////
    // Ex1 PR1 2c
    /////////////////////////////////
    // Check preconditions
    assert(catalog != NULL);
    assert(name != NULL);
    
    // Try to remove the film from the free film list
    freeFilmList_del(&(catalog->freeFilmList), name);
    
    // Then, remove the film from the list and return the result
    return filmList_del(&(catalog->filmList), name);
    /////////////////////////////////
    // return E_NOT_IMPLEMENTED;
}

// Return the number of total films
int film_catalog_len(tFilmCatalog catalog) {
    /////////////////////////////////
    // Ex1 PR1 2d
    /////////////////////////////////
    return catalog.filmList.count;
    /////////////////////////////////
    // return -1;
}

// Return the number of free films
int film_catalog_freeLen(tFilmCatalog catalog) {
    /////////////////////////////////
    // Ex1 PR1 2d
    /////////////////////////////////
    return catalog.freeFilmList.count;
    /////////////////////////////////
    // return -1;
}

// Remove the films from the catalog
tApiError film_catalog_free(tFilmCatalog* catalog) {
    /////////////////////////////////
    // Ex1 PR1 2e
    /////////////////////////////////
    // Check preconditions
    assert(catalog != NULL);
    
    freeFilmsList_free(&(catalog->freeFilmList));
    filmList_free(&(catalog->filmList));
    
    return E_SUCCESS;
    /////////////////////////////////
    // return E_NOT_IMPLEMENTED;
}

void filmList_sortByDate(tFilmList* list) {
    printf("[DEBUG: EX1_6/7] Entering filmList_sortByDate\n");
    
    if (list == NULL || list->first == NULL || list->first->next == NULL) {
        printf("[DEBUG: EX1_6/7] List is empty or has only one element\n");
        return;
    }
    
    int swapped;
    tFilmListNode* ptr1;
    tFilmListNode* lptr = NULL;
    
    do {
        swapped = 0;
        ptr1 = list->first;
        
        while (ptr1->next != lptr) {
            printf("[DEBUG: EX1_6/7] Comparing %s (%d) and %s (%d)\n", 
                   ptr1->elem.name, ptr1->elem.release.year,
                   ptr1->next->elem.name, ptr1->next->elem.release.year);
                   
            if (ptr1->elem.release.year > ptr1->next->elem.release.year) {
                // Swap the elements
                tFilm temp;
                film_cpy(&temp, ptr1->elem);
                film_cpy(&ptr1->elem, ptr1->next->elem);
                film_cpy(&ptr1->next->elem, temp);
                
                swapped = 1;
                
                printf("[DEBUG: EX1_6/7] Swapped %s and %s\n", 
                       ptr1->elem.name, ptr1->next->elem.name);
            }
            
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);
    
    printf("[DEBUG: EX1_6/7] Sorting complete\n");
}

void freeFilmList_sortByDate(tFreeFilmList* list) {
    printf("[DEBUG: EX1_6/7] Entering freeFilmList_sortByDate\n");
    
    if (list == NULL || list->first == NULL || list->first->next == NULL) {
        printf("[DEBUG: EX1_6/7] Free film list is empty or has only one element\n");
        return;
    }
    
    int swapped;
    tFreeFilmListNode* ptr1;
    tFreeFilmListNode* lptr = NULL;
    
    do {
        swapped = 0;
        ptr1 = list->first;
        
        while (ptr1->next != lptr) {
            printf("[DEBUG: EX1_6/7] Comparing free films %s (%d) and %s (%d)\n", 
                   ptr1->elem->name, ptr1->elem->release.year,
                   ptr1->next->elem->name, ptr1->next->elem->release.year);
                   
            if (ptr1->elem->release.year > ptr1->next->elem->release.year) {
                // Swap the elements (pointers)
                tFilm* temp = ptr1->elem;
                ptr1->elem = ptr1->next->elem;
                ptr1->next->elem = temp;
                
                swapped = 1;
                
                printf("[DEBUG: EX1_6/7] Swapped free films %s and %s\n", 
                       ptr1->elem->name, ptr1->next->elem->name);
            }
            
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);
    
    printf("[DEBUG: EX1_6/7] Free film sorting complete\n");
}
