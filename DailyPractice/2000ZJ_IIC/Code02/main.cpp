#include <iostream>
using namespace std;
void del_char( char *strz, char a)
{
	if( strz == nullptr )
		return;
	char * s1,* s2;
	int idx = 0;
	while( *(strz + idx) != '\0') //字符串结尾
	{
		if(*(strz + idx) == a )//匹配了，把后面的向前移动
		{
			s1 = strz + idx;
			s2 = s1 + 1;
			while(*s2 !='\0')
			{
				*s1++ = *s2++;
			}
			*s1 = '\0';
			
		}else{
			idx++;
		}	
	}	
}

int main(int argc, char** argv) {
	char  b;
	char * str = new char [1000];
	cin>>b;
	cin>>str;
	del_char(str,b);
	cout<<str;
	delete[] str;
	return 0;
}
