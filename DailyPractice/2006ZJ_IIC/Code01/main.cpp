#include <iostream>
using namespace std;

int main(int argc, char** argv) {
	int good=0,pass=0,fail=0;
	int score = 0;
	do{
		cin>>score;
		if( score <=0)
			break;
		if( score>=85)
			good++;
		else if( score>60)
			pass++;
		else
			fail++;
	}while(1);
	cout<<">=85:"<<good<<endl;
	cout<<"60-84:"<<pass<<endl;
	cout<<"<60:"<<fail<<endl;
	return 0;
}
