#include "gtest/gtest.h"
#include <limits>
#include "skiplist.hpp"
#include <string>
#include <random>
#include <map>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>

int16_t generaterand_random(int16_t llimit,int16_t rlimit) {
	static std::mt19937 engine{ std::random_device{} ()};
	static std::uniform_int_distribution<int16_t>distribution(llimit,rlimit);
	return distribution(engine);
}

TEST(skiplist,iterator_insert_erase) {
	std::unique_ptr<SkipList<int,std::string>> a(new SkipList<int,std::string>(32));
	std::map<int,std::string> mp;
	std::map<int,std::string>::iterator mpit;
	std::vector<int>keys;
	for (int i = 1 ; i <= 1000 ; i++) {
		int16_t value = generaterand_random(std::numeric_limits<int16_t>::min(),std::numeric_limits<int16_t>::max());
		mp[value] = std::to_string(1);
		a->insert(value,std::to_string(1));
		if (i & 1)
			keys.push_back(value);
	}
	//erase some key
	for (int i = 0 ; i < (int)keys.size() ; i++) {
		mp.erase(keys[i]);
		a->erase(keys[i]);
	}

	std::vector<int> res1,res2,res3;
	for (SkipList<int,std::string>::iterator skit = a->begin() ; skit != a->end() ; skit++)
		res1.push_back(skit->_key);
	for (SkipList<int,std::string>::const_iterator skit = a->begin() ; skit != a->end() ; skit++)
		res2.push_back(skit->_key);
	for (mpit = mp.begin() ; mpit != mp.end() ; mpit++)
		res3.push_back(mpit->first);
	ASSERT_EQ(res1,res2);
	ASSERT_EQ(res2,res3);
	
}

TEST(skiplist,find_lowerbound) {
	std::unique_ptr<SkipList<int,std::string>> a(new SkipList<int,std::string>(32));
	std::map<int,std::string> mp;
	std::map<int,std::string>::iterator mpit;
	for (int i = 1 ; i <= 1000 ; i++) {
		int16_t value = generaterand_random(std::numeric_limits<int16_t>::min(),std::numeric_limits<int16_t>::max());
		mp[value] = std::to_string(1);
		a->insert(value,std::to_string(1));
	}
	a->insert(134222,"values");
	mp[134222] = "values";
	SkipList<int,std::string>::const_iterator skit = a->find(134222);
	mpit = mp.find(134222);
	ASSERT_EQ(skit->_key,mpit->first);
	ASSERT_EQ(skit->_value,mpit->second);

	skit = a->lower_bound(10000);
	mpit = mp.lower_bound(10000);
	ASSERT_EQ(skit->_key,mpit->first);
	ASSERT_EQ(skit->_value,mpit->second);
}

TEST(skiplist,rank_about) {
	using order_statistic_tree = __gnu_pbds::tree<int,__gnu_pbds::null_type,std::less<int>,__gnu_pbds::rb_tree_tag,__gnu_pbds::tree_order_statistics_node_update>;

	std::unique_ptr<SkipList<int,std::string>> a(new SkipList<int,std::string>(32));

	order_statistic_tree tree;//this datastruct rank start from 0 not 1
	for (int i = 1 ; i <= 1000 ; i++) {
		int16_t value = generaterand_random(std::numeric_limits<int16_t>::min(),std::numeric_limits<int16_t>::max());
		a->insert(value,std::to_string(1));
		tree.insert(value);
	}
	a->insert(10000,"1");
	tree.insert(10000);
	unsigned long rank = a->getRank(10000);
	ASSERT_EQ(rank,tree.order_of_key(10000) + 1);

	SkipList<int,std::string>::const_iterator skit = a->getRankElement(500);
	ASSERT_EQ(skit->_key,*tree.find_by_order(500 - 1));
}
