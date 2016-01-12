#include <boost/core/lightweight_test.hpp>
#include "boost/trie/trie.hpp"
#include "boost/trie/trie_map.hpp"

#include <string>

class mytype {

public:
	explicit mytype() : value(0)
	{}

	mytype(int x) : value(x)
	{
	}

	mytype& operator=(const mytype& x)
	{
		value = x.value;
		return *this;
	}

	bool operator<(const mytype& x) const
	{
		return value < x.value;
	}
	bool operator ==(const mytype& x) const
	{
		return value == x.value;
	}

	int value;
};

typedef boost::tries::trie_map<mytype, mytype> tmm;
typedef std::vector<mytype> vm;

std::ostream& operator <<(std::ostream& out, const mytype& m)
{
	out << m.value;
	return out;
}

mytype ms1[] = {1, 2, 3, 4};
mytype ms2[] = {1, 2, 2, 4};
mytype ms3[] = {2, 2, 4, 4};
mytype ms4[] = {2, 2, 3, 4};
vm s(ms1, ms1 + 2);
vm s1(ms1, ms1 + 4);
vm s2(ms2, ms2 + 4);
vm s3(ms3, ms3 + 4);
vm s4(ms4, ms4 + 4);
vm s5(ms4, ms4 + 2);

void operator_test()
{
	tmm t;
	vm s;
	s.push_back(mytype(1));
	s.push_back(mytype(2));
	s.push_back(mytype(3));
	vm s2;
	s2.push_back(mytype(2));
	t[s] = 1;
	BOOST_TEST(t[s] == 1);
	t[s] = 5;
	BOOST_TEST(t[s] == mytype(5));
	t[s2] = t[s];
	BOOST_TEST(t.count_node() == 4);
	BOOST_TEST(t[s] == t[s2]);
	BOOST_TEST(t.count_node() == 4);
	BOOST_TEST((*t.begin()).second == mytype(5));
	BOOST_TEST(t.size() == 2);
	BOOST_TEST(t.count_node() == 4);
}

void insert_and_find_test()
{
	tmm t;
	t[s] = 1;
	BOOST_TEST((*t.find(s)).second == 1);
	BOOST_TEST(t.find(s) != t.end());
	BOOST_TEST(t.find(s2) == t.end());
	BOOST_TEST(t.find(s2) == t.end());
	t[s] = 2;
	BOOST_TEST((*t.find(s)).second == 2);
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
	tmm t, t2;
	t[s] = 1; t[s1] = 2; t[s2] = 3; t[s3] = 4;
	t2 = t;
	size_t node_cnt = 4 + 2 + 4;
	BOOST_TEST(t2.size() == 4);
	BOOST_TEST(t2.count_node() == node_cnt);
	BOOST_TEST(t2[s] == 1);
	BOOST_TEST(t2[s1] == 2);
	BOOST_TEST(t2[s2] == 3);
	BOOST_TEST(t2[s3] == 4);
	tmm t3(t2);
	BOOST_TEST(t3.size() == 4);
	BOOST_TEST(t3.count_node() == node_cnt);
	BOOST_TEST((*t3.find(s)).second == 1);
	BOOST_TEST((*t3.find(s1)).second == 2);
	BOOST_TEST((*t3.find(s2)).second == 3);
	BOOST_TEST((*t3.find(s3)).second == 4);
	BOOST_TEST(t3.find(ms1, ms1 + 3) == t3.end());
	BOOST_TEST(t3.find(ms1, ms1 + 4) != t3.end());
}

void iterator_operator_plus()
{
	tmm t;
	BOOST_TEST(t.empty() == true);
	BOOST_TEST(t.size() == 0);
	BOOST_TEST(t.begin() == t.end());
	t[s] = 1;
	t[s1] = 2;
	t[s2] = 3;
	BOOST_TEST(t.begin() != t.end());
	tmm::iterator ti;
	ti = t.begin();
	BOOST_TEST((*ti).second == 1);
	++ti;
	BOOST_TEST((*ti).second == 3);
	BOOST_TEST(t[s2] == 3);
	++ti;
	BOOST_TEST((*ti).second == 2);
	++ti;
	BOOST_TEST(ti == t.end());
	// test ++end()
	++ti;
	BOOST_TEST(ti == t.end());
}

