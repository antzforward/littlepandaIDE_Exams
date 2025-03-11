#include <iostream>
#include <iomanip>
using namespace std;
float Sum(int n){
	float result=0.f,value=1.0f;
	for(int i=1;i<=n;i++){
		value /= i;
		result += value;
	}
	return result;
}
int main(int argc, char** argv) {
	int n;
	cin>>n;
	cout<<fixed<<setprecision(4)<<Sum(n)<<endl;
	return 0;
}
