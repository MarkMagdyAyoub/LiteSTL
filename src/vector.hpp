#ifndef VECTOR_H
#define VECTOR_H

#include <cstddef>
#include <functional>
#include <algorithm>
#include <cstring>
#include <stdexcept>
#include <initializer_list>
#include <cassert>

// TODO: Vector(Iterator begin , Iterator end)

namespace Collections {

/**
 * @brief A dynamically resizable array class, similar to std::vector.
 * 
 * This container provides functionalities such as dynamic resizing, 
 * element access, memory management, and default value filling.
 * 
 * @tparam type The type of elements stored in the container.
 */
template<typename type>
class Vector {
private:
    // Current number of elements stored in the array
    size_t _size{};
    // Allocated memory capacity 
    size_t _capacity{};
    // Pointer to the actual array in dynamic memory
    type* _data_array{nullptr};

    /**
     * @brief Reallocates memory for the internal array to a new capacity.
     * 
     * Ensures that the new capacity is at least equal to the current size.
     * Copies existing elements to the newly allocated memory.
     * 
     * @param new_capacity The new capacity to allocate.
     */
    void reallocate(size_t new_capacity) {
        if (new_capacity < _size) {
            new_capacity = _size;
        }
        if (new_capacity == 0) {
            _capacity = 0;
            _size = 0;
            delete[] _data_array;
            _data_array = nullptr;
            return;
        }
        type* new_data_array = new type[new_capacity];
        try {
            std::copy(_data_array, _data_array + this->_size, new_data_array);
        } catch (...) {
            delete[] new_data_array;
            throw;
        }
        delete[] _data_array;
        _data_array = new_data_array;
        _capacity = new_capacity;
    }

    /**
     * @brief Doubles the capacity of the internal array or sets it to 10 if empty.
     */
    void extend() {
        size_t new_capacity = (_capacity == 0) ? 10 : _capacity * 2;
        reallocate(new_capacity);
    }

public:
    /**
     * @brief Iterator class for forward traversal of the Vector.
     */
    static class Iterator {
    private:
        type* current;

    public:
        Iterator(type* ptr) : current(ptr) {}
        
        type& operator*() { return *current; }
        
        Iterator& operator++() { ++current; return *this; }
        
        Iterator& operator--() { --current; return *this; }
        
        bool operator==(const Iterator& other) const { return current == other.current; }
        
        bool operator!=(const Iterator& other) const { return current != other.current; }
        
        Iterator operator+(int offset) const { return Iterator(current + offset); }
        
        Iterator operator-(int offset) const { return Iterator(current - offset); }
        
        int operator+(const Iterator& other) const { return this->current + other.current; }
        
        int operator-(const Iterator& other) const { return this->current - other.current; }
    };

    /**
     * @brief Reverse Iterator class for backward traversal of the Vector.
     */
    class ReversedIterator {
    private:
        type* current;

    public:
        ReversedIterator(type* ptr) : current(ptr) {}
        
        type& operator*() { return *current; }
        
        ReversedIterator& operator++() { --current; return *this; }
        
        ReversedIterator& operator--() { ++current; return *this; }
        
        bool operator==(const ReversedIterator& other) const { return current == other.current; }
        
        bool operator!=(const ReversedIterator& other) const { return current != other.current; }
        
        ReversedIterator operator+(int offset) const { return ReversedIterator(current - offset); }
        
        ReversedIterator operator-(int offset) const { return ReversedIterator(current + offset); }
    };

    /**
      * @brief A constant iterator class that provides read-only access
      *        to elements in a sequence.
      */
    class ConstIterator {
    private:
        const type* current;

    public:
        ConstIterator(const type* ptr) : current(ptr) {}
        
        const type& operator*() const { return *current; }
        
        ConstIterator& operator++() { ++current; return *this; }
        
        ConstIterator& operator--() { --current; return *this; }
        
        bool operator==(const ConstIterator& other) const { return current == other.current; }
        
        bool operator!=(const ConstIterator& other) const { return current != other.current; }
        
        ConstIterator operator+(int offset) const { return ConstIterator(current + offset); }
        
        ConstIterator operator-(int offset) const { return ConstIterator(current - offset); }
        
        int operator+(const ConstIterator& other) const { return this->current + other.current; }
        
        int operator-(const ConstIterator& other) const { return this->current - other.current; }
    };

    /**
     * @brief Constructs an empty Vector with default capacity.
     */
    Vector()
        : _size(0), _capacity(10), _data_array(new type[_capacity]) {}

