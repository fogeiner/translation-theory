#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <iterator>
#include <exception>
#include "create_map.h"
#include "Logger.h"

using std::pair;
using std::exception;
using std::iterator;
using std::map;
using std::vector;
using std::string;
using std::endl;
using std::cout;

// teminals: real, integer, ^, %, /, *, -, +, (, )
// non-terminals: term#, T#, number

enum NodeType {
	N_REAL, N_INTEGER, N_POWER, N_MOD, N_DIVISION, N_MULTIPLICATION, N_MINUS,
	N_PLUS, N_OPENING_RBRACKET, N_CLOSING_RBRACKET,
	
	N_TERM1, N_TERM2, N_TERM3, N_TERM4, N_TERMN,
	N_T1, N_T2, N_T3, N_T4, N_NUMBER};


class Node {
private:
	string _token;
	vector<Node*> _children;
public:
	typedef vector<Node *>::iterator node_iterator;
	
	Node(string token):
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
	
	virtual ~Node(){};
	virtual string XMLStart() const = 0;
	virtual string XMLEnd() const = 0;
};

// file is generated with genNodes script
#include "_Nodes.h"

class INodeCreator {
	public:
	virtual Node *create(string token) = 0;
};

template<typename T>
class NodeCreator: public INodeCreator {
	public:
	Node *create(string token) {
		return new T(token);
	}
};

class NodeFactoryException: exception {
	private:
		string _msg;
	public:
		NodeFactoryException(string msg):
		_msg(msg)
		{}
	
		~NodeFactoryException() throw()
		{}
	
		const char *what() const throw() {
			return _msg.c_str();
		}
};

class NodeFactory {
	private:
	static map<NodeType, INodeCreator*> _creators;
public:
	
	static Node *create(NodeType type) {
		return NodeFactory::create(type, "");
	}

	static Node *create(NodeType type, string token) {
		map<NodeType, INodeCreator*>::iterator it = _creators.find(type);
		if (it == _creators.end()) {
			throw NodeFactoryException(LOG_MSG("Requested NodeCreator not found"));
		}
		INodeCreator *creator = it->second;
		Node *node = creator->create(token);
		return node;
	}
	
	static void registerCreator(NodeType type, INodeCreator *creator) {
		// put into the map
		map<NodeType, INodeCreator*>::iterator  it = _creators.find(type);
		if (it != _creators.end()) {
			throw NodeFactoryException(LOG_MSG("NodeCreator already exists"));
		}
		_creators.insert(pair<NodeType, INodeCreator*>(type, creator));
	}
};

map<NodeType, INodeCreator*> NodeFactory::_creators;

string XMLTree(Node *node, int level) {
	string xml, start, end;

	start = node->XMLStart();
	end = node->XMLEnd();
	
	for (int i = 0; i < level; ++i) {
		xml += '\t';
	}

	xml += start;
	xml += '\n';

	for(Node::node_iterator it = node->begin();
			it != node->end(); ++it) {
		Node *child = *it;
		xml += XMLTree(child, level + 1);
	}

	for (int i = 0; i < level; ++i) {
		xml += '\t';
	}
	
	xml += end;
	
	if (end.length() != 0) {
		xml += '\n';
	}

	return xml;
}

string XMLTree(Node *node) {
	return XMLTree(node, 0);
}

int main(int argc, char *argv[]) {
	NodeFactory::registerCreator(N_REAL, new NodeCreator<RealNode>());
	NodeFactory::registerCreator(N_INTEGER, new NodeCreator<IntegerNode>());
	NodeFactory::registerCreator(N_POWER, new NodeCreator<PowerNode>());
	NodeFactory::registerCreator(N_MOD, new NodeCreator<ModNode>());
	NodeFactory::registerCreator(N_DIVISION, new NodeCreator<DivisionNode>());
	NodeFactory::registerCreator(N_MULTIPLICATION, new NodeCreator<MultiplicationNode>());
	NodeFactory::registerCreator(N_MINUS, new NodeCreator<MinusNode>());
	NodeFactory::registerCreator(N_PLUS, new NodeCreator<PlusNode>());
	NodeFactory::registerCreator(N_OPENING_RBRACKET, new NodeCreator<OpeningRBracketNode>());
	NodeFactory::registerCreator(N_CLOSING_RBRACKET, new NodeCreator<ClosingRBracketNode>());
	NodeFactory::registerCreator(N_TERM1, new NodeCreator<Term1Node>());
	NodeFactory::registerCreator(N_TERM2, new NodeCreator<Term2Node>());
	NodeFactory::registerCreator(N_TERM3, new NodeCreator<Term3Node>());
	NodeFactory::registerCreator(N_TERM4, new NodeCreator<Term4Node>());
	NodeFactory::registerCreator(N_TERMN, new NodeCreator<TermNNode>());
	NodeFactory::registerCreator(N_T1, new NodeCreator<T1Node>());
	NodeFactory::registerCreator(N_T2, new NodeCreator<T2Node>());
	NodeFactory::registerCreator(N_T3, new NodeCreator<T3Node>());
	NodeFactory::registerCreator(N_T4, new NodeCreator<T4Node>());
	NodeFactory::registerCreator(N_NUMBER, new NodeCreator<NumberNode>());

	Node *node = NodeFactory::create(N_TERM1);
	node->addChild(NodeFactory::create(N_TERM2));
	node->addChild(NodeFactory::create(N_T1));
	
	cout << XMLTree(node) << endl;

	return 0;
}
