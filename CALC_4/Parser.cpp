#include "Parser.h"
#include <cstdio>

std::string buildXMLTree(Node *root, int level) {

    std::string xml;
    std::string indent;

    for (int i = 0; i < level; ++i) {
        indent += "  ";
    }

    xml += indent;
    xml += root->XMLStart();
    xml += "\n";

    for(int i = 0; i < root->childrenCount(); ++i) {
        xml += buildXMLTree(root->get(i), level + 1);
    }

    std::string end = root->XMLEnd();

    if (end.length() != 0) {
        xml += indent;
        xml += root->XMLEnd();
        xml += "\n";
    }

    return xml;
}

std::map<std::string, Function *> Program::_functions;
std::map<std::string, bool> Program::_declarationMask;


int main2() {
    Node *pn = new ProgramNode();
    pn->addChild(new IdNode("abc"));
    ::printf("%s", buildXMLTree(pn).c_str());
    return 0;
}
