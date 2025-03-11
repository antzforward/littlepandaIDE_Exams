#include <iostream>
#include<iomanip>
using namespace std;

double fact(int k)
{
	double result = 0.0;
	double nsteps = 1.0;
	for( int i=1;i<=k;i++)
	{
		nsteps /= i;
		result += nsteps;
	}
	return result;
}
int main(int argc, char** argv) {
	int n=1;
	cin>>n;
	cout<<"sum="<<fixed<<setprecision(5)<<fact(n);
	return 0;
}
