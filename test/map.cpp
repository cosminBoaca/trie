#define BOOST_TEST_MODULE trie_test
#include <boost/test/unit_test.hpp>
#include "boost/trie/trie_map.hpp"
//multi include test
#include "boost/trie/trie_map.hpp"
#include "boost/trie/trie.hpp"

#include <string>

BOOST_AUTO_TEST_SUITE(trie_test)

typedef boost::tries::trie_map<char, int> tmci;
typedef tmci::iterator ti;
typedef tmci::const_iterator tci;
typedef tmci::reverse_iterator tri;
typedef tmci::const_reverse_iterator tcri;

BOOST_AUTO_TEST_CASE(operator_test)
{
	boost::tries::trie_map<char, int> t;
	std::string s = "aaa", s2 = "bbb";
	t[s] = 1;
	BOOST_CHECK(t[s] == 1);
	t[s] = 2;
	BOOST_CHECK(t[s] == 2);
	t[s2] = t[s];
	BOOST_CHECK(t[s] == t[s2]);
}

BOOST_AUTO_TEST_CASE(insert_and_find_test)
{
	boost::tries::trie_map<char, int> t;
	std::string s = "aaa", s1 = "aaaa", s2 = "aab", s3 = "bbb";
	t[s] = 1;
	BOOST_CHECK((*t.find(s)).second == 1);
	BOOST_CHECK(t.find(s) != t.end());
	BOOST_CHECK(t.find(s2) == t.end());
	BOOST_CHECK(t.find(s2) == t.end());
	t[s] = 2;
	BOOST_CHECK((*t.find(s)).second == 2);
	t[s2] = t[s];
	BOOST_CHECK(t.find(s2) != t.end());
	BOOST_CHECK(t.find(s1) == t.find(s3));
	BOOST_CHECK(t.find(s) != t.find(s2));
	BOOST_CHECK((t.insert(s1, 3).second == true));
	BOOST_CHECK((t.insert(s3, 10).second == true));
	BOOST_CHECK((t.insert(s2, 10)).second == false);
	BOOST_CHECK((t.insert(s, 10)).second == false);
}

BOOST_AUTO_TEST_CASE(copy_test)
{
	boost::tries::trie_map<char, int> t, t2;
	std::string s = "aaa", s1 = "aaaa", s2 = "aab", s3 = "bbb";
	t[s] = 1; t[s1] = 2; t[s2] = 3; t[s3] = 4;
	t2 = t;
	BOOST_CHECK(t2.size() == 4);
	BOOST_CHECK(t2.count_node() == 8);
	BOOST_CHECK(t2[s] == 1);
	BOOST_CHECK(t2[s1] == 2);
	BOOST_CHECK(t2[s2] == 3);
	BOOST_CHECK(t2[s3] == 4);
	boost::tries::trie_map<char, int> t3(t2);
	BOOST_CHECK(t3.size() == 4);
	BOOST_CHECK_MESSAGE(t3.count_node() == 8, t3.count_node());
	BOOST_CHECK((*t3.find(s)).second == 1);
	BOOST_CHECK((*t3.find(s1)).second == 2);
	BOOST_CHECK((*t3.find(s2)).second == 3);
	BOOST_CHECK((*t3.find(s3)).second == 4);
	t3[std::string("a")] = 10;
	BOOST_CHECK(t3.size() == 5);
	BOOST_CHECK_MESSAGE(t3.count_node() == 8, t3.count_node());
	BOOST_CHECK((*t3.begin()).second == 10);
}

BOOST_AUTO_TEST_CASE(iterator_operator_plus)
{
	boost::tries::trie_map<char, int> t;
	std::string s = "aaa", s1 = "aaaa", s2 = "aab", s3 = "bbb";
	BOOST_CHECK(t.empty() == true);
	BOOST_CHECK(t.size() == 0);
	BOOST_CHECK(t.begin() == t.end());
	t[s] = 1;
	t[s1] = 2;
	t[s2] = 3;
	BOOST_CHECK(t.begin() != t.end());
	boost::tries::trie_map<char, int>::iterator i;
	i = t.begin();
	BOOST_CHECK((*i).second == 1);
	++i;
	BOOST_CHECK((*i).second == 2);
	BOOST_CHECK(t[s2] == 3);
	++i;
	BOOST_CHECK((*i).second == 3);
	++i;
	BOOST_CHECK(i == t.end());
	++i;
	BOOST_CHECK(i == t.end());
}

