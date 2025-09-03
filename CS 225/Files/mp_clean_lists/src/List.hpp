/**
 * @file list.cpp
 * Doubly Linked List (MP 3).
 */

template <class T>
List<T>::List() { 
  // @TODO: graded in MP3.1
    head_ = NULL;
    tail_ = NULL;
    length_ = 0;
}

/**
 * Returns a ListIterator with a position at the beginning of
 * the List.
 */
template <typename T>
typename List<T>::ListIterator List<T>::begin() const {
  // @TODO: graded in MP3.1
  return List<T>::ListIterator(head_);
}

/**
 * Returns a ListIterator one past the end of the List.
 */
template <typename T>
typename List<T>::ListIterator List<T>::end() const {
  // @TODO: graded in MP3.1
  return List<T>::ListIterator();
}


/**
 * Destroys all dynamically allocated memory associated with the current
 * List class.
 */
template <typename T>
void List<T>::_destroy() {
  /// @todo Graded in MP3.1
  if (this->empty()) {
    return;
  }
  // Set the current node equal to head
  ListNode* cur_node = head_;
  while (cur_node != nullptr) { // While the current node isn't a nullptr, delete the current node and move to the next one
    ListNode* prev_node = cur_node;
    cur_node = cur_node->next;
    delete prev_node;
  }
  head_ = nullptr;
  tail_ = nullptr;
  length_ = 0;
}

/**
 * Inserts a new node at the front of the List.
 * This function **SHOULD** create a new ListNode.
 *
 * @param ndata The data to be inserted.
 */
template <typename T>
void List<T>::insertFront(T const & ndata) {
  /// @todo Graded in MP3.1
  ListNode * newNode = new ListNode(ndata);
  newNode->next = head_;
  newNode->prev = nullptr;
  
  if (head_ != nullptr) {
    head_ -> prev = newNode;
  }
  if (tail_ == nullptr) {
    tail_ = newNode;
  }
  head_ = newNode;

  length_++;
}

/**
 * Inserts a new node at the back of the List.
 * This function **SHOULD** create a new ListNode.
 *
 * @param ndata The data to be inserted.
 */
template <typename T>
void List<T>::insertBack(const T & ndata) {
  /// @todo Graded in MP3.1
  ListNode * newNode = new ListNode(ndata);
  newNode->next = nullptr;
  newNode->prev = tail_;
  
  if (head_ == nullptr) {
    head_ = newNode;
  }
  if (tail_ != nullptr) {
    tail_->next = newNode;
  }
  tail_ = newNode;

  length_++;
}

/**
 * Helper function to split a sequence of linked memory at the node
 * splitPoint steps **after** start. In other words, it should disconnect
 * the sequence of linked memory after the given number of nodes, and
 * return a pointer to the starting node of the new sequence of linked
 * memory.
 *
 * This function **SHOULD NOT** create **ANY** new List or ListNode objects!
 *
 * This function is also called by the public split() function located in
 * List-given.hpp
 *
 * @param start The node to start from.
 * @param splitPoint The number of steps to walk before splitting.
 * @return The starting node of the sequence that was split off.
 */
template <typename T>
typename List<T>::ListNode * List<T>::split(ListNode * start, int splitPoint) {
  /// @todo Graded in MP3.1
  if (start == nullptr) {
    std::cout << "Split: Invalid Starting Node" << std::endl;
    return nullptr;
  }
  if (splitPoint == 0) {
    if (start->prev != nullptr) {
      start->prev->next = nullptr;
      start->prev = nullptr;
      return start;
    }
  }
  ListNode * curr = start;
  
  for (int i = 0; i < splitPoint && curr != nullptr; i++) {
    curr = curr->next;
  }

  if (curr->next != nullptr) { // This splits the list at the point BEFORE curr
    curr->prev->next = nullptr;
    curr->prev = nullptr;
    return curr;
  }
  return nullptr;
}

/**
  * Modifies List using the rules for a TripleRotate.
  *
  * This function will to a wrapped rotation to the left on every three 
  * elements in the list starting for the first three elements. If the 
  * end of the list has a set of 1 or 2 elements, no rotation all be done 
  * on the last 1 or 2 elements.
  * 
  * You may NOT allocate ANY new ListNodes!
  */
