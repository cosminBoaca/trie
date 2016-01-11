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
#include <boost/trie/detail/value_storage_policy.hpp>

namespace boost { namespace tries {

template <typename Key, typename Value, bool multiValueNode = true>
class trie {
public:
	typedef Key key_type;
	typedef typename boost::mpl::if_
	<
		boost::is_void<Value>,
		boost::blank,
		Value
	>::type non_void_value_type;
	typedef typename detail::value_storage_policy
	<
		Value,
		multiValueNode,
		std::allocator<Value>
	>::type value_storage_policy;
	typedef Value value_type;
	typedef value_type* value_ptr;
	typedef trie<key_type, Value, multiValueNode> trie_type;
	typedef typename detail::trie_node<key_type, value_storage_policy> node_type;
	typedef node_type * node_ptr;
	typedef size_t size_type;
	typedef std::allocator<node_type> node_alloc_type;
	typedef detail::comparator comparator;
	/*
	 *typedef detail::value_remove_helper<node_type, value_alloc_type, multiValueNode> value_remove_helper;
	 *typedef detail::value_copy_helper<node_type, value_alloc_type, multiValueNode> value_copy_helper;
	 */

	typedef detail::trie_iterator<Key, value_storage_policy, multiValueNode> iterator;
	typedef typename iterator::const_iterator const_iterator;
	typedef std::reverse_iterator<iterator> reverse_iterator;
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
	typedef std::pair<iterator, bool> pair_iterator_bool;
	typedef std::pair<iterator, iterator> iterator_range;

private:
	/*
	 *value_remove_helper remove_values_from;
	 *value_copy_helper copy_data_from;
	 */
	node_alloc_type node_allocator;
	/*
	 *value_alloc_type value_allocator;
	 */

	comparator node_comparator;
	node_type root;
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
		node_count--;
		node->remove_values();
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
		if (other_root == &root)
			return;

		clear();
 		// because of the pred_node and next_node fields, copy() should implement by inserting one by one
		// but inserting one by one need key, so it is hard to do that

		std::stack<node_ptr> other_node_stk, self_node_stk;
		std::stack<typename node_type::children_iterator_type> ci_stk;
		other_node_stk.push(other_root);
		self_node_stk.push(&root);
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
				/*
				 *if (multiValueNode)
				 *    copy_data_from(new_node, c, value_allocator);
				 *else
				 *    copy_data_from(new_node, c);
				 */
				new_node->copy_data_from(*c);
				new_node->parent = self_cur;
				self_cur->children.insert(*new_node);
				// to next node
				++ci_stk.top();
				other_node_stk.push(c);
				ci_stk.push(c->children.begin());
				self_node_stk.push(new_node);
			}
		}
		/*
		 *if (multiValueNode)
		 *    copy_data_from(&root, other_root, value_allocator);
		 *else
		 *    copy_data_from(&root, other_root);
		 */
		root.copy_data_from(*other_root);
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
				typename node_type::children_iterator_type ci = p->children.iterator_to(*cur);
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

