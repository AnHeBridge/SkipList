##跳跃表skiplist

##说明
C++实现跳跃表，HeadOnly ，支持模板、迭代器。
提供基本的遍历，获取名次，获取名次对应element，提供lower_bound（定义同stl）方法
顺便熟悉一下智能指针、可修改为原生指针提高一波效率（~~~

##使用说明
```
std::unique_ptr<SkipList<int,std::string>> a(new SkipList<int,std::string>(32));
//插入key、value 返回插入后对应对象的迭代器
a->insert(value,std::to_string(1));
//删除对应key,返回是否删除成功
a->erase(key)
//遍历
for (SkipList<int,std::string>::const_iterator skit = a->begin() ; skit != a->end() ; skit++)
	std::cout << skit->_key << " " << skit->_value << std::endl;
for (SkipList<int,std::string>::iterator skit = a->begin() ; skit != a->end() ; skit++)
	std::cout << skit->_key << " " << skit->_value << std::endl;
//查询find 返回查找到的迭代器
SkipList<int,std::string>::const_iterator skit = a->find(key);
//lower_bound 返回>=对应key的首个迭代器
SkipList<int,std::string>::const_iterator skit = a->lower_bound(key);
//获取名次 获取key对应的名次，不存在返回0
unsigned long rank = a->getRank(key);
//getRankElement获取名次对应的迭代器
SkipList<int,std::string>::const_iterator skit = a->getRankElement(500);
```

### Build Test
```
cd build
cmake .. -Dgtest_build_samples=ON
make
ctest -VV
```

### Run BenchMark(需要先编译除gtest相关内容、执行Build Test相关或自行修改Cmake)
```
cd build
cmake .. -Dbenchmark_build_sample=ON
make
./first
```
