#ifndef BOOST_TRIE_ITERATOR_HPP
#define BOOST_TRIE_ITERATOR_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <boost/trie/detail/trie_node.hpp>
#include <boost/type_traits/remove_const.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/mpl/and.hpp>
#include <boost/blank.hpp>
#include <boost/trie/detail/value_storage_policy.hpp>

namespace boost {  namespace tries {

namespace detail {

template<typename Key, typename ValueStoragePolicy, bool isMultiValue>
struct trie_iterator;

template<typename Key, typename ValueStoragePolicy>
struct trie_iterator<Key, ValueStoragePolicy, true>
{
	typedef typename ValueStoragePolicy::value_type Value;
	typedef std::bidirectional_iterator_tag iterator_category;
	typedef Key key_type;
	typedef std::pair<std::vector<key_type>, Value> value_type;
	typedef std::pair<std::vector<key_type>, Value&> reference;
	typedef std::pair<std::vector<key_type>, Value&>* pointer;
	typedef ptrdiff_t difference_type;

	typedef typename ValueStoragePolicy::iterator value_iterator;
	typedef typename ValueStoragePolicy::non_const_value_policy_type non_const_value_storage_policy;
	typedef typename ValueStoragePolicy::const_value_policy_type const_value_storage_policy;
	typedef trie_iterator<Key, non_const_value_storage_policy, true> iterator;
	typedef trie_iterator<Key, ValueStoragePolicy, true> self;
	typedef trie_iterator<Key, const_value_storage_policy, true> const_iterator;
	typedef trie_node<Key, non_const_value_storage_policy> trie_node_type;
	typedef trie_node_type* trie_node_ptr;
	typedef size_t size_type;
	typedef typename trie_node_type::children_container_type node_children_type;
	typedef typename node_children_type::iterator children_iterator;
	typedef typename node_children_type::reverse_iterator children_reverse_iterator;

	trie_node_ptr tnode;
	value_iterator viter;

public:
	explicit trie_iterator() : tnode(0), viter(0)
	{
	}

	trie_iterator(trie_node_ptr x) : tnode(x), viter(x->value_list.begin())
	{
	}

	explicit trie_iterator(trie_node_ptr t, value_iterator v) : tnode(t), viter(v)
	{
	}

	trie_iterator(const iterator &it) : tnode(it.tnode), viter(it.viter)
	{
	}

	std::vector<key_type> get_key() const
	{
		std::vector<key_type> key_path;
		size_type path_length = 0;
		trie_node_ptr cur;
		for (cur = tnode; cur->parent != NULL; cur = cur->parent)
			path_length++;
		key_path.resize(path_length);
		for (cur = tnode; cur->parent != NULL; cur = cur->parent)
		{
			key_path[path_length - 1] = cur->key_elem();
			path_length--;
		}
		return key_path;
	}

	reference operator*() const
	{
		return reference(get_key(), *viter);
	}

	pointer operator->() const
	{
		return &(operator*());
	}

	bool operator==(const trie_iterator& other) const
	{
		return tnode == other.tnode && viter == other.viter;
	}

	bool operator!=(const trie_iterator& other) const
	{
		return tnode != other.tnode || viter != other.viter;
	}

	void go_up_forward() {
		typename node_children_type::iterator it;
		tnode = tnode->parent;
		viter = tnode->value_list.begin();
		while (tnode->parent != NULL) {
			it = ++(node_children_type::s_iterator_to(*tnode));
			if (it != tnode->parent->children.end()) {
				tnode = &(*it);
				if (tnode->no_value())
					go_down_forward();
				else
					viter = tnode->value_list.begin();
				break;
			}
			tnode = tnode->parent;
		}
	}

	bool go_down_forward() {
		if (tnode->children.empty())
			return false;
		do  {
			tnode = &(*(tnode->children.begin()));
		} while (!tnode->children.empty() && tnode->no_value());
		viter = tnode->value_list.begin();
		return true;
	}

