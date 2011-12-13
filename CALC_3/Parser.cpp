#include "Parser.h"
#include <cstdio>

std::string buildXMLTree(Node *root, int level = 0) {

    std::string xml;
    std::string indent;

    for (int i = 0; i < level; ++i) {
        indent += "  ";
    }

    xml += indent;
    xml += root->XMLStart();
    xml += "\n";

    for(int i = 0; i < root->childrenCount(); ++i) {
        xml += buildXMLTree((*root)[i], level + 1);
    }

    std::string end = root->XMLEnd();

    if (end.length() != 0) {
        xml += indent;
        xml += root->XMLEnd();
        xml += "\n";
    }

    return xml;
}


int main() {
    Node *pn = new ProgramNode();
    pn->addChild(new IdNode("abc"));
    ::printf("%s", buildXMLTree(pn).c_str());
    return 0;
}
