// A fast IO program
#include <iostream>  // 只需要这个头文件
using namespace std;
int taskCount(int n)
{
	if(n == 0)
		return 0;
	if( n==1 ) //快速终止
		return 1;
	int count = 1;//包含自身
	n = n/2;
	for(int i=1;i<=n;i++){
		count += taskCount(i);
	}
	return count;
}
int main()
{
	// added the two lines below
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);   
	int n;
	cin >> n ;
	bool isPositive = n>=1 && n<<9;
	if( !isPositive ){
		cout<<0;
		return -1;
	}
	//任务本身就是个递归转换形式，这里用传统递归的方式
	cout << taskCount(n);
	return 0;
}
