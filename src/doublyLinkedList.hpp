#ifndef DOUBLE_LINKED_LIST_H
#define DOUBLE_LINKED_LIST_H

#include <cstddef>
#include <stdexcept>
#include <functional>
#include <optional>

using namespace std;

/**
 * Collections namespace containing custom container implementations
 */
namespace Collections{
  
  /**
   * Node structure for doubly linked list
   * Contains data and pointers to next and previous nodes
   * @tparam type The data type stored in the node
   */
  template<typename type>
  struct node{
    type data;          // The actual data stored in this node
    node* next;         // Pointer to the next node in the list
    node* prev;         // Pointer to the previous node in the list

    /**
     * Constructor for node using perfect forwarding
     * Efficiently handles both lvalue and rvalue references
     * @tparam T Universal reference type
     * @param data The data to store in the node
     */
    template<typename T>
    node(T&& data) : data(std::forward<T>(data)) , next(nullptr) , prev(nullptr){}
  };

  /**
   * DoublyLinkedList - A templated doubly linked list implementation
   * Provides bidirectional traversal and efficient insertion/deletion at both ends
   * @tparam type The data type to store in the list
   */
  template<typename type>
  class DoublyLinkedList{
    private:
      node<type>* _head{nullptr};     // Pointer to the first node
      node<type>* _tail{nullptr};     // Pointer to the last node
      size_t _length{0};              // Current number of elements in the list

    public:
      /**
       * Bidirectional iterator for DoublyLinkedList
       * Allows forward and backward traversal through the list
       */
      class Iterator {
        private:
            node<type>* current;        // Current node being pointed to
            
        public:
            /**
             * Constructor for iterator
             * @param ptr Pointer to the node to start iteration from
             */
            Iterator(node<type>* ptr = nullptr) : current(ptr) {}

            /**
             * Dereference operator - returns reference to current node's data
             * @return Reference to the data in the current node
             */
            type& operator*() const { return current->data; }
            
            /**
             * Arrow operator - returns pointer to current node's data
             * @return Pointer to the data in the current node
             */
            type* operator->() const { return &(current->data); }

            /**
             * Pre-increment operator - moves iterator to next node
             * @return Reference to this iterator after incrementing
             */
            Iterator& operator++() { current = current->next; return *this; }
            
            /**
             * Post-increment operator - moves iterator to next node
             * @return Copy of iterator before incrementing
             */
            Iterator operator++(int) { Iterator temp = *this; ++(*this); return temp; }

            /**
             * Pre-decrement operator - moves iterator to previous node
             * @return Reference to this iterator after decrementing
             */
            Iterator& operator--() { current = current->prev; return *this; }
            
            /**
             * Post-decrement operator - moves iterator to previous node
             * @return Copy of iterator before decrementing
             */
            Iterator operator--(int) { Iterator temp = *this; --(*this); return temp; }

            /**
             * Equality comparison operator
             * @param other Iterator to compare with
             * @return true if both iterators point to the same node
             */
            bool operator==(const Iterator& other) const { return current == other.current; }
            
            /**
             * Inequality comparison operator
             * @param other Iterator to compare with
             * @return true if iterators point to different nodes
             */
            bool operator!=(const Iterator& other) const { return current != other.current; }
        };

      /**
       * Default constructor - creates an empty list
       */
      DoublyLinkedList() = default;

      /**
       * Constructor from initializer list
       * Creates a list with elements from the initializer list
       * @param list Initializer list containing elements to add
       * Example: DoublyLinkedList<int> myList = {1, 2, 3, 4};
       */
      DoublyLinkedList(std::initializer_list<type> list){
        for(auto&& item : list)
            this->push_back(std::move(item));
      } 

      /**
       * Constructor from iterator range
       * Creates a list by copying elements from [begin, end)
       * @param begin Iterator pointing to first element to copy
       * @param end Iterator pointing past the last element to copy
       */
      DoublyLinkedList(Iterator begin, Iterator end) {
        for (auto it = begin; it != end; ++it) 
            push_back(*it);
      }

      /**
       * Move constructor - efficiently transfers ownership from another list
       * The other list becomes empty after this operation
       * @param other List to move from (will be left in empty state)
       */
      DoublyLinkedList(DoublyLinkedList&& other) noexcept
        : _head(other._head), _tail(other._tail), _length(other._length)
      {
          other._head = nullptr;
          other._tail = nullptr;
          other._length = 0;
      }

