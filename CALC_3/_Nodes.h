#ifndef NODES_H
#define NODES_H
/*Nodes types:
RealNode
IntegerNode
PowerNode
ModNode
DivisionNode
MultiplicationNode
MinusNode
PlusNode
OpeningRBracketNode
ClosingRBracketNode
Term1Node
Term2Node
Term3Node
Term4Node
TermNNode
T1Node
T2Node
T3Node
T4Node
NumberNode
*/

#define NODE_NAME RealNode
#define NODE_XML_START  "<real>"+getToken()+"</real>" 
#define NODE_XML_END  "" 
#include "_Node.h"
#undef NODE_NAME
#undef NODE_XML_START
#undef NODE_XML_END

#define NODE_NAME IntegerNode
#define NODE_XML_START  "<integer>"+getToken()+"</integer>" 
#define NODE_XML_END  "" 
#include "_Node.h"
#undef NODE_NAME
#undef NODE_XML_START
#undef NODE_XML_END

#define NODE_NAME PowerNode
#define NODE_XML_START  "<power />" 
#define NODE_XML_END  "" 
#include "_Node.h"
#undef NODE_NAME
#undef NODE_XML_START
#undef NODE_XML_END

#define NODE_NAME ModNode
#define NODE_XML_START  "<mod />" 
#define NODE_XML_END  "" 
#include "_Node.h"
#undef NODE_NAME
#undef NODE_XML_START
#undef NODE_XML_END

#define NODE_NAME DivisionNode
#define NODE_XML_START  "<division />" 
#define NODE_XML_END  "" 
#include "_Node.h"
#undef NODE_NAME
#undef NODE_XML_START
#undef NODE_XML_END

#define NODE_NAME MultiplicationNode
#define NODE_XML_START  "<multiplication />" 
#define NODE_XML_END  "" 
#include "_Node.h"
#undef NODE_NAME
#undef NODE_XML_START
#undef NODE_XML_END

#define NODE_NAME MinusNode
#define NODE_XML_START  "<minus />" 
#define NODE_XML_END  "" 
#include "_Node.h"
#undef NODE_NAME
#undef NODE_XML_START
#undef NODE_XML_END

#define NODE_NAME PlusNode
#define NODE_XML_START  "<plus />" 
#define NODE_XML_END  "" 
#include "_Node.h"
#undef NODE_NAME
#undef NODE_XML_START
#undef NODE_XML_END

#define NODE_NAME OpeningRBracketNode
#define NODE_XML_START  "<opening_rbracket />" 
#define NODE_XML_END  "" 
#include "_Node.h"
#undef NODE_NAME
#undef NODE_XML_START
#undef NODE_XML_END

#define NODE_NAME ClosingRBracketNode
#define NODE_XML_START  "<closing_rbracket />" 
#define NODE_XML_END  "" 
#include "_Node.h"
#undef NODE_NAME
#undef NODE_XML_START
#undef NODE_XML_END

#define NODE_NAME Term1Node
#define NODE_XML_START  "<term1>" 
#define NODE_XML_END  "</term1>" 
#include "_Node.h"
#undef NODE_NAME
#undef NODE_XML_START
#undef NODE_XML_END

#define NODE_NAME Term2Node
#define NODE_XML_START  "<term2>" 
#define NODE_XML_END  "</term2>" 
#include "_Node.h"
#undef NODE_NAME
#undef NODE_XML_START
#undef NODE_XML_END

#define NODE_NAME Term3Node
#define NODE_XML_START  "<term3>" 
#define NODE_XML_END  "</term3>" 
#include "_Node.h"
#undef NODE_NAME
#undef NODE_XML_START
#undef NODE_XML_END

#define NODE_NAME Term4Node
#define NODE_XML_START  "<term4>" 
#define NODE_XML_END  "</term4>" 
#include "_Node.h"
#undef NODE_NAME
#undef NODE_XML_START
#undef NODE_XML_END

#define NODE_NAME TermNNode
#define NODE_XML_START  "<termn>" 
#define NODE_XML_END  "</termn>" 
#include "_Node.h"
#undef NODE_NAME
#undef NODE_XML_START
#undef NODE_XML_END

#define NODE_NAME T1Node
#define NODE_XML_START  "<t1>" 
#define NODE_XML_END  "</t1>" 
#include "_Node.h"
#undef NODE_NAME
#undef NODE_XML_START
#undef NODE_XML_END

#define NODE_NAME T2Node
#define NODE_XML_START  "<t2>" 
#define NODE_XML_END  "</t2>" 
#include "_Node.h"
#undef NODE_NAME
#undef NODE_XML_START
#undef NODE_XML_END

#define NODE_NAME T3Node
#define NODE_XML_START  "<t3>" 
#define NODE_XML_END  "</t3>" 
#include "_Node.h"
#undef NODE_NAME
#undef NODE_XML_START
#undef NODE_XML_END

#define NODE_NAME T4Node
#define NODE_XML_START  "<t4>" 
#define NODE_XML_END  "</t4>" 
#include "_Node.h"
#undef NODE_NAME
#undef NODE_XML_START
#undef NODE_XML_END

#define NODE_NAME NumberNode
#define NODE_XML_START  "<number>" 
#define NODE_XML_END  "</number>" 
#include "_Node.h"
#undef NODE_NAME
#undef NODE_XML_START
#undef NODE_XML_END
#endif
