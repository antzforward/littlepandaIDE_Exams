#include <iostream>
#include <iomanip>
using namespace std;

void MinMax(float* array, int n, float& _min, float& _max)
{
	if( !array || n == 0 )return;
	_min = _max = *array;
	for(int i=1;i<n;i++)
	{
		float value = *(array + i);
		if( _min > value ) _min = value;
		if( _max < value ) _max = value;
	}
}

int main(int argc, char** argv) {
	int n =0;
	cin>>n;
	float* arr = new float[n];
	for(int i=0; i<n; i++){
		cin>>*(arr+i);
	}
	float a,b;
	MinMax(arr, n, a,b);
	cout<<fixed<<setprecision(2)<<b<<' '<<a<<endl;
	return 0;
}
