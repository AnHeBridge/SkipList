#include <bits/stdc++.h>
#include "skiplist.hpp"

void testshare() {
	SkipList<int,std::string>  a = SkipList<int,std::string>(32);
	a.insert(7,"7",2);
	a.insert(-5,"-5",3);
	a.insert(9,"9",4);
	a.erase(9);
	a.printlist();
}


int main() {
	testshare();
	return 0;
}
