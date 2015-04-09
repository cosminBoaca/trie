#ifndef BOOST_TRIE_ITERATOR_HPP
#define BOOST_TRIE_ITERATOR_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <boost/trie/detail/trie_node.hpp>
#include <boost/type_traits/remove_const.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/blank.hpp>

namespace boost {  namespace tries {

namespace detail {

template<typename Key, typename Value, bool isMultiValue, typename Enable = void>
struct trie_iterator;

template<typename Key, typename Value>
struct trie_iterator<Key, Value, true>
{
	typedef std::bidirectional_iterator_tag iterator_category;
	typedef Key key_type;
	typedef std::pair<std::vector<key_type>, Value> value_type;
	typedef std::pair<std::vector<key_type>, Value&> reference;
	typedef std::pair<std::vector<key_type>, Value&>* pointer;
	typedef ptrdiff_t difference_type;
	typedef typename boost::remove_const<Value>::type non_const_value_type;
	typedef trie_iterator<Key, non_const_value_type, true> iterator;
	typedef trie_iterator<Key, Value, true> iter_type;
	typedef iter_type self;
	typedef trie_iterator<Key, const Value, true> const_iterator;
	typedef trie_node<Key, non_const_value_type, true> trie_node_type;
	typedef trie_node_type* trie_node_ptr;
	typedef value_list_node<Key, non_const_value_type> value_node_type;
	typedef value_node_type* value_node_ptr;
	typedef size_t size_type;
	typedef typename trie_node_type::children_type node_children_type;

	trie_node_ptr tnode;
	value_node_ptr vnode;

public:
	explicit trie_iterator() : tnode(0), vnode(0)
	{
	}

	trie_iterator(trie_node_ptr x) : tnode(x), vnode(x->value_list_header)
	{
	}

	trie_iterator(value_node_ptr x) : tnode(x->node_in_trie), vnode(x)
	{
	}

	explicit trie_iterator(trie_node_ptr t, value_node_ptr v) : tnode(t), vnode(v)
	{
	}

	trie_iterator(const iterator &it) : tnode(it.tnode), vnode(it.vnode)
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
		return reference(get_key(), vnode->value);
	}

	pointer operator->() const
	{
		return &(operator*());
	}

	bool operator==(const trie_iterator& other) const
	{
		return tnode == other.tnode && vnode == other.vnode;
	}

	bool operator!=(const trie_iterator& other) const
	{
		return tnode != other.tnode || vnode != other.vnode;
	}

	void trie_node_increment()
	{
		// at iterator end
		if (tnode->parent == NULL)
			return;
		tnode = tnode->next_node;
	}

	void trie_node_decrement()
	{
		// a begin iterator
		if (tnode->pred_node->parent == NULL)
			return;
		tnode = tnode->pred_node;
	}

	void increment()
	{
		if (vnode != NULL && vnode->next != NULL)
		{
			vnode = static_cast<value_node_ptr>(vnode->next);
			return;
		}
		trie_node_increment();
		vnode = tnode->value_list_header;
	}

	void decrement()
	{
		if (vnode != NULL && vnode->pred != NULL)
		{
			vnode = static_cast<value_node_ptr>(vnode->pred);
			return;
		}
		trie_node_decrement();
		vnode = tnode->value_list_tail;
	}

	/*
	   Delegate erasure of iterator to iterator instead of trie
	   in order to handle specializations that do not have a value list
	*/
	bool __erase_self_value_node() {
		value_node_ptr vp = this->vnode;
		trie_node_ptr trie_node = this->tnode;
		if (vp->next == NULL && vp->pred == NULL)
			return true;
		 if (vp->pred) {
			 vp->pred->next = vp->next;
		 } else {
			 trie_node->value_list_header = static_cast<value_node_ptr>(vp->next);
		 }
		 if (vp->next) {
			 vp->next->pred = vp->pred;
		 } else {
			trie_node->value_list_tail = static_cast<value_node_ptr>(vp->pred);
		 }
		 delete vp;
		 return false;
	}

	self& operator++()
	{
		increment();
		return *this;
	}

	self operator++(int) const
	{
		self tmp = *this;
		tmp.increment();
		return tmp;
	}

	self& operator--()
	{
		decrement();
		return *this;
	}

	self operator--(int) const
	{
		self tmp = *this;
		tmp.decrement();
		return tmp;
	}
};

template<typename Key, typename Value>
struct trie_iterator<Key, Value, false,
	typename boost::disable_if<boost::is_same<typename boost::remove_const<Value>::type, void> >::type>
{
	typedef std::bidirectional_iterator_tag iterator_category;
	typedef Key key_type;
	typedef std::pair<std::vector<key_type>, Value> value_type;
	typedef std::pair<std::vector<key_type>, Value&> reference;
	typedef std::pair<std::vector<key_type>, Value&>* pointer;
	typedef ptrdiff_t difference_type;
	typedef typename boost::remove_const<Value>::type non_const_value_type;
	typedef trie_iterator<Key, non_const_value_type, false> iterator;
	typedef trie_iterator<Key, Value, false> iter_type;
	typedef iter_type self;
	typedef trie_iterator<Key, const Value, false> const_iterator;
	typedef trie_node<Key, non_const_value_type, false> trie_node_type;
	typedef trie_node_type* trie_node_ptr;
	typedef size_t size_type;
	typedef typename trie_node_type::children_type node_children_type;
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

template<typename Key, typename Value>
struct trie_iterator<Key, Value, false,
	typename boost::enable_if<boost::is_same<typename boost::remove_const<Value>::type, void> >::type>
{
	typedef std::bidirectional_iterator_tag iterator_category;
	typedef Key key_type;
	typedef std::vector<key_type> value_type;
	typedef std::vector<key_type> reference;
	typedef std::vector<key_type>* pointer;
	typedef ptrdiff_t difference_type;
	typedef trie_iterator<Key, void, false> iterator;
	typedef trie_iterator<Key, Value, false> iter_type;
	typedef iter_type self;
	typedef trie_iterator<Key, const void, false> const_iterator;
	typedef trie_node<Key, void, false> trie_node_type;
	typedef trie_node_type* trie_node_ptr;
	typedef size_t size_type;

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

	void trie_node_increment()
	{
		// at iterator end
		if (tnode->parent == NULL)
			return;
		tnode = tnode->next_node;
	}

	void trie_node_decrement()
	{
		// at iterator begin
		if (tnode->pred_node->parent == NULL)
			return;
		tnode = tnode->pred_node;
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
