// Last modified at 2018/09/30 9:42

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <iomanip>	// for std::setw
#include <utility>	// for std::pair
#include <cstring>
#include <vector>	// You shall not use this lib in your code part

#define RAND_HALF (RAND_MAX)/2



// The frame of class has been provided for you, you may only need to implemnet member functions.
template<class Key, class Val>
class skiplist {
private:
	class _node {
	private:
		// key and val are encapsulated in _data with std::pair
		std::pair<const Key, Val> _data;
		_node **_forward;

		// friend class enables skiplist to access private members of _node
		friend class skiplist;
	public:
		// Constructor of _node, by default key and val are set as NULL (it's for dummy node)
		_node(int height, const Key &key = Key(), const Val &val = Val());
		// Destructor, free the memory allocated by _node
		~_node();
	};

private:
	// Current _level, at least 1, at most _height_limit
	int _level;

	int _height_limit;

	// _head is a dummy node for skiplist
	_node *_head;

public:
	// Constructor, set _height_limit and initialize the skiplist
	skiplist(int height_limit);

	// Destructor, free all the memory allocated by skiplist
	~skiplist();

	// Insert a new node
	int insert(const Key &key, const Val &val);


	// Search a key in skiplist
	bool search(const Key &key);


	// Remove a node with given key. Free all the memory allocated to this node, including its _forward
	bool remove(const Key &key);


	
private:
	// This function has been implemented for you
	// Generate a random level for the node
	// You will need this in insertion
	int randomLevel();

public:
	// This function has been implemented for you
	// This function is for testing and your debugging
	// Display the skiplist list
	// You will need this in debugging. To show the whole picture, output the answer with IO redirection and use a wide screen
	void display();
};
//======================= DO NOT MODIFY CODE ABOVE===========================

//============================ Begin Your Code ==============================

// Assign (key, val) to _data, allocate memory for _forward
template<class Key, class Val>
skiplist<Key, Val>::_node::_node(int height, const Key &key, const Val &val):_data(std::make_pair(key,val))
{
	_forward = new _node*[height];
};


template<class Key, class Val>
skiplist<Key, Val>::_node::~_node(){
	delete[] _forward;
};


// The initial level is 1
template<class Key, class Val>
skiplist<Key, Val>::skiplist(int height_limit) {
	_level = 1;
	_height_limit=height_limit;
	_head = new _node(_height_limit) ;
	for(int i=0;i<_height_limit;i++){
		_head->_forward[i]=NULL;
	}
};


// Free the whole skiplist
template<class Key, class Val>
skiplist<Key, Val>::~skiplist() {
	_node *p = _head;
 	_node  *tem ;
	while (p!= NULL){
		tem=p->_forward[0];
		delete p;
		p=tem;
	} 
};


// Insert a new node with key & val. Insert by key with increasing order, i.e. from head to tail, the keys of nodes are increasing. For convenience, you can simply use use <, > and == to compare as we will only use int and float for Key type
//		1) If the key does not exist before insertion, insert the new node with key & val in the proper place. Return 0
//		2) If the key exists before insertion, replace the old val with the new val. Return 1
// You will need function randomLevel() when generating a new node
template<class Key, class Val>
int skiplist<Key, Val>::insert(const Key &key, const Val &val) {
	//first serach if the key match  
	_node *insert_node = _head;
	_node *max_in_level[_height_limit];
	for (int i=_height_limit-1;i>=0;i--){
		max_in_level[i]=NULL;
	}

	for(int i = _level-1; i>= 0; i--){
		while(insert_node->_forward[i] != NULL && insert_node->_forward[i]->_data.first < key){
			insert_node = insert_node->_forward[i];
		}
		if(insert_node->_forward[i] != NULL && insert_node->_forward[i]->_data.first == key){
			insert_node->_forward[i]->_data.second = val;
			return 0;
		}
		else{
			max_in_level[i]=insert_node;			
		}
	}
	//if the key doesn't exsit, insert as following steps
	_node *real_node;
	int k = randomLevel();  	  //randomly hieght of the level
	real_node = new _node(k, key, val) ; //create the new node and store the key and value 
	//add _head node if k is higher
	while(k >  _level){
		max_in_level[_level]=_head;
		_level++;
	}
	//connect the pointer.
	for(int i=0; i<k; i++){   
		insert_node = max_in_level[i];
		real_node->_forward[i] =insert_node->_forward[i];
		insert_node->_forward[i]=real_node;
	}
	return 0;
};


// Search a key in skiplist. Return true if found, else return false
template<class Key, class Val>
    bool skiplist<Key, Val>::search(const Key &key) {
	_node *search_node = _head;
	for(int i = _level-1; i>= 0; i--){
		while(search_node->_forward[i] != NULL && search_node->_forward[i]->_data.first < key){
			search_node = search_node->_forward[i];
		}
	}
	search_node = search_node->_forward[0];
	if(search_node != NULL && search_node->_data.first == key){
		return true;
	}
	else{
		return false;
	}
};