      /**
       * Copy constructor - creates a deep copy of another list
       * All elements are copied, not just pointers
       * @param other List to copy from
       */
      DoublyLinkedList(const DoublyLinkedList& other) {
        for(node<type>* cur = other._head ; cur != nullptr ; cur = cur->next)
          this->push_back(cur->data);
      }

      /**
       * Destructor - automatically cleans up all allocated nodes
       * Traverses the list and deletes each node to prevent memory leaks
       */
      ~DoublyLinkedList(){
        node<type>* cur = _head;
        while(cur != nullptr){
          node<type>* deleted_node = cur;
          cur = cur->next;
          delete deleted_node;
        }
        _head = _tail = nullptr;
      }

      /**
       * Adds an element to the end of the list
       * Uses perfect forwarding for optimal performance with both copies and moves
       * Time complexity: O(1)
       * @tparam T Universal reference type
       * @param item Element to add (can be lvalue or rvalue)
       */
      template<typename T>
      void push_back(T&& item){
        node<type>* new_node = new node<type>(std::forward<T>(item));
        if (_head == nullptr){
            _head = _tail = new_node;
        } else {
            _tail->next = new_node;
            new_node->prev = _tail;
            _tail = new_node;
        }
        _length++;
      }

      /**
       * Adds an element to the beginning of the list
       * Uses perfect forwarding for optimal performance
       * Time complexity: O(1)
       * @tparam T Universal reference type
       * @param item Element to add (can be lvalue or rvalue)
       */
      template<typename T>
      void push_front(T&& item){
        node<type>* new_node = new node<type>(std::forward<T>(item));
        if (!_head){
          _head = _tail = new_node;
        } else {
          _head->prev = new_node;
          new_node->next = _head;
          _head = new_node;
        }
        ++_length;
      }

      /**
       * Removes the last element from the list
       * Time complexity: O(1)
       * @throws std::runtime_error if the list is empty
       */
      void pop_back(){
        if(_tail == nullptr)
          throw std::runtime_error("List Is Empty (Nothing To Return)");
        
        node<type>* deleted_node = _tail;

        if(_tail->prev){
          _tail = _tail->prev;
          _tail->next = nullptr;
        } else {
          _head = _tail = nullptr;
        }
        delete deleted_node;
        _length--;
      } 

      /**
       * Removes the first element from the list
       * Time complexity: O(1)
       * @throws std::runtime_error if the list is empty
       */
      void pop_front(){
        if(_head == nullptr)
          throw std::runtime_error("List Is Empty (Nothing To Return)");

        node<type>* deleted_node = _head;

        if(_head->next){
          _head = _head->next;
          _head->prev = nullptr;
        } else {  
          _head = _tail = nullptr;
        }
        delete deleted_node;
        --_length;
      }

      /**
       * Returns a reference to the last element
       * @return Reference to the last element's data
       * @throws std::runtime_error if the list is empty
       */
      type& back(){
        if(_tail == nullptr)
          throw std::runtime_error("List Is Empty (Nothing To Return)");
        return _tail->data;
      }

      /**
       * Returns a reference to the first element
       * @return Reference to the first element's data
       * @throws std::runtime_error if the list is empty
       */
      type& front(){
        if(_head == nullptr)
          throw std::runtime_error("List Is Empty (Nothing To Return)");
        return _head->data;
      }

      /**
       * Inserts an element at the specified position
       * If index is 0, behaves like push_front()
       * If index equals size(), behaves like push_back()
       * Time complexity: O(n) due to traversal to the insertion point
       * @tparam T Universal reference type
       * @param index Position to insert at (0-based)
       * @param item Element to insert
       * @throws std::invalid_argument if index is out of bounds
       */
      template<typename T>
      void insert(size_t index, T&& item){
        if (index > size())
          throw std::invalid_argument("Index Out Of Bounds");

        if (index == 0){
          push_front(std::forward<T>(item));
          return;
        }

        if (index == size()){
          push_back(std::forward<T>(item));
          return;
        }

        // Traverse to the insertion point
        size_t cur_index = 0;
        node<type>* current = _head;
        while (current != nullptr && cur_index < index){
          current = current->next;
          ++cur_index;
        }

        // Insert the new node
        node<type>* new_node = new node<type>(std::forward<T>(item));
        new_node->next = current;
        new_node->prev = current->prev;
        current->prev->next = new_node;
        current->prev = new_node;

        ++_length;
      }

