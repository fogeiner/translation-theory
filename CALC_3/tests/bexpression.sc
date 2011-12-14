def int main
int argc :
	int a;
	int b;
	int d;
	float c;
	c = a + b;
	read a;
	while [[a > 10 and c < 1.0] or b < 3] do
		d = ((a * b + c) - a * a) % b;
		print d;
	done
enddef
