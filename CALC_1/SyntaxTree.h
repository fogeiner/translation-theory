#ifndef SYNTAXTREE_H
#define SYNTAXTREE_H

#include <string>
#include <vector>
#include <map>

using std::vector;
using std::string;
using std::map;

// Nodes

class Node {
	private:
		vector<Node *> _children;
		string _token;
	public:
		Node(string token):
		_token(string)
		{}
		virtual ~Node() {}
		bool isLeaf(){
			return _children.empty();
		}
		void addChild(Node *node){
			_children.push_back(node);
		}
		virtual string XMLStart() = 0;
		virtual string XMLEnd() = 0;
		
};

// teminals: real, number, ^, %, /, *, -, +, (, )
class RealNode: public Node {
};
class IntegerNode: public Node {
};
class PowerNode: public Node {
};
class ModNode: public Node {
};
class DivisionNode: public Node {
};
class MultiplicationNode: public Node {
};
class MinusNode: public Node {
};
class PlusNode: public Node {
};
class OpeningRBracketNode: public Node {
};
class ClosingRBracketNode: public Node {
};

// non-terminals: term#, T#, number
class Term1Node: public Node {
};
class Term2Node: public Node {
};
class Term3Node: public Node {
};
class Term4Node: public Node {
};
class TermNNode: public Node {
};

class T1Node: public Node {
};
class T2Node: public Node {
};
class T3Node: public Node {
};
class T4Node: public Node {
};

class NumberNode: public Node {
};

// Creators

class NodeCreator {
	private:
	public:
};

class RealNodeCreator: public NodeCreator {
};
class IntegerNodeCreator: public NodeCreator {
};
class PowerNodeCreator: public NodeCreator {
};
class ModNodeCreator: public NodeCreator {
};
class DivisionNodeCreator: public NodeCreator {
};
class MultiplicationNodeCreator: public NodeCreator {
};
class MinusNodeCreator: public NodeCreator {
};
class PlusNodeCreator: public NodeCreator {
};
class OpeningRBracketNodeCreator: public NodeCreator {
};
class ClosingRBracketNodeCreator: public NodeCreator {
};

// non-terminals: term#, T#, number
class Term1NodeCreator: public NodeCreator {
};
class Term2NodeCreator: public NodeCreator {
};
class Term3NodeCreator: public NodeCreator {
};
class Term4NodeCreator: public NodeCreator {
};
class TermNNodeCreator: public NodeCreator {
};

class T1NodeCreator: public NodeCreator {
};
class T2NodeCreator: public NodeCreator {
};
class T3NodeCreator: public NodeCreator {
};
class T4NodeCreator: public NodeCreator {
};

class NumberNodeCreator: public NodeCreator {
};

class NodeFactory{
	private:
	public:
};

#endif