// Remove a node with given key. Free all the memory allocated to this node, including its _forward
// Return true if success, else return false.
template<class Key, class Val>
bool skiplist<Key, Val>::remove(const Key &key) {
	_node *current_node = _head;
	_node *node_stack[_height_limit];
	//search the node current_node, which is before the node we want to delete. If find the current_node, put it into stack to mark. 
	for (int i=_height_limit-1;i>=0;i--){
		node_stack[i]=NULL;
	}

	for(int i = _level-1; i>= 0; i--){
		while(current_node->_forward[i] != NULL && current_node->_forward[i]->_data.first < key){
			current_node = current_node->_forward[i];
		}
		if(current_node->_forward[i] != NULL && current_node->_forward[i]->_data.first == key){
			node_stack[i]=current_node;
		}
	}
	//this step is to make sure if the node exsits.
	if(current_node->_forward[0] == NULL || current_node->_forward[0]->_data.first != key){
		return false;
	}
	//to connect the front node and the next node.
	_node *tem = current_node->_forward[0];
	for (int i=_level-1; i>=0; i--){
		if(node_stack[i] != NULL ){
			_node *before_node = node_stack[i];
			before_node->_forward[i]=tem->_forward[i];
		}
	}
	delete tem;
	//update the level if the _level changes.
	int p = _level;
	for (int i=_level-1;i>0;i--){
		if (_head->_forward[i] == NULL){
			p--;
		}
	}
	_level=p;
	return true;
};




//=========================== End Your Code =================================



//=========================== DO NOT MODIFY CODE BELOW=======================

// Return a new level for your new node
template<class Key, class Val>
int skiplist<Key, Val>::randomLevel() {
	int new_level = 1;
	while (new_level < _height_limit && rand() < RAND_HALF) ++new_level;
	return new_level;
};




template<class Key, class Val>
void skiplist<Key, Val>::display() {
	std::cout << "********* Skiplist ********\n";


	for (int i = _level-1; i >= 0; --i) {
		_node *base = _head->_forward[0];
		_node *curr = _head->_forward[i];

		std::cout << "Level " << i+1 << ": ";
		while (curr != NULL) {
			while (curr != base && base != NULL) {
				std::cout << std::setw(3) << " " << std::setw(1) << " " << std::setw(2) << " " << std::setw(1) << " ";
				base = base->_forward[0];
			}

			if (curr == base) {
				std::cout << std::setw(3) << curr->_data.first << std::setw(1) << ":" << std::setw(2) << curr->_data.second << std::setw(1) << " ";
				base = base->_forward[0];
			}

			
			curr = curr->_forward[i];
		}
		std::cout << "\n" << '\n';
	}
};

void get_input(std::vector<std::string> *tokens, char *inputs) {
	std::string str = inputs;
	std::string token;
	size_t pos;

	while (str.size() != 0) {
		pos = str.find(" ");
		if (pos != std::string::npos) {
			token = str.substr(0, pos);
			str = str.substr(pos + 1);
		}
		else {
			token = str;
			str = "";
		}
		(*tokens).push_back(token);
	}
}

// IO has been provided for you
int main(void)
{
	srand(2);

	char c[20];

	std::cin.getline(c, 20, '\n');
	if (c[0] == '0') {
		skiplist<int, std::string> sl(10);

		while(std::cin.getline(c, 20, '\n')) {
			std::vector<std::string> tokens;
			get_input(&tokens, c);
			if (tokens[0] == "+") {
				sl.insert(atoi(tokens[1].c_str()), tokens[2]);
				std::cout << "insert: " << tokens[1] << ": " << tokens[2] << '\n';
			}
			else if (tokens[0] == "-") {
				sl.remove(atoi(tokens[1].c_str()));
				std::cout << "remove: " << tokens[1] << '\n';
			}
			else if (tokens[0] == "@") {
				std::cout << "search: " << tokens[1] << " " << std::boolalpha << sl.search(atoi(tokens[1].c_str())) << '\n';
			}
			else {
				std::cout << "display:" << '\n';
				sl.display();
			}
		}
	}
	else if (c[0] == '1') {
		skiplist<float, int> sl(10);

		while(std::cin.getline(c, 20, '\n')) {
			std::vector<std::string> tokens;
			get_input(&tokens, c);
			if (tokens[0] == "+") {
				sl.insert(atof(tokens[1].c_str()), atoi(tokens[2].c_str()));
				std::cout << "insert: " << tokens[1] << ": " << tokens[2] << '\n';
			}
			else if (tokens[0] == "-") {
				sl.remove(atof(tokens[1].c_str()));
				std::cout << "remove: " << tokens[1] << '\n';
			}
			else if (tokens[0] == "@") {
				std::cout << "search: " << tokens[1] << " " << std::boolalpha << sl.search(atof(tokens[1].c_str())) << '\n';
			}
			else {
				std::cout << "display:" << '\n';
				sl.display();
			}
		}
	}

	return 0;
}