    /**
     * @brief Constructs a Vector with a specified size and default value.
     * 
     * @param size The initial size of the Vector.
     * @param default_value The value to initialize each element with.
     */
    Vector(size_t size, type default_value = type{})
        : _size(size), _capacity(_size > 10 ? _size : 10), _data_array(new type[_capacity]) {
        std::fill(_data_array, _data_array + _size, default_value);
    }

    /**
     * @brief Constructs a Vector from an initializer list.
     * 
     * @param _list The initializer list containing elements.
     */
    Vector(std::initializer_list<type> _list)
        : _size(_list.size()), _capacity(_list.size()), _data_array(new type[_capacity]) {
        size_t index = 0;
        for (const auto& item : _list) {
            _data_array[index++] = item;
        }
    }

    /**
     * @brief Copy constructor for deep copying a Vector.
     * 
     * @param other The Vector to copy from.
     */
    Vector(const Vector<type>& other) {
        this->_size = other._size;
        
        this->_capacity = other._capacity;
        
        this->_data_array = new type[other._capacity];
        
        std::copy(other._data_array, other._data_array + other._size, this->_data_array);
    }

    /**
     * @brief Move constructor for transferring ownership of resources.
     * 
     * @param other The Vector to move from.
     */
    Vector(Vector<type>&& other)
        : _size(other._size), _capacity(other._capacity), _data_array(other._data_array) {
        
        other._size = 0;
      
        other._capacity = 0;
      
        other._data_array = nullptr;
    }

    /**
     * @brief Destructor to release dynamically allocated memory.
     */
    ~Vector() {
        delete[] _data_array;
    }

    /**
     * @brief Adds an element to the end of the Vector.
     * 
     * @param item The element to add.
     */
    void push_back(type&& item) {
        if (this->_size == this->_capacity)
            this->extend();
        this->_data_array[this->_size++] = std::move(item);
    }

    /**
     * @brief Adds an element to the beginning of the Vector.
     * 
     * @param item The element to add.
     */
    void push_front(type&& item) {
        if (this->_capacity - this->_size < 1)
            this->extend();
        
        for (size_t i = _size; i > 0; i--) {
            _data_array[i] = _data_array[i - 1];
        }
        _data_array[0] = item;
        _size++;
    }

    /**
     * @brief Removes the last element from the Vector.
     */
    void pop_back() {
        if (this->_size == 0)
            throw std::runtime_error("Vector is empty (pop_back() is not applicable)");
        this->_data_array[_size - 1] = type{};
        this->_size--;
    }


    /**
     * @brief Removes the first element from the Vector.
     */
    void pop_front() {
        if (_size > 0) {
            for (size_t i = 0; i < _size - 1; ++i) {
                _data_array[i] = _data_array[i + 1];
            }
            --_size;
        }
    }

    /**
     * @brief Inserts an element at a specified position.
     * 
     * @param index The position to insert at.
     * @param item The element to insert.
     */
    void insert(size_t index, type item) {
        if (index > this->_size)
            throw std::runtime_error("Index is out of bounds");
        
        if (this->_capacity == this->_size)
            this->extend();
        
        std::memmove(_data_array + index + 1, _data_array + index, (_size - index) * sizeof(type));
        
        _data_array[index] = item;
        _size++;
    }

    /**
     * @brief Clears all elements from the Vector.
     */
    void clear() {
        this->_size = 0;
        this->_capacity = 10;
        delete[] _data_array;
        this->_data_array = new type[_capacity];
    }

    /**
     * @brief Returns the current size of the Vector.
     * 
     * @return size_t The number of elements in the Vector.
     */
    inline size_t size() const {
        return this->_size;
    }

    /**
     * @brief Sorts the elements of the Vector using a custom comparator.
     * 
     * @param predicate The sorting comparator function.
     */
    void sort(std::function<bool(type, type)> predicate) {
        std::sort(this->_data_array, this->_data_array + this->_size, predicate);
    }

    /**
      * @brief Applies an action to each element in the vector, optionally filtering with a predicate.
      *
      * Iterates over all elements in the vector in either left-to-right or right-to-left order.
      * For each element that satisfies the given predicate, the specified action is applied.
      *
      * @param leftToRight If true, iteration is from beginning to end; if false, from end to beginning.
      * @param display_format A function to apply to each element that satisfies the action (goal: how type is displayed ?). 
      *                       Defaults to a no-op.
      * @param predicate A function that determines whether to apply the filter to a given element.
      *                  Defaults to always returning true (no filteration).
      */
    template<typename DisplayFunc = std::function<void(const type&)>,
    typename Predicate = std::function<bool(const type&)>>
    void for_each(bool leftToRight = true,
            DisplayFunc display_format = [](const type& item){},
            Predicate predicate = [](const type& item) { return true; })
        {
        int start = leftToRight ? 0 : _size - 1;
        int end = leftToRight ? _size : -1;
        int increment = leftToRight ? 1 : -1;

        for (int index = start; index != end; index += increment) {
            const type& item = _data_array[index];
            if (predicate(item)) {
                display_format(item);
            }
        }
    }


