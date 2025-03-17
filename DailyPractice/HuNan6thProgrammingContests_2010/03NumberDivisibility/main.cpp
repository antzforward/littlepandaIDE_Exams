#include <iostream>
#include <string>
using namespace std;
int main(int argc, char** argv) {
	string line;
	while(1){
		getline(cin, line);
		if( line == "0") break;
		int len = line.size();
		int rIndex = len-1;
		
		//这里要改一下，这其实是同余的做法，但是它的例题数据太小了。
		//定理：把一个至少两位的正整数的个位数字去掉，再从余下的数中减去个位数的5倍。当且仅当差是17的倍数时，原数也是17的倍数 
		//假设初始的数字是长为n的正整数[1,10100],首先取位数a，前n-1数够成b，如果5*a，b同余，则b*10+a 是17的倍数。
		//可以转换为，5*a%17 = c，然后构造新的数 b-c，然后判断这个n-1位数是否是17的倍数。
		//可以看到数的位数在逐渐减少，可以通过递归的方式来处理。
		while(rIndex>=3){
			int a = line[rIndex] - '0';
			a *= 5; //a = a*5
			a %= 17;//余数。
			int b = (line[rIndex-1] - '0') + (line[rIndex-2] - '0')*10 + (line[rIndex-3] - '0')*100; //防止出现负数
			b -= a;
			line[rIndex-1] = b%10 + '0';
			b = b/10;	
			line[rIndex-2] = b%10 + '0';
			b = b/10;	
			line[rIndex-3] = b%10 + '0';
			rIndex--;
		}
		//最后三位可以直接转成整数进行处理即可。
		{
			int a = line[rIndex] - '0';
			a *=5; //a = a*5
			a %= 17;//余数。
			int b = 0 ;
			if( rIndex - 1>=0) b = (line[rIndex-1] - '0');
			if( rIndex - 2>=0) b += (line[rIndex-2] - '0')*10;
			if( rIndex - 3>=0) b += (line[rIndex-3] - '0')*100;
			if( b%17 == a )
			{
				cout<<1<<endl;
			}else cout<<0<<endl;	
		}
	}
	return 0;
}
