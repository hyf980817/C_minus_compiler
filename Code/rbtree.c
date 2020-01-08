/**
 * C语言实现的红黑树(Red Black Tree)
 *
 * @author skywang
 * @date 2013/11/18
 * https://www.cnblogs.com/skywang12345/p/3624177.html#a3
 *
 * @ modified by huangyifeng
 * @ date 2019/12/16
 * change key to C string(char *) and delete some unnecessary function
 */

#include <stdio.h>
#include <stdlib.h>
#include "Tree.h"
#include <assert.h>
#include <stdarg.h>
#define rb_parent(r)   ((r)->parent)
#define rb_color(r) ((r)->color)
#define rb_is_red(r)   ((r)->color==RED)
#define rb_is_black(r)  ((r)->color==BLACK)
#define rb_set_black(r)  do { (r)->color = BLACK; } while (0)
#define rb_set_red(r)  do { (r)->color = RED; } while (0)
#define rb_set_parent(r,p)  do { (r)->parent = (p); } while (0)
#define rb_set_color(r,c)  do { (r)->color = (c); } while (0)
void readStmt(T* stmt, RBRoot* tables[], int stack_depth);
int readBLOCK(T* block, RBRoot* tables[], int stack_depth);



/*
 * 创建红黑树，返回"红黑树的根"！
 */
RBRoot* create_rbtree()
{
    RBRoot *root = (RBRoot *)malloc(sizeof(RBRoot));
    root->node = NULL;

    return root;
}

/*
 * 前序遍历"红黑树"
 */
static void preorder(RBTree tree)
{
    if(tree != NULL)
    {
        //printf("%d ", tree->key);
        printf("%s ", tree->key);
        preorder(tree->left);
        preorder(tree->right);
    }
}
void preorder_rbtree(RBRoot *root) 
{
    if (root)
        preorder(root->node);
}

/*
 * 中序遍历"红黑树"
 */
static void inorder(RBTree tree)
{
    if(tree != NULL)
    {
        inorder(tree->left);
        //printf("%d ", tree->key);
        printf("%s ", tree->key);
        inorder(tree->right);
    }
}

void inorder_rbtree(RBRoot *root) 
{
    if (root)
        inorder(root->node);
}

/*
 * 后序遍历"红黑树"
 */
static void postorder(RBTree tree)
{
    if(tree != NULL)
    {
        postorder(tree->left);
        postorder(tree->right);
        //printf("%d ", tree->key);
        printf("%s ", tree->key);
    }
}

void postorder_rbtree(RBRoot *root)
{
    if (root)
        postorder(root->node);
}

/*
 * (递归实现)查找"红黑树x"中键值为key的节点
 */
Node* search(RBTree x, KeyType key)
{
    //if (x==NULL || x->key==key)
    if (x==NULL || strcmp(x->key, key) == 0)
        return x;

    //if (key < x->key)
    if(strcmp(key,x->key) < 0)
        return search(x->left, key);
    else
        return search(x->right, key);
}
int rbtree_search(RBRoot *root, KeyType key)
{
    if (root)
        return search(root->node, key)? 0 : -1;
}


/*
 * (非递归实现)查找"红黑树x"中键值为key的节点
 */
static Node* iterative_search(RBTree x, KeyType key)
{
    while ((x!=NULL) && (x->key!=key))
    {
        //if (key < x->key)
        if(strcmp(key,x->key) < 0)
            x = x->left;
        else
            x = x->right;
    }

    return x;
}
int iterative_rbtree_search(RBRoot *root, KeyType key)
{
    if (root)
        return iterative_search(root->node, key) ? 0 : -1;
}



/* 
 * 对红黑树的节点(x)进行左旋转
 *
 * 左旋示意图(对节点x进行左旋)：
 *      px                              px
 *     /                               /
 *    x                               y                
 *   /  \      --(左旋)-->           / \                #
 *  lx   y                          x  ry     
 *     /   \                       /  \
 *    ly   ry                     lx  ly  
 *
 *
 */
