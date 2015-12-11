#include <boost/core/lightweight_test.hpp>
#include "boost/trie/trie_set.hpp"
// multi include test
#include "boost/trie/trie_set.hpp"
#include "boost/trie/trie.hpp"

#include <string>

typedef boost::tries::trie_set<char> tsci;
typedef tsci::iterator ti;
typedef tsci::const_iterator tci;
typedef tsci::reverse_iterator tri;
typedef tsci::const_reverse_iterator tcri;

void insert_erase_test()
{
	tsci t;
	std::string s = "aaa", s2 = "bbb";
	BOOST_TEST(t.empty());
	BOOST_TEST(t.begin() == t.end());
	BOOST_TEST(t.rbegin() == t.rend());
	BOOST_TEST(t.insert(s).second == true);
	BOOST_TEST(t.insert(s).second == false);
	BOOST_TEST(t.erase(s) == t.end());
	BOOST_TEST(t.insert(s).second == true);
	BOOST_TEST(t.insert(s2).second == true);
	BOOST_TEST(t.insert(s2).second == false);
	BOOST_TEST(t.erase(s2) == t.end());
	BOOST_TEST(t.insert(s2).second == true);
}

void insert_find_test()
{
	tsci t;
	std::string s1 = "aaa", s2 = "bbb", s3="aaaa", s4 = "aaab";

	t.insert(s1);
	t.insert(s2);
	std::vector<char> v = *(t.find(s1));
	BOOST_TEST(std::string(v.begin(), v.end()) == s1);
	v = *(t.find(s2));
	BOOST_TEST(std::string(v.begin(), v.end()) == s2);
	BOOST_TEST(t.find(s1) != t.end());
	BOOST_TEST(t.find(s2) != t.end());
	t.erase(s1);
	BOOST_TEST(t.find(s1) == t.end());
	BOOST_TEST(t.find(s2) != t.end());
	t.insert(s1);
	BOOST_TEST(t.insert(s2).second == false);
	t.insert(s3);
	t.insert(s4);
	BOOST_TEST(t.find(s1) != t.end());
	BOOST_TEST(t.find(s2) != t.end());
	BOOST_TEST(t.find(s3) != t.end());
	BOOST_TEST(t.find(s4) != t.end());
	v = *(t.find(s1));
	BOOST_TEST(std::string(v.begin(), v.end()) == s1);
	v = *(t.find(s2));
	BOOST_TEST(std::string(v.begin(), v.end()) == s2);
	v = *(t.find(s3));
	BOOST_TEST(std::string(v.begin(), v.end()) == s3);
	v = *(t.find(s4));
	BOOST_TEST(std::string(v.begin(), v.end()) == s4);
	BOOST_TEST(t.count_prefix(std::string("aa")) == 3);
	t.erase(s3);
	BOOST_TEST(t.count_prefix(std::string("aa")) == 2);
	t.erase(s4);
	BOOST_TEST(t.count_prefix(std::string("aa")) == 1);
	t.erase(s1);
	BOOST_TEST(t.count_prefix(std::string("aa")) == 0);
	BOOST_TEST(t.count_prefix(std::string("bbb")) == 1);
}

void copy_test()
{
	std::string s1 = "aaa", s2 = "bbb", s3="aaaa", s4 = "aaab";
	tsci t1, t2;
	t2 = t1;
	BOOST_TEST(t1.begin() == t1.end());
	BOOST_TEST(t1.empty());
	BOOST_TEST(t2.begin() == t2.end());
	BOOST_TEST(t2.empty());

	t1.insert(s1);
	t1.insert(s2);
	t1.insert(s3);
	t1.insert(s4);

	t2 = t1;
	BOOST_TEST(t1.count_node() == t2.count_node());
	BOOST_TEST(t2.find(s1) != t2.end());
	BOOST_TEST(t2.find(s2) != t2.end());
	BOOST_TEST(t2.find(s3) != t2.end());
	BOOST_TEST(t2.find(s4) != t2.end());
	BOOST_TEST(t2.count_prefix(std::string("aa")) == 3);
	BOOST_TEST(t2.count_prefix(std::string("xyz")) == 0);
	BOOST_TEST(t2.count_prefix(std::string("b")) == 1);

	tsci t3(t1);
	BOOST_TEST(t1.count_node() == t3.count_node());
	BOOST_TEST(t3.find(s1) != t3.end());
	BOOST_TEST(t3.find(s2) != t3.end());
	BOOST_TEST(t3.find(s3) != t3.end());
	BOOST_TEST(t3.find(s4) != t3.end());
	BOOST_TEST(t3.count_prefix(std::string("aa")) == 3);
	BOOST_TEST(t3.count_prefix(std::string("xyz")) == 0);
	BOOST_TEST(t3.count_prefix(std::string("b")) == 1);
}

void iterator_operator_plus()
{
	typedef boost::tries::trie_set<int> int_set;
	typedef int_set::iterator int_set_iter;
	typedef int_set::reverse_iterator int_set_riter;

	boost::tries::trie_set<int> t;
	std::vector<int> k1, k2, k3;
	k1.push_back(1);
	k1.push_back(2);
	k1.push_back(3);
	k2 = k1;
	k2.push_back(4);
	k3.push_back(2);
	k3.push_back(3);

	t.insert(k1);
	t.insert(k2);
	t.insert(k3);

	int_set_iter iter = t.begin();
	BOOST_TEST(*iter == k1);
	++iter;
	BOOST_TEST(*iter == k2);
	++iter;
	BOOST_TEST(*iter == k3);
	++iter;
	BOOST_TEST(iter == t.end());

	int_set_riter riter = t.rbegin();
	BOOST_TEST(*riter == k3);
	++riter;
	BOOST_TEST(*riter == k2);
	++riter;
	BOOST_TEST(*riter == k1);
	++riter;
	BOOST_TEST(riter == t.rend());
}

void iterator_operator_minus()
{
	typedef boost::tries::trie_set<int> int_set;
	typedef int_set::iterator int_set_iter;
	typedef int_set::reverse_iterator int_set_riter;

	boost::tries::trie_set<int> t;
	std::vector<int> k1, k2, k3;
	k1.push_back(1);
	k1.push_back(2);
	k1.push_back(3);
	k2 = k1;
	k2.push_back(4);
	k3.push_back(2);
	k3.push_back(3);
	t.insert(k1);
	t.insert(k2);
	t.insert(k3);

	int_set_iter iter = (--t.end());
	BOOST_TEST(*iter == k3);
	--iter;
	BOOST_TEST(*iter == k2);
	--iter;
	BOOST_TEST(*iter == k1);
	--iter;
	BOOST_TEST(iter == t.begin());

	int_set_riter riter = (--t.rend());
	BOOST_TEST(*riter == k1);
	--riter;
	BOOST_TEST(*riter == k2);
	--riter;
	BOOST_TEST(*riter == k3);
	--riter;
	BOOST_TEST(riter == t.rbegin());
}

int main() {
	insert_erase_test();
	insert_find_test();
	copy_test();
	iterator_operator_plus();
	iterator_operator_minus();
	return boost::report_errors();
}
