#pragma once

#include <deque>
#include <concepts>
#include <cstdint>
#include <functional>
#include <optional>
#include <unordered_map>
#include <set>
#include <stdexcept>

/**
 * @file lru_k_cache.h
 * @brief Implementation of an LRU-K Cache Replacement Policy.
 */

namespace Collections {

#define THROW_RUNTIME(msg) throw std::runtime_error(msg)

/** @brief Alias for the timestamp used in access history. */
using timestamp_t = uint64_t;

/**
 * @brief Concept ensuring a type is hashable and three-way comparable.
 */
template <typename T>
concept HashableAndComparable = requires(T a) {
    { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
} && std::three_way_comparable<T>;

/**
 * @brief A class implementing the LRU-K (Least Recently Used - K) Cache policy.
 * 
 * @tparam K Key type (must be hashable and comparable).
 * @tparam V Value type.
 */
template<HashableAndComparable K, typename V>
class LRU_K_Cache {
private:
    /**
     * @brief Internal structure representing a node in the cache.
     */
    struct LRUNode {
        K key_;                          ///< The key of the entry.
        V value_;                        ///< The associated value.
        std::deque<timestamp_t> history_; ///< Timestamps of accesses.
        bool is_evictable_;             ///< Indicates if the node is eligible for eviction.

        /**
         * @brief Constructs a new LRUNode with perfect forwarding.
         * 
         * @tparam KeyType Type of the key.
         * @tparam ValueType Type of the value.
         * @param key The key of the entry.
         * @param value The value of the entry.
         */
        template<typename KeyType, typename ValueType>
        LRUNode(KeyType&& key, ValueType&& value)
            : key_(std::forward<KeyType>(key)),
              value_(std::forward<ValueType>(value)),
              is_evictable_(false) {}
    };

    /**
     * @brief Comparator for the eviction set. Sorts by oldest k-th timestamp.
     */
    struct Compare {
        int k_;

        /**
         * @brief Constructs a Compare object.
         * @param k Number of accesses to track (K).
         */
        Compare(int k) : k_(k) {}

        /**
         * @brief Comparison function used by std::set.
         */
        bool operator()(const LRUNode* a, const LRUNode* b) const {
            if (a->history_.size() < k_ && b->history_.size() < k_) {
                return a->key_ < b->key_;
            }
            if (a->history_.size() < k_) return false;
            if (b->history_.size() < k_) return true;
            if (a->history_.front() == b->history_.front())
                return a->key_ < b->key_;
            return a->history_.front() < b->history_.front();
        }
    };

    size_t capacity_;                          ///< Maximum number of entries.
    size_t k_;                                 ///< Number of recent accesses to track.
    std::unordered_map<K, LRUNode*> cache_;    ///< Main cache storage.
    std::set<LRUNode*, Compare> eviction_set_; ///< Set of evictable entries.
    timestamp_t current_timestamp_;            ///< Current timestamp.

    /**
     * @brief Updates access history and eviction set after accessing a node.
     * 
     * @param node The accessed cache node.
     */
    void ResourceAccess(LRUNode* node) {
        if (current_timestamp_ == std::numeric_limits<timestamp_t>::max()) {
            THROW_RUNTIME("Timestamp overflow in LRU-K Cache");
        }

        if (node->history_.size() == k_) {
            eviction_set_.erase(node);
        }

        current_timestamp_++;
        node->history_.push_back(current_timestamp_);
        if (node->history_.size() > k_) {
            node->history_.pop_front();
        }

        if (node->history_.size() == k_) {
            if (!node->is_evictable_) {
                node->is_evictable_ = true;
            }

            if (node->is_evictable_) {
                eviction_set_.insert(node);
            }
        }
    }

    /**
     * @brief Evicts the least recently used node if cache is at capacity.
     */
    void Evict() {
        if (cache_.size() < capacity_) return;

        auto itr = eviction_set_.begin();
        while (itr != eviction_set_.end()) {
            LRUNode* node = *itr;
            if (node->is_evictable_) {
                eviction_set_.erase(itr);
                cache_.erase(node->key_);
                delete node;
                break;
            }
            ++itr;
        }
    }

public:
    /**
     * @brief Constructs an LRU-K Cache with given capacity and K value.
     * 
     * @param cache_size Maximum number of entries.
     * @param k Number of recent accesses to track per key.
     */
    LRU_K_Cache(size_t cache_size, size_t k)
        : capacity_(cache_size),
          k_(k),
          current_timestamp_(0),
          eviction_set_(Compare(k)) {}

    /**
     * @brief Destroys the cache and releases allocated memory.
     */
    ~LRU_K_Cache() {
        for (auto& [_, node] : cache_) {
            delete node;
        }
    }

    /**
     * @brief Retrieves the value associated with a key and updates access history.
     * 
     * @param key The key to look for.
     * @return An optional containing the value if found, otherwise std::nullopt.
     */
    std::optional<V> Get(const K& key) {
        if (cache_.find(key) == cache_.end()) {
            return std::nullopt;
        }

        LRUNode* node = cache_[key];
        ResourceAccess(node);
        return node->value_;
    }

    /**
     * @brief Inserts or updates a key-value pair in the cache.
     * 
     * @param key The key to insert or update.
     * @param value The associated value.
     */
    void Put(K&& key, V&& value) {
        auto it = cache_.find(key);
        if (it != cache_.end()) {
            LRUNode* node = it->second;
            ResourceAccess(node);
            node->value_ = std::forward<V>(value);
        } else {
            Evict();
            LRUNode* node = new LRUNode(std::forward<K>(key), std::forward<V>(value));
            cache_[node->key_] = node;
            ResourceAccess(node);
        }
    }

    /**
     * @brief Removes a key and its value from the cache.
     * 
     * @param k The key to remove.
     * @return True if the key existed and was removed, false otherwise.
     */
    bool Remove(const K& k) {
        auto itr = cache_.find(k);
        if (itr == cache_.end()) return false;

        LRUNode* node = itr->second;

        if (node->history_.size() == k_ && node->is_evictable_) {
            eviction_set_.erase(node);
        }

        cache_.erase(itr);
        delete node;
        return true;
    }

    /**
     * @brief Checks whether a key exists in the cache.
     * 
     * @param key The key to look for.
     * @return True if key is present, false otherwise.
     */
    bool contains(const K& key) const {
        return cache_.find(key) != cache_.end();
    }

    /**
     * @brief Returns the number of elements currently in the cache.
     * 
     * @return Current cache size.
     */
    size_t size() const { return cache_.size(); }
};

} // namespace Collections
