def int getPi 
int a:
	return 3;
enddef

def int main
int argc :
	int a;
	int b;
	int d;
	int c;
	c = a + b;
	read a;
	while [[a > 10 and c < 1] or b < 3] do
		d = ((a * b + c) - a * a) % b + {getPi a};
		print d;
	done
enddef

