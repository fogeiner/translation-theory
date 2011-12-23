def int isPrime
int n:
	int i;
	for i = 2 ; i < n - 1 ; i = i + 1 do
		if n % i == 0 then
			return 0;
		fi
	done
	return 1;
enddef

def int main :
	int n;
	read n;
	int i;
	for i = 2 ; i < n ; i = i + 1 do
		if {isPrime i} == 1 then
			print i;
		fi
	done
enddef

