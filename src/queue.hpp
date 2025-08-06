#pragma once
#include "doublyLinkedList.hpp"

namespace Collections{
    
template<typename T>
class Queue{
private:
    DoublyLinkedList<T> list;

public:
    Queue(const initializer_list<T>& list){
        for(auto& item : list)
            push(item);
    }

    ~Queue(){
        list.~DoublyLinkedList();
    }

    void push(T data){list.push_back(data);}

    optional<T&> front(){ return empty() ? list.front() : std::nullopt;}

    void pop(){
        if(list.empty()) list.pop_front();
    }

    bool empty() const { return list.empty();}

    size_t size() const {return list.size();}

    void clear(){list.clear();}

    optional<T&> back(){ return empty() ? list.back() : std::nullopt; }
};
}