BOOST_AUTO_TEST_CASE(iterator_operator_minus)
{
	boost::tries::trie_map<char, int> t;
	std::string s = "aaa", s1 = "aaaa", s2 = "aab", s3 = "bbb";
	boost::tries::trie_map<char, int>::iterator i;
	boost::tries::trie_map<char, int>::const_iterator ci(t.end());
	BOOST_CHECK(t.empty() == true);
	BOOST_CHECK(t.size() == 0);
	i = t.begin();
	BOOST_CHECK(i == t.end());
	++i;
	BOOST_CHECK(i == t.end());
	++i;
	BOOST_CHECK(i == t.end());
	--i;
	BOOST_CHECK(i == t.end());
	--i;
	BOOST_CHECK(i == t.end());
	ci = t.begin();
	BOOST_CHECK(ci == t.end());
	++ci;
	BOOST_CHECK(ci == t.end());
	++ci;
	BOOST_CHECK(ci == t.end());
	--ci;
	BOOST_CHECK(ci == t.end());
	--ci;
	BOOST_CHECK(ci == t.end());
	t[s] = 1;
	t[s1] = 2;
	t[s2] = 3;
	BOOST_CHECK(t.begin() != t.end());
	i = t.begin();
	BOOST_CHECK((*i).second == 1);
	(*i).second= 10;
	BOOST_CHECK((*i).second == 10);
	ci = t.begin();
	BOOST_CHECK((*ci).second == 10);
	--i;
	BOOST_CHECK(i == t.begin());
	BOOST_CHECK(t[s2] == 3);
	i = t.end();
	--i;
	BOOST_CHECK((*i).second == 3);
	t[s3] = 4;
	++i;
	BOOST_CHECK((*i).second == 4);
	++i;
	BOOST_CHECK(i == t.end());
	++i;
	BOOST_CHECK(i == t.end());
	--i;
	BOOST_CHECK((*i).second == 4);
	BOOST_CHECK(t.count_node() == 8);
	BOOST_CHECK(t.size() == 4);
}

BOOST_AUTO_TEST_CASE(reverse_iterator_operator_plus)
{
	boost::tries::trie_map<char, int> t;
	std::string s = "aaa", s1 = "aaaa", s2 = "aab", s3 = "bbb";
	BOOST_CHECK(t.empty() == true);
	BOOST_CHECK(t.size() == 0);
	BOOST_CHECK(t.begin() == t.end());
	t[s] = 1;
	t[s1] = 2;
	t[s2] = 3;
	BOOST_CHECK(t.begin() != t.end());
	tcri i;
	i = t.rbegin();
	BOOST_CHECK((*i).second == 3);
	--i;
	BOOST_CHECK((*i).second == 3);
	++i;
	BOOST_CHECK((*i).second == 2);
	BOOST_CHECK(t[s2] == 3);
	++i;
	BOOST_CHECK((*i).second == 1);
	++i;
	BOOST_CHECK(i == t.rend());
	++i;
	BOOST_CHECK(i == t.rend());
}

