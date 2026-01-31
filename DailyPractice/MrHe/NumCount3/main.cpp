#include <iostream>
#include <vector>
using namespace std;

long long taskCount(int n) {
	if (n == 0) return 0;
	if (n == 1) return 1;
	
	vector<long long> dp(n + 1);
	dp[0] = 0;
	dp[1] = 1;
	
	for (int i = 2; i <= n; i++) {
		dp[i] = 1; // 包含自身
		int half = i / 2;
		for (int j = 1; j <= half; j++) {
			dp[i] += dp[j];
		}
	}
	
	return dp[n];
}

int main() {
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);
	
	int n;
	cin >> n;
	
	if (n < 1 || n >= (1 << 9)) { // 修正条件判断
		cout << 0;
		return 0;
	}
	
	cout << taskCount(n);
	return 0;
}
