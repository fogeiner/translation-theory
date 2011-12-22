def int main :
	int n;
	read n;
	int i;
	for i = 2; i < n ; i = i + 1 do
		if n % i == 0 then	
			print 0;
			return 1;
		fi
	done
	print 1;
	return 0;
enddef
