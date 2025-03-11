#include <iostream>
using namespace std;

int main(int argc, char** argv) {
	char arr[100];
	cin.getline(arr, 100);
	int charNum=0,spaceNum=0,numberNum=0,otherNum=0;
	char c;
	char* target = arr;
	while((c=*target++) != '\0')
	{
		if(c == ' ') spaceNum++;
		else if( (c>='a'&&c<='z') || (c>='A'&&c<='Z')) charNum++;
		else if( c>='0' && c<='9') numberNum++;
		else otherNum++;
	}
	cout<<charNum<<' '<<numberNum<<' '<<spaceNum<<' '<<otherNum<<endl;
	return 0;
}