	void go_down_backward() {
		if (tnode->children.empty()) {
			viter = tnode->value_list.begin();
			return;
		}
		do {
			tnode = &(*(tnode->children.rbegin()));
		} while(!tnode->children.empty());
		viter = tnode->value_list.end();
		std::advance(viter, -1);
	}

	void go_up_backward() {
		typename node_children_type::iterator it;
		tnode = tnode->parent;
		while (tnode->parent && tnode->no_value()) {
			it = node_children_type::s_iterator_to(*tnode);
			if (it != tnode->parent->children.begin()) {
				--it;
				tnode = &(*it);
				go_down_backward();
				break;
			}
			tnode = tnode->parent;
		}
		if (tnode->parent == NULL)
			go_down_forward();
	}

	void trie_node_increment()
	{
		if (tnode->parent == NULL)
			return;
		/*
		 *First check is usefull when iterator is used for erasure
		 *by the clear function. In that case viter gets invalided
		 *and the viter->next would generate memory error
		 */
		if (!tnode->no_value() && viter != tnode->value_list.end()) {
			++viter;
			return;
		}

		if (!go_down_forward()) {
			typename node_children_type::iterator it =
				++(node_children_type::s_iterator_to(*tnode));
			if (it != tnode->parent->children.end()) {
				tnode = &(*it);
				if (tnode->no_value()) {
					go_down_forward();
				} else {
					viter = tnode->value_list.begin();
				}
			} else {
				go_up_forward();
			}
		}
	}

	void trie_node_decrement()
	{
		/*
		 *no_value() check is useful when iterator is on the root node
		 *and value may be NULL. This happens at rbegin() call
		 */
		if (!tnode->no_value() && viter != tnode->value_list.begin()) {
			--viter;
			return;
		}

		children_iterator it = node_children_type::s_iterator_to(*tnode);
		if (tnode->parent) {
			if (it != tnode->parent->children.begin()) {
				--it;
				tnode = &(*it);
				go_down_backward();
			} else {
				go_up_backward();
			}
		} else {
			go_down_backward();
		}
	}

	/*
	   Delegate erasure of iterator to iterator instead of trie
	   in order to handle specializations that do not have a value list
	*/
	bool __erase_self_value_node() {
		if (tnode->count() == 1)
			return true;
		tnode->value_list.erase(viter);
		return false;
	}

	self& operator++()
	{
		trie_node_increment();
		return *this;
	}

	self operator++(int) const
	{
		self tmp = *this;
		trie_node_increment();
		return tmp;
	}

	self& operator--()
	{
		trie_node_decrement();
		return *this;
	}

	self operator--(int) const
	{
		self tmp = *this;
		trie_node_decrement();
		return tmp;
	}
};

template<typename Key, typename ValueStoragePolicy>
struct trie_iterator<Key, ValueStoragePolicy, false>
{
	typedef typename ValueStoragePolicy::value_type Value;
	typedef std::bidirectional_iterator_tag iterator_category;
	typedef Key key_type;
	typedef std::pair<std::vector<key_type>, Value> value_type;
	typedef std::pair<std::vector<key_type>, Value&> reference;
	typedef std::pair<std::vector<key_type>, Value&>* pointer;
	typedef ptrdiff_t difference_type;

	typedef typename ValueStoragePolicy::non_const_value_policy_type non_const_value_storage_policy;
	typedef typename ValueStoragePolicy::const_value_policy_type const_value_storage_policy;
	typedef trie_iterator<Key, non_const_value_storage_policy, false> iterator;
	typedef trie_iterator<Key, ValueStoragePolicy, false> self;
	typedef trie_iterator<Key, const_value_storage_policy, false> const_iterator;
	typedef trie_node<Key, non_const_value_storage_policy> trie_node_type;
	typedef trie_node_type* trie_node_ptr;
	typedef size_t size_type;
	typedef typename trie_node_type::children_container_type node_children_type;
	typedef typename node_children_type::iterator children_iterator;
	typedef typename node_children_type::reverse_iterator children_reverse_iterator;

	trie_node_ptr tnode;

public:
	explicit trie_iterator() : tnode(0)
	{
	}