static void rbtree_left_rotate(RBRoot *root, Node *x)
{
    // 设置x的右孩子为y
    Node *y = x->right;

    // 将 “y的左孩子” 设为 “x的右孩子”；
    // 如果y的左孩子非空，将 “x” 设为 “y的左孩子的父亲”
    x->right = y->left;
    if (y->left != NULL)
        y->left->parent = x;

    // 将 “x的父亲” 设为 “y的父亲”
    y->parent = x->parent;

    if (x->parent == NULL)
    {
        //tree = y;            // 如果 “x的父亲” 是空节点，则将y设为根节点
        root->node = y;            // 如果 “x的父亲” 是空节点，则将y设为根节点
    }
    else
    {
        if (x->parent->left == x)
            x->parent->left = y;    // 如果 x是它父节点的左孩子，则将y设为“x的父节点的左孩子”
        else
            x->parent->right = y;    // 如果 x是它父节点的左孩子，则将y设为“x的父节点的左孩子”
    }
    
    // 将 “x” 设为 “y的左孩子”
    y->left = x;
    // 将 “x的父节点” 设为 “y”
    x->parent = y;
}

/* 
 * 对红黑树的节点(y)进行右旋转
 *
 * 右旋示意图(对节点y进行左旋)：
 *            py                               py
 *           /                                /
 *          y                                x                  
 *         /  \      --(右旋)-->            /  \                     #
 *        x   ry                           lx   y  
 *       / \                                   / \                   #
 *      lx  rx                                rx  ry
 * 
 */
static void rbtree_right_rotate(RBRoot *root, Node *y)
{
    // 设置x是当前节点的左孩子。
    Node *x = y->left;

    // 将 “x的右孩子” 设为 “y的左孩子”；
    // 如果"x的右孩子"不为空的话，将 “y” 设为 “x的右孩子的父亲”
    y->left = x->right;
    if (x->right != NULL)
        x->right->parent = y;

    // 将 “y的父亲” 设为 “x的父亲”
    x->parent = y->parent;

    if (y->parent == NULL) 
    {
        //tree = x;            // 如果 “y的父亲” 是空节点，则将x设为根节点
        root->node = x;            // 如果 “y的父亲” 是空节点，则将x设为根节点
    }
    else
    {
        if (y == y->parent->right)
            y->parent->right = x;    // 如果 y是它父节点的右孩子，则将x设为“y的父节点的右孩子”
        else
            y->parent->left = x;    // (y是它父节点的左孩子) 将x设为“x的父节点的左孩子”
    }

    // 将 “y” 设为 “x的右孩子”
    x->right = y;

    // 将 “y的父节点” 设为 “x”
    y->parent = x;
}

/*
 * 红黑树插入修正函数
 *
 * 在向红黑树中插入节点之后(失去平衡)，再调用该函数；
 * 目的是将它重新塑造成一颗红黑树。
 *
 * 参数说明：
 *     root 红黑树的根
 *     node 插入的结点        // 对应《算法导论》中的z
 */
static void rbtree_insert_fixup(RBRoot *root, Node *node)
{
    Node *parent, *gparent;

    // 若“父节点存在，并且父节点的颜色是红色”
    while ((parent = rb_parent(node)) && rb_is_red(parent))
    {
        gparent = rb_parent(parent);

        //若“父节点”是“祖父节点的左孩子”
        if (parent == gparent->left)
        {
            // Case 1条件：叔叔节点是红色
            {
                Node *uncle = gparent->right;
                if (uncle && rb_is_red(uncle))
                {
                    rb_set_black(uncle);
                    rb_set_black(parent);
                    rb_set_red(gparent);
                    node = gparent;
                    continue;
                }
            }

            // Case 2条件：叔叔是黑色，且当前节点是右孩子
            if (parent->right == node)
            {
                Node *tmp;
                rbtree_left_rotate(root, parent);
                tmp = parent;
                parent = node;
                node = tmp;
            }

            // Case 3条件：叔叔是黑色，且当前节点是左孩子。
            rb_set_black(parent);
            rb_set_red(gparent);
            rbtree_right_rotate(root, gparent);
        } 
        else//若“z的父节点”是“z的祖父节点的右孩子”
        {
            // Case 1条件：叔叔节点是红色
            {
                Node *uncle = gparent->left;
                if (uncle && rb_is_red(uncle))
                {
                    rb_set_black(uncle);
                    rb_set_black(parent);
                    rb_set_red(gparent);
                    node = gparent;
                    continue;
                }
            }

            // Case 2条件：叔叔是黑色，且当前节点是左孩子
            if (parent->left == node)
            {
                Node *tmp;
                rbtree_right_rotate(root, parent);
                tmp = parent;
                parent = node;
                node = tmp;
            }

            // Case 3条件：叔叔是黑色，且当前节点是右孩子。
            rb_set_black(parent);
            rb_set_red(gparent);
            rbtree_left_rotate(root, gparent);
        }
    }

    // 将根节点设为黑色
    rb_set_black(root->node);
}

