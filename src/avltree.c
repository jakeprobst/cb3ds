#include <stdlib.h>
#include <string.h>

#include "avltree.h"

int avltree_init(avltree_t* tree, int (*compare)(void*, void*))
{
    tree->root = NULL;
    tree->compare = compare;
    
    return 1;
}

int _avltree_node_destroy(avltree_node_t* node)
{
    if (node == NULL)
        return 0;
    _avltree_node_destroy(node->left);
    _avltree_node_destroy(node->right);
    free(node);
    
    return 1;
}

int avltree_destroy(avltree_t* tree)
{
    _avltree_node_destroy(tree->root);
    
    return 1;
}

int _max(int a, int b)
{
    if (a > b)
        return a;
    return b;
}

int _height(avltree_node_t* node)
{
    if (node == NULL)
        return 0;
    return node->height;
}

int _balance(avltree_node_t* node)
{
    if (node == NULL)
        return 0;
    return _height(node->left) - _height(node->right);
}

avltree_node_t* _rotate_left(avltree_node_t* node)
{
    avltree_node_t* tmp = node->right;
    node->right = tmp->left;
    tmp->left = node;
    
    node->height = _max(_height(node->left), _height(node->right)) + 1;
    tmp->height = _max(_height(tmp->left), _height(tmp->right)) + 1;
    return tmp;
}

avltree_node_t* _rotate_right(avltree_node_t* node)
{
    avltree_node_t* tmp = node->left;
    node->left = tmp->right;
    tmp->right = node;
    
    node->height = _max(_height(node->left), _height(node->right)) + 1;
    tmp->height = _max(_height(tmp->left), _height(tmp->right)) + 1;
    return tmp;
}


avltree_node_t* _tree_insert(avltree_node_t* root, avltree_node_t* node, int (*compare)(void*, void*))
{
    if (root == NULL) {
        return node;
    }
    
    int comp = compare(root->key, node->key);
    if (comp > 0) { // root < node
        root->left = _tree_insert(root->left, node, compare);
    }
    else if (comp < 0){ // root > node
        root->right = _tree_insert(root->right, node, compare);
    }
    else {
        // equal, just ignore it I guess. (this shouldnt happen)
        // memory leak if an item is added where a key already exists
    }
    
    root->height = _max(_height(root->right), _height(root->left)) + 1;
    int balance = _balance(root);
    
    if (balance > 1 && compare(root->left->key, node->key) > 0) { // root > node
        return _rotate_right(root);
    }
    if (balance > 1 && compare(root->left->key, node->key) < 0) { // root < node
        root->left = _rotate_left(root->left);
        return _rotate_right(root);
    }
    if (balance < -1 && compare(root->right->key, node->key) < 0) { // root < node
        return _rotate_left(root);
    }
    if (balance < -1 && compare(root->right->key, node->key) > 0) { // root < node
        root->right = _rotate_right(root->right);
        return _rotate_left(root);
    }
    
    return root;
}

int avltree_insert(avltree_t* tree, void* key, void* value)
{
    avltree_node_t* node = malloc(sizeof(avltree_node_t));
    if (node == NULL)
        return 0;
    node->key = key;
    node->value = value;
    node->height = 1;
    node->left = node->right = NULL;
    
    tree->root = _tree_insert(tree->root, node, tree->compare);
    
    return 1;
}

avltree_node_t* _tree_get(avltree_node_t* node, void* key, int (*compare)(void*, void*))
{
    if (node == NULL) {
        return NULL;
    }
    int comp = compare(node->key, key);
    if (comp > 0) {
        return _tree_get(node->left, key, compare);
    }
    if (comp < 0) {
        return _tree_get(node->right, key, compare);
    }
    else {
        return node;
    }
}

void* avltree_get(avltree_t* tree, void* key)
{
    /*avltree_node_t* cursor = tree->root;
    for(;;) {
        if (cursor == NULL) {
            return NULL;
        }
        if (tree->compare(cursor->key, key) > 0) {
            cursor = cursor->left;
        }
        else if (tree->compare(cursor->key, key) < 0) {
            cursor = cursor->right;
        }
        else {
            return cursor->value;
        }
    }
    return NULL;*/
    
    avltree_node_t* node = _tree_get(tree->root, key, tree->compare);
    if (node) {
        return node->value;
    }
    return NULL;
}

