#ifndef BOOST_TRIE_HPP
#define BOOST_TRIE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <stack>
#include <boost/trie/detail/trie_node.hpp>
#include <boost/trie/detail/trie_iterator.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_void.hpp>
#include <boost/mpl/if.hpp>
#include <boost/blank.hpp>

namespace boost { namespace tries {

template <typename Key, typename Value, bool multi_value_node = true>
class trie {
public:
	typedef Key key_type;
	typedef typename boost::mpl::if_
	<
		boost::is_void<Value>,
		boost::blank,
		Value
	>::type non_void_value_type;
	typedef Value value_type;
	typedef value_type* value_ptr;
	typedef trie<key_type, Value, multi_value_node> trie_type;
	typedef typename detail::trie_node<key_type, value_type, multi_value_node> node_type;
	typedef node_type * node_ptr;
	typedef typename detail::value_list_node<key_type, value_type> value_node_type;
	typedef value_node_type * value_node_ptr;
	typedef size_t size_type;
	typedef std::allocator<node_type> node_alloc_type;
	typedef std::allocator<value_node_type> value_alloc_type;
	typedef typename node_type::comparator comparator;
	typedef detail::value_remove_helper<node_type, value_alloc_type, multi_value_node> value_remove_helper;
	typedef detail::value_copy_helper<node_type, value_alloc_type, multi_value_node> value_copy_helper;

private:
	value_remove_helper remove_values_from;
	value_copy_helper copy_values_from;
	node_alloc_type node_allocator;
	value_alloc_type value_allocator;
	comparator node_comparator;

	node_ptr root;
	size_type node_count; // node_count is difficult and useless to maintain on each node, so, put it on the tree

	node_ptr create_trie_node()
	{
		node_ptr new_node = node_allocator.allocate(1);
		return new(new_node) node_type();
	}

	node_ptr create_trie_node(const key_type& key)
	{
		node_ptr new_node = node_allocator.allocate(1);
		return new(new_node) node_type(key);
	}

	void destroy_trie_node(node_ptr node)
	{
		//node->remove_values(value_allocator);
		if (multi_value_node)
			remove_values_from(node, value_allocator);
		else
			remove_values_from(node);
		node_allocator.destroy(node);
		node_allocator.deallocate(node, 1);
	}

	// need constant time to get leftmost
	node_ptr leftmost_node(node_ptr node) const
	{
		node_ptr cur = node;
		while (!cur->children.empty() && cur->no_value())
		{
			cur = &(*(cur->children.begin()));
		}
		return cur;
	}

	/*value_node_ptr leftmost_value(node_ptr node) const
	{
		node = leftmost_node(node);
		return static_cast<value_node_ptr>(node->value_list_header);
	}*/

	// need constant time to get rightmost
	node_ptr rightmost_node(node_ptr node) const
	{
		node_ptr cur = node;
		while (!cur->children.empty())
		{
			cur = &(*(cur->children.rbegin()));
		}
		return cur;
	}

	/*value_node_ptr rightmost_value(node_ptr node) const
	{
		node = rightmost_node(node);
		return static_cast<value_node_ptr>(node->value_list_tail);
	}*/

	// copy the whole trie tree
	void copy_tree(node_ptr other_root)
	{
		if (other_root == root)
			return;

		clear();
 		// because of the pred_node and next_node fields, copy() should implement by inserting one by one
		// but inserting one by one need key, so it is hard to do that

		std::stack<node_ptr> other_node_stk, self_node_stk;
		std::stack<typename node_type::children_iter> ci_stk;
		other_node_stk.push(other_root);
		self_node_stk.push(root);
		ci_stk.push(other_root->children.begin());
		for (; !other_node_stk.empty(); )
		{
			node_ptr other_cur = other_node_stk.top();
			node_ptr self_cur = self_node_stk.top();
			if (ci_stk.top() == other_cur->children.end())
			{
				other_node_stk.pop();
				ci_stk.pop();
				self_node_stk.pop();
			} else {
				node_ptr c = &(*ci_stk.top());
				// create new node
				node_ptr new_node = create_trie_node(c->key);
				if (new_node != NULL)
					node_count++;
				if (multi_value_node)
					copy_values_from(new_node, c, value_allocator);
				else
					copy_values_from(new_node, c);
				new_node->parent = self_cur;
				self_cur->children.insert(*new_node);
				// to next node
				++ci_stk.top();
				other_node_stk.push(c);
				ci_stk.push(c->children.begin());
				self_node_stk.push(new_node);
			}
		}
		if (multi_value_node)
			copy_values_from(root, other_root, value_allocator);
		else
			copy_values_from(root, other_root);
	}