      /**
       * Removes the element at the specified index
       * Time complexity: O(n) due to traversal to the removal point
       * @param index Position of element to remove (0-based)
       * @throws std::invalid_argument if index is out of bounds
       */
      void remove(size_t index){
        if(index >= this->size())
          throw std::invalid_argument("Index Out Of Bounds");
    
        if(index == 0){
          pop_front();
          return;
        }
    
        if(index == size()-1){
          pop_back();
          return;
        }
    
        // Traverse to the node to remove
        size_t cur_index = 0;
        node<type>* current = _head;
        while(current != nullptr && cur_index < index){
          current = current->next;
          ++cur_index;  
        }
    
        // Remove the node by updating pointers
        current->prev->next = current->next;
        current->next->prev = current->prev;
    
        delete current;
        _length--;
      }    

      /**
       * Replaces the element at the specified index with a new value
       * Time complexity: O(n) due to traversal to the target position
       * @tparam T Universal reference type
       * @param index Position of element to replace (0-based)
       * @param new_value New value to assign
       * @throws std::invalid_argument if index is out of bounds
       */
      template<typename T>
      void replace(size_t index , T&& new_value){
        if (index >= size())  
            throw std::invalid_argument("Index Out Of Bounds");
    
        node<type>* current = _head;
        size_t current_index = 0;
    
        // Traverse to the target node
        while (current != nullptr && current_index != index) {
            ++current_index;
            current = current->next;
        }
    
        if (current != nullptr) {
            current->data = std::forward<T>(new_value);
        } else {
            throw std::invalid_argument("Index Out Of Bounds");
        }
      }

      /**
       * Removes the first occurrence of a value from the list
       * Uses a custom comparison function to determine equality
       * Time complexity: O(n) - may need to traverse entire list
       * @tparam Comparer Function type for comparing elements
       * @param value Value to search for and remove
       * @param compare Comparison function (defaults to equality operator)
       */
      template<typename Comparer = function<bool(type , type)>>
      void remove_value(const type& value ,
         Comparer compare = [](const type& t1 , const type& t2) -> bool {return t1 == t2;})
      {
        node<type>* cur = _head;
        while(cur != nullptr){
          if(compare(cur->data , value)){
              if(cur == _head){
                pop_front();
              } else if(cur == _tail){
                pop_back();
              } else {
                // Remove middle node
                cur->prev->next = cur->next;
                cur->next->prev = cur->prev;
                delete cur;
                --_length;
              }
              return; // Only remove first occurrence
          }
          cur = cur->next;
        }
      }

      /**
       * Returns the number of elements in the list
       * Time complexity: O(1)
       * @return Current size of the list
       */
      size_t size() const{
        return this->_length;
      }

      /**
       * Checks if the list is empty
       * Time complexity: O(1)
       * @return true if list contains no elements, false otherwise
       */
      bool empty() const {
        return this->_length == 0;
      }

      /**
       * Removes all elements from the list
       * Time complexity: O(n)
       */
      void clear(){
        while(!empty())
          pop_back();
      }

      /**
       * Copy assignment operator - performs deep copy
       * Includes self-assignment protection
       * @param other List to copy from
       * @return Reference to this list
       */
      DoublyLinkedList& operator=(const DoublyLinkedList& other){
        if(this == &other) return *this; // Self-assignment protection

        // Clear current contents
        while(!this->empty())
          pop_back();

        // Copy all elements from other list
        node<type>* current = other._head;
        while(current != nullptr){
          push_back(current->data);
          current = current->next;
        }

        return *this;
      }

      /**
       * Move assignment operator - efficiently transfers ownership
       * The other list becomes empty after this operation
       * @param other List to move from
       * @return Reference to this list
       */
      DoublyLinkedList& operator=(DoublyLinkedList&& other) noexcept {
        if(this == &other) return *this; // Self-assignment protection

        // Clear current contents
        while(!this->empty())
          pop_back();

        // Transfer ownership
        this->_head = other._head;
        this->_tail = other._tail;
        this->_length = other._length;

        // Leave other in valid empty state
        other._head = nullptr;
        other._tail = nullptr;
        other._length = 0;
        return *this;
      }