template <typename T>
void List<T>::tripleRotate() {
  // @todo Graded in MP3.1
  ListNode* cur_node = head_;
  if (length_ < 3) { // If the list is shorter than 3, nothing needs to be done
    return;
  }
  head_ = head_->next; // Since nodes 123 are reordered 231, the new head_ should point to node 2
  if (length_ % 3 == 0) { // If the list is a multiple of 3, then the last 3 nodes will be reordered into 231
    tail_ = tail_->prev->prev;
  }
  for (int i = 0; i < length_; i += 3) {
    // elem_zero = cur_node.prev              // points to elem_neg_one, elem_one
    ListNode* elem_one = cur_node;            // Points to elem_zero, elem_two
    ListNode* elem_two = elem_one->next;      // Points to elem_one, elem_three
    ListNode* elem_three = elem_two->next;    // Points to elem_two, elem_four

    /** Rotated Order:  elem_two, elem_three, elem_one
     * elem_zero:       points to elem_neg_one, elem_two
     * elem_two:        points to elem_zero, elem_three
     * elem_three:      points to elem_two, elem_one
     * elem_one:        points to elem_three, elem_four
     * elem_four:       points to elem_one, elem_five
     */
    if (elem_one->prev != nullptr) { // If elem_zero exists
      elem_one->prev->next = elem_two;
    }
    elem_two->prev = elem_one->prev;    // elem_two: elem_zero, elem_three
    elem_one->prev = elem_three;        // elem_one: elem_three, elem_two
    elem_one->next = elem_three->next;  // elem_one: elem_three, elem_four
    elem_three->next = elem_one;        // elem_three: elem_two, elem_one
    if (elem_one->next != nullptr) { // If elem_four exists
      elem_one->next->prev = elem_one;
    }

    cur_node = cur_node->next; // cur_node initially pointed at node 1 in 123. Now the list is 231, and node 1 now points to node 4
  }
}


/**
 * Reverses the current List.
 */
template <typename T>
void List<T>::reverse() {
  reverse(head_, tail_);
}

/**
 * Helper function to reverse a sequence of linked memory inside a List,
 * starting at startPoint and ending at endPoint. You are responsible for
 * updating startPoint and endPoint to point to the new starting and ending
 * points of the rearranged sequence of linked memory in question.
 *
 * @param startPoint A pointer reference to the first node in the sequence
 *  to be reversed.
 * @param endPoint A pointer reference to the last node in the sequence to
 *  be reversed.
 */
template <typename T>
void List<T>::reverse(ListNode *& startPoint, ListNode *& endPoint) {
  /// @todo Graded in MP3.2
  if (startPoint == endPoint || startPoint == nullptr || endPoint == nullptr) {
    return;
  }
  /** Before Reverse:
   * node_0: node_-1, node_1
   * node_1: node_0, node_2
   * node_2: node_1, node_3
   * node_3: node_2, node_4
   * node_4: node_3, node_5
   *
   ** After Reverse:
   * node_1: node_2, node_0
   * node_2: node_3, node_1
   * node_3: node_4, node_2
   *
   * Start at node_1 (node_0, node_2)
   * Swap its prev and next values (node_2, node_0)
   * Move to its new prev value
   * Now you're on node_2 (node_1, node_3)
   * Swap its prev and next values (node_3, node_1)
   * Move to its new prev value
   * Now you're on node_3
   */
  ListNode* prev_node = startPoint->prev;
  ListNode* cur_node = startPoint;
  while(cur_node != endPoint) {
    reverseDirection(cur_node);
    cur_node = cur_node->prev;
  }
  ListNode* next_node = cur_node->next;
  reverseDirection(cur_node);
  // Once everything's reversed, swap the startPoint and endPoint pointers
  ListNode* temp = startPoint;
  startPoint = endPoint;
  endPoint = temp;
  // And stitch the segment back into the list
  startPoint->prev = prev_node;
  if (prev_node != nullptr) {
    prev_node->next = startPoint;
  }
  endPoint->next = next_node;
  if (next_node != nullptr) {
    next_node->prev = endPoint;
  }
}

template <typename T>
void List<T>::reverseDirection(ListNode * node) {
  ListNode* temp = node->prev;
  node->prev = node->next;
  node->next = temp;
}

/**
 * Reverses blocks of size n in the current List. You should use your
 * reverse( ListNode * &, ListNode * & ) helper function in this method!
 *
 * @param n The size of the blocks in the List to be reversed.
 */
template <typename T>
void List<T>::reverseNth(int n) {
  /// @todo Graded in MP3.2
  if (empty() || n <= 1) {
    return;
  }
  if (n > length_) {
    reverse();
    return;
  }
  /**
   * reverseNth(3) of 1 2 3 4 5 6 7 8
   * 3 2 1 6 5 4 8 7
   */
  ListNode* start_node = head_;
  ListNode* end_node = head_;
  for (int i = n; i < length_; i += n) {
    for (int j = 1; j < n; j++) {
      end_node = end_node->next;
    }
    reverse(start_node, end_node); // Since we reversed the nodes, the start_node has swapped with the end_node
    if (i == n) {
      head_ = start_node;
    }
    start_node = end_node->next;
    end_node = start_node;
  }
  end_node = tail_;
  reverse(start_node, end_node); // One final reverse to flip the remaining numbers
  tail_ = end_node;
}

