#ifndef SYNTAXTREE_H
#define SYNTAXTREE_H

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
    // terminals
    N_REAL, N_INTEGER, N_POWER, N_MOD, N_DIVISION, N_MULTIPLICATION, N_MINUS,
    N_PLUS, N_OPENING_RBRACKET, N_CLOSING_RBRACKET,
    N_TERM1, N_TERM2, N_TERM3, N_TERM4, N_TERMN,
    // non-terminals
    N_T1, N_T2, N_T3, N_T4, N_NUMBER
};

class Node {
private:
    string _token;
    vector<Node*> _children;
public:
    typedef vector<Node *>::iterator node_iterator;

    Node(string token) :
    _token(token) {
    }

    string getToken() const {
        return _token;
    }

    void addChild(Node *node) {
        _children.push_back(node);
    }

    node_iterator begin() {
        return _children.begin();
    }

    node_iterator end() {
        return _children.end();
    }

    int childrenCount() const {
        return _children.size();
    }

    Node *operator[](int index) const {
        return _children[index];
    }

    virtual ~Node() {
        for (Node::node_iterator it = this->begin();
                it != this->end(); ++it) {
            Node *node = *it;
            delete node;
        }
    }

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
class NodeCreator : public INodeCreator {
public:

    Node *create(string token) {
        return new T(token);
    }
};

class NodeFactoryException : exception {
private:
    string _msg;
public:

    NodeFactoryException(string msg) :
    _msg(msg) {
    }

    ~NodeFactoryException() throw () {
    }

    const char *what() const throw () {
        return _msg.c_str();
    }
};

class NodeFactory {
private:
    static map<NodeType, INodeCreator*> _creators;
    static bool _inited;
public:
    static void defaultInit();

    static Node *create(NodeType type, string token = "") {
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
        map<NodeType, INodeCreator*>::iterator it = _creators.find(type);
        if (it != _creators.end()) {
            throw NodeFactoryException(LOG_MSG("NodeCreator already exists"));
        }
        _creators.insert(pair<NodeType, INodeCreator*>(type, creator));
    }
};

string XMLTree(Node *node, int level = 0);

#endif