BOOST_AUTO_TEST_CASE(reverse_iterator_operator_minus)
{
	boost::tries::trie_map<char, int> t;
	std::string s = "aaa", s1 = "aaaa", s2 = "aab", s3 = "bbb";
	tri i;
	tcri ci;
	BOOST_CHECK(t.empty() == true);
	BOOST_CHECK(t.size() == 0);
	i = t.rbegin();
	BOOST_CHECK(i == t.rend());
	++i;
	BOOST_CHECK(i == t.rend());
	++i;
	BOOST_CHECK(i == t.rend());
	--i;
	BOOST_CHECK(i == t.rend());
	--i;
	BOOST_CHECK(i == t.rend());
	ci = t.rbegin();
	BOOST_CHECK(ci == t.rend());
	++ci;
	BOOST_CHECK(ci == t.rend());
	++ci;
	BOOST_CHECK(ci == t.rend());
	--ci;
	BOOST_CHECK(ci == t.rend());
	--ci;
	BOOST_CHECK(ci == t.rend());
	t[s1] = 2;
	t[s2] = 3;
	BOOST_CHECK(t.rbegin() != t.rend());
	i = t.rbegin();
	BOOST_CHECK((*i).second == 3);
	(*i).second= 10;
	BOOST_CHECK((*i).second == 10);
	ci = i;
	BOOST_CHECK((*ci).second == 10);
	--ci;
	BOOST_CHECK(ci == t.rbegin());
	t[s3] = 4;
	--ci;
	BOOST_CHECK(ci == t.rbegin());
	i = t.rend();
	BOOST_CHECK(i == t.rend());
	ci = --i;
	BOOST_CHECK((*ci).second == 2);
	--ci;
	BOOST_CHECK_MESSAGE((*ci).second == 10, (*ci).second);
	--ci;
	BOOST_CHECK((*ci).second == 4);
	--ci;
	BOOST_CHECK((*ci).second == 4);
	++ci;
	BOOST_CHECK((*ci).second == 10);
	++ci;
	BOOST_CHECK((*ci).second == 2);
	++ci;
	BOOST_CHECK(ci == t.rend());
	++ci;
	BOOST_CHECK(ci == t.rend());
	--ci;
	BOOST_CHECK((*ci).second == 2);
	++ci;
	BOOST_CHECK(ci == t.rend());
	t[s] = 1;
	BOOST_CHECK(ci != t.rend());
	BOOST_CHECK((*ci).second == 1);
	++ci;
	BOOST_CHECK(ci == t.rend());
	BOOST_CHECK((*ci).second == 1);
	++ci;
	BOOST_CHECK(ci == t.rend());
	BOOST_CHECK(t.count_node() == 8);
	BOOST_CHECK(t.size() == 4);
}

BOOST_AUTO_TEST_CASE(clear)
{
	boost::tries::trie_map<char, int> t;
	std::string s = "aaa", s1 = "aaaa", s2 = "aab", s3 = "bbb";
	t[s] = t[s1] = t[s2] = t[s3] = 10;
	size_t node_cnt = t.count_node();
	BOOST_CHECK(t.size() == 4);
	BOOST_CHECK(t.count_node() == node_cnt);
	t.clear();
	BOOST_CHECK_MESSAGE(t.size() == 0, t.size());
	BOOST_CHECK(t.count_node() == 0);
	BOOST_CHECK(t[s] == 0);
	BOOST_CHECK_MESSAGE(t.size() == 1, t.size());
	BOOST_CHECK(t.count_node() == 3);
}

BOOST_AUTO_TEST_CASE(erase_iterator)
{
	boost::tries::trie_map<char, int> t;
	std::string s = "aaa", s1 = "aaaa", s2 = "aab", s3 = "bbb";
	t[s] = 1;
	t[s1] = 2;
	t[s2] = 3;
	t[s3] = 4;
	size_t node_cnt = t.count_node();
	BOOST_CHECK(t.size() == 4);
	BOOST_CHECK(t.count_node() == node_cnt);
	boost::tries::trie_map<char, int>::iterator i;
	i = t.begin();
	t.erase(t.begin());
	i = t.begin();
	BOOST_CHECK(t.size() == 3);
	BOOST_CHECK(t.count_node() == 8);
	BOOST_CHECK((*i).second == 2);
	t.erase(i);
	BOOST_CHECK(t.size() == 2);
	i = t.begin();
	BOOST_CHECK((*i).second == 3);
	BOOST_CHECK(t.count_node() == 6);
	BOOST_CHECK(t[s] == 0);
	i = t.begin();
	BOOST_CHECK(t.size() == 3);
	BOOST_CHECK((*i).second == 0);
	BOOST_CHECK(t.count_node() == 7);
}