    /**
     * @brief Reverses the order of elements in the Vector.
     */
    void reverse() {
        for (size_t l = 0, r = _size - 1; l < r; l++, r--)
            std::swap(_data_array[l], _data_array[r]);
    }

    /**
     * @brief Returns a reference to the first element.
     * 
     * @return type& Reference to the first element.
     */
    type& front() {
        if (this->_size == 0)
            throw std::runtime_error("Vector is empty (front() is not applicable)");
        return this->_data_array[0];
    }

    /**
     * @brief Returns a const reference to the first element.
     * 
     * @return const type& Const reference to the first element.
     */
    const type& front() const {
        if (this->_size == 0)
            throw std::runtime_error("Vector is empty (front() is not applicable)");
        return this->_data_array[0];
    }

    /**
     * @brief Returns a reference to the last element.
     * 
     * @return type& Reference to the last element.
     */
    type& back() {
        if (this->_size == 0)
            throw std::runtime_error("Vector is empty (back() is not applicable)");
        return this->_data_array[this->_size - 1];
    }

    /**
     * @brief Returns a const reference to the last element.
     * 
     * @return const type& Const reference to the last element.
     */
    const type& back() const {
        if (this->_size == 0)
            throw std::runtime_error("Vector is empty (back() is not applicable)");
        return this->_data_array[this->_size - 1];
    }

    /**
     * @brief Checks if the Vector is empty.
     * 
     * @return true If the Vector is empty.
     * @return false Otherwise.
     */
    inline bool empty() const {
        return this->_size == 0;
    }

    /**
     * @brief Swaps the contents of two Vectors.
     * 
     * @param other The Vector to swap with.
     */
    void swap(Vector<type>& other) {
        size_t tmp_size = this->_size;
        size_t tmp_capacity = this->_capacity;
        type* ptr = this->_data_array;
        this->_size = other._size;
        this->_capacity = other._capacity;
        this->_data_array = other._data_array;
        other._size = tmp_size;
        other._capacity = tmp_capacity;
        other._data_array = ptr;
    }

    /**
     * @brief Reduces the capacity to fit the current size.
     */
    void shrink_to_fit() {
        if (_size < _capacity) {
            type* new_data_array{nullptr};
            if (_size > 0) {
                new_data_array = new type[_size];
                try {
                    std::copy(_data_array, _data_array + _size, new_data_array);
                } catch (...) {
                    delete[] new_data_array;
                    throw std::runtime_error("shrink to fit failed");
                }
            }
            delete[] _data_array;
            _data_array = new_data_array;
            _capacity = _size;
        }
    }

    /**
     * @brief Reserves memory for at least the specified capacity.
     * 
     * @param new_capacity The new capacity to reserve.
     */
    void reserve(size_t new_capacity) {
        if (new_capacity == 0 && _size != 0)
            throw std::logic_error("Cannot reserve zero capacity while size is not zero");
        if (new_capacity < _size)
            new_capacity = _size;
        if (new_capacity > _capacity) {
            type* new_data_array = new type[new_capacity];
            try {
                std::copy(_data_array, _data_array + _size, new_data_array);
            } catch (...) {
                delete[] new_data_array;
                throw std::runtime_error("Data Allocation Failed");
            }
            delete[] _data_array;
            _data_array = new_data_array;
            _capacity = new_capacity;
        }
    }

    /**
     * @brief Resizes the Vector to a new size.
     * 
     * @param new_size The new size of the Vector.
     * @param default_value The value to initialize new elements with.
     */
    void resize(size_t new_size, type default_value = type{}) {
        if (new_size < size()) {
            std::fill(_data_array + new_size, _data_array + size(), default_value);
        } else if (new_size > size()) {
            if (new_size > capacity())
                reallocate(new_size);
            std::fill(_data_array + size(), _data_array + new_size, default_value);
        }
        _size = new_size;
    }

