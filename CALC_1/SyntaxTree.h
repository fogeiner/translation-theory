#ifndef SYNTAXTREE_H
#define SYNTAXTREE_H

#include <string>
#include <vector>
using std::vector;
using std::string;

class Node {
	private:
		int _data;
		vector<Node *> _children;
	public:
		Node(int data):
			_data(data)
		{}
		void addChild(Node *node){
			_children.push_back(node);
		}
};

#endif
