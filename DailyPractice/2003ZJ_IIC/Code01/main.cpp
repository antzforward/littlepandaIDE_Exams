#include <iostream>
#include <cmath>
using namespace std;
int main(int argc, char** argv) {
	int data[10];
	for( int i=0;i<10;i++){
		cin>>data[i];
	}
	double result = 0;
	for( int i=0;i<10;i++){
		result += data[i];
	}
	double mean = result/10;
	int count = 0;
	for( int i=0;i<10;i++) {
		if( data[i] > mean ) count++;
	}
	cout<<count;
	return 0;
}
