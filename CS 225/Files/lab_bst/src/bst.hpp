/**
 * @file bst.cpp
 * Definitions of the binary tree functions you'll be writing for this lab.
 * You'll need to modify this file.
 */

/**
 * Stupid function that doesn't say what to do when you can't find the key
 * If you can't find the key, the empty template will be returned
*/
template <class K, class V>
V BST<K, V>::find(const K& key)
{
    // your code here
    if (root == nullptr) {
        return V();
    }
    if (root->key == key) {
        return root->value;
    }
    Node* node = find(root, key);
    if (node->left != nullptr && node->left->key == key) {
        return node->left->value;
    } else if (node->right != nullptr && node->right->key == key) {
        return node->right->value;
    } else {
        return V();
    }
}

/**
 * Returns the parent node to the given key. If it reaches a leaf node,
 * it returns that instead. Does not check the root node, nor if the
 * root node is a nullptr
 */
template <class K, class V>
struct BST<K, V>::Node* & BST<K, V>::find(Node* & subtree, const K& key)
{
    // Your code here
    if (subtree->left == nullptr && subtree->right == nullptr) {
        return subtree;                     // Leaf node
    } else if ((subtree->left != nullptr && subtree->left->key == key) || (subtree->right != nullptr && subtree->right->key == key)) {   
        return subtree;                     // Parent node found
    } else if (subtree->key > key) {    
        return find(subtree->left, key);    // e.g subtree.key = 13, key = 10
    } else if (subtree->key < key) {
        return find(subtree->right, key);   // e.g. subtree.key = 4, key = 10
    } else {
        // std::cout << "Find Helper Error: subtree->key == key" << std::endl;
        return subtree;                     // root.key == key
    }

}

template <class K, class V>
void BST<K, V>::insert(const K & key, const V & value)
{
    // your code here
    insert(root, key, value);
    // if (root == nullptr || find(key) != V()) {
    //     return;
    // }
    // Node* parent_node = find(root, key);
    // if (key < parent_node->key) {
    //     parent_node->left = new Node(key, value);
    // } else {
    //     parent_node->right = new Node(key, value);
    // }
}

template <class K, class V>
void BST<K, V>::insert(Node*& subtree, const K& key, const V& value)
{
    // your code here
    if (subtree == nullptr || find(subtree, key) != V()) {
        return;
    }
    Node* parent_node = find(subtree, key);
    if (key < parent_node->key) {
        parent_node->left = new Node(key, value);
    } else {
        parent_node->right = new Node(key, value);
    }
    // if (subtree == nullptr) { // The only way for subtree to equal nullptr is if the tree was empty, giving root = nullptr
    //     root = new Node(key, value);
    // } else if (subtree->key == key) {   // e.g. subtree.key = 10, key = 10
    //     return;
    // } else if (subtree->key < key) {    // e.g. subtree.key = 4, key = 10
    //     if (subtree->left == nullptr) {
    //         subtree->left = new Node(key, value);
    //     } else {
    //         insert(subtree->left, key, value);
    //     }
    // } else {                            // e.g. subtree.key = 13, key = 10
    //     if (subtree->right == nullptr) {
    //         subtree->right = new Node(key, value);
    //     } else {
    //         return insert(subtree->right, key, value);
    //     }
    // }
}

template <class K, class V>
void BST<K, V>::swap(Node*& first, Node*& second)
{
    // your code here
}

template <class K, class V>
void BST<K, V>::remove(const K& key)
{
    // your code here
    remove(root, key);
}

template <class K, class V>
void BST<K, V>::remove(Node*& subtree, const K& key)
{
    // your code here
    if (subtree == nullptr || find(subtree, key) == V()) {
        return;
    }
    Node* parent_node = find(subtree, key);
    if (key < parent_node->key) {
        parent_node->left = new Node(key, value);
    } else {
        parent_node->right = new Node(key, value);
    }
}

template <class K, class V>
BST<K, V> listBuild(std::vector<std::pair<K, V>> inList){
    // your code here
}

template <class K, class V>
std::vector<int> allBuild(std::vector<std::pair<K, V>> inList){
    // your code here
}