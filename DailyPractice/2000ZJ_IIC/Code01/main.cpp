#include <iostream>
using namespace std;

int main(int argc, char** argv) {
	int n;
	cin>>n;
	int result = 0;
	int data = 0;
	for(int i=0; i<n;i++)
	{
		cin>>data;
		if( data %2 == 0 ) result += data;
	}
	cout<<result;
	return 0;
}
