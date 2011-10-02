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

class NodeCreator {
	public:
	virtual Node *create(string token) = 0;
};

template<typename T>
class NodeCreatorImpl: public NodeCreator {
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

class NodeFactory{
	private:
	static map<NodeType, NodeCreator*> _creators;
	static NodeFactory
	NodeFactory() {}
public:
	
	static Node *create(NodeType type, string token) {
		map<NodeType, NodeCreator*>::iterator it = _creators.find(type);
		if (it == _creators.end()) {
			throw NodeFactoryException(LOG_MSG("Requested NodeCreator not found"));
		}
		NodeCreator *creator = it->second;
		Node *node = creator->create(token);
		return node;
	}
	
	static void registerCreator(NodeType type, NodeCreator *creator) {
		// put into the map
		map<NodeType, NodeCreator*>::iterator  it = _creators.find(type);
		if(it != _creators.end()) {
			throw NodeFactoryException(LOG_MSG("NodeCreator already exists"));
		}
		_creators.insert(pair<NodeType, NodeCreator*>(type, creator));
	}
};

map<NodeType, NodeCreator*> NodeFactory::_creators;

int main(int argc, char *argv[]) {
	NodeFactory::registerCreator(N_REAL, new NodeCreatorImpl<RealNode>());
	NodeFactory::registerCreator(N_INTEGER, new NodeCreatorImpl<IntegerNode>());
	NodeFactory::registerCreator(N_POWER, new NodeCreatorImpl<PowerNode>());
	NodeFactory::registerCreator(N_MOD, new NodeCreatorImpl<ModNode>());
	NodeFactory::registerCreator(N_DIVISION, new NodeCreatorImpl<DivisionNode>());
	NodeFactory::registerCreator(N_MULTIPLICATION, new NodeCreatorImpl<MultiplicationNode>());
	NodeFactory::registerCreator(N_MINUS, new NodeCreatorImpl<MinusNode>());
	NodeFactory::registerCreator(N_PLUS, new NodeCreatorImpl<PlusNode>());
	NodeFactory::registerCreator(N_OPENING_RBRACKET, new NodeCreatorImpl<OpeningRBracketNode>());
	NodeFactory::registerCreator(N_CLOSING_RBRACKET, new NodeCreatorImpl<ClosingRBracketNode>());
	NodeFactory::registerCreator(N_TERM1, new NodeCreatorImpl<Term1Node>());
	NodeFactory::registerCreator(N_TERM2, new NodeCreatorImpl<Term2Node>());
	NodeFactory::registerCreator(N_TERM3, new NodeCreatorImpl<Term3Node>());
	NodeFactory::registerCreator(N_TERM4, new NodeCreatorImpl<Term4Node>());
	NodeFactory::registerCreator(N_TERMN, new NodeCreatorImpl<TermNNode>());
	NodeFactory::registerCreator(N_T1, new NodeCreatorImpl<T1Node>());
	NodeFactory::registerCreator(N_T2, new NodeCreatorImpl<T2Node>());
	NodeFactory::registerCreator(N_T3, new NodeCreatorImpl<T3Node>());
	NodeFactory::registerCreator(N_T4, new NodeCreatorImpl<T4Node>());
	NodeFactory::registerCreator(N_NUMBER, new NodeCreatorImpl<NumberNode>());

	Node *node = NodeFactory::create(N_PLUS, "+");
	node->addChild(NodeFactory::create(N_NUMBER, ""));
	node->addChild(NodeFactory::create(N_INTEGER, "1"));
	
	for(Node::node_iterator it = node->begin();
		it != node->end(); ++it) {
			Node *node = *it;
		cout << node->XMLStart() << "\t" << node->XMLEnd() << endl;
	}
	return 0;
}