/*
 * 添加节点：将节点(node)插入到红黑树中
 *
 * 参数说明：
 *     root 红黑树的根
 *     node 插入的结点        // 对应《算法导论》中的z
 */
static void rbtree_insert(RBRoot *root, Node *node)
{
    Node *y = NULL;
    Node *x = root->node;

    // 1. 将红黑树当作一颗二叉查找树，将节点添加到二叉查找树中。
    while (x != NULL)
    {
        y = x;
        
        //if (node->key < x->key)
        if(strcmp(node->key, x->key) < 0)
            x = x->left;
        else
            x = x->right;
    }
    rb_parent(node) = y;

    if (y != NULL)
    {
        //if (node->key < y->key)
        if (strcmp(node->key, y->key) < 0)
            y->left = node;                // 情况2：若“node所包含的值” < “y所包含的值”，则将node设为“y的左孩子”
        else
            y->right = node;            // 情况3：(“node所包含的值” >= “y所包含的值”)将node设为“y的右孩子” 
    }
    else
    {
        root->node = node;                // 情况1：若y是空节点，则将node设为根
    }

    // 2. 设置节点的颜色为红色
    node->color = RED;

    // 3. 将它重新修正为一颗二叉查找树
    rbtree_insert_fixup(root, node);
}

/*
 * 创建结点
 *
 * 参数说明：
 *     key 是键值。
 *     parent 是父结点。
 *     left 是左孩子。
 *     right 是右孩子。
 */
static Node* create_rbtree_node(KeyType key, Node *parent, Node *left, Node* right)
{
    Node* p;

    if ((p = (Node *)malloc(sizeof(Node))) == NULL)
        return NULL;
    p->key = key;
    p->left = left;
    p->right = right;
    p->parent = parent;
    p->color = BLACK; // 默认为黑色

    return p;
}

/* 
 * 新建结点(节点键值为key)，并将其插入到红黑树中
 *
 * 参数说明：
 *     root 红黑树的根
 *     key 插入结点的键值
 * 返回值：
 *     0，插入成功
 *     -1，插入失败
 */
int insert_rbtree(RBRoot *root, KeyType key, symbol s)
{
    Node *node;    // 新建结点

    // 不允许插入相同键值的节点。
    // (若想允许插入相同键值的节点，注释掉下面两句话即可！)
    if (search(root->node, key) != NULL)
        return -1;

    // 如果新建结点失败，则返回。
    if ((node=create_rbtree_node(key, NULL, NULL, NULL)) == NULL)
        return -1;
    node->value = s;
    rbtree_insert(root, node);

    return 0;
}

/*
 * 红黑树删除修正函数
 *
 * 在从红黑树中删除插入节点之后(红黑树失去平衡)，再调用该函数；
 * 目的是将它重新塑造成一颗红黑树。
 *
 * 参数说明：
 *     root 红黑树的根
 *     node 待修正的节点
 */
static void rbtree_delete_fixup(RBRoot *root, Node *node, Node *parent)
{
    Node *other;

    while ((!node || rb_is_black(node)) && node != root->node)
    {
        if (parent->left == node)
        {
            other = parent->right;
            if (rb_is_red(other))
            {
                // Case 1: x的兄弟w是红色的  
                rb_set_black(other);
                rb_set_red(parent);
                rbtree_left_rotate(root, parent);
                other = parent->right;
            }
            if ((!other->left || rb_is_black(other->left)) &&
                (!other->right || rb_is_black(other->right)))
            {
                // Case 2: x的兄弟w是黑色，且w的俩个孩子也都是黑色的  
                rb_set_red(other);
                node = parent;
                parent = rb_parent(node);
            }
            else
            {
                if (!other->right || rb_is_black(other->right))
                {
                    // Case 3: x的兄弟w是黑色的，并且w的左孩子是红色，右孩子为黑色。  
                    rb_set_black(other->left);
                    rb_set_red(other);
                    rbtree_right_rotate(root, other);
                    other = parent->right;
                }
                // Case 4: x的兄弟w是黑色的；并且w的右孩子是红色的，左孩子任意颜色。
                rb_set_color(other, rb_color(parent));
                rb_set_black(parent);
                rb_set_black(other->right);
                rbtree_left_rotate(root, parent);
                node = root->node;
                break;
            }
        }
        else
        {
            other = parent->left;
            if (rb_is_red(other))
            {
                // Case 1: x的兄弟w是红色的  
                rb_set_black(other);
                rb_set_red(parent);
                rbtree_right_rotate(root, parent);
                other = parent->left;
            }
            if ((!other->left || rb_is_black(other->left)) &&
                (!other->right || rb_is_black(other->right)))
            {
                // Case 2: x的兄弟w是黑色，且w的俩个孩子也都是黑色的  
                rb_set_red(other);
                node = parent;
                parent = rb_parent(node);
            }
            else
            {
                if (!other->left || rb_is_black(other->left))
                {
                    // Case 3: x的兄弟w是黑色的，并且w的左孩子是红色，右孩子为黑色。  
                    rb_set_black(other->right);
                    rb_set_red(other);
                    rbtree_left_rotate(root, other);
                    other = parent->left;
                }
                // Case 4: x的兄弟w是黑色的；并且w的右孩子是红色的，左孩子任意颜色。
                rb_set_color(other, rb_color(parent));
                rb_set_black(parent);
                rb_set_black(other->left);
                rbtree_right_rotate(root, parent);
                node = root->node;
                break;
            }
        }
    }
    if (node)
        rb_set_black(node);
}