	node_ptr next_node_with_value(node_ptr tnode)
	{
		// at iterator end
		if (tnode->parent == NULL)
			return tnode;
		node_ptr cur = tnode;
		if (!cur->children.empty())
		{ // go down to the first node with a value in it, and there always be at least one
			do {
				cur = &(*(cur->children.begin()));
			} while (cur->no_value());
			tnode = cur;
		} else {
			// go up till there is a sibling next to cur
			// the algorithm here is not so efficient
			while (cur->parent != NULL)
			{
				node_ptr p = cur->parent;
				typename node_type::children_iter ci = p->children.iterator_to(*cur);
				++ci;
				if (ci != p->children.end())
				{
					cur = &(*ci);
					//"change value to self_value_count
					while (cur->no_value()) {
						cur = &(*(cur->children.begin()));
					}
					break;
				}
				cur = p;
			}
			tnode = cur;
		}
		return tnode;
	}


public:
	// iterators still unavailable here

	explicit trie() : node_allocator(), value_allocator(),
		root(create_trie_node()),
		node_count(0)
	{
	}

	explicit trie(const trie_type& t) : node_allocator(), value_allocator(),
		root(create_trie_node()),
		node_count(0)
	{
		copy_tree(t.root);
	}

	trie_type& operator=(const trie_type& t)
	{
		copy_tree(t.root);
		return *this;
	}

	typedef detail::trie_iterator<Key, Value, multi_value_node> iterator;
	typedef typename iterator::const_iterator const_iterator;
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
	typedef std::pair<iterator, bool> pair_iterator_bool;
	typedef std::pair<iterator, iterator> iterator_range;

	iterator begin()
	{
		return leftmost_node(root);
	}

	const_iterator begin() const
	{
		return leftmost_node(root);
	}

	const_iterator cbegin() const
	{
		return leftmost_node(root);
	}

	iterator end()
	{
		return root;
	}

	const_iterator end() const
	{
		return root;
	}

	const_iterator cend() const
	{
		return root;
	}

	reverse_iterator rbegin()
	{
		return static_cast<reverse_iterator>(end());
	}

	const_reverse_iterator rbegin() const
	{
		return static_cast<const_reverse_iterator>(cend());
	}

	const_reverse_iterator crbegin() const
	{
		return rbegin();
	}

	reverse_iterator rend()
	{
		return static_cast<reverse_iterator>(begin());
	}

	const_reverse_iterator rend() const
	{
		return static_cast<const_reverse_iterator>(cbegin());
	}

	const_reverse_iterator crend() const
	{
		return rend();
	}

	template<typename Iter>
	void __insert(node_ptr& cur, Iter& first, Iter last) {
		for (; first != last; ++first)
		{
			const key_type& cur_key = *first;
			node_ptr new_node = create_trie_node(cur_key);
			node_count++;
			new_node->parent = cur;
			cur->children.insert(*new_node);
			cur = new_node;
		}
	}

	template<typename Iter>
		iterator __insert_single_value(node_ptr cur, Iter first, Iter last,
				const non_void_value_type& value)
		{
			__insert(cur, first, last);
			cur->value = value;
			cur->has_value = true;
			// update value_count on the path
			node_ptr tmp = cur;
			while (tmp != NULL) // until root
			{
				++tmp->value_count;
				tmp = tmp->parent;
			}
			return cur;
		}

