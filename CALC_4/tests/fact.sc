def int fact
int n :
	
	if n == 0 then
		return 1;
	else 
		return n * {fact n - 1};
	fi
enddef

def int main :
	int n;
	read n;
	print {fact n};
	return 0;
enddef
