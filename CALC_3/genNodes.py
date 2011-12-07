#!/usr/bin/env python
# -*- coding: utf8 -*-


#
#
#					// N_NUMBER
#					#define NODE_NAME NumberNode
#					#define NODE_XML_START "<number>"
#					#define NODE_XML_END "</number>"
#					#include "_Node.h"
#					#undef NODE_NAME
#					#undef NODE_XML_START
#					#undef NODE_XML_END

#	N_REAL, N_INTEGER, N_POWER, N_MOD, N_DIVISION, N_MULTIPLICATION, N_MINUS,
#		N_PLUS, N_OPENING_RBRACKET, N_CLOSING_RBRACKET,
#			
#				N_TERM1, N_TERM2, N_TERM3, N_TERM4, N_TERMN,
#					N_T1, N_T2, N_T3, N_T4, N_NUMBER};
def g1(name, xml_name=""):
	if xml_name == "":
		xml_name = name.lower();
	return ('''{0}Node'''.format((name)), ''' "<{0} />" '''.format((xml_name)), ''' "" ''')

def g2(name, xml_name=""):
	if xml_name == "":
		xml_name = name.lower();
	return ('''{0}Node'''.format((name)), ''' "<{0}>"+getToken()+"</{0}>" '''.format((xml_name)), ''' "" ''')

def g3(name, xml_name=""):
	if xml_name == "":
		xml_name = name.lower()
	return ('''{0}Node'''.format((name)), ''' "<{0}>" '''.format((xml_name)), ''' "</{0}>" '''.format((xml_name)))

def main():
#	nodes = NodeName, XMLStart, XMLEnd;
	nodes = [
			g2('Real'),
			g2('Integer'),
			g1('Power'),
			g1('Mod'),
			g1('Division'),
			g1('Multiplication'),
			g1('Minus'),
			g1('Plus'),
			g1('OpeningRBracket', 'opening_rbracket'),
			g1('ClosingRBracket', 'closing_rbracket'),
			g3('Term1'),
			g3('Term2'),
			g3('Term3'),
			g3('Term4'),
			g3('TermN'),
			g3('T1'),
			g3('T2'),
			g3('T3'),
			g3('T4'),
			g3('Number')
			]
	with open('_Nodes.h', 'wt') as f:
		f.write('''#ifndef NODES_H
#define NODES_H
''')
		f.write('''/*Nodes types:
''')
		for node in nodes:
			name = node[0]
			f.write(name + '\n')
		f.write('''*/
''')
		for node in nodes:
			f.write('''
#define NODE_NAME {0}
#define NODE_XML_START {1}
#define NODE_XML_END {2}
#include "_Node.h"
#undef NODE_NAME
#undef NODE_XML_START
#undef NODE_XML_END
'''.format(*node))
		f.write('''#endif
''')



if __name__ == '__main__':
	main()
