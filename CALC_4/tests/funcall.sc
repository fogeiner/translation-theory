def int f
int a,
int b
enddef

def int main
int argc,
int argv :
	int a;
	a = {f 6, 5};
	print a;
	return 0;
enddef

def int f
int a,
int b :
	return a - b;
enddef