	template<typename Iter>
		iterator __insert_multiple_value(node_ptr cur, Iter first, Iter last,
				const non_void_value_type& value) {
			__insert(cur, first, last);
			cur->add_value(value, value_allocator);
			// update value_count on the path
			node_ptr tmp = cur;
			while (tmp != NULL) // until root
			{
				++tmp->value_count;
				tmp = tmp->parent;
			}
			return cur->value_list_header;
		}

	template<typename Iter>
		pair_iterator_bool insert_unique(Iter first, Iter last)
		{
			BOOST_STATIC_ASSERT_MSG(boost::is_void<Value>::value,
					"Value template parameter should be void");

			node_ptr cur = root;
			for (; first != last; ++first)
			{
				const key_type& cur_key = *first;
				typename node_type::children_iter ci = cur->children.find(cur_key, node_comparator);
				if (ci == cur->children.end())
					break;
				cur = &(*ci);
			}

			if (first == last && !cur->no_value()) {
				return std::make_pair(iterator(cur), false);
			}

			for (; first != last; ++first)
			{
				const key_type& cur_key = *first;
				node_ptr new_node = create_trie_node(cur_key);
				node_count++;
				new_node->parent = cur;
				cur->children.insert(*new_node);
				cur = new_node;
			}

			cur->key_ends_here = true;

			// update value_count on the path
			node_ptr tmp = cur;
			while (tmp != NULL) // until root
			{
				++tmp->value_count;
				tmp = tmp->parent;
			}

			return std::make_pair(iterator(cur), true);
		}

	template<typename Container>
		pair_iterator_bool insert_unique(const Container &container)
		{
			BOOST_STATIC_ASSERT_MSG(boost::is_void<Value>::value,
					"Value template parameter should be void");
			return insert_unique(container.begin(), container.end());
		}

	template<typename Iter>
		pair_iterator_bool insert_unique(Iter first, Iter last, const non_void_value_type& value)
		{
			node_ptr cur = root;
			for (; first != last; ++first)
			{
				const key_type& cur_key = *first;
				typename node_type::children_iter ci = cur->children.find(cur_key, node_comparator);
				if (ci == cur->children.end())
				{
					return std::make_pair(__insert_single_value(cur, first, last, value), true);
				}
				cur = &(*ci);
			}

			if (cur->no_value())
			{
				return std::make_pair(__insert_single_value(cur, first, last, value), true);
			}

			return std::make_pair(iterator(cur), false);
		}

	template<typename Container>
		pair_iterator_bool insert_unique(const Container &container, const non_void_value_type& value)
		{
			return insert_unique(container.begin(), container.end(), value);
		}

	template<typename Iter>
		iterator insert_equal(Iter first, Iter last,
				const non_void_value_type& value)
		{
			node_ptr cur = root;
			for (; first != last; ++first)
			{
				const key_type& cur_key = *first;
				typename node_type::children_iter ci = cur->children.find(cur_key, node_comparator);
				if (ci == cur->children.end())
				{
					return __insert_multiple_value(cur, first, last, value);
				}
				cur = &(*ci);
			}
			return __insert_multiple_value(cur, first, last, value);
		}

	template<typename Container>
		iterator insert_equal(const Container &container, const non_void_value_type& value)
		{
			return insert_equal(container.begin(), container.end(), value);
		}

	template<typename Iter>
		node_ptr find_node(Iter first, Iter last)
		{
			node_ptr cur = root;
			for (; first != last; ++first)
			{
				const key_type& cur_key = *first;
				typename node_type::children_iter ci = cur->children.find(cur_key, node_comparator);
				if (ci == cur->children.end())
				{
					return NULL;
				}
				cur = &(*ci);
			}
			return cur;
		}

	template<typename Container>
		node_ptr find_node(const Container &container)
		{
			return find_node(container.begin(), container.end());
		}

	template<typename Iter>
		iterator find(Iter first, Iter last)
		{
			node_ptr node = find_node(first, last);
			if (node == NULL || node->no_value())
				return end();
			return node;
		}

	template<typename Container>
		iterator find(const Container &container)
		{
			return find(container.begin(), container.end());
		}

	// count
	template<typename Iter>
		size_type count(Iter first, Iter last)
		{
			node_ptr node = find_node(first, last);
			if (node == NULL || node->no_value())
				return 0;
			return node->count();
		}

