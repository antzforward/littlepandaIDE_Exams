#include <iostream>
#include <iomanip>
using namespace std;

int main(int argc, char** argv) {
	for(int s=-100;s<=150; s+=5)
	{
		cout<<"c="<<s<<"->f="<<fixed<<setprecision(0)<<(32+s*9/5)<<endl;
	}
	return 0;
}
