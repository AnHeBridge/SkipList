#ifndef _SKIPLIST_H_
#define _SKIPLIST_H_

#include <memory>
#include <vector>
#include <iostream>
#include <random>

unsigned int generaterand_random(unsigned int llimit,unsigned int rlimit) {
	static std::mt19937 engine{ std::random_device{} ()};
	static std::uniform_int_distribution<unsigned int>distribution(llimit,rlimit - 1);
	return distribution(engine);
}

template<class Tk,class Tv>
class SkipListNode;

template<class Tk,class Tv>
struct skiplist_iterator {
	typedef SkipListNode<Tk,Tv> value_type;
	typedef SkipListNode<Tk,Tv>& reference;

	typedef std::shared_ptr<value_type> value_sptr;
	typedef std::weak_ptr<value_type> value_wptr;

	typedef skiplist_iterator<Tk,Tv> _Self;

	explicit skiplist_iterator(value_sptr _x) : _node(_x){}

	skiplist_iterator() = default;
	
	reference operator *() const { return *this->_node; }

	value_sptr operator ->() const {return this->_node; }

	_Self& operator++() {
		_node = _node->level[0].forward;
		return *this;
	}

	_Self operator++(int) {
		_Self tmp = *this;
		_node = _node->level[0].forward;
		return tmp;
	}

	_Self& operator--() {
		_node = _node->backward;
		return *this;
	}

	_Self operator-- (int) {
		_Self tmp = *this;
		_node = _node->backward;
		return tmp;
	}
	
	bool operator == (const _Self& rhs) const { return _node == rhs._node;}
	bool operator != (const _Self& rhs) const { return _node != rhs._node;}

	value_sptr _node;
};


template<class Tk,class Tv>
struct const_skiplist_iterator {
	typedef const SkipListNode<Tk,Tv> value_type;
	typedef const SkipListNode<Tk,Tv>& reference;

	typedef std::shared_ptr<value_type> value_sptr;
	typedef std::weak_ptr<value_type> value_wptr;

	typedef const_skiplist_iterator<Tk,Tv> _Self;

	explicit const_skiplist_iterator(value_sptr _x) : _node(_x){}

	const_skiplist_iterator(const skiplist_iterator<Tk,Tv>& _x) : _node(_x._node){}

	const_skiplist_iterator() = default;
	
	reference operator *() const { return *this->_node; }

	value_sptr operator ->() const {return this->_node; }

	_Self& operator++() {
		_node = _node->level[0].forward;
		return *this;
	}

	_Self operator++(int) {
		_Self tmp = *this;
		_node = _node->level[0].forward;
		return tmp;
	}

	_Self& operator--() {
		_node = _node->backward;
		return *this;
	}

	_Self operator-- (int) {
		_Self tmp = *this;
		_node = _node->backward;
		return tmp;
	}
	
	bool operator == (const _Self& rhs) { return _node == rhs._node;}
	bool operator != (const _Self& rhs) { return _node != rhs._node;}

	value_sptr _node;
};


template<class Tk,class Tv>
class SkipListNode {
	struct levelnode {
		std::shared_ptr<SkipListNode> forward;
		unsigned long span = 0;
	};
public :
	SkipListNode() = delete;
	~SkipListNode() { std::cout << "destroctor node" << std::endl; }
	SkipListNode(const Tk& node_key,const Tv& value,int levelnum);

public : 
	const Tk _key;
	Tv _value;
	std::weak_ptr<SkipListNode<Tk,Tv>> backward;
	
	std::vector<levelnode> level;
};

template<class Tk,class Tv>
class SkipList {
	using listnode = SkipListNode<Tk,Tv>;
public :
	using iterator = skiplist_iterator<Tk,Tv>;
	using const_iterator = const_skiplist_iterator<Tk,Tv>;
private :
	void deleteNode(std::vector<std::shared_ptr<listnode>>& update,std::shared_ptr<listnode> del_node);
public :
	SkipList(int levelnum);
	~SkipList() { std::cout << "destruct skiplist" << std::endl; }

	std::shared_ptr<listnode> insert(const Tk& key,const Tv& value,int rand_level);

	bool erase(const Tk& key);
	
	void printlist();

	iterator begin() {
		if (this->length == 0)
			return iterator(nullptr);
		else 
			return iterator(head->level[0].forward);
	}

	const_iterator begin() const { 
		if (length == 0) 
			return const_iterator(nullptr);
		else 
			return const_iterator(static_cast<std::shared_ptr<const SkipListNode<Tk,Tv> >>(head->level[0].forward));
	}

	iterator  end() { return iterator(nullptr);}
	const_iterator end() const { return const_iterator(nullptr);}

	iterator lower_bound(const Tk& find_key);
	const_iterator lower_bound(const Tk& find_key) const;

	iterator find(const Tk& find_key);
	const_iterator find(const Tk& find_key) const;

