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
void insertChild(T* root, T* newnode);
void insertBrotherToLeft(T* root, T* newnode);
void updateSyntaxTree(T* root, char* name, int isReduction, int reduceLength);