	trie_iterator(trie_node_ptr x) : tnode(x)
	{
	}

	trie_iterator(const iterator &it) : tnode(it.tnode)
	{
	}

	std::vector<key_type> get_key() const
	{
		std::vector<key_type> key_path;
		size_type path_length = 0;
		trie_node_ptr cur;
		for (cur = tnode; cur->parent != NULL; cur = cur->parent)
			path_length++;
		key_path.resize(path_length);
		for (cur = tnode; cur->parent != NULL; cur = cur->parent)
		{
			key_path[path_length - 1] = cur->key_elem();
			path_length--;
		}
		return key_path;
	}

	/*
	   This iterator doesn't have any value node. If erase it's called on it
	   the coresponding node in trie should be removed too
	*/
	bool __erase_self_value_node() {
		return true;
	}

	reference operator*() const
	{
		return reference(get_key(), tnode->value);
	}

	pointer operator->() const
	{
		return &(operator*());
	}

	bool operator==(const trie_iterator& other) const
	{
		return tnode == other.tnode;
	}

	bool operator!=(const trie_iterator& other) const
	{
		return tnode != other.tnode;
	}

	void go_up_forward() {
		typename node_children_type::iterator it;
		tnode = tnode->parent;
		while (tnode->parent != NULL) {
			it = ++(node_children_type::s_iterator_to(*tnode));
			if (it != tnode->parent->children.end()) {
				tnode = &(*it);
				if (tnode->no_value())
					go_down_forward();
				break;
			}
			tnode = tnode->parent;
		}
	}

	bool go_down_forward() {
		if (tnode->children.empty())
			return false;
		do  {
				tnode = &(*(tnode->children.begin()));
		} while (!tnode->children.empty() && tnode->no_value());
		return true;
	}

	void go_down_backward() {
		if (tnode->children.empty())
			return;
		do {
			tnode = &(*(tnode->children.rbegin()));
		} while(!tnode->children.empty());
	}

	void go_up_backward() {
		typename node_children_type::iterator it;
		tnode = tnode->parent;
		while (tnode->parent && tnode->no_value()) {
			it = node_children_type::s_iterator_to(*tnode);
			if (it != tnode->parent->children.begin()) {
				--it;
				tnode = &(*it);
				go_down_backward();
				break;
			}
			tnode = tnode->parent;
		}
		if (tnode->parent == NULL)
			go_down_forward();
	}

	void trie_node_increment()
	{
		if (tnode->parent == NULL)
			return;

		if (!go_down_forward()) {
			typename node_children_type::iterator it =
				++(node_children_type::s_iterator_to(*tnode));
			if (it != tnode->parent->children.end()) {
				tnode = &(*it);
				if (tnode->no_value()) {
					go_down_forward();
				}
			} else {
				go_up_forward();
			}
		}
	}

	void trie_node_decrement()
	{
		children_iterator it = node_children_type::s_iterator_to(*tnode);
		if (tnode->parent) {
			if (it != tnode->parent->children.begin()) {
				--it;
				tnode = &(*it);
				go_down_backward();
			} else {
				go_up_backward();
			}
		} else {
			go_down_backward();
		}
	}

	self& operator++()
	{
		trie_node_increment();
		return *this;
	}

	self operator++(int) const
	{
		self tmp = *this;
		trie_node_increment();
		return tmp;
	}

	self& operator--()
	{
		trie_node_decrement();
		return *this;
	}

	self operator--(int) const
	{
		self tmp = *this;
		trie_node_decrement();
		return tmp;
	}
};

template<typename Key>
struct trie_iterator<Key, no_value_policy, false>
{
	typedef std::bidirectional_iterator_tag iterator_category;
	typedef Key key_type;
	typedef std::vector<key_type> value_type;
	typedef std::vector<key_type> reference;
	typedef std::vector<key_type>* pointer;
	typedef ptrdiff_t difference_type;
	typedef trie_iterator<Key, no_value_policy, false> iterator;
	typedef trie_iterator<Key, no_value_policy, false> self;
	typedef trie_iterator<Key, no_value_policy, false> const_iterator;
	typedef trie_node<Key, no_value_policy> trie_node_type;
	typedef trie_node_type* trie_node_ptr;
	typedef size_t size_type;
	typedef typename trie_node_type::children_container_type node_children_type;
	typedef typename node_children_type::iterator children_iterator;
	typedef typename node_children_type::reverse_iterator children_reverse_iterator;

