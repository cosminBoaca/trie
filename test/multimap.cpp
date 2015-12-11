#include <boost/core/lightweight_test.hpp>
#include "boost/trie/trie_multimap.hpp"
// multi include test
#include "boost/trie/trie_multimap.hpp"
#include "boost/trie/trie.hpp"

#include <string>

typedef boost::tries::trie_multimap<char, int> tci;
typedef tci::iterator iter_type;

void operator_test()
{
	boost::tries::trie_multimap<char, int> t;
	std::string s = "aaa", s2 = "bbb";
	t.insert(s, 1);
	t.insert(s, 2);
	BOOST_TEST(t.count(s) == 2);
}

void insert_and_find_test()
{
	boost::tries::trie_multimap<char, int> t;
	std::string s = "aaa", s1 = "aaaa", s2 = "aab", s3 = "bbb";
	t.insert(s, 1);
	BOOST_TEST((*t.find(s)).second == 1);
	BOOST_TEST(t.find(s) != t.end());
	BOOST_TEST(t.find(s2) == t.end());
	BOOST_TEST(t.find(s2) == t.end());
	t.insert(s, 2);
	BOOST_TEST((*t.find(s)).second == 2);
	t.insert(s2, (*t.find(s)).second);
	BOOST_TEST(t.find(s2) != t.end());
	BOOST_TEST(t.find(s1) == t.find(s3));
	BOOST_TEST(t.find(s) != t.find(s2));
	BOOST_TEST((*t.find(s)).second == (*t.find(s2)).second);
	iter_type i = t.insert(s2, 1);
	BOOST_TEST((*i).second == 1);
	++i;
	BOOST_TEST((*i).second == 2);
	BOOST_TEST(t.count_prefix(std::string("a")) == 4);
	BOOST_TEST(t.count_prefix(std::string("aa")) == 4);
	BOOST_TEST(t.count_prefix(std::string("aaa")) == 2);
	BOOST_TEST(t.count_prefix(std::string("aab")) == 2);
}

