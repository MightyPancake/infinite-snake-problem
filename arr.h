#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef ARR_H
#define ARR_H

typedef unsigned long long arr_index;

typedef struct Array {
    int              cap;
    arr_index        len;
    int              res;
    void*            arr;
} Array;

//new_arr - Creates a new Array without given type.
#define new_arr() ({ \
    Array* arr = malloc(sizeof(Array)); \
    arr->len = 0; \
    arr->cap = 0; \
    arr->res = 10; \
    arr->arr = malloc(0); \
    arr; \
})

//new_arr - Creates a new EMPTY Array of given type and length of LEN.
#define new_arr_len(T, LEN) ({ \
    Array* arr = malloc(sizeof(Array)); \
    arr->len = LEN; \
    arr->cap = LEN; \
    arr->res = 10; \
    arr->arr = calloc(LEN, sizeof(T)); \
    arr; \
})

/*
//arr_from - Creates a new Array of the given type. The type doesn't really change anything in this macro, since the initial size is 0 anyway.
#define arr_from(T, STATIC_ARR) ({ \
    Array* dynarr = malloc(sizeof(Array)); \
    T arr[ ] = STATIC_ARR; \
    dynarr->arr; \
    dynarr->len = sizeof(arr)/sizeof(T); \
    dynarr->cap = dynarr->len; \
    dynarr->res = 10; \
    dynarr; \
})
*/


//arr_push - Appends ELEM to the end of ARR of type T. Returns newly pushed element.
#define arr_push(T, ARR, ELEM) ({ \
    if (ARR->len == ARR->cap){ \
        ARR->cap += (ARR->res); \
        ARR->arr = realloc(ARR->arr, ARR->cap*sizeof(T)); \
    } \
    ((T*)(ARR->arr))[ARR->len] = (ELEM); \
    ARR->len++; \
})

//arr_pop - Pops and returns last element from ARR of type T
#define arr_pop(T, ARR) ({ \
    ARR->len--; \
    (T)(((T*)(ARR->arr))[ARR->len]); \
})

//arr_set - Sets the value of ARR of type T at INDEX to ELEM. Returns nothing(?)
#define arr_set(T, ARR, INDEX, ELEM) ({ \
    ((T*)(ARR->arr))[INDEX] = ELEM; \
})

//arr_last - Returns the last element from ARR of type T
#define arr_last(T, ARR) ({ (T)(((T*)(ARR->arr))[ARR->len-1]); })
//arr_last - Returns the last element from ARR of type T
#define arr_first(T, ARR) ({ (T)(((T*)(ARR->arr))[0]); })

//arr_at - Returns the INDEX-th element of ARR
#define arr_at(T, ARR, INDEX) ({ (T)(((T*)(ARR->arr))[INDEX]); })

//arr_ins - Inserts ELEM at INDEX in ARR of type T. This pushes all next elements by one, therefore array can change size!
#define arr_ins(T, ARR, INDEX, ELEM) ({ \
    if (ARR->len == ARR->cap){ \
        ARR->cap += (ARR->res); \
        ARR->arr = realloc(ARR->arr, ARR->cap*sizeof(T)); \
    } \
    memmove(&(((int*)(ARR->arr))[INDEX+1]), &(((int*)(ARR->arr))[INDEX]), (ARR->len - INDEX)*sizeof(T)); \
    ((T*)(ARR->arr))[INDEX] = (ELEM); \
    ARR->len++; \
    (T)(((T*)(ARR->arr))[INDEX]); \
})

//arr_del - Deletes (and returns!) element at INDEX in ARR of type T. This makes the ARR's len smaller by one.
#define arr_del(T, ARR, INDEX) ({ \
    T ret = (T)(((T*)(ARR->arr))[INDEX]); \
    memmove(&(((int*)(ARR->arr))[INDEX]), &(((int*)(ARR->arr))[INDEX+1]), (ARR->len - INDEX)*sizeof(T)); \
    ARR->len--; \
    (T)ret; \
})

//arr_rsz - Resizes the ARR of type T.
#define arr_rsz(T, ARR, SZ) ({ \
    (ARR)->cap = SZ; \
    (ARR)->arr = realloc((ARR)->arr, ((ARR)->cap)*sizeof(T)); \
})

//arr_cpy - Returns a copy of ARR of type T.
#define arr_cpy(T, ARR) ({ \
    Array* cpy = malloc(sizeof(Array)); \
    cpy->len = ARR->len; \
    cpy->cap = ARR->cap; \
    cpy->res = ARR->res; \
    cpy->arr = malloc(sizeof(T)*(ARR->len)); \
    memcpy(cpy->arr, ARR->arr, sizeof(T)*(ARR->len)); \
    cpy; \
})

//arr_for - Macro to iterate over ARR of type T. INDEX will be increased, ELEM is the element at INDEX, code is for's body.
#define for_arr(T, ARR, INDEX, ELEM, CODE) ({ \
    T ELEM; \
    for (arr_index INDEX = 0; INDEX<ARR->len; INDEX++){ \
        ELEM = (T)(((T*)(ARR->arr))[INDEX]); \
        CODE \
    } \
});

//Free
void free_arr(Array* arr){
    if (arr->arr != NULL && arr->len > 0)
        free(arr->arr);
    free(arr);
    arr = NULL;
}

#endif      //ARR_H DEFINED