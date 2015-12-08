#include <boost/core/lightweight_test.hpp>
#include "boost/trie/trie.hpp"
#include "boost/trie/trie_map.hpp"

#include <iostream>
#include <string>
#include <map>

void default_construction_and_destruction()
{
    boost::tries::trie_map<std::string, std::string> t;
    (void)t;
}

class no_default_constructor_type {
    explicit no_default_constructor_type(int /*i*/){}
};

void no_default_constructor_key()
{
	std::map<no_default_constructor_type, int> t;
	(void)t;
	/*
    boost::tries::trie_map<no_default_constructor_type, int> t;
    (void)t;
	*/
}

class counter_type {
    static int m_counts_constructed;
    static int m_counts_deleted;

    int m_value;
    int m_deleted;
public:
    static void reset() {
        m_counts_constructed = 0;
        m_counts_deleted = 0;
    }

    static int counts_constructed() { return m_counts_constructed; }
    static int counts_deleted() { return m_counts_deleted; }
    static int counts_alive() { return m_counts_constructed - m_counts_deleted; }

    counter_type() : m_value(-1), m_deleted(0) { ++ m_counts_constructed; }
    counter_type(int value) : m_value(value), m_deleted(0) { ++ m_counts_constructed; }
    counter_type(const counter_type& ct) : m_value(ct.m_value), m_deleted(0) { ++ m_counts_constructed; }

    ~counter_type() {
        ++ m_deleted;
        BOOST_TEST_EQ(m_deleted, 1);
        ++ m_counts_deleted;
    }

    int value() const { return m_value; }
};


int counter_type::m_counts_constructed = 0;
int counter_type::m_counts_deleted = 0;

inline bool operator< (const counter_type& v1, const counter_type& v2) {
    return v1.value() < v2.value();
}

void counting_constructions_and_destructions_key()
{
    counter_type::reset();
    std::vector<counter_type> data;
    data.push_back(counter_type(0));
    data.push_back(counter_type(0));
    data.push_back(counter_type(1));
    data.push_back(counter_type(2));
    data.push_back(counter_type(3));
    std::cout << counter_type::counts_alive() << std::endl;
    std::cout.flush();
    const int data_size = static_cast<int>(data.size());
    BOOST_TEST_EQ(counter_type::counts_alive(), data_size);

    /*
       +1 is because all the nodes are copied / destructed in node_ref_from function
       excepting the last one that is only copied. It is only destructed when the destructor is called
    */
    boost::tries::trie_map<counter_type, int> *t = new boost::tries::trie_map<counter_type, int>();
    t->insert(data, 0);
    BOOST_TEST_EQ(counter_type::counts_alive(), data_size * 2 + 1);

    t->insert(data, 0);
    BOOST_TEST_EQ(counter_type::counts_alive(), data_size * 2 + 1);

    t->insert(data.begin(), data.end(), 0);
    BOOST_TEST_EQ(counter_type::counts_alive(), data_size * 2 + 1);

    t->insert(data.begin() + 1, data.end(), 0);
    BOOST_TEST_EQ(counter_type::counts_alive(), data_size * 3 - 1);

    t->clear();
    BOOST_TEST_EQ(counter_type::counts_alive(), data_size + 1);
    delete t;
    /* This check is performed to prove that the comment above is true */
    BOOST_TEST_EQ(counter_type::counts_alive(), data_size);
}

void counting_alive_equal_keys()
{
    counter_type::reset();
    std::vector<counter_type> data(25,counter_type(0));
    const int data_size = static_cast<int>(data.size());

    /*
       See counting_constructions_and_destructions test for explanations about why
       + 1 is required in BOOST_TEST_EQ
    */
    boost::tries::trie_map<counter_type, int> t;
    t.insert(data, 0);
    BOOST_TEST_EQ(counter_type::counts_alive(), data_size * 2 + 1);

    t.insert(data, 0);
    BOOST_TEST_EQ(counter_type::counts_alive(), data_size * 2 + 1);

    std::vector<counter_type>::const_iterator it = data.begin(),
            end = data.end();
    for (; it != end; ++it)
    {
        t.insert(it, end, 0);
        BOOST_TEST_EQ(counter_type::counts_alive(), data_size * 2 + 1);
    }
}

int main() {
    default_construction_and_destruction();
    no_default_constructor_key();
    counting_constructions_and_destructions_key();
    counting_alive_equal_keys();
    return boost::report_errors();
}
