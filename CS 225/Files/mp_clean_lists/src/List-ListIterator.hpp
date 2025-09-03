
class ListIterator : public std::iterator<std::bidirectional_iterator_tag, T> {
  public:
    ListIterator() : position_(NULL) { }
    ListIterator(ListNode* x) : position_(x) {
        // if (x == nullptr) {
        //     return;
        // }
        // ListNode* cur_node = x;
        // while(cur_node->prev != nullptr) {
        //     cur_node = cur_node->prev;
        // }
        // root_ = cur_node;
    }


    // Pre-Increment, ++iter
    ListIterator& operator++() {
        // @TODO: graded in MP3.1
        position_ = position_->next;
        return *this;
    }
    
    // Post-Increment, iter++
    ListIterator operator++(int) {
        // @TODO: graded in MP3.1
        ListNode* temp = position_;
        position_ = position_->next;
        return ListIterator(temp);
    }

    // Pre-Decrement, --iter
    ListIterator& operator--() {
        // @TODO: graded in MP3.1
        position_ = position_->prev;
        return *this;
    }

    // Post-Decrement, iter--
    ListIterator operator--(int) {
        // @TODO: graded in MP3.1
        ListNode* temp = position_;
        position_ = position_->prev;
        return ListIterator(temp);
    }

    bool operator!=(const ListIterator& rhs) {
        // @TODO: graded in MP3.1
        bool thisEmpty = position_ == nullptr;
        bool rhsEmpty = rhs.position_ == nullptr;
        
        if (thisEmpty && rhsEmpty) { // If they are equal to each other, return false
            return false;
        }
        if ((!thisEmpty) && (!rhsEmpty)) { // If they are both not empty, compare their root nodes
            return !(position_ == rhs.position_);
        }
        return true; // If one is empty and the other is not, they can't equal each other. Return true
    }

    bool operator==(const ListIterator& rhs) {
        return !(*this != rhs);
    }

    const T& operator*() {
        return position_->data;
    }

    const T* operator->() {
        return &(position_->data);
    }

  private:
    // @TODO: graded in MP3.1
    // ListNode* root_;
    ListNode* position_;
};
