#ifndef GLTHREAD_H
#define GLTHREAD_H

typedef struct glthread{

    struct glthread *left;
    struct glthread *right;
} glthread_t;

void glthreadAddNext(glthread_t *base_glthread, glthread_t *new_glthread);

void glthreadAddBefore(glthread_t *base_glthread, glthread_t *new_glthread);

void removeGlthread(glthread_t *glthread);

void initGlthread(glthread_t *glthread);

void glthreadAddLast(glthread_t *base_glthread, glthread_t *new_glthread);


#define IS_GLTHREAD_LIST_EMPTY(glthreadptr)\
    ((glthreadptr)->right == NULL && (glthreadptr)->left == NULL)

#define GLTHREAD_TO_STRUCT(fn_name, structure_name, field_name)\
static inline structure_name *fn_name(glthread_t *ptr){\
    return (structure_name*)((char*)ptr-(char*)&(((structure_name*)0)->field_name));\
}

#define BASE(glthreadptr)   ((glthreadptr)->right)

#define ITERATE_GLTHREAD_BEGIN(glthreadptrstart, glthreadptr)                                      \
{                                                                                                  \
    glthread_t *_glthread_ptr = NULL;                                                              \
    glthreadptr = BASE(glthreadptrstart);                                                          \
    for(; glthreadptr!= NULL; glthreadptr = _glthread_ptr){                                        \
        _glthread_ptr = (glthreadptr)->right;

#define ITERATE_GLTHREAD_END(glthreadptrstart, glthreadptr)                                        \
}}

#define GLTHREAD_GET_USER_DATA_FROM_OFFSET(glthreadptr, offset)\
    (void *)((char *)(glthreadptr) - offset)

void deleteGlthreadList(glthread_t *base_glthread);

unsigned int glthreadListCount(glthread_t *base_glthread);

void glthreadPriorityInsert(glthread_t *base_glthread,glthread_t *glthread,int (*comp_fn)(void *, void *),int offset);

#endif