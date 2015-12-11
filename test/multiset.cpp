#include <boost/core/lightweight_test.hpp>
#include "boost/trie/trie_multiset.hpp"
// multi include test
#include "boost/trie/trie_multiset.hpp"
#include "boost/trie/trie.hpp"
#include <string>

typedef boost::tries::trie_multiset<char> tmsi;
typedef tmsi::iterator ti;
typedef tmsi::reverse_iterator rti;

void insert_find_test()
{
	tmsi a;
	std::string s = "aaa", s2 = "bbb";

	BOOST_TEST(a.size() == 0);
	BOOST_TEST(a.find(s) == a.end());
	BOOST_TEST(a.find(s2) == a.end());
	BOOST_TEST(a.count(s) == 0);
	BOOST_TEST(a.count(s2) == 0);

	a.insert(s);
	BOOST_TEST(a.find(s) != a.end());
	BOOST_TEST(a.size() == 1);
	BOOST_TEST(a.count(s) == 1);
	a.insert(s);
	BOOST_TEST(a.size() == 2);
	BOOST_TEST(a.count(s) == 2);
	a.insert(s);
	BOOST_TEST(a.size() == 3);
	BOOST_TEST(a.count(s) == 3);

	a.insert(s2);
	BOOST_TEST(a.size() == 4);
	BOOST_TEST(a.count(s2) == 1);
	BOOST_TEST(a.find(s2) != a.end());
	a.insert(s2);
	BOOST_TEST(a.size() == 5);
	BOOST_TEST(a.count(s2) == 2);
	a.insert(s2);
	BOOST_TEST(a.size() == 6);
	BOOST_TEST(a.count(s2) == 3);
	a.insert(s2);
	BOOST_TEST(a.size() == 7);
	BOOST_TEST(a.count(s2) == 4);
}

void erase_test()
{
	tmsi a;
	ti i;
	std::string s = "aaa", s2 = "bbb";

	BOOST_TEST(a.size() == 0);
	BOOST_TEST(a.find(s) == a.end());
	BOOST_TEST(a.find(s2) == a.end());
	BOOST_TEST(a.count(s) == 0);
	BOOST_TEST(a.count(s2) == 0);

	i = a.insert(s);
	a.erase(i);
	BOOST_TEST(a.size() == 0);
	BOOST_TEST(a.find(s) == a.end());
	BOOST_TEST(a.count(s) == 0);

	i = a.insert(s);
	i = a.insert(s);
	a.erase(i);
	BOOST_TEST(a.size() == 1);
	BOOST_TEST(a.find(s) != a.end());
	BOOST_TEST(a.count(s) == 1);
	i = a.insert(s);
	BOOST_TEST(a.size() == 2);
	BOOST_TEST(a.find(s) != a.end());
	BOOST_TEST(a.count(s) == 2);

	i = a.insert(s);
	i = a.insert(s);
	a.erase(s);
	BOOST_TEST(a.size() == 0);
	BOOST_TEST(a.find(s) == a.end());
	BOOST_TEST(a.count(s) == 0);

	BOOST_TEST(a.count_node() == 0);
	i = a.insert(s2);
	BOOST_TEST(a.count_node() == 3);
	i = a.insert(s);
	BOOST_TEST(a.count_node() == 6);

	BOOST_TEST(a.size() == 2);
	BOOST_TEST(a.find(s) != a.end());
	BOOST_TEST(a.count(s) == 1);

	i = a.insert(s);
	BOOST_TEST(a.size() == 3);
	BOOST_TEST(a.count(s) == 2);

	i = a.insert(s2);
	BOOST_TEST(a.size() == 4);
	BOOST_TEST(a.count(s2) == 2);

	i = a.erase(i);
	BOOST_TEST(a.size() == 3);
	BOOST_TEST(a.count(s2) == 1);

	a.erase(i);
	BOOST_TEST(a.size() == 2);
	BOOST_TEST(a.count(s2) == 0);
	
	a.insert(s2);
	a.insert(s);
	i = a.begin();
	size_t size = 3;
	while (i != a.end())
	{
		i = a.erase(i);
		BOOST_TEST(a.size() == size);
		--size;
	}
}

void equal_range_test()
{
	tmsi a;
	ti i;
	std::string s = "aaa", s2 = "bbb";


	i = a.insert(s);
	a.erase(i);
	std::pair< ti, ti > pii = a.equal_range(s);
	size_t count = 0;
	ti first = pii.first, last = pii.second;
	for (; first != last; ++first)
	{
		++count;
	}
	BOOST_TEST(count == 0);
	i = a.insert(s);
	pii = a.equal_range(s);
	count = 0;
	first = pii.first, last = pii.second;
	for (; first != last; ++first)
	{
		++count;
	}
	BOOST_TEST(a.count(s) == 1);
	BOOST_TEST(count == 1);
	i = a.insert(s);
	pii = a.equal_range(s);
	count = 0;
	first = pii.first, last = pii.second;
	for (; first != last; ++first)
	{
		++count;
	}
	BOOST_TEST(a.count(s) == 2);
	BOOST_TEST(count == 2);
}