	template<typename Container>
		size_type count(const Container &container)
		{
			return count(container.begin(), container.end());
		}

	// find by prefix, return a pair of iterator(begin, end)
	template<typename Iter>
		iterator_range find_prefix(Iter first, Iter last)
		{
			node_ptr node = find_node(first, last);
			if (node == NULL)
			{
				return std::make_pair(end(), end());
			}
			iterator begin = leftmost_node(node);
			// optimization is needed here
			node = rightmost_node(node);
			iterator end(node);
			while(end.tnode == node)
				++end;
			return std::make_pair(begin, end);
		}

	template<typename Container>
		iterator_range find_prefix(const Container &container)
		{
			return find_prefix(container.begin(), container.end());
		}

	// count_prefix() to count values with the same prefix
	template<typename Iter>
		size_type count_prefix(Iter first, Iter last)
		{
			node_ptr node = find_node(first, last);
			if (node == NULL)
			{
				return 0;
			}
			return node->value_count;
		}

	template<typename Container>
		size_type count_prefix(const Container &container)
		{
			return count_prefix(container.begin(), container.end());
		}

	// upper_bound() to find the first node that greater than the key
	template<typename Iter>
		node_ptr upper_bound(Iter first, Iter last)
		{
			node_ptr cur = root;
			// use a stack to store iterator in order to avoid the iterator cannot go backward
			std::stack< Iter > si;
			for (; first != last; ++first)
			{
				si.push(first);
				const key_type& cur_key = *first;
				typename node_type::children_iter ci = cur->children.find(cur_key, node_comparator);
				// using upper_bound needs comparison in every step, so using find until ci == NULL
				if (ci == cur->children.end())
				{
					// find a node that
					ci = cur->children.upper_bound(cur_key, node_comparator);
					si.pop();
					while (ci == cur->children.end())
					{
						if (cur->parent == NULL)
							return root;
						cur = cur->parent;
						ci = cur->children.upper_bound(*si.top());
					}
					cur = ci->second;
					while (cur->no_value())
					{
						cur = &(*(cur->children.begin()));
					}
					return cur;
				}
				cur = &(*ci);
			}
			// if find a full match, then increment it
			iterator tmp(cur);
			tmp.trie_node_increment();
			cur = tmp.tnode;
			return cur;
		}

	template<typename Container>
		node_ptr upper_bound(const Container &container)
		{
			return upper_bound(container.begin(), container.end());
		}

	// lower_bound()
	template<typename Iter>
		node_ptr lower_bound(Iter first, Iter last)
		{
			node_ptr cur = root;
			// use a stack to store iterator in order to avoid the iterator cannot go backward
			std::stack< Iter > si;
			for (; first != last; ++first)
			{
				si.push(first);
				const key_type& cur_key = *first;
				typename node_type::children_iter ci = cur->children.find(cur_key);
				// using upper_bound needs comparison in every step, so using find until ci == NULL
				if (ci == cur->children.end())
				{
					// find a node that
					ci = cur->children.upper_bound(cur_key);
					si.pop();
					while (ci == cur->children.end())
					{
						if (cur->parent == NULL)
							return root;
						cur = cur->parent;
						ci = cur->children.upper_bound(*si.top());
					}
					cur = ci->second;
					while (cur->no_value())
					{
						cur = &(*(cur->children.begin()));
					}
					return cur;
				}
				cur = &(*ci);
			}
			// lower_bound() needn't increment here!!!
			return cur;
		}

	template<typename Container>
		node_ptr lower_bound(const Container &container)
		{
			return lower_bound(container.begin(), container.end());
		}

	// equal_range() is the same as find_prefix? the meaning is different
	// equal_range() aims at one node, while find_prefix aims at the whole sub-trie
	template<typename Iter>
		iterator_range equal_range(Iter first, Iter last)
		{
			// @that is not right here
			//return make_pair(lower_bound(first, last), upper_bound(first, last));
			node_ptr node = find_node(first, last);
			if (node == NULL || node->value_list_header == NULL)
				return std::make_pair(iterator(root), iterator(root));
			iterator it_end = iterator(node->value_list_tail);
			++it_end;
			return std::make_pair(iterator(node->value_list_header), it_end);
		}

