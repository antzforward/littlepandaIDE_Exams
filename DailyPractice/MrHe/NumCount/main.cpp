#include <queue>
#include <cstdio>
using namespace std;
int main() {
	int n;
	queue<int> task;
	queue<int> result;
	scanf("%d",&n);
	if(!(n>0&&n<=9)){//非一个正整数的形式。
		printf("0");
		return -1;
	}
	task.push(n);
	while(!task.empty()){
		//取出任务组第一个任务
		int d = task.front();
		task.pop();
		//自己先完成
		result.push(d);
		//获得首数字：firstNumber，以及首数字的位数
		int firstNumber;
		int d1 = d;
		int multiVaule = 1;//默认个位
		while(d1!=0){
			firstNumber = d1%10;//取尾数，比如1234，取4
			d1 /= 10;//向后移动一个位数，比如1234 变成123
			multiVaule *= 10;//位数前进一位，倍数增加10倍
		}
		firstNumber /= 2;//取一半
		if( firstNumber ==  0 )//没有数字可以添加
			continue;
		//添加新的数字到任务队列中
		for(int i=1;i<=firstNumber;i++){
			task.push(i*multiVaule + d);
		}
	}
	printf("%d\n",result.size());
	return 0;
}
