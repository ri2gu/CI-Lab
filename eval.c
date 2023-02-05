/**************************************************************************
 * C S 429 EEL interpreter
 * 
 * eval.c - This file contains the skeleton of functions to be implemented by
 * you. When completed, it will contain the code used to evaluate an expression
 * based on its AST.
 * 
 * Copyright (c) 2021. S. Chatterjee, X. Shen, T. Byrd. All rights reserved.
 * May not be used, modified, or copied without permission.
 **************************************************************************/ 

#include "ci.h"

/* Function interfaces */

/* Returns true if the given token is a binary operator and false otherwise */
extern bool is_binop(token_t);
/* Returns true if the given token is a unary operator and false otherwise */
extern bool is_unop(token_t);
/* It might be helpful to note that TOK_QUESTION is the only ternary operator. */

char *strrev(char *str);


/* infer_type() - set the type of a non-root node based on the types of children
 * Parameter: A node pointer, possibly NULL.
 * Return value: None.
 * Side effect: The type field of the node is updated.
 * (STUDENT TODO)
 */

static void infer_type(node_t *nptr) {
    // check running status - you can ignore this
    if(nptr == NULL){
        return; 
    }
    if (terminate || ignore_input) return;

    // Week 1 TODO: Implement a recursive post-order traversal of the AST. Remember to include a base case.
    
    infer_type(nptr -> children[0]); 
    infer_type(nptr -> children[1]); 
    infer_type(nptr -> children[2]); 


    switch (nptr->node_type) {
        // For each week, you will also need to include error checking for each type.
        // Week 1 TODO: Implement type inference for all operators on int and bool types.
        // Week 2 TODO: Extend type inference to handle operators on string types.
        // Week 3 TODO: Implement type evaluation for variables.
        case NT_INTERNAL:
            switch (nptr->tok) {
                // For reference, the identity (do nothing) operator is implemented for you.
                case TOK_IDENTITY:
                    nptr->type = nptr->children[0]->type; 
                    break;

                //Set the type 
                case TOK_QUESTION:
                    if(nptr -> children[0] -> type != BOOL_TYPE || nptr -> children[1] -> type != nptr -> children[2]-> type){
                        handle_error(ERR_TYPE); 
                        return; 
                    }
                    nptr -> type = nptr -> children[1] -> type; 
                    break; 
                
                case TOK_PLUS:
                    if(!((nptr -> children[0] -> type == INT_TYPE && nptr -> children[1] -> type == INT_TYPE) ||
                        (nptr -> children[0] -> type == STRING_TYPE && nptr -> children[1] -> type == STRING_TYPE))){
                            handle_error(ERR_TYPE); 
                            return;
                    }
                    if(nptr -> children[0] -> type == INT_TYPE){
                        nptr -> type = INT_TYPE; 
                    }

                    if(nptr -> children[0] -> type == STRING_TYPE){
                        nptr -> type = STRING_TYPE; 
                    } 
                    break;
                
                case TOK_BMINUS:
                    if(nptr -> children[0] -> type != INT_TYPE || nptr -> children[1] -> type != INT_TYPE){
                        handle_error(ERR_TYPE);
                        return; 
                    } 
                    nptr -> type = INT_TYPE; 
                    break;
                
                case TOK_TIMES:
                    if(!((nptr -> children[0] -> type == INT_TYPE && nptr -> children[1] -> type == INT_TYPE) ||
                        (nptr -> children[0] -> type == STRING_TYPE && nptr -> children[1] -> type == INT_TYPE))){                          
                            handle_error(ERR_TYPE); 
                            return; 
                    }

                    if(nptr -> children[0] -> type == STRING_TYPE){
                        nptr -> type = STRING_TYPE;
                    }
                    else{
                        nptr -> type = INT_TYPE; 
                    }
                    break; 

                case TOK_DIV:
                    if(nptr -> children[0] -> type != INT_TYPE || nptr -> children[1] -> type != INT_TYPE){
                        handle_error(ERR_TYPE);
                        return; 
                    }
                    nptr -> type = INT_TYPE; 
                    break;

                case TOK_MOD:
                    if(nptr -> children[0] -> type != INT_TYPE || nptr -> children[1] -> type != INT_TYPE){
                        handle_error(ERR_TYPE);
                        return;
                    } 
                    nptr -> type = INT_TYPE; 
                    break;

                case TOK_AND:
                    if(nptr -> children[0] -> type != BOOL_TYPE || nptr -> children[1] -> type != BOOL_TYPE){
                        handle_error(ERR_TYPE); 
                        return;
                    }
                    nptr -> type = BOOL_TYPE; 
                    break;

                case TOK_OR:
                    if(nptr -> children[0] -> type != BOOL_TYPE || nptr -> children[1] -> type != BOOL_TYPE){
                        handle_error(ERR_TYPE); 
                        return; 
                    }
                    nptr -> type = BOOL_TYPE; 
                    break;

                case TOK_LT:
                    if(!((nptr -> children[0] -> type == INT_TYPE && nptr -> children[1] -> type == INT_TYPE) ||
                        (nptr -> children[0] -> type == STRING_TYPE && nptr -> children[1] -> type == STRING_TYPE))){
                            handle_error(ERR_TYPE); 
                            return; 
                    }
                    nptr -> type = BOOL_TYPE; 
                    break;

                case TOK_GT:
                    if(!((nptr -> children[0] -> type == INT_TYPE && nptr -> children[1] -> type == INT_TYPE) ||
                        (nptr -> children[0] -> type == STRING_TYPE && nptr -> children[1] -> type == STRING_TYPE))){
                            handle_error(ERR_TYPE); 
                            return; 
                    }
                    nptr -> type = BOOL_TYPE; 
                    break;

                case TOK_EQ:
                    if(nptr -> children[0] -> type != nptr -> children[1] -> type){
                            //logging(LOG_ERROR, "Failed ~");
                            handle_error(ERR_TYPE);
                            return; 
                    } 
                    nptr -> type = BOOL_TYPE; 
                    break; 

                case TOK_UMINUS:
                    if(!(nptr -> children[0] -> type == INT_TYPE || nptr -> children[0] -> type == STRING_TYPE)){
                        handle_error(ERR_TYPE); 
                        return; 
                    }
                    if(nptr -> children[0] -> type == STRING_TYPE){
                        nptr -> type = STRING_TYPE;
                    }
                    else{
                        nptr -> type = INT_TYPE; 
                    }
                    break;
                             
                case TOK_NOT:
                    if(nptr -> children[0] -> type != BOOL_TYPE){
                        handle_error(ERR_TYPE); 
                        return; 
                    }
                    nptr -> type = BOOL_TYPE; 
                    break;
                default:
                    break;
            }
        case NT_LEAF:
            break;
        default:
            break;
    }
    return;
}

