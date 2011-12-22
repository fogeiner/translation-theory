def int main:
	int a;
	int b;
	int c;
	int d;

	a = 3;
	b = 2;
	c = 2;
	d = 1;

#<
	if a < b then
		print 1;
	else
		print 0;
	fi
#>
	if b > a then
		print 1;
	else
		print 0;
	fi
#<=
	if b <= c then
		print 1;
	else
		print 0;
	fi
#>=
	if b >= c then
		print 1;
	else
		print 0;
	fi
#==
	if b == c then
		print 1;
	else
		print 0;
	fi
#!=
	if a != d then
		print 1;
	else
		print 0;
	fi

	if false then
		print 10;
	else
		print 11;
	fi

	if not true then
		print 20;
	else
		print 21;
	fi

	return 0;
enddef
