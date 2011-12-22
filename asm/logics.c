int main() {
	int a = 2;
	int b = 3;
	int c = 4;
	__asm("#a = 2, b = 3, c = 4");

	__asm("# if (a < b) && (b > c)");
	if ( (a < b) && (b > c)) {
		__asm("# if-then");
	} else {
		__asm("# if-else");
	}
	__asm("# endif");

	__asm("# if ((a < b) && (b > c)) || (a > c)");
	if ( ((a < b) && (b > c)) || (a > c)) {
		__asm("# if-then");
	} else {
		__asm("# if-else");
	}
	return 0;
}