	template<typename Iter>
	void insert(node_ptr& cur, Iter& first, Iter last) {
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
		iterator insert_single_value(node_ptr cur, Iter first, Iter last,
				const non_void_value_type& value)
		{
			insert(cur, first, last);
			cur->add_value(value);
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
		iterator insert_multiple_value(node_ptr cur, Iter first, Iter last,
				const non_void_value_type& value) {
			insert(cur, first, last);
			cur->add_value(value);
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
		node_ptr find_node(Iter first, Iter last)
		{
			node_ptr cur = const_cast<node_ptr>(&root);
			for (; first != last; ++first)
			{
				const key_type& cur_key = *first;
				typename node_type::children_iterator_type ci = cur->children.find(cur_key, node_comparator);
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
		std::pair<iterator, bool> lower_bound_impl(Iter first, Iter last)
		{
			typedef typename node_type::children_iterator_type children_iterator;
			node_ptr cur = &root;
			node_ptr last_lb_candidate = NULL;

			for (; first != last; ++first)
			{
				const key_type& cur_key = *first;
				children_iterator child_iter =
					cur->children.find(cur_key, node_comparator);
				if (child_iter == cur->children.end()) {
					break;
				}
				children_iterator lb_candidate_iter = child_iter;
				lb_candidate_iter++;
				if (lb_candidate_iter != cur->children.end()) {
					last_lb_candidate = &(*lb_candidate_iter);
				}
				cur = &(*child_iter);
			}

			if (first != last) {
				children_iterator lb_candidate_iter =
					cur->children.upper_bound(*first, node_comparator);
				if (lb_candidate_iter == cur->children.end()) {
					if (last_lb_candidate != NULL) {
						cur = last_lb_candidate;
					} else {
						return std::make_pair(&root, false);
					}
				} else {
					return std::make_pair(&(*lb_candidate_iter), false);
				}
			}

			if (!cur->no_value() && first == last) {
				return std::make_pair(cur, true);
			}

			while (cur->no_value()) {
				cur = &(*cur->children.begin());
			}

			return std::make_pair(cur, false);
		}

	void erase_check_ancestor(node_ptr cur, size_type delta) // delete empty ancestors and update value_count
	{
		while (cur != &root && cur->children.empty() && cur->no_value())
		{
			node_ptr parent = cur->parent;
			parent->children.erase(parent->children.iterator_to(*cur));
			destroy_trie_node(cur);
			cur = parent;
		}

		// update value_count on each ancestral node
		while (cur != NULL)
		{
			cur->value_count -= delta;
			cur = cur->parent;
		}
	}

	void decrease_value_count_of_ancestors(node_ptr node) {
		node_ptr cur = node->parent;
		size_type delta = node->count();
		while (cur != NULL)
		{
			cur->value_count -= delta;
			cur = cur->parent;
		}
	}

	//erase one node with value, and erase empty ancestors
	size_type erase_node(node_ptr node)
	{
		size_type ret = 0;
		if (node == NULL)
			return ret;
		ret = node->count();
		node_ptr cur = node;
		node->remove_values();
		erase_check_ancestor(cur, ret);
		return ret;
	}

	void clear_impl(node_ptr node, bool deallocate_node) {
		typedef typename node_type::children_iterator_type children_iterator;

		node_ptr son;
		children_iterator son_iter = node->children.begin();
		children_iterator next_son_iter(son_iter);
		if (next_son_iter != node->children.end()) {
			++next_son_iter;
		}

		while (son_iter != node->children.end()) {
			son = &(*son_iter);
			son_iter = next_son_iter;
			if (next_son_iter != node->children.end()) {
				++next_son_iter;
			}
			clear_impl(son, true);
		}

		node->children.clear();
		decrease_value_count_of_ancestors(node);
		if (deallocate_node)
			destroy_trie_node(node);
	}

public:
	// iterators still unavailable here

	explicit trie() : node_allocator(), node_count(0)
	{
	}

	explicit trie(const trie_type& t) : node_allocator(), node_count(0)
	{
		copy_tree(const_cast<node_ptr>(&t.root));
	}

	trie_type& operator=(const trie_type& t)
	{
		copy_tree(const_cast<node_ptr>(&t.root));
		return *this;
	}


	iterator begin()
	{
		return leftmost_node(&root);
	}

	const_iterator begin() const
	{
		return leftmost_node(const_cast<node_ptr>(&root));
	}

	const_iterator cbegin() const
	{
		return leftmost_node(const_cast<node_ptr>(&root));
	}

	iterator end()
	{
		return (&root);
	}

	const_iterator end() const
	{
		return const_cast<node_ptr>(&root);
	}

	const_iterator cend() const
	{
		return const_cast<node_ptr>(&root);
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
		pair_iterator_bool insert_unique(Iter first, Iter last)
		{
			BOOST_STATIC_ASSERT_MSG(boost::is_void<Value>::value,
					"Value template parameter should be void");

			node_ptr cur = const_cast<node_ptr>(&root);
			for (; first != last; ++first)
			{
				const key_type& cur_key = *first;
				typename node_type::children_iterator_type ci = cur->children.find(cur_key, node_comparator);
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
			node_ptr cur = const_cast<node_ptr>(&root);
			for (; first != last; ++first)
			{
				const key_type& cur_key = *first;
				typename node_type::children_iterator_type ci = cur->children.find(cur_key, node_comparator);
				if (ci == cur->children.end())
				{
					return std::make_pair(insert_single_value(cur, first, last, value), true);
				}
				cur = &(*ci);
			}

			if (cur->no_value())
			{
				return std::make_pair(insert_single_value(cur, first, last, value), true);
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
			node_ptr cur = const_cast<node_ptr>(&root);
			for (; first != last; ++first)
			{
				const key_type& cur_key = *first;
				typename node_type::children_iterator_type ci = cur->children.find(cur_key, node_comparator);
				if (ci == cur->children.end())
				{
					return insert_multiple_value(cur, first, last, value);
				}
				cur = &(*ci);
			}
			return insert_multiple_value(cur, first, last, value);
		}

	template<typename Container>
		iterator insert_equal(const Container &container, const non_void_value_type& value)
		{
			return insert_equal(container.begin(), container.end(), value);
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
		iterator upper_bound(Iter first, Iter last)
		{
			std::pair<iterator, bool> lb_result = lower_bound_impl(first, last);
			// Full match
			if (lb_result.second) {
				++lb_result.first;
			}
			return lb_result.first;
		}

	template<typename Container>
		iterator upper_bound(const Container &container)
		{
			return upper_bound(container.begin(), container.end());
		}


	template<typename Iter>
		iterator lower_bound(Iter first, Iter last)
		{
			return lower_bound_impl(first, last).first;
		}

	template<typename Container>
		iterator lower_bound(const Container &container)
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
			if (node == NULL || node->count() == 0)
				return std::make_pair(iterator(&root), iterator(&root));
			iterator it_end = iterator(--(node->value_list.end()));
			++it_end;
			return std::make_pair(iterator(node->value_list.begin()), it_end);
		}

	template<typename Container>
		iterator_range equal_range(const Container &container)
		{
			return equal_range(container.begin(), container.end());
		}


	// erase one value, after erasing value, check if it is necessary to erase node
	iterator erase_impl(iterator it)
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

	iterator erase(iterator it)
	{
		return erase_impl(it);
	}

	iterator erase(const_iterator it)
	{
		return erase_impl(iterator(it.tnode, it.viter));
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



	void clear(node_ptr node) {
		clear_impl(node, false);
	}
		
	void swap(trie_type& t)
	{
		// is it OK?
		std::swap(root, t.root);
		std::swap(t.node_count, node_count);
	}

	void clear()
	{
		clear(&root);
	}

	size_type count_node() const {
		return node_count;
	}

	size_type size() const {
		return root.value_count;
	}

	bool empty() const {
		return root.value_count == 0;
	}

	~trie()
	{
		clear();
	}
};

} // tries
} // boost
#endif // BOOST_TRIE_HPP
