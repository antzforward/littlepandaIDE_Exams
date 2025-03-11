#include <iomanip>
#include <iostream>
#include <limits>
#include <numbers>
#include <cmath>
using namespace std;

int main(int argc, char** argv) {
	double x;
	cin>>x;
	double result = 0.0;
	if( x<0)
		result = fabs(x);
	else if( x<2.0)
		result = sqrtf( x + 1.0 );
	else if( x<4.0)
		result = pow( x + 2, 3);
	else
		result = 2*x+5;
	cout<<fixed<<setprecision(2)<<result;
	return 0;
}