/* 
 * 删除结点
 *
 * 参数说明：
 *     tree 红黑树的根结点
 *     node 删除的结点
 */
void rbtree_delete(RBRoot *root, Node *node)
{
    Node *child, *parent;
    int color;

    // 被删除节点的"左右孩子都不为空"的情况。
    if ( (node->left!=NULL) && (node->right!=NULL) ) 
    {
        // 被删节点的后继节点。(称为"取代节点")
        // 用它来取代"被删节点"的位置，然后再将"被删节点"去掉。
        Node *replace = node;

        // 获取后继节点
        replace = replace->right;
        while (replace->left != NULL)
            replace = replace->left;

        // "node节点"不是根节点(只有根节点不存在父节点)
        if (rb_parent(node))
        {
            if (rb_parent(node)->left == node)
                rb_parent(node)->left = replace;
            else
                rb_parent(node)->right = replace;
        } 
        else 
            // "node节点"是根节点，更新根节点。
            root->node = replace;

        // child是"取代节点"的右孩子，也是需要"调整的节点"。
        // "取代节点"肯定不存在左孩子！因为它是一个后继节点。
        child = replace->right;
        parent = rb_parent(replace);
        // 保存"取代节点"的颜色
        color = rb_color(replace);

        // "被删除节点"是"它的后继节点的父节点"
        if (parent == node)
        {
            parent = replace;
        } 
        else
        {
            // child不为空
            if (child)
                rb_set_parent(child, parent);
            parent->left = child;

            replace->right = node->right;
            rb_set_parent(node->right, replace);
        }

        replace->parent = node->parent;
        replace->color = node->color;
        replace->left = node->left;
        node->left->parent = replace;

        if (color == BLACK)
            rbtree_delete_fixup(root, child, parent);
        free(node);

        return ;
    }

    if (node->left !=NULL)
        child = node->left;
    else 
        child = node->right;

    parent = node->parent;
    // 保存"取代节点"的颜色
    color = node->color;

    if (child)
        child->parent = parent;

    // "node节点"不是根节点
    if (parent)
    {
        if (parent->left == node)
            parent->left = child;
        else
            parent->right = child;
    }
    else
        root->node = child;

    if (color == BLACK)
        rbtree_delete_fixup(root, child, parent);
    free(node);
}

/* 
 * 删除键值为key的结点
 *
 * 参数说明：
 *     tree 红黑树的根结点
 *     key 键值
 */
void delete_rbtree(RBRoot *root, KeyType key)
{
    Node *z, *node; 

    if ((z = search(root->node, key)) != NULL)
        rbtree_delete(root, z);
}

/*
 * 销毁红黑树
 */
static void rbtree_destroy(RBTree tree)
{
    if (tree==NULL)
        return ;

    if (tree->left != NULL)
        rbtree_destroy(tree->left);
    if (tree->right != NULL)
        rbtree_destroy(tree->right);

    free(tree);
}

void destroy_rbtree(RBRoot *root)
{
    if (root != NULL)
        rbtree_destroy(root->node);

    free(root);
}

/*
 * 打印"红黑树"
 *
 * tree       -- 红黑树的节点
 * key        -- 节点的键值 
 * direction  --  0，表示该节点是根节点;
 *               -1，表示该节点是它的父结点的左孩子;
 *                1，表示该节点是它的父结点的右孩子。
 */
