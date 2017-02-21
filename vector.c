#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vector.h"

void vector_init(vector *v)
{
    v->data = NULL;
    v->size = 0;
    v->count = 0;
}
void vector_init_ps(vector *v)
{
    v->count = 0;
    v->size = 10;
    v->data = malloc(sizeof(void*) * v->size);
    memset(v->data, '\0', sizeof(void*) * v->size);
    v->count = v->size;
}
int vector_count(vector *v)
{
    return v->count;
}

void vector_add_st(vector *v, void *e)
{
    if (v->size == 0) {
        v->size = 10;
        v->data = malloc(sizeof(void*) * v->size);
        memset(v->data, '\0', sizeof(void*) * v->size);
    }

    if (v->size == v->count) {
        v->size *= 2;
        v->data = realloc(v->data, sizeof(void*) * v->size);
    }

    v->data[v->count] = e;
    v->count++;
}

void vector_add_int(vector *v, int e)
{
    if (v->size == 0) {
        v->size = 10;
        v->data = malloc(sizeof(int) * v->size);
        memset(v->data, '\0', sizeof(int) * v->size);
    }

    if (v->size == v->count) {
        v->size *= 2;
        v->data = realloc(v->data, sizeof(int) * v->size);
    }

    v->data[v->count] = e;
    v->count++;
}

void vector_set(vector *v, int index, void *e)
{
    if (index >= v->count) {
        v->size *= 2;
        v->data = realloc(v->data, sizeof(int) * v->size);
    }

    v->data[index] = e;
}

void *vector_get(vector *v, int index)
{
    if (index >= v->count) {
        return NULL;
    }

    return v->data[index];
}

void* vector_get_int(vector *v, int index)
{
    if (index >= v->count) {
        return NULL;
    }

    return v->data[index];
}

void vector_delete(vector *v, int index)
{
    if (index >= v->count) {
        return;
    }
    int i, j;
    for (i = index +1, j = index; i < v->count; ++i) {
        printf("i : %d , j : %d\n",i,j );
        v->data[j] = v->data[i];
        ++j;
    }

    v->count--;
}

void vector_free(vector *v)
{
    free(v->data);
}
/*
int main(void)
{
    vector v;
    vector_init(&v);

    vector_add(&v, "emil");
    vector_add(&v, "hannes");
    vector_add(&v, "lydia");
    vector_add(&v, "olle");
    vector_add(&v, "erik");

    int i;
    printf("first round:\n");
    for (i = 0; i < vector_count(&v); i++) {
        printf("%s\n", (char*)vector_get(&v, i));
    }

    vector_delete(&v, 3);
 //   vector_delete(&v, 3);

    printf("second round:\n");
    for (i = 0; i < vector_count(&v); i++) {
        printf("%s\n", (char*)vector_get(&v, i));
    }

    vector_free(&v);

    return 0;
}
*/