void reverse_iterator_test()
{
	tmsi a;
	std::string s = "aaa", s2 = "bbb";

	a.insert(s);
	a.insert(s2);
	//a.insert(s);

	size_t count = 0;
	rti ri = a.rbegin();
	for (; ri != a.rend(); ++ri)
	{
		++count;
	}
	BOOST_TEST(count == a.size());

	ti i = a.begin();
	rti j(i); // convert iterator to reverse_iterator
	//a.erase(a.rbegin()); // compile error, which is the same as std::set
	a.erase(a.begin()); 
	BOOST_TEST(a.size() == 1);

	a.clear();

	a.insert(s);
	a.insert(s2);
	a.insert(s);

	count = 0;
	i = a.begin();
	for (; i != a.end(); ++i)
	{
		++count;
	}
	BOOST_TEST(count == a.size());

	count = 0;
	ri = a.rbegin();
	for (; ri != a.rend(); ++ri)
	{
		std::vector<char> vs((*ri).first);
		++count;
	}
	BOOST_TEST(count == a.size());
}
/*
void insert_and_find_test()
{
	boost::tries::trie_map<char, int> t;
	std::string s = "aaa", s1 = "aaaa", s2 = "aab", s3 = "bbb";
	t[s] = 1;
	BOOST_TEST(*t.find(s) == 1);
	BOOST_TEST(t.find(s) != t.end());
	BOOST_TEST(t.find(s2) == t.end());
	BOOST_TEST(t.find(s2) == t.end());
	t[s] = 2;
	BOOST_TEST(*t.find(s) == 2);
	t[s2] = t[s];
	BOOST_TEST(t.find(s2) != t.end());
	BOOST_TEST(t.find(s1) == t.find(s3));
	BOOST_TEST(t.find(s) != t.find(s2));
	BOOST_TEST((t.insert(s1, 3).second == true));
	BOOST_TEST((t.insert(s3, 10).second == true));
	BOOST_TEST((t.insert(s2, 10)).second == false);
	BOOST_TEST((t.insert(s, 10)).second == false);
}

void copy_test()
{
	boost::tries::trie_map<char, int> t, t2;
	std::string s = "aaa", s1 = "aaaa", s2 = "aab", s3 = "bbb";
	t[s] = 1; t[s1] = 2; t[s2] = 3; t[s3] = 4;
	t2 = t;
	BOOST_TEST(t2.size() == 4);
	BOOST_TEST(t2.count_node() == 8);
	BOOST_TEST(t2[s] == 1);
	BOOST_TEST(t2[s1] == 2);
	BOOST_TEST(t2[s2] == 3);
	BOOST_TEST(t2[s3] == 4);
	boost::tries::trie_map<char, int> t3(t2);
	BOOST_TEST(t3.size() == 4);
	BOOST_TEST(t3.count_node() == 8);
	BOOST_TEST(*t3.find(s) == 1);
	BOOST_TEST(*t3.find(s1) == 2);
	BOOST_TEST(*t3.find(s2) == 3);
	BOOST_TEST(*t3.find(s3) == 4);
	t3[std::string("a")] = 10;
	BOOST_TEST(t3.size() == 5);
	BOOST_TEST(t3.count_node() == 8);
	BOOST_TEST(*t3.begin() == 10);
}

void iterator_operator_plus()
{
	boost::tries::trie_map<char, int> t;
	std::string s = "aaa", s1 = "aaaa", s2 = "aab", s3 = "bbb";
	BOOST_TEST(t.empty() == true);
	BOOST_TEST(t.size() == 0);
	BOOST_TEST(t.begin() == t.end());
	t[s] = 1;
	t[s1] = 2;
	t[s2] = 3;
	BOOST_TEST(t.begin() != t.end());
	boost::tries::trie_map<char, int>::iterator ti;
	ti = t.begin();
	BOOST_TEST(*ti == 1);
	++ti;
	BOOST_TEST(*ti == 2);
	BOOST_TEST(t[s2] == 3);
	++ti;
	BOOST_TEST(*ti == 3);
	++ti;
	BOOST_TEST(ti == t.end());
	// test ++end()
	++ti;
	BOOST_TEST(ti == t.end());
}

void iterator_operator_minus()
{
	boost::tries::trie_map<char, int> t;
	std::string s = "aaa", s1 = "aaaa", s2 = "aab", s3 = "bbb";
	BOOST_TEST(t.empty() == true);
	BOOST_TEST(t.size() == 0);
	BOOST_TEST(t.begin() == t.end());
	t[s] = 1;
	t[s1] = 2;
	t[s2] = 3;
	BOOST_TEST(t.begin() != t.end());
	boost::tries::trie_map<char, int>::iterator ti;
	boost::tries::trie_map<char, int>::const_iterator cti(t.end());
	ti = t.begin();
	BOOST_TEST(*ti == 1);
	*ti = 10;
	BOOST_TEST(*ti == 10);
	cti = t.begin();
	BOOST_TEST(*cti == 10);
	--ti;
	BOOST_TEST(ti == t.end());
	BOOST_TEST(t[s2] == 3);
	--ti;
	BOOST_TEST(*ti == 3);
	t[s3] = 4;
	++ti;
	BOOST_TEST(*ti == 4);
	++ti;
	BOOST_TEST(ti == t.end());
	++ti;
	BOOST_TEST(ti == t.end());
	--ti;
	BOOST_TEST(*ti == 4);
	BOOST_TEST(t.count_node() == 8);
	BOOST_TEST(t.size() == 4);
}

void clear()
{
	boost::tries::trie_map<char, int> t;
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
	boost::tries::trie_map<char, int> t;
	std::string s = "aaa", s1 = "aaaa", s2 = "aab", s3 = "bbb";
	t[s] = 1;
	t[s1] = 2;
	t[s2] = 3;
	t[s3] = 4;
	int node_cnt = t.count_node();
	BOOST_TEST(t.size() == 4);
	BOOST_TEST(t.count_node() == node_cnt);
	boost::tries::trie_map<char, int>::iterator ti;
	ti = t.begin();
	t.erase(t.begin());
	ti = t.begin();
	BOOST_TEST(t.size() == 3);
	BOOST_TEST(t.count_node() == 8);
	BOOST_TEST(*ti == 2);
	t.erase(ti);
	BOOST_TEST(t.size() == 2);
	ti = t.begin();
	BOOST_TEST(*ti == 3);
	BOOST_TEST(t.count_node() == 6);
	BOOST_TEST(t[s] == 0);
	ti = t.begin();
	BOOST_TEST(t.size() == 3);
	BOOST_TEST(*ti == 0);
	BOOST_TEST(t.count_node() == 7);
}

void erase_key()
{
	boost::tries::trie_map<char, int> t;
	std::string s = "aaa", s1 = "aaaa", s2 = "aab", s3 = "bbb";
	t[s] = 1;
	t[s1] = 2;
	t[s2] = 3;
	t[s3] = 4;
	int node_cnt = t.count_node();
	BOOST_TEST(t.size() == 4);
	BOOST_TEST(t.count_node() == node_cnt);
	boost::tries::trie_map<char, int>::iterator ti;
	ti = t.begin();
	t.erase(s);
	ti = t.begin();
	BOOST_TEST(t.size() == 3);
	BOOST_TEST(t.count_node() == 8);
	BOOST_TEST(*ti == 2);
	t.erase(s1);
	BOOST_TEST(t.size() == 2);
	ti = t.begin();
	BOOST_TEST(*ti == 3);
	BOOST_TEST(t.count_node() == 6);
	BOOST_TEST(t[s] == 0);
	ti = t.begin();
	BOOST_TEST(t.size() == 3);
	BOOST_TEST(*ti == 0);
	BOOST_TEST(t.count_node() == 7);
	t.erase(std::string("bababa"));
	ti = t.begin();
	BOOST_TEST(t.size() == 3);
	BOOST_TEST(*ti == 0);
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
	BOOST_TEST(*r.second == 4);
	int j = 1;
	for (tci::iterator i = r.first; i != r.second; ++i)
	{
		BOOST_TEST(*i == j);
		++j;
	}
	r = t.find_prefix(std::string("aa"));
	BOOST_TEST(*r.second == 4);
	j = 1;
	for (tci::iterator i = r.first; i != r.second; ++i)
	{
		BOOST_TEST(*i == j);
		++j;
	}
	r = t.find_prefix(std::string("aaa"));
	BOOST_TEST(*r.second == 3);
	j = 1;
	for (tci::iterator i = r.first; i != r.second; ++i)
	{
		BOOST_TEST(*i == j);
		++j;
	}
	r = t.find_prefix(std::string("b"));
	BOOST_TEST(r.second == t.end());
	for (tci::iterator i = r.first; i != r.second; ++i)
	{
		BOOST_TEST(*i == 4);
	}
	r = t.find_prefix(std::string("bbbbb"));
	BOOST_TEST(r.second == t.end());
	for (tci::iterator i = r.first; i != r.second; ++i)
	{
		BOOST_TEST(*i == 1);
	}
}

void get_key_test()
{
	tci t;
	std::string s = "aaa", s1 = "aaaa", s2 = "aab", s3 = "bbb";
	t[s] = 1;
	t[s1] = 2;
	t[s2] = 3;
	t[s3] = 4;
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
		BOOST_TEST(*i == *j);
		BOOST_TEST(i.get_key() == j.get_key());
	}
	j = t2.erase(s2);
	BOOST_TEST(*j == 4);
}
*/

int main() {
	insert_find_test();
	erase_test();
	equal_range_test();
	reverse_iterator_test();
	/*
	insert_and_find_test();
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
