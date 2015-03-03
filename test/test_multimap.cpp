#define BOOST_TEST_MODULE trie_test
#include <boost/test/unit_test.hpp>
#include "boost/trie/trie_multimap.hpp"
// multi include test
#include "boost/trie/trie_multimap.hpp" 
#include "boost/trie/trie.hpp"

#include <string>
#include <iostream>

BOOST_AUTO_TEST_SUITE(trie_test)

typedef boost::tries::trie_multimap<char, int> tci;
typedef tci::iterator iter_type;

BOOST_AUTO_TEST_CASE(operator_test)
{
	boost::tries::trie_multimap<char, int> t;
	std::string s = "aaa", s2 = "bbb";
	t.insert(s, 1);
	t.insert(s, 2);
	BOOST_CHECK_MESSAGE(t.count(s) == 2, t.count(s));
}

BOOST_AUTO_TEST_CASE(insert_and_find_test)
{
	boost::tries::trie_multimap<char, int> t;
	std::string s = "aaa", s1 = "aaaa", s2 = "aab", s3 = "bbb";
	t.insert(s, 1);
	BOOST_CHECK((*t.find(s)).second == 1);
	BOOST_CHECK(t.find(s) != t.end());
	BOOST_CHECK(t.find(s2) == t.end());
	BOOST_CHECK(t.find(s2) == t.end());
	t.insert(s, 2);
	BOOST_CHECK((*t.find(s)).second == 2);
	t.insert(s2, (*t.find(s)).second);
	BOOST_CHECK(t.find(s2) != t.end());
	BOOST_CHECK(t.find(s1) == t.find(s3));
	BOOST_CHECK(t.find(s) != t.find(s2));
	BOOST_CHECK((*t.find(s)).second == (*t.find(s2)).second);
	iter_type i = t.insert(s2, 1);
	BOOST_CHECK_MESSAGE((*i).second == 1, (*i).second);
	++i;
	BOOST_CHECK_MESSAGE((*i).second == 2, (*i).second);
	BOOST_CHECK_MESSAGE(t.count_prefix(std::string("a")) == 4, 
			t.count_prefix(std::string("a")));
	BOOST_CHECK_MESSAGE(t.count_prefix(std::string("aa")) == 4, 
			t.count_prefix(std::string("aa")));
	BOOST_CHECK_MESSAGE(t.count_prefix(std::string("aaa")) == 2, 
			t.count_prefix(std::string("aaa")));
	BOOST_CHECK_MESSAGE(t.count_prefix(std::string("aab")) == 2, 
			t.count_prefix(std::string("aab")));
}

