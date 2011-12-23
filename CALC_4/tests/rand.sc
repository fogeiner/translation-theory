def int rand
enddef

def int srand
int seed
enddef

def int time
int t
enddef

def int main:
	int r;
	r = {srand {time 0}};
	int i;
	for i = 0 ; i < 10 ; i = i + 1 do
		print {rand};
	done
	return 0;
enddef
	
	
