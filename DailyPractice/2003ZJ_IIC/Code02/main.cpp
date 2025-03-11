#include <iostream>
#include <cmath>
using namespace std;
bool isPrimer(int num){
	if(num == 1 )return false;
	if(num == 2) return true;
	int stop = int(sqrt(num) + 0.5);
	for( int i=2; i<=stop; i++)
	{
		if( num % i == 0 )
			return false;
	}
	return true;
}
int main(int argc, char** argv) {
	int m,n;
	cin>>m>>n;
	int result = 0;
	for(int i=m; i<=n;i++)
	{
		if(isPrimer(i)) result += i;
	}
	cout<<result;
	return 0;
}
