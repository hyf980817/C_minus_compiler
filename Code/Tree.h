#ifndef _TREE_
#define _TREE_

#define RED      0    // 红色节点
#define BLACK    1    // 黑色节点
#define FUNC 2
#define VAR 3
#define MAX_DEPTH 10
#define VAR_INT 255
#define VAR_FLOAT 256
#define VAR_CHAR 257
#define VAR_VOID 258



/**************************************symbol数据结构的定义**************************************/
#include <string.h>


typedef char* KeyType;

typedef struct{
    int id_type;
    union{
        int var_type;
        int return_type;
    };
    int array[MAX_DEPTH]; /*数组类型, 第一个元素存储维度, 如果不是数组, 第一个元素为0,后面的元素是各个维度的长度, 至多支持4维*/
    void *syntaxTreeNode; //这个用于指向语法树中对应的节点, 无论是函数还是变量永远指向id
}symbol;




/**************************************红黑树的定义**************************************/
// 红黑树的节点
typedef struct RBTreeNode{
    unsigned char color;        // 颜色(RED 或 BLACK)
    KeyType   key;                    // 关键字(键值)
    struct RBTreeNode *left;    // 左孩子
    struct RBTreeNode *right;    // 右孩子
    struct RBTreeNode *parent;    // 父结点
    symbol value;
}Node, *RBTree;

// 红黑树的根
typedef struct rb_root{
    Node *node;
}RBRoot;

// 创建红黑树，返回"红黑树的根"！
RBRoot* create_rbtree();

// 销毁红黑树
void destroy_rbtree(RBRoot *root);

// 将结点插入到红黑树中。插入成功，返回0；失败返回-1。
int insert_rbtree(RBRoot *root, KeyType key, symbol s);

// 删除结点(key为节点的值)
void delete_rbtree(RBRoot *root, KeyType key);


// 前序遍历"红黑树"
void preorder_rbtree(RBRoot *root);
// 中序遍历"红黑树"
void inorder_rbtree(RBRoot *root);
// 后序遍历"红黑树"
void postorder_rbtree(RBRoot *root);

// (递归实现)查找"红黑树"中键值为key的节点。找到的话，返回0；否则，返回-1。
int rbtree_search(RBRoot *root, KeyType key);
// (非递归实现)查找"红黑树"中键值为key的节点。找到的话，返回0；否则，返回-1。
int iterative_rbtree_search(RBRoot *root, KeyType key);

// 打印红黑树
void print_rbtree(RBRoot *root);

int pushNewSymbolTable(RBRoot* s[], int old_depth, RBRoot *r);


/**************************************语法树的定义**************************************/
typedef struct T {
    union {
        int type_int;
        float type_flat;
        char type_char;
        char* type_str;
    };
    char *name;
    RBRoot *table;
    struct T* child;
    struct T* l_brother;
    struct T* r_brother;
}T;

T* initTreeNode(const char* const name);
void insertChild(T* root, int n , ...);
void insertBrotherToRight(T* root, T* newnode);

void addSymbolTable(T* root);

#endif