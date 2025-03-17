#include <iostream>
#include <string>
#include <cctype>
using namespace std;

int main(int argc, char** argv) {
	int correctCount = 0;
	string line;
	while( getline(cin,line))
	{
		int a,b,c;
		char op,eq;
		// 解析输入的格式 a+b=c 或 a-b=c
		int pos = line.find('+'); // 查找加号的位置
		if (pos != string::npos) { // 如果找到加号
			op = '+';
			a = stoi(line.substr(0, pos)); // 提取 a
			b = stoi(line.substr(pos + 1, line.find('=') - pos - 1)); // 提取 b
		} else {
			pos = line.find('-'); // 查找减号的位置
			op = '-';
			a = stoi(line.substr(0, pos)); // 提取 a
			b = stoi(line.substr(pos + 1, line.find('=') - pos - 1)); // 提取 b
		}
		// 提取等号后的答案 c
		string result = line.substr(line.find('=') + 1);
		if (result == "?") { // 如果答案是 "?"，跳过这道题
			continue;
		} else {
			c = stoi(result); // 将答案转换为整数
		}
		// 判断答案是否正确
		if (op == '+' && a + b == c) {
			correctCount++;
		} else if (op == '-' && a - b == c) {
			correctCount++;
		}
		
	}
	
	// 输出答对的题目数量
	cout << correctCount << endl;
	return 0;
}
