#include <iostream>
using namespace std;

int main(int argc, char** argv) {
	int n;
	cin>>n;
	int* data = new int[n*n];
	int  idx = 0,idy=0;
	for(int i=0;i<n;i++)
	{
		for( int j=0;j<n;j++)
		{
			cin>>data[i*n+j];
			if( data[i*n+j] > data[idx*n + idy])
			{
				idx = i;
				idy = j;
			}
		}
	}
	cout<<data[idx*n + idy]<<" "<<idx+1<<" "<<idy+1;
	return 0;
}