    /**
     * @brief Copy assignment operator.
     * 
     * @param other The Vector to copy from.
     * @return Vector& Reference to the current Vector.
     */
    Vector& operator=(const Vector<type>& other) {
        if (this != &other) {
            this->_size = other._size;
            this->_capacity = other._capacity;
            delete[] this->_data_array;
            this->_data_array = nullptr;
            type* new_data_array = new type[this->_capacity];
            try {
                std::copy(other._data_array, other._data_array + other._size, new_data_array);
            } catch (...) {
                delete[] new_data_array;
                throw std::runtime_error("Data Allocation Failed");
            }
            this->_data_array = new_data_array;
        }
        return *this;
    }

    /**
     * @brief Move assignment operator.
     * 
     * @param other The Vector to move from.
     * @return Vector& Reference to the current Vector.
     */
    Vector& operator=(Vector<type>&& other) noexcept {
        if (other != this) {
            delete[] _data_array;
            this->_capacity = other._capacity;
            this->_size = other._size;
            this->_data_array = other._data_array;
            other._size = 0;
            other._capacity = 0;
            other._data_array = nullptr;
        }
        return *this;
    }

    /**
     * @brief Returns the current capacity of the Vector.
     * 
     * @return size_t The capacity of the Vector.
     */
    inline size_t capacity() const noexcept {
        return this->_capacity;
    }

    /**
     * @brief Returns an iterator to the beginning of the Vector.
     * 
     * @return Iterator Iterator to the beginning.
     */
    Iterator begin() {
        return Iterator(_data_array);
    }

    /**
     * @brief Returns an constant iterator to the begin of the Vector.
     * 
     * @return ContIterator Contant Iterator to the begin.
     */
    ConstIterator begin() const {
        return ConstIterator(_data_array);
    }

    /**
     * @brief Returns an iterator to the end of the Vector.
     * 
     * @return Iterator Iterator to the end.
     */
    Iterator end() {
        return Iterator(_data_array + _size);
    }

    /**
     * @brief Returns an constant iterator to the end of the Vector.
     * 
     * @return ContIterator Contant Iterator to the end.
     */
    ConstIterator end() const {
        return ConstIterator(_data_array+_size);
    }

    /**
     * @brief Returns a reverse iterator to the beginning of the Vector.
     * 
     * @return ReversedIterator Reverse iterator to the beginning.
     */
    ReversedIterator rbegin() {
        return ReversedIterator(_data_array + _size - 1);
    }

    /**
     * @brief Returns a reverse iterator to the end of the Vector.
     * 
     * @return ReversedIterator Reverse iterator to the end.
     */
    ReversedIterator rend() {
        return ReversedIterator(_data_array - 1);
    }

    /**
     * @brief Erases an element at a specified position.
     * 
     * @param position Iterator pointing to the element to erase.
     * @return Iterator Iterator to the next element.
     */
    Iterator erase(Iterator position) {
        size_t index = position - begin();
        for (size_t i = index; i < size() - 1; i++)
            _data_array[i] = std::move(_data_array[i + 1]);
        _data_array[_size - 1].~type();
        --_size;
        return begin() + index;
    }

    /**
     * @brief Erases a range of elements.
     * 
     * @param start Iterator to the start of the range.
     * @param end Iterator to the end of the range.
     * @return Iterator Iterator to the next element after the erased range.
     */
    Iterator erase(Iterator start, Iterator end) {
        size_t start_index = start - begin();
        size_t end_index = end - begin();
        size_t block = end_index - start_index;
        if (start_index > end_index || end_index > _size)
            throw std::out_of_range("Invalid range for erase");
        for (size_t i = 0; i < _size - end_index; ++i)
            _data_array[start_index + i] = std::move(_data_array[end_index + i]);
        for (size_t i = _size - block; i < _size; ++i)
            _data_array[i].~type();
        _size -= block;
        return begin() + start_index;
    }

    /**
     * @brief Accesses an element at a specified index with bounds checking.
     * 
     * @param index The index of the element to access.
     * @return type& Reference to the element.
     */
    type& at(size_t index) {
        if (index > this->_capacity)
            throw std::logic_error("Index is out of boundaries");
        return this->_data_array[index];
    }

    /**
     * @brief Accesses an element at a specified index without bounds checking.
     * 
     * @param index The index of the element to access.
     * @return type& Reference to the element.
     */
    type& operator[](size_t index) {
        return this->_data_array[index];
    }


    /**
     * @brief Accesses an element at a specified index without bounds checking.
     * 
     * @param index The index of the element to access.
     * @return a const reference to the element of type type at the specified index.
     */
    const type& operator[](size_t index) const {
        return this->_data_array[index];
    }
};

} // namespace Collections

#endif