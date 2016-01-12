#ifndef BOOST_TRIE_NODE_HPP
#define BOOST_TRIE_NODE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <map>
#include <vector>
#include <boost/utility.hpp>
#include <boost/intrusive/set.hpp>
#include <memory>

namespace boost { namespace tries {

namespace detail {

/*
 *template <typename Key, typename Value, bool isMultiValue>
 *struct trie_node;
 *
 *struct list_node_base : protected boost::noncopyable {
 *        typedef list_node_base *base_ptr;
 *        base_ptr pred;
 *        base_ptr next;
 *
 *        list_node_base() : pred(0), next(0)
 *        {}
 *};
 *
 *template <typename Key, typename Value>
 *struct value_list_node : public list_node_base {
 *        typedef Key key_type;
 *        typedef Value value_type;
 *        typedef trie_node<key_type, value_type, true> trie_node_type;
 *        typedef trie_node_type * trie_node_ptr;
 *        typedef value_list_node<key_type, value_type> node_type;
 *        typedef node_type * node_ptr;
 *        typedef list_node_base * base_ptr;
 *        value_type value;
 *        trie_node_ptr node_in_trie;
 *
 *        explicit value_list_node() : value(), node_in_trie(0)
 *        {
 *        }
 *
 *        explicit value_list_node(const value_type& x) : value(x), node_in_trie(0)
 *        {
 *        }
 *
 *        ~value_list_node() {
 *                node_in_trie->self_value_count--;
 *        }
 *};
 */

typedef boost::intrusive::link_mode<boost::intrusive::safe_link> safe_link_mode;
typedef boost::intrusive::link_mode<boost::intrusive::normal_link> normal_link_mode;
typedef boost::intrusive::optimize_size<true> optimized_size;
typedef boost::intrusive::optimize_size<false> unoptimized_size;
typedef boost::intrusive::constant_time_size<true> constant_time_size;
typedef boost::intrusive::constant_time_size<false> not_constant_time_size;

template <typename Key, typename ValueStoragePolicy>
struct trie_node;

struct comparator {
    template <typename Key, typename ValueStoragePolicy>
    bool operator () (const trie_node<Key, ValueStoragePolicy>& a, const trie_node<Key, ValueStoragePolicy>& b) const {
        return a.key < b.key;
    }

    template <typename Key, typename ValueStoragePolicy>
    bool operator () (const Key& a, const trie_node<Key, ValueStoragePolicy>& b) const {
        return a < b.key;
    }

    template <typename Key, typename ValueStoragePolicy>
    bool operator () (const trie_node<Key, ValueStoragePolicy>& a, const Key& b) const {
        return a.key < b;
    }
};

template <typename Key, typename ValueStoragePolicy>
inline bool operator < (const trie_node<Key, ValueStoragePolicy>& a, const trie_node<Key, ValueStoragePolicy>& b) {
    return a.key < b.key;
}

template <typename Key, typename ValueStoragePolicy>
inline bool operator > (const trie_node<Key, ValueStoragePolicy>& a, const trie_node<Key, ValueStoragePolicy>& b) {
    return a.key > b.key;
}

template <typename Key, typename ValueStoragePolicy>
inline bool operator == (const trie_node<Key, ValueStoragePolicy>& a, const trie_node<Key, ValueStoragePolicy>& b) {
    return a.key == b.key;
}

template<typename Key, typename ValueStoragePolicy>
struct trie_node :
    private boost::noncopyable,
    public ValueStoragePolicy,
    public boost::intrusive::set_base_hook<optimized_size, normal_link_mode>
{
    typedef Key key_type;
    typedef trie_node<Key, ValueStoragePolicy> self;
    typedef self* node_ptr;
    typedef boost::intrusive::set<self, not_constant_time_size> children_container_type;
    typedef typename children_container_type::iterator children_iterator_type;
    typedef size_t size_type;

    key_type key;
    children_container_type children;
    node_ptr parent;
    size_type value_count;

    explicit trie_node() : parent(NULL), value_count(0) { }
    explicit trie_node(const key_type& key) : key(key), parent(NULL), value_count(0) { }

    void copy_data_from(const self& rhs) {
	value_count = rhs.value_count;
	ValueStoragePolicy::copy_values_from(static_cast<ValueStoragePolicy>(rhs));
    }

    const key_type& key_elem() const {
	return key;
    }
};

} /* detail */
} /* tries */
} /* boost */

#endif
