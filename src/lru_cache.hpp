#ifndef LRU_CACHE
#define LRU_CACHE

#include <optional>
#include <unordered_map>

namespace Collections {

template <typename K, typename V>
struct Node {
  std::optional<K> key;
  std::optional<V> value;
  Node* next;
  Node* prev;

  Node()
      : next(nullptr), prev(nullptr), key(std::nullopt), value(std::nullopt) {}

  Node(K key, V value)
      : key(std::move(key)),
        value(std::move(value)),
        next(nullptr),
        prev(nullptr) {}
};

template <typename K, typename V>
  requires std::equality_comparable<K> && requires(K k) { std::hash<K>{}(k); }
class LRUCache {
 private:
  int _capacity;
  std::unordered_map<K, Node<K, V>*> _cache_mapper;
  Node<K, V>* _head;
  Node<K, V>* _tail;

  void add(Node<K, V>* node) {  // Time O(1) , Space O(1)
    Node<K, V>* head_next = _head->next;
    link(node, head_next);
    link(_head, node);
  }

  void link(Node<K, V>* node1, Node<K, V>* node2) {  // Time O(1) , Space O(1)
    node1->next = node2;
    node2->prev = node1;
  }

  void remove(Node<K, V>* node) {  // Time O(1) , Space O(1)
    Node<K, V>* prev_node = node->prev;
    Node<K, V>* next_node = node->next;
    link(prev_node, next_node);  // link them together to avoid null dereference
  }

 public:
  LRUCache(int capacity) : _capacity(capacity) {
    _head = new Node<K, V>();  // dummy nodes
    _tail = new Node<K, V>();  // dummy nodes
    link(_head, _tail);
  }

  ~LRUCache() {  // Time O(capacity=n)
    Node<K, V>* curr = _head;
    while (curr != nullptr) {
      Node<K, V>* next = curr->next;
      delete curr;
      curr = next;
    }
  }

  void put(K key, V value) {  // Time O(1) , Space O(1)
    if (_cache_mapper.find(key) != _cache_mapper.end()) {
      Node<K, V>* old_node = _cache_mapper[key];
      remove(old_node);
      delete old_node;
    }

    Node<K, V>* new_node = new Node<K, V>(key, value);
    _cache_mapper[key] = new_node;
    add(new_node);

    if (_cache_mapper.size() > _capacity) {
      Node<K, V>* tail_prev = _tail->prev;
      remove(tail_prev);
      if (tail_prev->key.has_value()) {
        _cache_mapper.erase(tail_prev->key.value());
      }
      delete tail_prev;
    }
  }

  std::optional<V> get(const K& key) {  // Time O(1) , Space O(1)
    if (_cache_mapper.find(key) != _cache_mapper.end()) {
      Node<K, V>* node = _cache_mapper[key];
      remove(node);
      add(node);
      return node->value;
    }
    return std::nullopt;
  }
};
}  // namespace Collections
#endif