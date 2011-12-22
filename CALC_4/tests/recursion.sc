def int fact
int n :
	int n2;
	n2  = {fact (n-1)};
	return n * n2;
enddef

def int main :
	print {fact 5};
	return 0;
enddef