void iterator_operator_minus()
{
	tmm t;
	BOOST_TEST(t.empty() == true);
	BOOST_TEST(t.size() == 0);
	BOOST_TEST(t.begin() == t.end());
	t[s] = 1;
	t[s1] = 2;
	t[s2] = 3;
	BOOST_TEST(t.begin() != t.end());
	tmm::iterator ti;
	tmm::const_iterator cti(t.end());
	ti = t.begin();
	BOOST_TEST((*ti).second == 1);
	(*ti).second = 10;
	BOOST_TEST((*ti).second == 10);
	cti = t.begin();
	BOOST_TEST((*cti).second == 10);
	--ti;
	BOOST_TEST(ti == t.begin());
	BOOST_TEST(t[s2] == 3);
	ti = t.end();
	--ti;
	BOOST_TEST((*ti).second == 2);
	t[s3] = 4;
	++ti;
	BOOST_TEST((*ti).second == 4);
	++ti;
	BOOST_TEST(ti == t.end());
	++ti;
	BOOST_TEST(ti == t.end());
	--ti;
	BOOST_TEST((*ti).second == 4);
	size_t node_cnt = 4 + 2 + 4 + 2;
	t[s4] = 5;
	t[s5] = 6;
	BOOST_TEST(t.count_node() == node_cnt);
	BOOST_TEST(t.size() == 6);
}

void clear()
{
	tmm t;
	t[s] = t[s1] = t[s2] = t[s3] = 10;
	size_t node_cnt = t.count_node();
	BOOST_TEST(t.size() == 4);
	BOOST_TEST(t.count_node() == node_cnt);
	t.clear();
	BOOST_TEST(t.size() == 0);
	BOOST_TEST(t.count_node() == 0);
	BOOST_TEST(t.begin() == t.end());
	BOOST_TEST(t[s] == 0);
	BOOST_TEST(t.size() == 1);
	BOOST_TEST(t.count_node() == 2);
}

void erase_iterator()
{
	tmm t;
	t[s] = 1;
	t[s1] = 2;
	t[s2] = 3;
	t[s3] = 4;
	size_t node_cnt = t.count_node();
	BOOST_TEST(t.size() == 4);
	BOOST_TEST(t.count_node() == node_cnt);
	tmm::iterator ti;
	ti = t.begin();
	t.erase(t.begin());
	ti = t.begin();
	BOOST_TEST(t.size() == 3);
	BOOST_TEST(t.count_node() == node_cnt);
	BOOST_TEST((*ti).second == 3);
	t.erase(ti);
	BOOST_TEST(t.size() == 2);
	ti = t.begin();
	BOOST_TEST((*ti).second == 2);
	BOOST_TEST(t.count_node() == 8);
	BOOST_TEST(t[s] == 0);
	ti = t.begin();
	BOOST_TEST(t.size() == 3);
	BOOST_TEST((*ti).second == 0);
	BOOST_TEST(t.count_node() == 8);
}

void erase_key()
{
	tmm t;
	t[s] = 1;
	t[s1] = 2;
	t[s2] = 3;
	t[s3] = 4;
	size_t node_cnt = t.count_node();
	BOOST_TEST(t.size() == 4);
	BOOST_TEST(t.count_node() == node_cnt);
	tmm::iterator ti;
	ti = t.begin();
	t.erase(ms1, ms1 + 1);
	ti = t.begin();
	// erasion failure
	BOOST_TEST(t.size() == 4);
	BOOST_TEST((*ti).second == 1);
	t.erase(ms1, ms1 + 2);
	ti = t.begin();
	BOOST_TEST(t.size() == 3);
	BOOST_TEST((*ti).second == 3);
	t.erase(s1);
	BOOST_TEST(t.size() == 2);
	ti = t.begin();
	BOOST_TEST((*ti).second == 3);
	BOOST_TEST(t[s] == 0);
	ti = t.begin();
	BOOST_TEST(t.size() == 3);
	BOOST_TEST((*ti).second == 0);
	ti = t.begin();
	BOOST_TEST(t.size() == 3);
	BOOST_TEST((*ti).second == 0);
}

int main() {
	operator_test();
	insert_and_find_test();
	copy_test();
	iterator_operator_plus();
	iterator_operator_minus();
	clear();
	erase_iterator();
	erase_key();
	return boost::report_errors();
}
