def int gcd
int a,
int b :
	int c;
	while b != 0 do
		c = a % b;
		a = b;
		b = c;
	done

	return a;
enddef

def int main
int argc :
	int a;
	int b;
	read a;
	read b;
	int c;
	c = {gcd a, b};
	print c;
	return 0;
enddef
