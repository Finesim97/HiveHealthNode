#include <iostream>

struct Time{
int time;
};



Time times[] = {{60000},{120000},{19000}};


int gcd(int a, int b){
	while(b!=0){
		int h = a%b;
		a = b;
		b = h;
	}	
	return a;
}

int gcdSleeptime(Time* _times, int n){
	int res=times[0].time;
	for(int i=1;i<n;i++){
		res=gcd(res,times[i].time);
	}
	return res;
}

int main(){
 std::cout << gcdSleeptime(times,3) << std::endl;
 return 0;
}