BOOST_AUTO_TEST_CASE(erase_key)
{
	boost::tries::trie_map<char, int> t;
	std::string s = "aaa", s1 = "aaaa", s2 = "aab", s3 = "bbb";
	t[s] = 1;
	t[s1] = 2;
	t[s2] = 3;
	t[s3] = 4;
	size_t node_cnt = t.count_node();
	BOOST_CHECK(t.size() == 4);
	BOOST_CHECK(t.count_node() == node_cnt);
	boost::tries::trie_map<char, int>::iterator i;
	i = t.begin();
	t.erase(s);
	i = t.begin();
	BOOST_CHECK(t.size() == 3);
	BOOST_CHECK(t.count_node() == 8);
	BOOST_CHECK((*i).second == 2);
	t.erase(s1);
	BOOST_CHECK(t.size() == 2);
	i = t.begin();
	BOOST_CHECK((*i).second == 3);
	BOOST_CHECK(t.count_node() == 6);
	BOOST_CHECK(t[s] == 0);
	i = t.begin();
	BOOST_CHECK(t.size() == 3);
	BOOST_CHECK((*i).second == 0);
	BOOST_CHECK(t.count_node() == 7);
	t.erase(std::string("bababa"));
	i = t.begin();
	BOOST_CHECK(t.size() == 3);
	BOOST_CHECK((*i).second == 0);
	BOOST_CHECK(t.count_node() == 7);
}

BOOST_AUTO_TEST_CASE(find_prefix)
{
	tmci t;
	std::string s = "aaa", s1 = "aaaa", s2 = "aab", s3 = "bbb";
	t[s] = 1;
	t[s1] = 2;
	t[s2] = 3;
	t[s3] = 4;
	tmci::iterator_range r = t.find_prefix(std::string("a"));
	BOOST_CHECK((*r.second).second == 4);
	int j = 1;
	for (tmci::iterator i = r.first; i != r.second; ++i)
	{
		BOOST_CHECK((*i).second == j);
		++j;
	}
	r = t.find_prefix(std::string("aa"));
	BOOST_CHECK((*r.second).second == 4);
	j = 1;
	for (tmci::iterator i = r.first; i != r.second; ++i)
	{
		BOOST_CHECK((*i).second == j);
		++j;
	}
	r = t.find_prefix(std::string("aaa"));
	BOOST_CHECK((*r.second).second == 3);
	j = 1;
	for (tmci::iterator i = r.first; i != r.second; ++i)
	{
		BOOST_CHECK((*i).second == j);
		++j;
	}
	r = t.find_prefix(std::string("b"));
	BOOST_CHECK(r.second == t.end());
	for (tmci::iterator i = r.first; i != r.second; ++i)
	{
		BOOST_CHECK((*i).second == 4);
	}
	r = t.find_prefix(std::string("bbbbb"));
	BOOST_CHECK(r.second == t.end());
	for (tmci::iterator i = r.first; i != r.second; ++i)
	{
		BOOST_CHECK((*i).second == 1);
	}
}

BOOST_AUTO_TEST_CASE(get_key_test)
{
	tmci t;
	std::string s = "aaa", s1 = "aaaa", s2 = "aab", s3 = "bbb";
	t[s] = 1;
	t[s1] = 2;
	t[s2] = 3;
	t[s3] = 4;
	tmci t2;
	tmci::iterator i = t.begin();
	t2[i.get_key()] = 1;
	++i;
	t2[i.get_key()] = 2;
	++i;
	t2[i.get_key()] = 3;
	++i;
	t2[i.get_key()] = 4;
	tmci::iterator j = t2.begin();
	for (i = t.begin(); i != t.end(); ++i, ++j)
	{
		BOOST_CHECK(*i == *j);
		BOOST_CHECK(i.get_key() == j.get_key());
	}
	j = t2.erase(s2);
	BOOST_CHECK((*j).second == 4);
}

BOOST_AUTO_TEST_CASE(get_key_reverse_test)
{
	tmci t;
	std::string s = "aaa", s1 = "aaaa", s2 = "aab", s3 = "bbb";
	t[s] = 1;
	t[s1] = 2;
	t[s2] = 3;
	t[s3] = 4;
	tcri i = t.rbegin();
	tmci t2;
	t2[(*i).first] = 4;
	++i;
	t2[(*i).first] = 3;
	++i;
	t2[(*i).first] = 2;
	++i;
	t2[(*i).first] = 1;
	tcri j = t2.rbegin();
	for (i = t.rbegin(); i != t.rend(); ++i, ++j)
	{
		BOOST_CHECK(*i == *j);
		BOOST_CHECK((*i).first == (*j).first);
	}
}
BOOST_AUTO_TEST_SUITE_END()