/**
 * Merges the given sorted list into the current sorted list.
 *
 * @param otherList List to be merged into the current list.
 */
template <typename T>
void List<T>::mergeWith(List<T> & otherList) {
    // set up the current list
    if (head_->data < otherList.head_->data) {
      head_ = merge(head_, otherList.head_);
    } else {
      head_ = merge(otherList.head_, head_);
    }
    tail_ = head_;

    // make sure there is a node in the new list
    if (tail_ != NULL) {
        while (tail_->next != NULL)
            tail_ = tail_->next;
    }
    length_ = length_ + otherList.length_;

    // empty out the parameter list
    otherList.head_ = NULL;
    otherList.tail_ = NULL;
    otherList.length_ = 0;
}

template <typename T>
void List<T>::print(std::string message, ListNode * head) const {
  std::cout << message;
  ListNode* cur_node = head;
  while (cur_node != nullptr) {
    std::cout << cur_node->data << " ";
    cur_node = cur_node->next;
  }
  std::cout << std::endl;
}

/**
 * Helper function to merge two **sorted** and **independent** sequences of
 * linked memory. The result should be a single sequence that is itself
 * sorted.
 *
 * This function **SHOULD NOT** create **ANY** new List objects.
 *
 * @param first The starting node of the first sequence.
 * @param second The starting node of the second sequence.
 * @return The starting node of the resulting, sorted sequence.
 */
template <typename T>
typename List<T>::ListNode * List<T>::merge(ListNode * first, ListNode* second) {
  /// @todo Graded in MP3.2
  if (first == nullptr && second == nullptr) { // Edge cases
    return nullptr;
  } else if (first == nullptr) {
    return second;
  } else if (second == nullptr) {
    return first;
  }
  // Actual code
  ListNode* line = first;
  ListNode* ant = second;
  if (second->data < first->data) {
    line = second;
    ant = first;
  }
  while(ant != nullptr) {
    // 1 3
    // 2
    if (line->next == nullptr || ant->data < line->next->data || ant->data == line->next->data) {
      ant = insertNode(line, ant);
      // print("Inserted: ", line);
    }
    // 1 2
    // 3
    line = line->next;
  }
  ListNode* head = line;
  while(head->prev != nullptr) {
    head = head->prev;
  }
  return head;
}

template <typename T>
bool List<T>::complete(ListNode * head) const {
  if (head == nullptr) {
    std::cout << "Head is nullptr" << std::endl;
    return false;
  }
  if (head->prev != nullptr) {
    std::cout << "Head.prev is not nullptr" << std::endl;
    return false;
  }
  ListNode* cur_node = head;
  while (cur_node->next != nullptr) {
    cur_node = cur_node->next;
  }
  while(cur_node->prev != nullptr) {
    cur_node = cur_node->prev;
  }
  if (cur_node == head) {
    std::cout << "Head is complete" << std::endl;
    return true;
  } else {
    std::cout << "Head is incomplete" << std::endl;
    return false;
  }
}

template <typename T>
typename List<T>::ListNode* List<T>::insertNode(ListNode * insertPoint, ListNode * node) {
  if (insertPoint == nullptr || node == nullptr) {
    std::cout << "Invalid Insertion" << std::endl;
    return node;
  }

  /**
   * insertPoint = B
   * node = 2
   * A B C
   * 1 2 3
   * 
   * A B 2 C
   * 1 3
   * 
   * Separate
   * Insert
   * Repair
   */
  ListNode* temp = node->next;
  if (node->next != nullptr) {
    node->next->prev = nullptr;
  }
  node->prev = insertPoint; // Insert
  node->next = insertPoint->next;
  insertPoint->next = node; // Mend
  if (node->next != nullptr) {
    node->next->prev = node;
  }
  return temp;
}


/**
 * Sorts a chain of linked memory given a start node and a size.
 * This is the recursive helper for the Mergesort algorithm (i.e., this is
 * the divide-and-conquer step).
 *
 * Called by the public sort function in List-given.hpp
 *
 * @param start Starting point of the chain.
 * @param chainLength Size of the chain to be sorted.
 * @return A pointer to the beginning of the now sorted chain.
 */
template <typename T>
typename List<T>::ListNode* List<T>::mergesort(ListNode * start, int chainLength) {
  /// @todo Graded in MP3.2
  if (chainLength == 1) {
    return start;
  }
  if (chainLength == 0) {
    return nullptr;
  }
  int first_length = chainLength / 2;
  int second_length = chainLength - first_length;
  ListNode* first_half = start;
  ListNode* second_half = split(start, first_length); // Split detatches the second half from the first half
  first_half = mergesort(first_half, first_length);
  second_half = mergesort(second_half, second_length);
  ListNode* merged = merge(first_half, second_half);
  return merged;
}