/*
void copy_test()
{
	boost::tries::trie_multimap<char, int> t, t2;
	std::string s = "aaa", s1 = "aaaa", s2 = "aab", s3 = "bbb";
	t[s] = 1; t[s1] = 2; t[s2] = 3; t[s3] = 4;
	t2 = t;
	BOOST_TEST(t2.size() == 4);
	BOOST_TEST(t2.count_node() == 8);
	BOOST_TEST(t2[s] == 1);
	BOOST_TEST(t2[s1] == 2);
	BOOST_TEST(t2[s2] == 3);
	BOOST_TEST(t2[s3] == 4);
	boost::tries::trie_multimap<char, int> t3(t2);
	BOOST_TEST(t3.size() == 4);
	BOOST_TEST(t3.count_node() == 8);
	BOOST_TEST((*t3.find(s)).second == 1);
	BOOST_TEST((*t3.find(s1)).second == 2);
	BOOST_TEST((*t3.find(s2)).second == 3);
	BOOST_TEST((*t3.find(s3)).second == 4);
	t3[std::string("a")] = 10;
	BOOST_TEST(t3.size() == 5);
	BOOST_TEST(t3.count_node() == 8);
	BOOST_TEST((*t3.begin()).second == 10);
}

void iterator_operator_plus()
{
	boost::tries::trie_multimap<char, int> t;
	std::string s = "aaa", s1 = "aaaa", s2 = "aab", s3 = "bbb";
	BOOST_TEST(t.empty() == true);
	BOOST_TEST(t.size() == 0);
	BOOST_TEST(t.begin() == t.end());
	t[s] = 1;
	t[s1] = 2;
	t[s2] = 3;
	BOOST_TEST(t.begin() != t.end());
	boost::tries::trie_multimap<char, int>::iterator ti;
	ti = t.begin();
	BOOST_TEST((*ti).second == 1);
	++ti;
	BOOST_TEST((*ti).second == 2);
	BOOST_TEST(t[s2] == 3);
	++ti;
	BOOST_TEST((*ti).second == 3);
	++ti;
	BOOST_TEST(ti == t.end());
	// test ++end()
	++ti;
	BOOST_TEST(ti == t.end());
}

void iterator_operator_minus()
{
	boost::tries::trie_multimap<char, int> t;
	std::string s = "aaa", s1 = "aaaa", s2 = "aab", s3 = "bbb";
	BOOST_TEST(t.empty() == true);
	BOOST_TEST(t.size() == 0);
	BOOST_TEST(t.begin() == t.end());
	t[s] = 1;
	t[s1] = 2;
	t[s2] = 3;
	BOOST_TEST(t.begin() != t.end());
	boost::tries::trie_multimap<char, int>::iterator ti;
	boost::tries::trie_multimap<char, int>::const_iterator cti(t.end());
	ti = t.begin();
	BOOST_TEST((*ti).second == 1);
	(*ti).second= 10;
	BOOST_TEST((*ti).second == 10);
	cti = t.begin();
	BOOST_TEST((*cti).second == 10);
	--ti;
	BOOST_TEST(ti == t.end());
	BOOST_TEST(t[s2] == 3);
	--ti;
	BOOST_TEST((*ti).second == 3);
	t[s3] = 4;
	++ti;
	BOOST_TEST((*ti).second == 4);
	++ti;
	BOOST_TEST(ti == t.end());
	++ti;
	BOOST_TEST(ti == t.end());
	--ti;
	BOOST_TEST((*ti).second == 4);
	BOOST_TEST(t.count_node() == 8);
	BOOST_TEST(t.size() == 4);
}

void clear()
{
	boost::tries::trie_multimap<char, int> t;
	std::string s = "aaa", s1 = "aaaa", s2 = "aab", s3 = "bbb";
	t[s] = t[s1] = t[s2] = t[s3] = 10;
	int node_cnt = t.count_node();
	BOOST_TEST(t.size() == 4);
	BOOST_TEST(t.count_node() == node_cnt);
	t.clear();
	BOOST_TEST(t.size() == 0);
	BOOST_TEST(t.count_node() == 0);
	BOOST_TEST(t[s] == 0);
	BOOST_TEST(t.size() == 1);
	BOOST_TEST(t.count_node() == 3);
}

void erase_iterator()
{
	boost::tries::trie_multimap<char, int> t;
	std::string s = "aaa", s1 = "aaaa", s2 = "aab", s3 = "bbb";
	t[s] = 1;
	t[s1] = 2;
	t[s2] = 3;
	t[s3] = 4;
	int node_cnt = t.count_node();
	BOOST_TEST(t.size() == 4);
	BOOST_TEST(t.count_node() == node_cnt);
	boost::tries::trie_multimap<char, int>::iterator ti;
	ti = t.begin();
	t.erase(t.begin());
	ti = t.begin();
	BOOST_TEST(t.size() == 3);
	BOOST_TEST(t.count_node() == 8);
	BOOST_TEST((*ti).second == 2);
	t.erase(ti);
	BOOST_TEST(t.size() == 2);
	ti = t.begin();
	BOOST_TEST((*ti).second == 3);
	BOOST_TEST(t.count_node() == 6);
	BOOST_TEST(t[s] == 0);
	ti = t.begin();
	BOOST_TEST(t.size() == 3);
	BOOST_TEST((*ti).second == 0);
	BOOST_TEST(t.count_node() == 7);
}

void erase_key()
{
	boost::tries::trie_multimap<char, int> t;
	std::string s = "aaa", s1 = "aaaa", s2 = "aab", s3 = "bbb";
	t[s] = 1;
	t[s1] = 2;
	t[s2] = 3;
	t[s3] = 4;
	int node_cnt = t.count_node();
	BOOST_TEST(t.size() == 4);
	BOOST_TEST(t.count_node() == node_cnt);
	boost::tries::trie_multimap<char, int>::iterator ti;
	ti = t.begin();
	t.erase(s);
	ti = t.begin();
	BOOST_TEST(t.size() == 3);
	BOOST_TEST(t.count_node() == 8);
	BOOST_TEST((*ti).second == 2);
	t.erase(s1);
	BOOST_TEST(t.size() == 2);
	ti = t.begin();
	BOOST_TEST((*ti).second == 3);
	BOOST_TEST(t.count_node() == 6);
	BOOST_TEST(t[s] == 0);
	ti = t.begin();
	BOOST_TEST(t.size() == 3);
	BOOST_TEST((*ti).second == 0);
	BOOST_TEST(t.count_node() == 7);
	t.erase(std::string("bababa"));
	ti = t.begin();
	BOOST_TEST(t.size() == 3);
	BOOST_TEST((*ti).second == 0);
	BOOST_TEST(t.count_node() == 7);
}

void find_prefix()
{
	tci t;
	std::string s = "aaa", s1 = "aaaa", s2 = "aab", s3 = "bbb";
	t[s] = 1;
	t[s1] = 2;
	t[s2] = 3;
	t[s3] = 4;
	tci::iterator_range r = t.find_prefix(std::string("a"));
	// the statement is tested when I know r.second != t.end()
	BOOST_TEST((*r.second).second == 4);
	int j = 1;
	for (tci::iterator i = r.first; i != r.second; ++i)
	{
		BOOST_TEST((*i).second == j);
		++j;
	}
	r = t.find_prefix(std::string("aa"));
	BOOST_TEST((*r.second).second == 4);
	j = 1;
	for (tci::iterator i = r.first; i != r.second; ++i)
	{
		BOOST_TEST((*i).second == j);
		++j;
	}
	r = t.find_prefix(std::string("aaa"));
	BOOST_TEST((*r.second).second == 3);
	j = 1;
	for (tci::iterator i = r.first; i != r.second; ++i)
	{
		BOOST_TEST((*i).second == j);
		++j;
	}
	r = t.find_prefix(std::string("b"));
	BOOST_TEST(r.second == t.end());
	for (tci::iterator i = r.first; i != r.second; ++i)
	{
		BOOST_TEST((*i).second == 4);
	}
	r = t.find_prefix(std::string("bbbbb"));
	BOOST_TEST(r.second == t.end());
	for (tci::iterator i = r.first; i != r.second; ++i)
	{
		BOOST_TEST((*i).second == 1);
	}
}

void get_key_test()
{
	tci t;
	std::string s = "aaa", s1 = "aaaa", s2 = "aab", s3 = "bbb";
	t.insert(s, 1);
	t.insert(s1, 2);
	t.insert(s2, 3);
	t.insert(s3, 4);
	tci t2;
	tci::iterator i = t.begin();
	t2[i.get_key()] = 1;
	++i;
	t2[i.get_key()] = 2;
	++i;
	t2[i.get_key()] = 3;
	++i;
	t2[i.get_key()] = 4;
	tci::iterator j = t2.begin();
	for (i = t.begin(); i != t.end(); ++i, ++j)
	{
		BOOST_TEST((*i).second == *j);
		BOOST_TEST(i.get_key() == j.get_key());
	}
	j = t2.erase(s2);
	BOOST_TEST((*j).second == 4);
}
*/

int main() {
	operator_test();
	insert_and_find_test();
	/*
	copy_test();
	iterator_operator_plus();
	iterator_operator_minus();
	clear();
	erase_iterator();
	erase_key();
	find_prefix();
	get_key_test();
	*/
	return boost::report_errors();
}