	unsigned long getRank(const Tk& find_key) const;

	iterator getRankElement(unsigned long rank);
	const_iterator getRankElement(unsigned long rank) const;

public :
	std::shared_ptr<listnode> head,tail;
	uint64_t length = 0;
	int level = 1;
	int max_level;
};


template<class Tk,class Tv>
SkipListNode<Tk,Tv>::SkipListNode(const Tk& node_key,const Tv& value,int levelnum) : _key(node_key),_value(value),level(levelnum) { }

template<class Tk,class Tv>
SkipList<Tk,Tv>::SkipList(int levelnum) : max_level(levelnum),tail(nullptr) {
	Tk default_k = Tk();
	Tv default_v = Tv();
	head = std::make_shared<SkipList<Tk,Tv>::listnode>(default_k,default_v,levelnum);
}

/*
 * @param rand_level != -1 represent assign insert node level
 */
template<class Tk,class Tv>
std::shared_ptr<SkipListNode<Tk,Tv>> SkipList<Tk,Tv>::insert(const Tk& key,const Tv& value,int rand_level = -1) {
	SkipListNode<Tk,Tv>* update[this->max_level];
	std::shared_ptr<SkipListNode<Tk,Tv>> x;
	unsigned long rank[this->max_level];
	memset(rank,0,sizeof(rank));
	memset(update,0,sizeof(update));

	x = this->head;
	for (int i = this->level - 1; i >= 0 ; i--) {
		rank[i] = (i == this->level - 1) ? 0 : rank[i + 1];
		while (x->level[i].forward && x->level[i].forward->_key < key) {
			rank[i] += x->level[i].span;
			x = x->level[i].forward;
		}
		update[i] = x.get();
	}
	
	//same key replace value
	if (x->level[0].forward && x->level[0].forward->_key == key) {
		x->level[0].forward->_value = value;
		return x->level[0].forward;
	}
	
	if (rand_level == -1) 
		rand_level = generaterand_random(1,this->max_level);

	if (rand_level > this->level) {
		for (int i = this->level ; i < rand_level ; i++) {
			rank[i] = 0;
			update[i] = this->head.get();
			update[i]->level[i].span = this->length;
		}
		this->level = rand_level;
	}

	std::shared_ptr<SkipList<Tk,Tv>::listnode> insert_node = std::make_shared<SkipList<Tk,Tv>::listnode>(key,value,this->max_level);
	for (int i = 0 ; i < rand_level ; i++) {
		insert_node->level[i].forward = update[i]->level[i].forward;
		update[i]->level[i].forward = insert_node;
		insert_node->level[i].span = update[i]->level[i].span - (rank[0] - rank[i]);
		update[i]->level[i].span = rank[0] - rank[i] + 1;
	}

	for (int i = rand_level ; i < this->level ; i++) 
		update[i]->level[i].span++;
	
	insert_node->backward = (update[0] == this->head.get()) ? NULL : x;
	if (insert_node->level[0].forward)
		insert_node->level[0].forward->backward = insert_node;
	else 
		this->tail = insert_node;
	
	this->length++;
	return insert_node;
}
	
template<class Tk,class Tv>
bool SkipList<Tk,Tv>::erase(const Tk& key) {
	std::vector<std::shared_ptr<SkipListNode<Tk,Tv>>>update(this->max_level);
	std::shared_ptr<SkipListNode<Tk,Tv>> x = this->head;

	for (int i = this->level - 1  ; i >= 0 ; i--) {
		while(x->level[i].forward && x->level[i].forward->_key < key)
			x = x->level[i].forward;
		update[i] = x;
	}
	x = x->level[0].forward;
	if (x && x->_key == key) {
		deleteNode(update,x);
		return true;
	}
	return false;
}

template<class Tk,class Tv>
void SkipList<Tk,Tv>::deleteNode(std::vector<std::shared_ptr<listnode>>& update,std::shared_ptr<listnode> del_node) {
	for (int i = 0 ; i < this->level ; i++) {
		if (update[i]->level[i].forward && update[i]->level[i].forward->_key == del_node->_key) {
			update[i]->level[i].span += del_node->level[i].span - 1;
			update[i]->level[i].forward = del_node->level[i].forward;
		}
		else 
			update[i]->level[i].span--;
	}

	if (del_node->level[0].forward)
		del_node->level[0].forward->backward = del_node->backward;
	else 
		this->tail = del_node->backward.lock();

	while(this->level > 1 && !this->head->level[this->level - 1].forward)
		this->level--;
	this->length--;
}