/*
BOOST_AUTO_TEST_CASE(copy_test)
{
	boost::tries::trie_multimap<char, int> t, t2;
	std::string s = "aaa", s1 = "aaaa", s2 = "aab", s3 = "bbb";
	t[s] = 1; t[s1] = 2; t[s2] = 3; t[s3] = 4;
	t2 = t;
	BOOST_CHECK(t2.size() == 4);
	BOOST_CHECK(t2.count_node() == 8);
	BOOST_CHECK(t2[s] == 1);
	BOOST_CHECK(t2[s1] == 2);
	BOOST_CHECK(t2[s2] == 3);
	BOOST_CHECK(t2[s3] == 4);
	boost::tries::trie_multimap<char, int> t3(t2);
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
	boost::tries::trie_multimap<char, int> t;
	std::string s = "aaa", s1 = "aaaa", s2 = "aab", s3 = "bbb";
	BOOST_CHECK(t.empty() == true);
	BOOST_CHECK(t.size() == 0);
	BOOST_CHECK(t.begin() == t.end());
	t[s] = 1;
	t[s1] = 2;
	t[s2] = 3;
	BOOST_CHECK(t.begin() != t.end());
	boost::tries::trie_multimap<char, int>::iterator ti;
	std::cout << "dfd" << std::endl;
	ti = t.begin();
	BOOST_CHECK((*ti).second == 1);
	std::cout << "dfd" << std::endl;
	++ti;
	BOOST_CHECK((*ti).second == 2);
	std::cout << "dfd" << std::endl;
	BOOST_CHECK(t[s2] == 3);
	++ti;
	BOOST_CHECK((*ti).second == 3);
	std::cout << "dfd" << std::endl;
	++ti;
	BOOST_CHECK(ti == t.end());
	// test ++end()
	++ti;
	BOOST_CHECK(ti == t.end());
	std::cout << "dfd" << std::endl;
}

BOOST_AUTO_TEST_CASE(iterator_operator_minus)
{
	boost::tries::trie_multimap<char, int> t;
	std::string s = "aaa", s1 = "aaaa", s2 = "aab", s3 = "bbb";
	BOOST_CHECK(t.empty() == true);
	BOOST_CHECK(t.size() == 0);
	BOOST_CHECK(t.begin() == t.end());
	t[s] = 1;
	t[s1] = 2;
	t[s2] = 3;
	BOOST_CHECK(t.begin() != t.end());
	boost::tries::trie_multimap<char, int>::iterator ti;
	boost::tries::trie_multimap<char, int>::const_iterator cti(t.end());
	ti = t.begin();
	BOOST_CHECK((*ti).second == 1);
	(*ti).second= 10;
	BOOST_CHECK((*ti).second == 10);
	cti = t.begin();
	BOOST_CHECK((*cti).second == 10);
	--ti;
	BOOST_CHECK(ti == t.end());
	BOOST_CHECK(t[s2] == 3);
	--ti;
	BOOST_CHECK((*ti).second == 3);
	t[s3] = 4;
	++ti;
	BOOST_CHECK((*ti).second == 4);
	++ti;
	BOOST_CHECK(ti == t.end());
	++ti;
	BOOST_CHECK(ti == t.end());
	--ti;
	BOOST_CHECK((*ti).second == 4);
	BOOST_CHECK(t.count_node() == 8);
	BOOST_CHECK(t.size() == 4);
}

BOOST_AUTO_TEST_CASE(clear)
{
	boost::tries::trie_multimap<char, int> t;
	std::string s = "aaa", s1 = "aaaa", s2 = "aab", s3 = "bbb";
	t[s] = t[s1] = t[s2] = t[s3] = 10;
	int node_cnt = t.count_node();
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
	boost::tries::trie_multimap<char, int> t;
	std::string s = "aaa", s1 = "aaaa", s2 = "aab", s3 = "bbb";
	t[s] = 1;
	t[s1] = 2;
	t[s2] = 3;
	t[s3] = 4;
	int node_cnt = t.count_node();
	BOOST_CHECK(t.size() == 4);
	std::cout << t.size() << ' ' << t.count_node() << std::endl;
	BOOST_CHECK(t.count_node() == node_cnt);
	std::cout << (*t.begin()).second << std::endl;
	boost::tries::trie_multimap<char, int>::iterator ti;
	ti = t.begin();
	t.erase(t.begin());
	ti = t.begin();
	BOOST_CHECK(t.size() == 3);
	BOOST_CHECK(t.count_node() == 8);
	BOOST_CHECK((*ti).second == 2);
	t.erase(ti);
	BOOST_CHECK(t.size() == 2);
	ti = t.begin();
	BOOST_CHECK((*ti).second == 3);
	BOOST_CHECK(t.count_node() == 6);
	BOOST_CHECK(t[s] == 0);
	ti = t.begin();
	BOOST_CHECK(t.size() == 3);
	BOOST_CHECK((*ti).second == 0);
	BOOST_CHECK(t.count_node() == 7);
}

BOOST_AUTO_TEST_CASE(erase_key)
{
	boost::tries::trie_multimap<char, int> t;
	std::string s = "aaa", s1 = "aaaa", s2 = "aab", s3 = "bbb";
	t[s] = 1;
	t[s1] = 2;
	t[s2] = 3;
	t[s3] = 4;
	int node_cnt = t.count_node();
	BOOST_CHECK(t.size() == 4);
	std::cout << t.size() << ' ' << t.count_node() << std::endl;
	BOOST_CHECK(t.count_node() == node_cnt);
	std::cout << (*t.begin()).second << std::endl;
	boost::tries::trie_multimap<char, int>::iterator ti;
	ti = t.begin();
	t.erase(s);
	ti = t.begin();
	BOOST_CHECK(t.size() == 3);
	BOOST_CHECK(t.count_node() == 8);
	BOOST_CHECK((*ti).second == 2);
	t.erase(s1);
	BOOST_CHECK(t.size() == 2);
	ti = t.begin();
	BOOST_CHECK((*ti).second == 3);
	BOOST_CHECK(t.count_node() == 6);
	BOOST_CHECK(t[s] == 0);
	ti = t.begin();
	BOOST_CHECK(t.size() == 3);
	BOOST_CHECK((*ti).second == 0);
	BOOST_CHECK(t.count_node() == 7);
	t.erase(std::string("bababa"));
	ti = t.begin();
	BOOST_CHECK(t.size() == 3);
	BOOST_CHECK((*ti).second == 0);
	BOOST_CHECK(t.count_node() == 7);
}

BOOST_AUTO_TEST_CASE(find_prefix)
{
	tci t;
	std::string s = "aaa", s1 = "aaaa", s2 = "aab", s3 = "bbb";
	t[s] = 1;
	t[s1] = 2;
	t[s2] = 3;
	t[s3] = 4;
	tci::iterator_range r = t.find_prefix(std::string("a"));
	// the statement is tested when I know r.second != t.end()
	std::cout << (*r.first).second << " " << (*r.second).second << std::endl;
	BOOST_CHECK((*r.second).second == 4);
	int j = 1;
	for (tci::iterator i = r.first; i != r.second; ++i)
	{
		std::cout << (*i).second << std::endl;
		BOOST_CHECK((*i).second == j);
		++j;
	}
	r = t.find_prefix(std::string("aa"));
	std::cout << (*r.first).second << " " << (*r.second).second << std::endl;
	BOOST_CHECK((*r.second).second == 4);
	j = 1;
	for (tci::iterator i = r.first; i != r.second; ++i)
	{
		std::cout << (*i).second << std::endl;
		BOOST_CHECK((*i).second == j);
		++j;
	}
	r = t.find_prefix(std::string("aaa"));
	std::cout << (*r.first).second << " " << (*r.second).second << std::endl;
	BOOST_CHECK((*r.second).second == 3);
	j = 1;
	for (tci::iterator i = r.first; i != r.second; ++i)
	{
		std::cout << (*i).second << std::endl;
		BOOST_CHECK((*i).second == j);
		++j;
	}
	r = t.find_prefix(std::string("b"));
	//std::cout << (*r.first).second << " " << (*r.second).second << std::endl;
	BOOST_CHECK(r.second == t.end());
	for (tci::iterator i = r.first; i != r.second; ++i)
	{
		std::cout << (*i).second << std::endl;
		BOOST_CHECK((*i).second == 4);
	}
	r = t.find_prefix(std::string("bbbbb"));
	//std::cout << (*r.first).second << " " << (*r.second).second << std::endl;
	BOOST_CHECK(r.second == t.end());
	for (tci::iterator i = r.first; i != r.second; ++i)
	{
		std::cout << (*i).second << std::endl;
		BOOST_CHECK((*i).second == 1);
	}
}

BOOST_AUTO_TEST_CASE(get_key_test)
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
		BOOST_CHECK((*i).second == *j);
		BOOST_CHECK(i.get_key() == j.get_key());
	}
	j = t2.erase(s2);
	BOOST_CHECK((*j).second == 4);
}
*/
BOOST_AUTO_TEST_SUITE_END()
