// TEST FILE
#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>

using namespace std;

class Node {
private:
	string _token;
	vector<Node*> _children;
public:
	typedef vector<Node *>::iterator node_iterator;
	
	Node(string token = ""):
	_token(token)
	{}
	
	string getToken() const {
		return _token;
	}
	
	void addChild(Node *node) {
		_children.push_back(node);
	}
	
	node_iterator begin() {
		return _children.begin();
	}
	node_iterator end(){
		return _children.end();
	}
	
	Node *operator[](int index) const {
		return _children[index];
	}
	
	virtual ~Node(){};
	virtual string XMLStart() const = 0;
	virtual string XMLEnd() const = 0;
};

class PlusNode: public Node {
public:

PlusNode() 
	{}
	
	string XMLStart() const {
		return "<plus />";
	}

	string XMLEnd() const {
		return "";
	}

	double compute() const {
		Node *n1 = (*this)[0];
		Node *n2 = (*this)[1];
		
		return n1->compute() + n2->compute();
	}
};

class IntegerNode: public Node {
public:
IntegerNode(string str):
	Node(str) 
	{}
	
string XMLStart() const {
	return string("<integer>") + getToken() + string("</integer>");
}
string XMLEnd() const {
	return "";
}
double compute() const {
	return atof(getToken().c_str());
}
};

int main(int argc, char *argv[]) {
	Node *n = new PlusNode();
	n->addChild(new IntegerNode("10"));
	n->addChild(new IntegerNode("100"));
	cout << n->compute() << endl;
	return 0;
}