/* infer_root() - set the type of the root node based on the types of children
 * Parameter: A pointer to a root node, possibly NULL.
 * Return value: None.
 * Side effect: The type field of the node is updated. 
 */

static void infer_root(node_t *nptr) {
    if (nptr == NULL) return;
    // check running status
    if (terminate || ignore_input) return;

    // check for assignment
    if (nptr->type == ID_TYPE) {
        infer_type(nptr->children[1]);
    } else {
        for (int i = 0; i < 3; ++i) {
            infer_type(nptr->children[i]);
        }
        if (nptr->children[0] == NULL) {
            logging(LOG_ERROR, "failed to find child node");
            return;
        }
        nptr->type = nptr->children[0]->type;
    }
    return;
}

/* eval_node() - set the value of a non-root node based on the values of children
 * Parameter: A node pointer, possibly NULL.
 * Return value: None.
 * Side effect: The val field of the node is updated.
 * (STUDENT TODO) 
 */

static void eval_node(node_t *nptr) {
    // check running status - you can ignore this.
    if (terminate || ignore_input) return;

    // Week 1 TODO: Implement a recursive post-order traversal of the AST. Remember to include a base case.
    if(nptr == NULL){
        return; 
    }
    switch (nptr->tok){        
        case TOK_QUESTION: 
            eval_node(nptr -> children[0]);
            if(nptr -> children[0] -> val.bval == true){
                eval_node(nptr -> children[1]);
            }

            else{
                eval_node(nptr -> children[2]); 
            }
            break; 
        
        default:
            eval_node(nptr -> children[0]); 
            eval_node(nptr -> children[1]); 
            eval_node(nptr -> children[2]); 
            break; 

    }

    switch (nptr->node_type) {
        case NT_INTERNAL:
            // Week 1 TODO: Implement evaluation for all operators on int and bool types.
            // Week 2 TODO: Extend evaluation to handle operators on string types.
            if (is_unop(nptr->tok)) {
                switch (nptr->tok) {
                    case TOK_UMINUS:
                        if(nptr -> children[0] -> type == INT_TYPE){
                            nptr -> val.ival = nptr -> children[0] -> val.ival * -1;
                        }
                        //else it would be a string reversal 
                        else{
                            //malloc, deep copy, free
                            char *allocate = malloc(strlen(nptr -> children[0] -> val.sval) + 1); 
                            strcpy(allocate, nptr -> children[0] -> val.sval); 
                            nptr -> val.sval = strrev(allocate); 
                            //char *string = strrev(allocate);
                            free(allocate); 
                            
                        }
                        break; 

                    case TOK_NOT:
                        nptr -> val.bval = !nptr -> children[0] -> val.bval; 
                        break;    
                    default:
                        break;
                }
            }
            if (is_binop(nptr->tok)) {
                switch (nptr->tok) {
                    case TOK_PLUS:
                        if(nptr -> children[0] -> type == INT_TYPE){
                            nptr -> val.ival = nptr -> children[0] -> val.ival + nptr -> children[1] -> val.ival; 
                        }

                        else{
                            char *ptr = malloc(strlen(nptr->children[0]->val.sval) + strlen(nptr -> children[1] ->val.sval) + 1);
                            nptr -> val.sval = ptr; 
                            ptr[0] = '\0'; 
                            strcat(nptr -> val.sval, nptr -> children[0] -> val.sval); 
                            strcat(nptr -> val.sval, nptr -> children[1] -> val.sval); 
                            free(ptr); 



                            // char *allocate = malloc(strlen(nptr -> children[0] -> val.sval) + 1); 
                            // strcpy(allocate, nptr -> children[0] -> val.sval); 
                            // nptr -> val.sval = strrev(allocate); 
                            // //char *string = strrev(allocate);
                            // free(allocate); 






                        }
                        break;
                    
                    case TOK_BMINUS:
                        nptr -> val.ival = nptr -> children[0] -> val.ival - nptr -> children[1] -> val.ival; 
                         break;
                    
                    case TOK_TIMES:
                        if(nptr -> children[0] -> type == INT_TYPE){
                            nptr -> val.ival = nptr -> children[0] -> val.ival * nptr -> children[1] -> val.ival;   
                        }
                        
                        //loop to print how many times it is specified to repeat 
                        else{
                            //shoudl be heap instead of stack 
                            char *ptr = malloc(strlen(nptr -> children[0] -> val.sval) * nptr -> children[1] -> val.ival +1); 
                            nptr -> val.sval = ptr; 
                            int index = 0; 
                            ptr[index] = '\0'; 
                            for(int i = 0; i < nptr -> children[1] -> val.ival; i++){
                                //copy into sval directly and strcat
                                strcat(nptr-> val.sval, nptr -> children[0] -> val.sval); 
                                index++; 
                            }



                        }                 
                        break; 
                    
                    case TOK_DIV:
                        if(nptr -> children[1] -> val.ival == 0){
                            handle_error(ERR_EVAL); 
                            return;
                        }
                        nptr -> val.ival = nptr -> children[0] -> val.ival / nptr -> children[1] -> val.ival;                 
                        break;
                    
                    case TOK_MOD:
                        if(nptr -> children[1] -> val.ival == 0){
                            handle_error(ERR_EVAL); 
                            return; 
                        }
                        nptr -> val.ival = nptr -> children[0] -> val.ival % nptr -> children[1] -> val.ival; 
                        break;
                    
                    case TOK_AND:
                        nptr -> val.bval = nptr -> children[0] -> val.bval && nptr -> children[1] -> val.bval; 
                        break;
                    
                    case TOK_OR:
                        nptr -> val.bval = nptr -> children[0] -> val.bval || nptr -> children[1] -> val.bval; 
                        break;
                    
                    case TOK_GT:
                        if(nptr -> children[0] -> type == INT_TYPE){
                            nptr -> val.bval =  nptr -> children[0] -> val.ival > nptr -> children[1] -> val.ival;
                        }

                        else{
                            int val = strcmp(nptr -> children[0] -> val.sval, nptr -> children[1] -> val.sval); 
                                if(val > 0){
                                    nptr -> val.bval = true; 
                                }

                                else{
                                    nptr -> val.bval = false; 
                                }
                        }
                        break;
                    
                    case TOK_LT:
                            if(nptr -> children[0] -> type == INT_TYPE){
                                nptr -> val.bval = nptr -> children[0] -> val.ival < nptr -> children[1] -> val.ival;
                            }

                            else{
                                int val = strcmp(nptr -> children[0] -> val.sval, nptr -> children[1] -> val.sval); 
                                if(val < 0){
                                    nptr -> val.bval = true; 
                                }

                                else{
                                    nptr -> val.bval = false; 
                                }
                                
                            }
                        break;
                    
                    case TOK_EQ:
                        if(nptr -> children[0] -> type == INT_TYPE){
                            nptr -> val.bval = nptr -> children[0] -> val.ival == nptr -> children[1] -> val.ival; 
                        }

                        else{
                            int val = strcmp(nptr -> children[0] -> val.sval, nptr -> children[1] -> val.sval); 
                            if(val == 0){
                                nptr -> val.bval = true; 
                            }

                            else{
                                nptr -> val.bval = false; 
                            }
                        }
                        break; 

                    default:
                        break;
                }
            }

            //make deep copies to solve the double free error 
            if (nptr->tok == TOK_QUESTION) {
                eval_node(nptr -> children[0]);
                if(nptr -> children[0] -> val.bval == true){
                    eval_node(nptr -> children[1]);
                    switch(nptr -> children[1] -> type){
                        case INT_TYPE:
                            nptr -> val.ival = nptr -> children[1] -> val.ival; 
                            break;
                        case BOOL_TYPE:
                            nptr -> val.bval = nptr -> children[1] -> val.bval; 
                            break; 
                        case STRING_TYPE:; 
                            char *ptr = malloc(strlen(nptr -> children[1] ->val.sval) + 1);
                            nptr -> val.sval = ptr; 
                            // strcpy(nptr -> val.sval, nptr -> children[1] -> val.sval); 
                            // char *ptr = malloc(strlen(nptr -> children[1] -> val.sval) + 1); 
                            // nptr -> val.sval = ptr; 
                            
                            ptr[0] = '\0'; 
                            strcat(nptr-> val.sval, nptr -> children[1] -> val.sval);



                            // (nptr->val).sval = (char *) malloc(sizeof(nptr->children[1]->val.ival) + 1);
                            // strcpy(nptr -> val.sval, nptr -> children[1] -> val.ival);
                            break; 
                        default:
                            break; 
                    }

                }
                else{
                    eval_node(nptr -> children[2]);
                    switch(nptr -> children[2] -> type){
                        case INT_TYPE:
                            nptr -> val.ival = nptr -> children[2] -> val.ival; 
                            break; 
                        case BOOL_TYPE:
                            nptr -> val.bval = nptr -> children[2] -> val.bval;
                            break;  
                        case STRING_TYPE:; 
                            char *ptr = malloc(strlen(nptr -> children[2] -> val.sval) + 1); 
                            nptr -> val.sval = ptr; 
                            ptr[0] = '\0'; 
                            strcat(nptr-> val.sval, nptr -> children[2] -> val.sval);
                            // char *ptr = malloc(strlen(nptr -> children[2] ->val.sval) + 1);
                            // nptr -> val.sval = ptr; 
                            // strcpy(nptr -> val.sval, nptr -> children[2] -> val.sval); 
                            // (nptr->val).sval = (char *) malloc(strlen(nptr->children[2]->val.sval) + 1);
                            // strcpy(nptr -> val.sval, nptr -> children[2] -> val.sval);
                            break; 
                        default:
                            break; 
                    }
                }
                break; 
            }
            // For reference, the identity (do-nothing) operator has been implemented for you.
            if (nptr->tok == TOK_IDENTITY) {
                if (nptr->type == STRING_TYPE) {
                    // Week 2 TODO: You'll need to make a copy of the string.
                    (nptr->val).sval = (char *) malloc(strlen(nptr->children[0]->val.sval) + 1);
                    strcpy(nptr->val.sval, nptr->children[0]->val.sval);
                    // char *ptr = malloc(strlen(nptr -> children[0] -> val.sval) + 1); 
                    // nptr -> val.sval = ptr; 
                    // ptr[0] = '\0'; 
                    // strcat(nptr-> val.sval, nptr -> children[0] -> val.sval);
                } else {
                    nptr->val.ival = nptr->children[0]->val.ival;
                }
            }
            break;
        case NT_LEAF:
            break;
        default:
            break;
    }
    return;
}