avltree_node_t* _min_value_node(avltree_node_t* root)
{
    avltree_node_t* node = root;
    while(node->left != NULL) {
        node = node->left;
    }
    return node;
}

avltree_node_t* _tree_delete(avltree_node_t* root, avltree_node_t* node, int (*compare)(void*, void*))
{
    if (root == NULL || node == NULL) {
        return NULL;
    }
    
    int comp = compare(root->key, node->key);
    if (comp > 0) {
        root->left = _tree_delete(root->left, node, compare);
    }
    else if (comp < 0) {
        root->right = _tree_delete(root->right, node, compare);
    }
    else {
        if (root->left == NULL && root->right == NULL) {
            free(root);
            return NULL;
        }
        else if (root->left == NULL || root->right == NULL) {
            if (root->left != NULL) {
                *root = *root->left;
                free(root->left);
            }
            else if (root->right != NULL) {
                *root = *root->right;
                free(root->right);
            }
        }
        else {
            avltree_node_t* temp = _min_value_node(root->right);
            
            root->key = temp->key;
            root->value = temp->value;
            
            root->right = _tree_delete(root->right, node, compare);
        }
    }
    
    if (root == NULL) {
        return NULL;
    }
    
    root->height = _max(_height(root->right), _height(root->left)) + 1;
    int balance = _balance(root);
    
    if (balance > 1 && _balance(root->left) >= 0) { // root > node
        return _rotate_right(root);
    }
    if (balance > 1 && _balance(root->left) < 0) { // root < node
        root->left = _rotate_left(root->left);
        return _rotate_right(root);
    }
    if (balance < -1 && _balance(root->right) <= 0) { // root < node
        return _rotate_left(root);
    }
    if (balance < -1 && _balance(root->right) > 0) { // root < node
        root->right = _rotate_right(root->right);
        return _rotate_left(root);
    }
    
    return root;
}


int avltree_remove(avltree_t* tree, void* key, int (*func)(void*, void*, void*))
{
    avltree_node_t* node = _tree_get(tree->root, key, tree->compare);
    if (node == NULL) {
        return -1;
    }
    void* k = node->key;
    void* v = node->value;
    
    tree->root = _tree_delete(tree->root, node, tree->compare);
    if (func) {
        func(k, v, NULL);
    }
    //free(node);
    
    return 0;
}


int _avltree_node_foreach_inorder(avltree_node_t* node, int (*func)(void*, void*, void*), void* param)
{
    
    if (node != NULL) {
        _avltree_node_foreach_inorder(node->left, func, param);
        func(node->key, node->value, param);
        _avltree_node_foreach_inorder(node->right, func, param);
    }
    return 1;
}

int _avltree_node_foreach_postorder(avltree_node_t* node, int (*func)(void*, void*, void*), void* param)
{
    
    if (node != NULL) {
        _avltree_node_foreach_postorder(node->left, func, param);
        _avltree_node_foreach_postorder(node->right, func, param);
        func(node->key, node->value, param);
    }
    return 1;
}

int avltree_foreach_inorder(avltree_t* tree, int (*func)(void*, void*, void*), void* param)
{
    _avltree_node_foreach_inorder(tree->root, func, param);
    return 1;
}

int avltree_foreach_postorder(avltree_t* tree, int (*func)(void*, void*, void*), void* param)
{
    _avltree_node_foreach_postorder(tree->root, func, param);
    return 1;
}

int avltree_compare_int(void* a, void* b)
{
    int* at = a;
    int* bt = b;
    return *at - *bt;
}

int avltree_compare_str(void* a, void* b)
{
    char* at = a;
    char* bt = b;
    return strcmp(at, bt);
}

int avltree_free_key(void* key, void* value, void* param)
{
    free(key);
    return 0;
}

int avltree_free_keyvalue(void* key, void* value, void* param)
{
    free(key);
    free(value);
    return 0;
}
