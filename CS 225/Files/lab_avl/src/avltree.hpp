/**
 * @file avltree.cpp
 * Definitions of the binary tree functions you'll be writing for this lab.
 * You'll need to modify this file.
 */

template <class K, class V>
V AVLTree<K, V>::find(const K& key) const
{
    return find(root, key);
}

template <class K, class V>
V AVLTree<K, V>::find(Node* subtree, const K& key) const
{
    if (subtree == NULL)
        return V();
    else if (key == subtree->key)
        return subtree->value;
    else {
        if (key < subtree->key)
            return find(subtree->left, key);
        else
            return find(subtree->right, key);
    }
}

template <class K, class V>
void AVLTree<K, V>::recalculateHeight(Node* t) {
    // This assumes that u and v are not nullptr
    //     t - - - - v - d
    //     |         |
    //     u - b     c
    //     |
    //     a
    int u_height = -1;
    if (t->left != nullptr) {
        Node* u = t->left;
        int a_height = heightOrNeg1(u->left);
        int b_height = heightOrNeg1(u->right);

        u->height = std::max(a_height, b_height) + 1;
        u_height = u->height;
    }

    int v_height = -1;
    if (t->right != nullptr) {
        Node* v = t->right;
        int c_height = heightOrNeg1(v->left);
        int d_height = heightOrNeg1(v->right);

        v->height = std::max(c_height, d_height) + 1;
        v_height = v->height;
    }
    t->height = std::max(u_height, v_height) + 1;
}

template <class K, class V>
void AVLTree<K, V>::rotateLeft(Node*& t)
{
    functionCalls.push_back("rotateLeft"); // Stores the rotation name (don't remove this)
    // your code here
    //     t - u - v - d rotateLeft(t)
    //     |   |   |
    //     a   b   c
    //     u - - - - v - d
    //     |         |
    //     t - b     c
    //     |
    //     a
    Node* u = t->right;
    t->right = u->left;
    u->left = t;
    t = u; // t is now the center node
    recalculateHeight(t);
}

template <class K, class V>
void AVLTree<K, V>::rotateLeftRight(Node*& t)
{
    functionCalls.push_back("rotateLeftRight"); // Stores the rotation name (don't remove this)
    // Implemented for you:
    rotateLeft(t->left);
    rotateRight(t);
}

template <class K, class V>
void AVLTree<K, V>::rotateRight(Node*& t)
{
    functionCalls.push_back("rotateRight"); // Stores the rotation name (don't remove this)
    // your code here
    // d - v - u - t     rotateLeft(t)
    //     |   |   |
    //     c   b   a
    // d - v - - - - u
    //     |         |
    //     c     b - t
    //               |
    //               a
    Node* u = t->left;
    t->left = u->right;
    u->right = t;
    t = u; // t is now the center node
    recalculateHeight(t);
}

template <class K, class V>
void AVLTree<K, V>::rotateRightLeft(Node*& t)
{
    functionCalls.push_back("rotateRightLeft"); // Stores the rotation name (don't remove this)
    // your code here
    rotateRight(t->right);
    rotateLeft(t);
}

template <class K, class V>
void AVLTree<K, V>::rebalance(Node*& subtree)
{
    // your code here
    if (subtree == nullptr) {
        return;
    }
    int left_height = heightOrNeg1(subtree->left);
    int right_height = heightOrNeg1(subtree->right);
    int difference = left_height - right_height;
    if (difference == 2) { // The left side is higher by 2 nodes
        int left_left_height = heightOrNeg1(subtree->left->left);
        int left_right_height = heightOrNeg1(subtree->left->right);
        int left_difference = left_left_height - left_right_height;

        if (left_difference == 1) {
            rotateRight(subtree);
        } else if (left_difference == -1) {
            rotateLeftRight(subtree);
        }
    } else if (difference == -2) { // The right side is higher by 2 nodes
        int right_left_height = heightOrNeg1(subtree->right->left);
        int right_right_height = heightOrNeg1(subtree->right->right);
        int right_difference = right_left_height - right_right_height;

        if (right_difference == -1) {
            rotateLeft(subtree);
        } else if (right_difference == 1) {
            rotateRightLeft(subtree);
        }
    } else {
        recalculateHeight(subtree);
    }
}

template <class K, class V>
void AVLTree<K, V>::insert(const K & key, const V & value)
{
    insert(root, key, value);
}

template <class K, class V>
void AVLTree<K, V>::insert(Node*& subtree, const K& key, const V& value)
{
    // your code here
    if (subtree == nullptr) {
        subtree = new Node(key, value);
        return;
    }
    if (key < subtree->key) { // Go left
        insert(subtree->left, key, value);
    } else if (subtree->key < key) { // Go right
        insert(subtree->right, key, value);
    }
    rebalance(subtree);
}

template <class K, class V>
void AVLTree<K, V>::remove(const K& key)
{
    remove(root, key);
}

template <class K, class V>
void AVLTree<K, V>::remove(Node*& subtree, const K& key)
{
    if (subtree == NULL)
        return;

    if (key < subtree->key) {
        // your code here
        remove(subtree->left, key);
    } else if (key > subtree->key) {
        // your code here
        remove(subtree->right, key);
    } else {
        if (subtree->left == NULL && subtree->right == NULL) {
            /* no-child remove */
            // your code here
            delete subtree;
            subtree = nullptr;
        } else if (subtree->left != NULL && subtree->right != NULL) {
            /* two-child remove */
            // your code here
            Node* prev_node = nullptr;
            Node* in_order_predecessor = subtree->left;
            while (in_order_predecessor->right != nullptr) {
                prev_node = in_order_predecessor;
                in_order_predecessor = in_order_predecessor->right;
            }
            swap(subtree, in_order_predecessor);
            if ((prev_node != nullptr) && (in_order_predecessor->left != nullptr)) {
                prev_node->right = in_order_predecessor->left;
            } else {
                prev_node->right = nullptr;
            }
            delete in_order_predecessor;
        } else {
            /* one-child remove */
            // your code here
            if (subtree->left != nullptr) {
                swap(subtree, subtree->left);
                Node* temp = subtree->left;
                subtree->left = subtree->left->left;
                delete temp;
            } else {
                swap(subtree, subtree->right);
                Node* temp = subtree->right;
                subtree->right = subtree->right->right;
                delete temp;
            }
        }
        // your code here
    }
    rebalance(subtree);
}
