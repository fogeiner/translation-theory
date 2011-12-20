def int main
int argc,
int argv :
	read argc;
	print argc;
	print argv;
	int a;
	a = 10;

	if true then
		argv = argv * 2 + 1 / argc;
	fi

	if false then
		a = 10;
	else
		a = 10 + 1;
	fi

	while [ a > 10 ] do
		a = a - 1;
	done

	return -(argc + argv) + a;
enddef