      /**
       * Provides random access to elements by index
       * Time complexity: O(n) - requires traversal from head
       * @param index Position of element to access (0-based)
       * @return Reference to the element at the specified index
       * @throws std::out_of_range if index is invalid
       */
      type& at(size_t index){
        if(index >= size()) 
          throw std::out_of_range("Index Out Of Bounds");
        int cur_index = 0;
        for(node<type>* cur = this->_head ; cur != nullptr ; cur = cur->next , ++cur_index){
          if(index == cur_index)
            return cur->data;
        }
      }

      /**
       * Finds the index of the first occurrence of a value
       * Uses custom comparison function for flexibility
       * Time complexity: O(n)
       * @tparam Comparer Function type for comparing elements
       * @param value Value to search for
       * @param compare Comparison function (defaults to equality operator)
       * @return Optional containing the index if found, nullopt otherwise
       */
      template<typename Comparer = function<bool(const type&, const type&)>>
      optional<size_t> index_of(
          const type& value,
          Comparer compare = [](const type& item1, const type& item2) { return item1 == item2; }
      ) const {
          int cur_index = 0;
          for (node<type>* cur = this->_head; cur != nullptr; cur = cur->next, ++cur_index) {
              if (compare(value, cur->data))
                  return cur_index;
          }
          return std::nullopt; // Value not found
      }

      /**
       * Checks if the list contains a specific value
       * Uses custom comparison function for flexibility
       * Time complexity: O(n)
       * @tparam Comparer Function type for comparing elements
       * @param value Value to search for
       * @param compare Comparison function (defaults to equality operator)
       * @return true if value is found, false otherwise
       */
      template<typename Comparer = std::function<bool(const type&, const type&)>>
      bool contains(const type& value, Comparer compare = [](const type& item1, const type& item2) { return item1 == item2; }) const {
          for (node<type>* cur = this->_head; cur != nullptr; cur = cur->next) {
              if (compare(value, cur->data)) {
                  return true;
              }
          }
          return false;
      }

      /**
       * Equality comparison operator
       * Two lists are equal if they have the same size and all corresponding elements are equal
       * Time complexity: O(n)
       * @param other List to compare with
       * @return true if lists are equal, false otherwise
       */
      bool operator==(const DoublyLinkedList& other) const {
        if(this->size() != other.size()) return false;
        else{
          for(
            node<type>* this_cur = this->_head , *other_cur = other._head ; 
            this_cur != nullptr ; 
            this_cur = this_cur->next , other_cur = other_cur->next
          ){
            if(this_cur->data != other_cur->data) return false;
          }
          return true;
        }
      }

      /**
       * Inequality comparison operator
       * Time complexity: O(n) in worst case
       * @param other List to compare with
       * @return true if lists are not equal, false otherwise
       */
      bool operator!=(const DoublyLinkedList& other) const {
        if(this->size() != other.size()) return true;
        else{
          for(
            node<type>* this_cur = this->_head , *other_cur = other._head ; 
            this_cur != nullptr ; 
            this_cur = this_cur->next , other_cur = other_cur->next
          ){
            if(this_cur->data != other_cur->data) return true;
          }
          return false;
        }
      }

      // Iterator access methods for range-based for loops and STL compatibility
      
      /**
       * Returns iterator to the first element
       * @return Iterator pointing to the first element
       */
      Iterator begin() { return Iterator(_head); }
      
      /**
       * Returns iterator past the last element
       * @return Iterator pointing past the last element
       */
      Iterator end() { return Iterator(nullptr); }

      /**
       * Returns const iterator to the first element
       * @return Const iterator pointing to the first element
       */
      const Iterator begin() const { return Iterator(_head); }
      
      /**
       * Returns const iterator past the last element
       * @return Const iterator pointing past the last element
       */
      const Iterator end() const { return Iterator(nullptr); }

      /**
       * Returns explicit const iterator to the first element
       * @return Const iterator pointing to the first element
       */
      const Iterator cbegin() const { return Iterator(_head); }
      
      /**
       * Returns explicit const iterator past the last element
       * @return Const iterator pointing past the last element
       */
      const Iterator cend() const { return Iterator(nullptr); }

  };
}
#endif