/* eval_root() - set the value of the root node based on the values of children 
 * Parameter: A pointer to a root node, possibly NULL.
 * Return value: None.
 * Side effect: The val field of the node is updated. 
 */

void eval_root(node_t *nptr) {
    if (nptr == NULL) return;
    // check running status
    if (terminate || ignore_input) return;

    // check for assignment
    if (nptr->type == ID_TYPE) {
        eval_node(nptr->children[1]);
        if (terminate || ignore_input) return;
        
        if (nptr->children[0] == NULL) {
            logging(LOG_ERROR, "failed to find child node");
            return;
        }
        put(nptr->children[0]->val.sval, nptr->children[1]);
        return;
    }

    for (int i = 0; i < 2; ++i) {
        eval_node(nptr->children[i]);
    }
    if (terminate || ignore_input) return;
    
    if (nptr->type == STRING_TYPE) {
        (nptr->val).sval = (char *) malloc(strlen(nptr->children[0]->val.sval) + 1);
        if (! nptr->val.sval) {
            //logging(LOG_FATAL, "failed to allocate string");
            return;
        }
        strcpy(nptr->val.sval, nptr->children[0]->val.sval);
    } else {
        nptr->val.ival = nptr->children[0]->val.ival;
    }
    return;
}

/* infer_and_eval() - wrapper for calling infer() and eval() 
 * Parameter: A pointer to a root node.
 * Return value: none.
 * Side effect: The type and val fields of the node are updated. 
 */

void infer_and_eval(node_t *nptr) {
    infer_root(nptr);
    eval_root(nptr);
    return;
}

/* strrev() - helper function to reverse a given string 
 * Parameter: The string to reverse.
 * Return value: The reversed string. The input string is not modified.
 * (STUDENT TODO)
 */

char *strrev(char *str) {
    // Week 2 TODO: Implement copying and reversing the string.
    char *string = malloc(strlen(str) + 1); 
    int n = strlen(str); 
    int index = 0; 
    for(int i = n-1; i >= 0; i--){
        string[index] = str[i]; 
        index++; 
    }
    string[index] = '\0'; 

    return string;
}

/* cleanup() - frees the space allocated to the AST
 * Parameter: The node to free.
 */
void cleanup(node_t *nptr) {
    // Week 2 TODO: Recursively free each node in the AST
    if(nptr != NULL){
        cleanup(nptr -> children[0]);
        cleanup(nptr -> children[1]);
        cleanup(nptr -> children[2]); 
        //free the string 
        if(nptr -> type == STRING_TYPE){
            free(nptr -> val.sval);
        }
        //free the node 
        free(nptr); 
    }
    return;
}