	trie_node_ptr tnode;

public:
	explicit trie_iterator() : tnode(0)
	{
	}

	trie_iterator(trie_node_ptr x) : tnode(x)
	{
	}

	trie_iterator(const iterator &it) : tnode(it.tnode)
	{
	}

	std::vector<key_type> get_key() const
	{
		std::vector<key_type> key_path;
		size_type path_length = 0;
		trie_node_ptr cur;
		for (cur = tnode; cur->parent != NULL; cur = cur->parent)
			path_length++;
		key_path.resize(path_length);
		for (cur = tnode; cur->parent != NULL; cur = cur->parent)
		{
			key_path[path_length - 1] = cur->key_elem();
			path_length--;
		}
		return key_path;
	}

	/*
	   This iterator doesn't have any value node. If erase it's called on it
	   the coresponding node in trie should be removed too
	*/
	bool __erase_self_value_node() {
		return true;
	}

	reference operator*() const
	{
		return get_key();
	}

	pointer operator->() const
	{
		return &(operator*());
	}

	bool operator==(const trie_iterator& other) const
	{
		return tnode == other.tnode;
	}

	bool operator!=(const trie_iterator& other) const
	{
		return tnode != other.tnode;
	}

	void go_up_forward() {
		typename node_children_type::iterator it;
		tnode = tnode->parent;
		while (tnode->parent != NULL) {
			it = ++(node_children_type::s_iterator_to(*tnode));
			if (it != tnode->parent->children.end()) {
				tnode = &(*it);
				if (tnode->no_value())
					go_down_forward();
				break;
			}
			tnode = tnode->parent;
		}
	}

	bool go_down_forward() {
		if (tnode->children.empty())
			return false;
		do  {
				tnode = &(*(tnode->children.begin()));
		} while (!tnode->children.empty() && tnode->no_value());
		return true;
	}

	void go_down_backward() {
		if (tnode->children.empty())
			return;
		do {
			tnode = &(*(tnode->children.rbegin()));
		} while(!tnode->children.empty());
	}

	void go_up_backward() {
		typename node_children_type::iterator it;
		tnode = tnode->parent;
		while (tnode->parent && tnode->no_value()) {
			it = node_children_type::s_iterator_to(*tnode);
			if (it != tnode->parent->children.begin()) {
				--it;
				tnode = &(*it);
				go_down_backward();
				break;
			}
			tnode = tnode->parent;
		}
		if (tnode->parent == NULL)
			go_down_forward();
	}

	void trie_node_increment()
	{
		if (tnode->parent == NULL)
			return;

		if (!go_down_forward()) {
			typename node_children_type::iterator it =
				++(node_children_type::s_iterator_to(*tnode));
			if (it != tnode->parent->children.end()) {
				tnode = &(*it);
				if (tnode->no_value()) {
					go_down_forward();
				}
			} else {
				go_up_forward();
			}
		}
	}

	void trie_node_decrement()
	{
		children_iterator it = node_children_type::s_iterator_to(*tnode);
		if (tnode->parent) {
			if (it != tnode->parent->children.begin()) {
				--it;
				tnode = &(*it);
				go_down_backward();
			} else {
				go_up_backward();
			}
		} else {
			go_down_backward();
		}
	}

	self& operator++()
	{
		trie_node_increment();
		return *this;
	}

	self operator++(int) const
	{
		self tmp = *this;
		tmp.trie_node_increment();
		return tmp;
	}

	self& operator--()
	{
		trie_node_decrement();
		return *this;
	}

	self operator--(int) const
	{
		self tmp = *this;
		tmp.trie_node_decrement();
		return tmp;
	}
};

} /* detail */
} /* tries */
} /* boost */

#endif
