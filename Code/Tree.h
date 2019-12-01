#pragma once
typedef struct T {
    union {
        int type_int;
        float type_flat;
        char type_char;
        char* type_str;
    };
    char *name;
    struct T* child;
    struct T* l_brother;
    struct T* r_brother;
}T;

T* initTreeNode(char* name);
void insertChild(T* root, int n , ...);
void insertBrotherToRight(T* root, T* newnode);


