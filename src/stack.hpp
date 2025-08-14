#pragma once 
#include "./doublyLinkedList.hpp"

namespace Collections {

/**
 * @brief A generic stack (LIFO) container built on top of a DoublyLinkedList.
 * 
 * Provides standard stack operations such as push, pop, top, and supports
 * emplacing multiple elements at once with compile-time type checking.
 * 
 * @tparam T Type of elements stored in the stack.
 */
template<typename T>
class Stack {
private:
    DoublyLinkedList<T> list_; /**< Underlying container storing stack elements. */

public:
    /**
     * @brief Default constructor.
     */
    Stack() = default;

    /**
     * @brief Destructor.
     */
    ~Stack() = default;

    /**
     * @brief Pushes a single element onto the stack.
     * 
     * Uses perfect forwarding and accepts any type that is convertible to T.
     * 
     * @tparam U Type of the element to push (must be convertible to T).
     * @param item The element to add to the stack.
     */
    template<typename U, typename = std::enable_if<std::is_convertible_v<U, T>>>
    void push(U&& item) {
        list_.push_back(static_cast<T>(std::forward<U>(item)));
    }

    /**
     * @brief Emplaces one or more elements onto the stack.
     * 
     * Accepts a variable number of arguments. Each argument must be convertible to T.
     * Elements are forwarded to the push() method individually.
     * 
     * @tparam Args Types of elements to emplace (all must be convertible to T).
     * @param args Elements to add to the stack.
     */
    template<typename... Args>
    requires (std::convertible_to<Args, T> && ...)
    void emplace(Args&&... args) {
        (push(std::forward<Args>(args)), ...);
    }

    /**
     * @brief Returns a reference to the top element of the stack.
     * 
     * @return std::optional containing a reference to the top element, or std::nullopt if the stack is empty.
     */
    std::optional<std::reference_wrapper<T>> top() {
        return empty()  ? std::nullopt 
                        : std::optional<std::reference_wrapper<T>>(list_.back());
    }

    /**
     * @brief Removes the top element from the stack.
     * 
     * Does nothing if the stack is empty.
     */
    void pop() {
        if(!empty())
            list_.pop_back();
    }

    /**
     * @brief Swaps the contents of this stack with another stack.
     * 
     * @param other Stack to swap with.
     */
    void swap(Stack& other) noexcept {
        std::swap(list_, other.list_);
    }

    /**
     * @brief Removes all elements from the stack.
     */
    void clear() {
        list_.clear();
    }

    /**
     * @brief Checks whether the stack is empty.
     * 
     * @return true if the stack contains no elements, false otherwise.
     */
    bool empty() {
        return list_.empty();
    }

    /**
     * @brief Returns the number of elements in the stack.
     * 
     * @return Number of elements.
     */
    size_t size() {
        return list_.size();
    }
};

} // namespace Collections
