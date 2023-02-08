/**************************************************************************
 * C S 429 EEL interpreter
 * 
 * variable.c - This file contains the skeleton of functions to be implemented
 * for EEL-2. When completed, it will contain the code to maintain a hashtable
 * for defined variables.
 * 
 * Work on it only after finishing EEL-0 and EEL-1.
 * 
 * Copyright (c) 2021. S. Chatterjee, X. Shen, T. Byrd. All rights reserved.
 * May not be used, modified, or copied without permission.
 **************************************************************************/ 

#include "ci.h"

table_t *var_table = NULL;
static char *bool_print[] = {"false", "true"};

void init_table(void) {
    var_table = (table_t *) calloc(1, sizeof(table_t));
    if (! var_table) {
        logging(LOG_FATAL, "failed to allocate table");
        return;
    }
    var_table->entries = (entry_t **) calloc(CAPACITY, sizeof(entry_t *));
    if (! var_table->entries) {
        free(var_table);
        logging(LOG_FATAL, "failed to allocate entries");
    }
    return;
}

void delete_entry(entry_t* eptr) {
    if (! eptr) return;
    if (eptr->type == STRING_TYPE) {
        free(eptr->val.sval);
    }
    free(eptr->id);
    free(eptr);
    return;
}

void delete_entries(entry_t *eptr) {
    if (! eptr) return;
    delete_entries(eptr->next);
    delete_entry(eptr);
    return;
}

void delete_table(void) {
    if (! var_table) return;

    for (int i = 0; i < CAPACITY; ++i) {
        delete_entries(var_table->entries[i]);
    }
    free(var_table->entries);
    free(var_table);
    return;
}

/* Pre-defined hash function to index variables by their names. */
unsigned long hash_function(char *s) {
    unsigned long i = 0;
    for (int j=0; s[j]; j++)
        i += s[j];
    return i % CAPACITY;
}

/* init_entry() - provided entry constructor
 * Parameters: Variable name, pointer to a node.
 * Return value: An allocated entry. */
entry_t * init_entry(char *id, node_t *nptr) {
    if (nptr == NULL) {
        logging(LOG_FATAL, "failed to allocate entry");
        return NULL;
    }
    entry_t *eptr = (entry_t *) calloc(1, sizeof(entry_t));
    if (! eptr) {
        logging(LOG_FATAL, "failed to allocate entry");
        return NULL;
    }
    eptr->id = (char *) malloc(strlen(id) + 1);
    if (! id) {
        logging(LOG_FATAL, "failed to allocate entry id");
        free(eptr);
        return NULL;
    }
    strcpy(eptr->id, id);
    eptr->type = nptr->type;
    if (eptr->type == STRING_TYPE) {
        (eptr->val).sval = (char *) malloc(strlen(nptr->val.sval) + 1);
        if (! eptr->val.sval) {
            logging(LOG_FATAL, "failed to allocate string");
            free(eptr->id);
            free(eptr);
            return NULL;
        }
        strcpy(eptr->val.sval, nptr->val.sval);
    } else {
        eptr->val.ival = nptr->val.ival;
    }
    return eptr;
}

/* put() - insert an entry into the hashtable or update the existing entry.
 * Use a linked list to handle collisions.
 * Parameters: Variable name, pointer to a node.
 * Return value: None.
 * Side effect: The entry is inserted into the hashtable, or is updated if
 * it already exists.
 * (STUDENT TODO) 
 */
void put(char *id, node_t *nptr) {
    // Week 3 TODO: Implement adding to the hashtable.
    unsigned long index = hash_function(id); 
    //if there isn't any entry for that hashcode in the hashtable 
    if(var_table->entries[index] == NULL){
        var_table->entries[index] = init_entry(id, nptr); 
    }

    //if the hashcodes are the same and you run into a collision 
    else if(var_table->entries[index] != NULL){
       entry_t* root = var_table->entries[index];
       //go thru all of the entries for that hashcode to check for same id 
       if(strcmp(root -> id, id) == 0){
           delete_entry(root); 
           var_table->entries[index] = init_entry(id, nptr); 
           return; 
       }
       while(root -> next != NULL){
           //if the ids are the same
           if(strcmp(id, root -> next -> id) == 0){
               //delete the old node 
               delete_entry(root -> next); 
               //create the new node 
               var_table->entries[index] = init_entry(id, nptr); 
               return; 
           }
            //if that entry didn't match the current id 
           root -> next = root; 
       }
       //if there were no entries with matching ids
       root -> next = init_entry(id, nptr);
    }


    //if you reassign the variable, then you want to put the new one in its place 
    // else{
    //     //delete the old node
    //     //loop through, strcmp, pass into delete entry
    //     for(int i = 0; i < sizeof(var_table -> entries); i++){
    //         if(strcmp(var_table -> entries[i] -> val.sval, id) == 0){
    //             delete_entry(var_table -> entries[i]); 
    //         }
    //     }

    //     //create the new node 
    //     var_table->entries[index] = init_entry(id, nptr);  
    // }
    return;
}

/* get() - search for an entry in the hashtable.
 * Parameter: Variable name.
 * Return value: Pointer to the matching entry, or NULL if not found.
 * (STUDENT TODO) 
 */
entry_t* get(char* id) {
    // Week 3 TODO: Implement retrieving from the hashtable.
    int index = hash_function(id); 
    entry_t* place = var_table->entries[index];
    //looping through all of the entries in the hashtable for that hashcode 
    while(place != NULL){
        //checking if the id matches
        if(strcmp(place -> id, id) == 0){
            return place; 
        }

        //moving on to the next entry for that hashcode if it doesn't 
        else{
            place = place -> next; 
        }
    }
    
    //if the same id is never found, just return NULL 
    return NULL;
}

void print_entry(entry_t *eptr) {
    if (! eptr) return;
    switch (eptr->type) {
        case INT_TYPE:
            fprintf(outfile, "%s = %d; ", eptr->id, eptr->val.ival);
            break;
        case BOOL_TYPE:
            fprintf(outfile, "%s = %s; ", eptr->id, bool_print[eptr->val.bval]);
            break;
        case STRING_TYPE:
            fprintf(outfile, "%s = \"%s\"; ", eptr->id, eptr->val.sval);
            break;
        default:
            logging(LOG_ERROR, "unsupported entry type for printing");
            break;
    }
    print_entry(eptr->next);
    return;
}

void print_table(void) {
    if (! var_table) {
        logging(LOG_ERROR, "variable table doesn't exist");
        return;
    }
    fprintf(outfile, "\t");
    for (int i = 0; i < CAPACITY; ++i) {
        print_entry(var_table->entries[i]);
    }
    fprintf(outfile, "\n");
    return;
}
