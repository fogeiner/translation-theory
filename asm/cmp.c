int main(int argc) {
	int a = 1;
	int b = 10;

	__asm("# if a > b");
	if (a > b) {
		__asm("# if-then");
	} else {
		__asm("# if-else");
	}
	__asm("# endif");
	
	__asm("# if a >= b");
	if (a >= b) {
		__asm("# if-then");
	} else {
		__asm("# if-else");
	}
	__asm("# endif");

	__asm("# if a < b");
	if (a < b) {
		__asm("# if-then");
	} else {
		__asm("# if-else");
	}
	__asm("# endif");

	__asm("# if a <= b");
	if (a <= b) {
		__asm("# if-then");
	} else {
		__asm("# if-else");
	}
	__asm("# endif");

	__asm("# if a == b");
	if (a == b) {
		__asm("# if-then");
	} else {
		__asm("# if-else");
	}
	__asm("# endif");

	__asm("# if a != b");
	if (a != b) {
		__asm("# if-then");
	} else {
		__asm("# if-else");
	}
	__asm("# endif");
	return 0;
}
