#include <iostream>
#include <cmath>
#include <iomanip>
using namespace std;
double calc(double x)
{
	if( x<-2.0) return x*x -sin( x );
	else if (x <=2.0) return pow(2,x)+x;
	else return sqrt( x*x + x + 1.0);
}

int main(int argc, char** argv) {
	int n;
	cin>>n;
	double x;
	while(n--)
	{
		cin>>x;
		cout<<fixed<<setprecision(2)<<calc(x)<<endl;
	}
	return 0;
}
