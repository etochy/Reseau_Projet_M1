#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vector.h"

#define TAILLE 10;

void vector_init(vector *v)
{
    v->data = NULL;
    v->size = 0;
    v->count = 0;
}

int vector_count(vector *v)
{
    return v->count;
}

void vector_add_st(vector *v, void *e)
{
    if (v->size == 0) {
        v->size = TAILLE;
        v->data = malloc(sizeof(void*) * v->size);
        memset(v->data, '\0', sizeof(void*) * v->size);
    }

    if (v->size == v->count) {
        v->size *= 2;
        int *item = realloc(v->data, sizeof(void*) * v->size);
        if(item){
            v->data = item;
        }
    }

    v->data[v->count] = e;
    v->count++;
}

void vector_add_int(vector *v, int e)
{
    if (v->size == 0) {
        v->size = TAILLE;
        v->data = malloc(sizeof(int) * v->size);
        memset(v->data, '\0', sizeof(int) * v->size);
    }

    if (v->size == v->count) {
        v->size *= 2;
        printf("test 1");
        int *item = realloc(v->data, sizeof(int*) * v->size);
        printf("test 2");
        if(item){
            printf("test 3");
            v->data = item;
        }
    }

    v->data[v->count] = e;
    v->count++;
}

void vector_set(vector *v, int index, void *e)
{
    if (index >= v->count) {
        v->size *= 2;
        int *item = realloc(v->data, sizeof(void*) * v->size);
        if(item){
            v->data = item;
        }
    }

    char a[20];
    strcpy(a,e);
    v->data[index] = a;
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

void vector_init_ps(vector *v)
{
    v->count = 0;
    v->size = TAILLE;
    v->data = malloc(sizeof(void*) * v->size);
    memset(v->data, '\0', sizeof(void*) * v->size);
    v->count = v->size;

    int i;
    char an[20] = "client";
    
    for (i = 0; i < v->size; ++i){
        //v->data[i] = an;
        vector_set_ps(v,i,an);
        printf("init pseudo %d : %s\n", i, vector_get(v,i));
        //an = "";
    }

    for (i = 0; i < v->size; ++i){
        printf("init pseudo %d : %s\n", i, v->data[i]);
    }
}
void vector_add_ps(vector *v)
{
    if (v->size == v->count) {
        v->size *= 2;
        int *item = realloc(v->data, sizeof(void*) * v->size);
        if(item){
            v->data = item;
        }
    }
    /*
    int i;
    char in[10];
    char an[20] = "client";

    for (i = v->count ; i < v->size; ++i){
        strcpy(an,"client");
        sprintf(in, "%d", i);
        strcat(an, " ");
        strcat(an, in);
        v->data[i] = an;
    }
    */
    v->count = v->size;
}

void vector_set_ps(vector *v, int index, void *e)
{
    if (index >= v->count) {
        vector_add_ps(v);
    }
    v->data[index] = e;
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