template<class Tk,class Tv>
void SkipList<Tk,Tv>::printlist() {
	std::cout << "mylevel is " << this->level << std::endl;
	std::cout << "tail is " << this->tail->_key << std::endl;
	for (int i = this->max_level - 1;  i >= 0; i--) {
		std::cout << "level " << i << std::endl;
		std::shared_ptr<SkipListNode<Tk,Tv>> x = this->head;
		while (x->level[i].forward) {
			std::cout << "span is" << x->level[i].span << ",key is " << x->level[i].forward->_key << ",value is" << x->level[i].forward->_value << " ";
			auto f = x->level[i].forward->backward.lock();
			if (f) std::cout << "backward:" << f->_key;
			x = x->level[i].forward;
		}
		std::cout << std::endl;
	}
}

template<class Tk,class Tv>
typename SkipList<Tk,Tv>::iterator SkipList<Tk,Tv>::lower_bound(const Tk& find_key) {
	std::shared_ptr<SkipList<Tk,Tv>::listnode> x = this->head;
	for (int i = this->level - 1 ; i >= 0 ; i--) {
		while (x->level[i].forward && x->level[i].forward->_key < find_key) 
			x = x->level[i].forward;
	}
	return SkipList<Tk,Tv>::iterator(x->level[0].forward);
}

template<class Tk,class Tv>
typename SkipList<Tk,Tv>::const_iterator SkipList<Tk,Tv>::lower_bound(const Tk& find_key) const {
	std::shared_ptr<SkipList<Tk,Tv>::listnode> x = this->head;
	for (int i = this->level - 1 ; i >= 0 ; i--) {
		while (x->level[i].forward && x->level[i].forward->_key < find_key) 
			x = x->level[i].forward;
	}
	return SkipList<Tk,Tv>::const_iterator(x->level[0].forward);
}

template<class Tk,class Tv>
typename SkipList<Tk,Tv>::iterator SkipList<Tk,Tv>::find(const Tk& find_key) {
	std::shared_ptr<SkipList<Tk,Tv>::listnode> x = this->head;
	for (int i = this->level - 1 ; i >= 0 ; i--) {
		while (x->level[i].forward && x->level[i].forward->_key < find_key) 
			x = x->level[i].forward;
	}
	if (x->level[0].forward && x->level[0].forward->_key == find_key)
		return SkipList<Tk,Tv>::iterator(x->level[0].forward);
	else 
		return this->end();
}

template<class Tk,class Tv>
typename SkipList<Tk,Tv>::const_iterator SkipList<Tk,Tv>::find(const Tk& find_key) const {
	std::shared_ptr<SkipList<Tk,Tv>::listnode> x = this->head;
	for (int i = this->level - 1 ; i >= 0 ; i--) {
		while (x->level[i].forward && x->level[i].forward->_key < find_key) 
			x = x->level[i].forward;
	}
	if (x->level[0].forward && x->level[0].forward->_key == find_key)
		return SkipList<Tk,Tv>::const_iterator(x->level[0].forward);
	else 
		return this->end();
}

template<class Tk,class Tv>
unsigned long SkipList<Tk,Tv>::getRank(const Tk& find_key) const {
	SkipList<Tk,Tv>::listnode* x = this->head.get();
	unsigned long rank = 0;
	for (int i = this->level - 1 ; i >= 0 ; i--) {
		while (x->level[i].forward && x->level[i].forward->_key <= find_key) {
			rank += x->level[i].span;
			if (x->level[i].forward->_key == find_key)
				return rank;
			x = x->level[i].forward.get();
		}
	}

	if (x->level[0].forward && x->level[0].forward->_key == find_key)
		return rank + x->level[0].span;
	return 0;
}

template<class Tk,class Tv>
typename SkipList<Tk,Tv>::iterator SkipList<Tk,Tv>::getRankElement(unsigned long rank) {
	if (rank > this->length or rank == 0)
		return iterator(nullptr);
	std::shared_ptr<SkipList<Tk,Tv>::listnode> x = this->head;
	unsigned long current_rank = 0;
	for (int i = this->level - 1 ; i >= 0 ; i--) {
		while (x->level[i].forward && x->level[i].span + current_rank <= rank) {
			current_rank += x->level[i].span;
			x = x->level[i].forward;
			if (current_rank == rank)
				return iterator(x);
		}
	}
	return iterator(nullptr);
}

template<class Tk,class Tv>
typename SkipList<Tk,Tv>::const_iterator SkipList<Tk,Tv>::getRankElement(unsigned long rank) const {
	if (rank > this->length or rank == 0)
		return const_iterator(nullptr);

	std::shared_ptr<SkipList<Tk,Tv>::listnode> x = this->head;
	unsigned long current_rank = 0;
	for (int i = this->level - 1 ; i >= 0 ; i--) {
		while (x->level[i].forward && x->level[i].span + current_rank <= rank) {
			current_rank += x->level[i].span;
			x = x->level[i].forward;
			if (current_rank == rank)
				return const_iterator(x);
		}
	}
	return const_iterator(nullptr);
}

#endif
