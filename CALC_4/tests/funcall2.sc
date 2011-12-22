def int f 
int b:
	return b;
enddef

def int main :
	int a;
	read a;
	print a * {f a};
	return 0;
enddef

