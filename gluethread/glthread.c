#include "glthread.h"
#include <stdlib.h>

void initGlthread(glthread_t *glthread){

    glthread->left = NULL;
    glthread->right = NULL;
}

void glthreadAddNext(glthread_t *curr_glthread, glthread_t *new_glthread){
    if (!curr_glthread || !new_glthread) {
        // Handle the case where either current or new glthread is null.
        return;
    } 
    if(!curr_glthread->right){
        curr_glthread->right = new_glthread;
        new_glthread->left = curr_glthread;
        return;
    }
    glthread_t *temp = curr_glthread->right;
    curr_glthread->right = new_glthread;
    new_glthread->left = curr_glthread;
    new_glthread->right = temp;
    temp->left = new_glthread;
}

void glthreadAddBefore(glthread_t *curr_glthread, glthread_t *new_glthread){

    if(!curr_glthread->left){
        new_glthread->left = NULL;
        new_glthread->right = curr_glthread;
        curr_glthread->left = new_glthread;
        return;
    }

    glthread_t *temp = curr_glthread->left;
    temp->right = new_glthread;
    new_glthread->left = temp;
    new_glthread->right = curr_glthread;
    curr_glthread->left = new_glthread;
}

void removeGlthread(glthread_t *curr_glthread){
    if(curr_glthread->right) curr_glthread->right->left=curr_glthread->left;
    if(curr_glthread->left) curr_glthread->left->right=curr_glthread->right;
    free(curr_glthread);
}

void deleteGlthreadList(glthread_t *base_glthread){
    glthread_t *glthreadptr = NULL;
    ITERATE_GLTHREAD_BEGIN(base_glthread,glthreadptr){
        removeGlthread(base_glthread);
    } ITERATE_GLTHREAD_END(base_glthread,glthreadptr);
}

void glthreadAddLast(glthread_t *base_glthread, glthread_t *new_glthread){
    glthread_t *glthreadptr = NULL;
    glthread_t *prevglthreadptr = NULL;

    ITERATE_GLTHREAD_BEGIN(base_glthread,glthreadptr){
        prevglthreadptr = base_glthread;
    } ITERATE_GLTHREAD_END(base_glthread,glthreadptr);
  
    if(prevglthreadptr) glthreadAddNext(prevglthreadptr, new_glthread); 

    else glthreadAddNext(base_glthread, new_glthread);
}


unsigned int getGlthreadListCount(glthread_t *base_glthread){

    unsigned int count = 0;
    glthread_t *glthreadptr = NULL;
    ITERATE_GLTHREAD_BEGIN(base_glthread,glthreadptr){
        count++; 
    }    
    ITERATE_GLTHREAD_END(base_glthread,glthreadptr);
    return count;
}


 

#if 0
void *gl_thread_search(glthread_t *base_glthread, 
    void *(*thread_to_struct_fn)(glthread_t *), void *key,int (*comparison_fn)(void *, void *)){
    return NULL;
}
#endif