static void rbtree_print(RBTree tree, KeyType key, int direction)
{
    if(tree != NULL)
    {
        if(direction==0)    // tree是根节点
            printf("%2s(B) is root\n", tree->key);
        else                // tree是分支节点
            printf("%2s(%s) is %2s's %6s child\n", tree->key, rb_is_red(tree)?"R":"B", key, direction==1?"right" : "left");

        rbtree_print(tree->left, tree->key, -1);
        rbtree_print(tree->right,tree->key,  1);
    }
}

void print_rbtree(RBRoot *root)
{
    if (root!=NULL && root->node!=NULL)
        rbtree_print(root->node, root->node->key, 0);
}


/*添加一张新的符号表到栈中, 如果添加成功, 返回新的depth, 否则,返回0*/
int pushNewSymbolTable(RBRoot* s[], int old_depth, RBRoot *r)
{
    if(old_depth + 1 >= MAX_DEPTH) {
        return 0;
    }
    s[old_depth + 1] = r;
    return old_depth + 1;
}


//创建一个symbol, 用于插入符号表
symbol createSymbol(int id_type, int var_type, T* syntaxTreeNode)
{
    symbol s;
    s.id_type = id_type;
    s.var_type = var_type;
    s.array[0] = 0;
    s.syntaxTreeNode = NULL;
    return s;
}

/*读取一个变量的类型/函数的返回类型, type最初指向TYPE*/
int readVarType(T* type)
{
    int var_type;
    switch(type->child->type[5])
    {
        case 'I':
            var_type = VAR_INT;
            break;
        case 'F':
            var_type = VAR_FLOAT;
            break;
        case 'C':
            var_type = VAR_CHAR;
            break;
        case 'V':
            var_type = VAR_VOID;
            break;
        default:
            var_type = -1;
    }

    return var_type;
}



/*读取一行变量定义语句, Var最初指向VarDefStmt*/
/* 如果是变量定义, 需要考虑一条语句定义多个变量
 *  VarDefStmt
 *    TYPE
 *        TYPE_INT
 *    VarDecList
 *        VarDec
 *            ID
 *        COMMA
 *        VarDecList
 *            VarDec
 *                ID
 *            COMMA
 *            VarDecList
 *                VarDec
 *                    ID
 *    SEMI
 */
void readVarDefStmt(T* var, RBRoot* table)
{
    var = var->child;
    int var_type = readVarType(var);

    /*读取完type,开始依次读取var_type*/
    var = var->r_brother; //现在var指向VarDecList
    assert(strcmp(var->type, "VarDecList") == 0);
    while(var != NULL)
    {
        
        var = var->child;
        assert(strcmp(var->type, "VarDec") == 0);
        T* id = var->child;
        if(search(table->node, id->id) != NULL)
            fprintf(stderr, "Same variable definition :%s", id->id);
        symbol s = createSymbol(VAR, var_type, var);
        s.syntaxTreeNode = (void *)id;
        if(id->r_brother == NULL) //纯一个id,不是数组, 没有赋值
        {
            ;//目前直接插入即可
        }
        else if(id->r_brother->type[0] == 'L') //左中括号, 数组, 注意考虑多维数组的情况
        {
            
            int i = 1;
            T* arr_num = id->r_brother->r_brother;
            while(arr_num != NULL)
            {
                s.array[i] = arr_num->int_val;
                i++;
                arr_num = arr_num->r_brother->r_brother;
            }
        }
        else //赋值定义,如a=3语句
        {
            ;//目前直接插入即可
        }
        insert_rbtree(table, id->id, s);

        var = var->r_brother;
        if(var != NULL)
            var = var->r_brother;
    }
}



/*读取一个stmt中的block, 读取新block前需要创建新的符号表*/
void readStmt(T* stmt, RBRoot* tables[], int stack_depth)
{
    //找子block, 可以产生子block的Stmt产生式:
    // Stmt: BLOCK
    // Stmt: WHILE...
    // Stmt: FOR ...
    // Stmt IF...
    assert(stmt!= NULL && stmt->child !=NULL);
    T* token = stmt->child;

    if(strcmp(token->type, "BLOCK") == 0)
    {
        RBRoot* new_table = create_rbtree();
        stack_depth = pushNewSymbolTable(tables, stack_depth, new_table);
        stack_depth = readBLOCK(token, tables, stack_depth);
        
    }
    else
    {
        switch(token->type[0])
        {
            case 'B':break;
            case 'C':break;
            case 'R':break;
            case 'E':break;
            case 'I': //IF, 含有Stmt
                token = token->r_brother->r_brother->r_brother->r_brother;
                assert(token->type[0] == 'S');
                readStmt(token, tables, stack_depth);
                if(token->r_brother != NULL)
                    readStmt(token->r_brother->r_brother, tables, stack_depth);
                break;
            case 'W':
                token = token->r_brother->r_brother->r_brother->r_brother;
                assert(token->type[0] == 'S');
                readStmt(token, tables, stack_depth);
                break;
            case 'F':
                token = token->r_brother->r_brother->r_brother->r_brother;
                token = token->r_brother->r_brother->r_brother->r_brother;
                assert(token->type[0] == 'S');
                readStmt(token, tables, stack_depth);
                break;
        }
    }
   
}


