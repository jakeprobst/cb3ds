#ifndef _AVLTREE_H_
#define _AVLTREE_H_

// some sort of avl tree thing that I attempted to implment after reading about
// it on wikipedia. it seems to be correct.
// used for reference:
// http://www.geeksforgeeks.org/avl-tree-set-1-insertion/ 
// http://www.geeksforgeeks.org/avl-tree-set-2-deletion/
// http://www.eternallyconfuzzled.com/tuts/datastructures/jsw_tut_avl.aspx

typedef struct avltree_node_t {
    void* key;
    void* value;
    
    int height;
    struct avltree_node_t* left;
    struct avltree_node_t* right;
} avltree_node_t;


typedef struct avltree_t {
    avltree_node_t* root;
    
    int (*compare)(void*, void*);
    int (*destroy)(void*);
} avltree_t;



int avltree_init(avltree_t*, int (*compare)(void*, void*));
int avltree_destroy(avltree_t*);

int avltree_insert(avltree_t*, void*, void*);
int avltree_remove(avltree_t*, void*, int (*func)(void*, void*, void*));
void* avltree_get(avltree_t*, void*);

int avltree_foreach_inorder(avltree_t*, int (*func)(void*, void*, void*), void*);
int avltree_foreach_postorder(avltree_t*, int (*func)(void*, void*, void*), void*);

int avltree_compare_int(void*, void*);
int avltree_compare_str(void*, void*);

int avltree_free_key(void*, void*, void*);
int avltree_free_keyvalue(void*, void*, void*);




#endif /* _AVLTREE_H_ */
