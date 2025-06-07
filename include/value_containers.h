#ifndef CHRONOVYAN_VALUE_CONTAINERS_H
#define CHRONOVYAN_VALUE_CONTAINERS_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "value_fwd.h"

namespace chronovyan {

/**
 * @class ValueArray
 * @brief A wrapper around std::vector<std::shared_ptr<Value>> to break circular dependency
 */
class ValueArray {
public:
    using ElementType = std::shared_ptr<Value>;
    using ContainerType = std::vector<ElementType>;
    using iterator = ContainerType::iterator;
    using const_iterator = ContainerType::const_iterator;
    using size_type = ContainerType::size_type;

    ValueArray() = default;
    explicit ValueArray(const ContainerType& elements) : elements_(elements) {}
    explicit ValueArray(ContainerType&& elements) : elements_(std::move(elements)) {}

    // Element access
    ElementType& operator[](size_type pos) { return elements_[pos]; }
    const ElementType& operator[](size_type pos) const { return elements_[pos]; }
    ElementType& at(size_type pos) { return elements_.at(pos); }
    const ElementType& at(size_type pos) const { return elements_.at(pos); }
    ElementType& front() { return elements_.front(); }
    const ElementType& front() const { return elements_.front(); }
    ElementType& back() { return elements_.back(); }
    const ElementType& back() const { return elements_.back(); }

    // Iterators
    iterator begin() noexcept { return elements_.begin(); }
    const_iterator begin() const noexcept { return elements_.begin(); }
    const_iterator cbegin() const noexcept { return elements_.cbegin(); }
    iterator end() noexcept { return elements_.end(); }
    const_iterator end() const noexcept { return elements_.end(); }
    const_iterator cend() const noexcept { return elements_.cend(); }

    // Capacity
    bool empty() const noexcept { return elements_.empty(); }
    size_type size() const noexcept { return elements_.size(); }
    size_type max_size() const noexcept { return elements_.max_size(); }
    void reserve(size_type new_cap) { elements_.reserve(new_cap); }
    size_type capacity() const noexcept { return elements_.capacity(); }
    void shrink_to_fit() { elements_.shrink_to_fit(); }

    // Modifiers
    void clear() noexcept { elements_.clear(); }
    iterator insert(const_iterator pos, const ElementType& value) {
        return elements_.insert(pos, value);
    }
    iterator insert(const_iterator pos, ElementType&& value) {
        return elements_.insert(pos, std::move(value));
    }
    template <class... Args>
    iterator emplace(const_iterator pos, Args&&... args) {
        return elements_.emplace(pos, std::forward<Args>(args)...);
    }
    iterator erase(const_iterator pos) { return elements_.erase(pos); }
    iterator erase(const_iterator first, const_iterator last) {
        return elements_.erase(first, last);
    }
    void push_back(const ElementType& value) { elements_.push_back(value); }
    void push_back(ElementType&& value) { elements_.push_back(std::move(value)); }
    template <class... Args>
    void emplace_back(Args&&... args) {
        elements_.emplace_back(std::forward<Args>(args)...);
    }
    void pop_back() { elements_.pop_back(); }
    void resize(size_type count) { elements_.resize(count); }
    void resize(size_type count, const ElementType& value) { elements_.resize(count, value); }
    void swap(ValueArray& other) noexcept { elements_.swap(other.elements_); }

    // Access to underlying container
    const ContainerType& get() const { return elements_; }
    ContainerType& get() { return elements_; }

private:
    ContainerType elements_;
};

/**
 * @class ValueMap
 * @brief A wrapper around std::map<std::string, std::shared_ptr<Value>> to break circular
 * dependency
 */
class ValueMap {
public:
    using key_type = std::string;
    using mapped_type = std::shared_ptr<Value>;
    using value_type = std::pair<const key_type, mapped_type>;
    using ContainerType = std::map<key_type, mapped_type>;
    using iterator = ContainerType::iterator;
    using const_iterator = ContainerType::const_iterator;
    using size_type = ContainerType::size_type;

    ValueMap() = default;
    explicit ValueMap(const ContainerType& elements) : elements_(elements) {}
    explicit ValueMap(ContainerType&& elements) : elements_(std::move(elements)) {}

    // Element access
    mapped_type& operator[](const key_type& key) { return elements_[key]; }
    mapped_type& at(const key_type& key) { return elements_.at(key); }
    const mapped_type& at(const key_type& key) const { return elements_.at(key); }

    // Iterators
    iterator begin() noexcept { return elements_.begin(); }
    const_iterator begin() const noexcept { return elements_.begin(); }
    const_iterator cbegin() const noexcept { return elements_.cbegin(); }
    iterator end() noexcept { return elements_.end(); }
    const_iterator end() const noexcept { return elements_.end(); }
    const_iterator cend() const noexcept { return elements_.cend(); }

    // Capacity
    bool empty() const noexcept { return elements_.empty(); }
    size_type size() const noexcept { return elements_.size(); }
    size_type max_size() const noexcept { return elements_.max_size(); }

    // Modifiers
    void clear() noexcept { elements_.clear(); }
    std::pair<iterator, bool> insert(const value_type& value) { return elements_.insert(value); }
    template <class P>
    std::pair<iterator, bool> insert(P&& value) {
        return elements_.insert(std::forward<P>(value));
    }
    template <class... Args>
    std::pair<iterator, bool> emplace(Args&&... args) {
        return elements_.emplace(std::forward<Args>(args)...);
    }
    iterator erase(const_iterator pos) { return elements_.erase(pos); }
    size_type erase(const key_type& key) { return elements_.erase(key); }
    iterator erase(const_iterator first, const_iterator last) {
        return elements_.erase(first, last);
    }
    void swap(ValueMap& other) noexcept { elements_.swap(other.elements_); }

    // Lookup
    size_type count(const key_type& key) const { return elements_.count(key); }
    iterator find(const key_type& key) { return elements_.find(key); }
    const_iterator find(const key_type& key) const { return elements_.find(key); }
    std::pair<iterator, iterator> equal_range(const key_type& key) {
        return elements_.equal_range(key);
    }
    std::pair<const_iterator, const_iterator> equal_range(const key_type& key) const {
        return elements_.equal_range(key);
    }
    iterator lower_bound(const key_type& key) { return elements_.lower_bound(key); }
    const_iterator lower_bound(const key_type& key) const { return elements_.lower_bound(key); }
    iterator upper_bound(const key_type& key) { return elements_.upper_bound(key); }
    const_iterator upper_bound(const key_type& key) const { return elements_.upper_bound(key); }

    // Access to underlying container
    const ContainerType& get() const { return elements_; }
    ContainerType& get() { return elements_; }

private:
    ContainerType elements_;
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_VALUE_CONTAINERS_H