	template<typename Container>
		iterator_range equal_range(const Container &container)
		{
			return equal_range(container.begin(), container.end());
		}

	void erase_check_ancestor(node_ptr cur, size_type delta) // delete empty ancestors and update value_count
	{
		while (cur != root && cur->children.empty() && cur->no_value())
		{
			node_ptr parent = cur->parent;
			parent->children.erase(parent->children.iterator_to(*cur));
			destroy_trie_node(cur);
			node_count--;
			cur = parent;
		}

		// update value_count on each ancestral node
		while (cur != NULL)
		{
			cur->value_count -= delta;
			cur = cur->parent;
		}
	}

public:
	//erase one node with value, and erase empty ancestors
	size_type erase_node(node_ptr node)
	{
		size_type ret = 0;
		if (node == NULL)
			return ret;
		ret = node->count();
		node_ptr cur = node;
		if (multi_value_node)
			remove_values_from(node, value_allocator);
		else
			remove_values_from(node);
		erase_check_ancestor(cur, ret);
		return ret;
	}

	// erase one value, after erasing value, check if it is necessary to erase node
	iterator erase(iterator it)
	{
		if (it == end())
			return it;
		iterator ret = it;
		++ret;
		bool should_erase_node = it.__erase_self_value_node();
		if (should_erase_node) {
			erase_node(it.tnode);
		} else {
			erase_check_ancestor(it.tnode, 1);
		}
		return ret;
	}

	iterator erase(const_iterator it)
	{
		return erase(iterator(it.tnode, it.vnode));
	}

	template<typename Iter>
		iterator erase(Iter first, Iter last)
		{
			iterator it = find(first, last);
			return erase(it);

		}

	template<typename Container>
		iterator erase(const Container &container)
		{
			return erase(container.begin(), container.end());
		}

	template<typename Iter>
		size_type erase_node(Iter first, Iter last)
		{
			return erase_node(find_node(first, last));
		}

	template<typename Container>
		size_type erase_node(const Container &container)
		{
			return erase_node(container.begin(), container.end());
		}

	// erase a range of iterators
	void erase(iterator first, iterator last)
	{
		for (; first != last; ++first)
			erase(first);
	}

	// erase all subsequences with prefix
	template<typename Iter>
		size_type erase_prefix(Iter first, Iter last)
		{
			node_ptr cur = find_node(first, last);
			size_type ret = cur->value_count;
			clear(cur);
			return ret;
		}

	template<typename Container>
		size_type erase_prefix(const Container &container)
		{
			return erase_prefix(container.begin(), container.end());
		}

	void clear(node_ptr node)
	{
		node_ptr cur = node;
		node_ptr leftmost = leftmost_node(cur);
		node_ptr rightmost = rightmost_node(cur);

		iterator it(cur);

		if (cur == root)
			it = begin();

		if (leftmost == rightmost)
			return;

		if (leftmost == cur)
		{
			cur = (++it).tnode;
		}
		else {
			cur = leftmost;
		}
		for (node_ptr next; cur != rightmost; cur = next)
		{
			/*
			 *This is necessary because the ++ operator on the multivalue
			 *iterator wouldn't always modify tnode and this will result
			 *in double free
			 */
			if (multi_value_node)
				remove_values_from(it.tnode, value_allocator);
			next = (++it).tnode;
			erase_node(cur);
		}
		erase_node(cur);
	}

	void swap(const trie_type& t)
	{
		// is it OK?
		std::swap(root, t.root);
		std::swap(t.node_count, node_count);
	}

	void clear(bool delete_root = false)
	{
		clear(root);
		if (delete_root) {
			destroy_trie_node(root);
			node_count--;
		}
	}

	size_type count_node() const {
		return node_count;
	}

	size_type size() const {
		return root->value_count;
	}

	bool empty() const {
		return root->value_count == 0;
	}

	~trie()
	{
		clear(true);
	}
};

} // tries
} // boost
#endif // BOOST_TRIE_HPP
