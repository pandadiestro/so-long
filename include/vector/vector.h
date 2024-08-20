#ifndef _vector_h
#define _vector_h
#include <stddef.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define define_vector_for_type(typeofx) \
typedef struct { \
    typeofx (*arr); \
    size_t size; \
    size_t cap; \
} vector_ ## typeofx; \
\
vector_ ## typeofx* \
newVec_ ## typeofx (void) \
{ \
    vector_ ## typeofx* newvec = (vector_ ## typeofx*) calloc(1, sizeof(vector_ ## typeofx)); \
    newvec->arr = calloc(2, sizeof(typeofx)); \
    newvec->cap = 2; \
    return newvec; \
} \
\
static int8_t \
grow_ ## typeofx (vector_ ## typeofx* vec, size_t factor) \
{ \
    if (vec == NULL) \
        return -1; \
 \
    if (vec->cap*factor <= vec->cap) \
        return -2; \
 \
    vec->cap *= factor; \
    vec->arr = realloc(vec->arr, vec->cap * sizeof(typeofx)); \
    return 0; \
} \
\
int8_t \
push_ ## typeofx (vector_ ## typeofx* vec, typeofx in) \
{ \
    if (vec == NULL) \
        return -1; \
 \
    if (vec->size+1 <= vec->size) \
        return -2; \
 \
    if (++vec->size > vec->cap) { \
        int err = grow_ ## typeofx (vec, 2); \
        if (err != 0) \
            return err; \
    } \
 \
    vec->arr[vec->size-1] = in; \
    return 0; \
} \
\
void \
printall_ ## typeofx (vector_ ## typeofx* vec, char* format)\
{\
    for (size_t i = 0; i < vec->size; i++)\
        printf(format, vec->arr[i]);\
}\
\
void \
destroy_ ## typeofx (vector_ ## typeofx* vec) \
{ \
    free(vec->arr); \
    free(vec); \
    vec = NULL; \
    return; \
} \
\
void* \
arr_ ## typeofx (vector_ ## typeofx* vec) \
{ \
    return (void*) vec->arr; \
} \
\
size_t \
size ## typeofx (vector_ ## typeofx* vec) \
{ \
    return vec->size; \
} \
\
size_t \
cap_ ## typeofx (vector_ ## typeofx* vec) \
{ \
    return vec->cap; \
}

/*
 * generic vector type catcher constructor */
#define vect(t) vector_ ## t*

/*
 * returns a new vector of type `t`, however, there
 * is no predefined "return type catcher" in c (say,
 * let, var, etc.) so you must use `vec(t)` as return
 * type, which will expand to a pointer to the vector */
#define newvector(t) newVec_ ## t()

#define pushtovec(t, vec, in) push_ ## t (vec, in)

#define destroyvec(t, vec) destroy_ ## t (vec)

#define printall(t, vec, format) printall_ ## t (vec, format)

#endif

