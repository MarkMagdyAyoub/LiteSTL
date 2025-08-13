#pragma once
#include "doublyLinkedList.hpp"
#include <functional> 
#include <optional>   

namespace Collections {

/**
 * @brief A generic FIFO (First-In-First-Out) queue implementation using a DoublyLinkedList.
 *
 * This class provides a queue data structure backed by a custom DoublyLinkedList container.
 * Elements are inserted at the back and removed from the front.
 *
 * @tparam T The type of elements stored in the queue.
 */
template<typename T>
class Queue {
private:
    DoublyLinkedList<T> list_; /**< Underlying container for queue elements.*/

public:
    /**
     * @brief Constructs an empty queue.
     */
    Queue() = default;

    /**
     * @brief Constructs a queue from an initializer list.
     *
     * Example:
     * @code
     * Queue<int> q = {1, 2, 3};
     * @endcode
     *
     * @param init Initializer list of elements to populate the queue.
     */
    Queue(std::initializer_list<T> init) : list_(init) {}

    /**
     * @brief Destructor (default).
     */
    ~Queue() = default;

    /**
     * @brief Inserts an element at the back of the queue.
     *
     * Accepts both lvalue and rvalue references via perfect forwarding.
     *
     * @tparam U Type of the element (deduced automatically).
     * @param data The element to insert.
     */
    template<typename U , typename = std::enable_if_t<std::is_convertible_v<U, T>>> // force U to convert into T if possible
    void push(U&& data) {
        list_.push_back(static_cast<T>(std::forward<U>(data)));
    }

    /**
     * @brief Returns a reference to the front element (if any) without removing it.
     *
     * @note Uses `std::reference_wrapper` because `std::optional<T&>` is not allowed by the C++ standard.
     *       `std::reference_wrapper` is copyable and can store references in `std::optional`.
     *
     * @return `std::nullopt` if the queue is empty, otherwise an optional reference to the front element.
     */
    std::optional<std::reference_wrapper<T>> front() {
        return empty()  ? std::nullopt
                        : std::optional<std::reference_wrapper<T>>(list_.front());
    }

    /**
     * @brief Returns a const reference to the front element (if any) without removing it.
     *
     * @return `std::nullopt` if the queue is empty, otherwise an optional const reference to the front element.
     */
    std::optional<std::reference_wrapper<const T>> front() const {
        return empty()  ? std::nullopt
                        : std::optional<std::reference_wrapper<const T>>(list_.front());
    }

    /**
     * @brief Returns a reference to the back element (if any) without removing it.
     *
     * @return `std::nullopt` if the queue is empty, otherwise an optional reference to the back element.
     */
    std::optional<std::reference_wrapper<T>> back() {
        return empty()  ? std::nullopt
                        : std::optional<std::reference_wrapper<T>>(list_.back());
    }

    /**
     * @brief Returns a const reference to the back element (if any) without removing it.
     *
     * @return `std::nullopt` if the queue is empty, otherwise an optional const reference to the back element.
     */
    std::optional<std::reference_wrapper<const T>> back() const {
        return empty()  ? std::nullopt
                        : std::optional<std::reference_wrapper<const T>>(list_.back());
    }

    /**
     * @brief Removes the front element from the queue.
     *
     * If the queue is empty, this function does nothing.
     */
    void pop() {
        if (!list_.empty())
            list_.pop_front();
    }

    /**
     * @brief Checks whether the queue is empty.
     *
     * @return `true` if the queue has no elements, otherwise `false`.
     */
    bool empty() const {
        return list_.empty();
    }

    /**
     * @brief Returns the number of elements in the queue.
     *
     * @return The size of the queue.
     */
    size_t size() const {
        return list_.size();
    }

    /**
     * @brief Removes all elements from the queue.
     */
    void clear() {
        list_.clear();
    }


    /**
     * @brief Returns an iterator to the first element of the queue.
     * 
     * Allows iteration over the queue in a forward direction.
     * Modifications through the iterator affect the actual elements in the queue.
     * 
     * @return Iterator pointing to the first element.
     */
    auto begin() { return list_.begin(); }

    /**
     * @brief Returns an iterator past the last element of the queue.
     * 
     * This is used to define the end of a range when iterating.
     * 
     * @return Iterator pointing past the last element.
     */
    auto end() { return list_.end(); }

    /**
     * @brief Returns a const iterator to the first element of the queue.
     * 
     * Allows read-only iteration over the queue.
     * 
     * @return Const iterator pointing to the first element.
     */
    auto begin() const { return list_.begin(); }

    /**
     * @brief Returns a const iterator past the last element of the queue.
     * 
     * This is used to define the end of a read-only range when iterating.
     * 
     * @return Const iterator pointing past the last element.
     */
    auto end() const { return list_.end(); }
};

} // namespace Collections
