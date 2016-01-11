#ifndef BOOST_TRIE_VALUE_STORAGE_POLICY_HPP
#define BOOST_TRIE_VALUE_STORAGE_POLICY_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <list>
#include <cstddef>
#include <boost/type_traits/remove_const.hpp>
#include <boost/type_traits/add_const.hpp>

namespace boost {
namespace tries {
namespace detail {

template<class Value, class Allocator>
struct multiple_value_storage_policy {
    typedef typename boost::remove_const<Value>::type stored_value_type;
    typedef Value value_type;
    typedef typename boost::add_const<stored_value_type>::type const_value_type;
    typedef typename std::list<stored_value_type, Allocator>::iterator iterator;
    typedef multiple_value_storage_policy<const_value_type, Allocator>
        const_value_policy_type;
    typedef multiple_value_storage_policy<stored_value_type, Allocator>
        non_const_value_policy_type;

    std::list<stored_value_type, Allocator> value_list;

    bool no_value() const {
        return value_list.empty();
    }

    void add_value(const stored_value_type& value) {
        value_list.push_front(value);
    }

    void copy_values_from(const multiple_value_storage_policy<Value, Allocator>& rhs) {
        value_list = rhs.value_list;
    }

    void remove_values() {
        value_list.clear();
    }

    size_t count() const {
        return value_list.size();
    }
};

template<class Value>
struct single_value_storage_policy {
    typedef Value value_type;
    typedef typename boost::remove_const<Value>::type stored_value_type;
    typedef typename boost::add_const<stored_value_type>::type const_value_type;
    typedef single_value_storage_policy<const_value_type> const_value_policy_type;
    typedef single_value_storage_policy<stored_value_type> non_const_value_policy_type;

    stored_value_type value;
    bool has_value;

    single_value_storage_policy() : has_value(false) { }

    bool no_value() const {
        return !has_value;
    }

    void add_value(const stored_value_type& value) {
        this->value = value;
        has_value = true;
    }

    void copy_values_from(const single_value_storage_policy<Value>& rhs) {
        value = rhs.value;
        has_value = rhs.has_value;
    }

    void remove_values() {
        has_value = false;
    }

    size_t count() const {
        return has_value;
    }
};

struct no_value_policy {
    bool key_ends_here;

    no_value_policy() : key_ends_here(false) { }

    bool no_value() const {
        return !key_ends_here;
    }

    //???
    void add_value() {
        key_ends_here = true;
    }

    void copy_values_from(const no_value_policy& rhs) {
        key_ends_here = rhs.key_ends_here;
    }

    size_t count() const {
        return key_ends_here;
    }

    void remove_values() {
        key_ends_here = false;
    }
};

template<class Value, bool multiValueNode, class Allocator>
struct value_storage_policy;

template<class Allocator>
struct value_storage_policy<void, false, Allocator> {
    typedef no_value_policy type;
};

template<class Value, class Allocator>
struct value_storage_policy<Value, false, Allocator> {
    typedef single_value_storage_policy<Value> type;
};

template<class Value, class Allocator>
struct value_storage_policy<Value, true, Allocator> {
    typedef multiple_value_storage_policy<Value, Allocator> type;
};

template<class Allocator>
struct value_storage_policy<void, true, Allocator> {
};

}}}

#endif
