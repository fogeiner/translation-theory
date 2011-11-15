int f(int a, int b) {
	int c = a * b;
	return c;
}

int main(int argc, char *argv[]) {
	int a = 5;
	int b = 7;
	int c = f(a, b);
	printf("%d", c);
	return 0;
}
