#include <iostream>
using namespace std;
int main(int argc, char** argv) {
	int n=0;
	int numbers = 0;
	while(1)
	{
		numbers = 0;
		cin>>n;
		if( n == 0) goto Exit01;
		while(1){
			numbers += n/3;
			n = n/3 + n%3;
			if( n<3){
				if( n == 2 )
				{
					numbers++;
				}
				break;
			} 
		}
		cout<<numbers<<endl;
	}	
Exit01:
	return 0;
}
