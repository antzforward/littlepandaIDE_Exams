#include <iostream>

int main(int argc, char** argv) {
	double x;
	int n;
	std::cin>>x>>n;
	double sum = 0.0;
	int multi = 1;
	double value = 1.0;
	double sign = -1.0;
	for( int i=1;i<=n;i++){
		value *= x;
		multi *= i;
		sign = -sign;
		sum += value * sign / multi;
	}
	std::cout<<sum;
	return 0;
}