/*读取一个BLOCK中的变量定义语句, 假定该block的符号表已经创建好, 位于tables[depth]*/
int readBLOCK(T* block, RBRoot* tables[], int depth)
{
    RBRoot* currentTable = tables[depth];
    //readBLOCK, 由于我们对BLOCK的定义,所有定义都在BLOCK开头的VarDefStmtList中, 所以首先需要读取最开始的语句
    T* varDefStmt = block->child->r_brother->child;
    while(varDefStmt != NULL)
    {
        readVarDefStmt(varDefStmt, currentTable);
        varDefStmt = varDefStmt->r_brother->child;
    }

    //剩下还有子block里的语句
    //子block位于SentenceList中的Sentence的Stmt中
    T* sentenceList = block->child->r_brother->r_brother;
    T* sentence = sentenceList->child;
    while(sentence != NULL)
    {
        T* stmt = sentence->child;
        if(stmt != NULL && stmt->child != NULL)
        {
           readStmt(stmt, tables, depth);
        }
        sentence = sentence->r_brother;
        if(sentence != NULL)
            sentence = sentence->child;
    }

    //最后将这个语法表附着到这个block上
    block->table = currentTable;
    depth--;
    return depth; 
}

/*扫描整个语法树, 创建符号表*/
void addSymbolTable(T* root)
{
    assert(root !=NULL);
    RBRoot* tables[MAX_DEPTH];
    tables[0] = create_rbtree();
    RBRoot* currentTable = tables[0];
    int stack_depth = 0;

    root->table = currentTable;
    
    //首先让root指向第一条语句
    if(root != NULL)
        root = root->child;
    else
    {
        assert(0);
    }
    
    if(root != NULL)
        root = root->child;
    else
    {
        assert(0);
    }
    while(root != NULL)
    {

        if(root->type[0] == 'V')
        {
            readVarDefStmt(root, currentTable);
        }

        else //函数
        {
        /*    
        **    FunDef
        **        TYPE
        **            TYPE_INT
        **        FunDec
        **            ID
        **            LP
        **            RP
        **        BLOCK
        */    
            assert(strcmp(root->type, "FunDef") == 0);
            T* id_type = root->child;
            int var_type = readVarType(id_type);
           

            T* FunDec = id_type->r_brother;

            //检测这个函数的id是否已经存在, 如果不存在, 创建新的符号表
            T* id = FunDec->child;
            if(search(tables[0]->node, id->id) != NULL)
                printf("Same function defition: %s", id->id);
            
            symbol func_symbol = createSymbol(FUNC, var_type, id);
            func_symbol.syntaxTreeNode = (void *)id;
            insert_rbtree(currentTable, id->id, func_symbol);
            RBRoot* func_table = create_rbtree();
            stack_depth++;
            if(stack_depth >= MAX_DEPTH)
                printf("Too many symbol tables!!");
            tables[stack_depth] = func_table;

            //接下来读取函数参数
            T* paralist = id->r_brother->r_brother;
            if(strcmp(paralist->type, "ParaList") == 0)
            {
                while(paralist != NULL)
                {
                    paralist = paralist->child;
                    T* para_type = paralist->child;
                    int var_type = readVarType(para_type);
                    

                    T* para_id = para_type->r_brother->child;
                    symbol s = createSymbol(VAR, var_type, para_type->r_brother);
                    insert_rbtree(tables[stack_depth], para_id->id, s);

                    paralist = paralist->r_brother;
                    if(paralist != NULL)
                        paralist = paralist->r_brother;
                }
            }

            //接下来读取block里的变量
            T* fun_block = FunDec->r_brother;
            readBLOCK(fun_block, tables, stack_depth);


        }

        root = root->r_brother;
        if(root != NULL)
            root = root->child;
        
    }

}