#include "SyntaxTree.h"


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
	xml += '\n';
	
	return xml;
}


int main2(int argc, char *argv[]) {
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
