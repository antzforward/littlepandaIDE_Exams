#include <iostream>
#include <iomanip>
#define _USE_MATH_DEFINES
#include <cstring>
#include <cmath>
using namespace std;

struct Info {
	int L, W;
	int x, y;
	int R;
	int a;
	int v;
	int s;
	Info() {
		L = W = x = y = R = a = v = s = 0;
	}
};

bool operator==(const Info& lhs, const Info& rhs) {
	return lhs.L == rhs.L && lhs.W == rhs.W &&
	lhs.x == rhs.x && lhs.y == rhs.y &&
	lhs.R == rhs.R && lhs.a == rhs.a &&
	lhs.v == rhs.v && lhs.s == rhs.s;
}

// 计算镜像后的坐标
double calculateFinalPosition(double initial, double delta, double length) {
	double total = initial + delta;
	int n = static_cast<int>(floor(total / length));
	double remainder = fmod(total, length);
	if (remainder < 0) {
		remainder += length;
		n = static_cast<int>(floor((total) / length)); // 调整n的值
	}
	if (n % 2 == 0) { //核心在这里，镜像有往返的效果，当为偶数时，刚好布局相同。
		return remainder;
	} else {
		return length - remainder;
	}
}

int main() {
	Info info;
	while (cin >> info.L >> info.W >> info.x >> info.y >> info.R >> info.a >> info.v >> info.s) {
		if (info == Info()) break;
		
		double radian = info.a * M_PI / 180.0;
		double vx = info.v * cos(radian);
		double vy = info.v * sin(radian);
		
		// 转换为有效区域坐标（相对于R的起点）
		double x_eff = info.x - info.R;
		double y_eff = info.y - info.R;
		double L_eff = info.L - 2 * info.R;
		double W_eff = info.W - 2 * info.R;
		
		// 计算总位移
		double dx = vx * info.s;
		double dy = vy * info.s;
		
		// 处理X和Y方向的镜像
		double x_final_eff = calculateFinalPosition(x_eff, dx, L_eff);
		double y_final_eff = calculateFinalPosition(y_eff, dy, W_eff);
		
		// 转换回原坐标系
		double x_final = info.R + x_final_eff;
		double y_final = info.R + y_final_eff;
		
		// 确保坐标在有效范围内
		x_final = max((double)info.R, min((double)info.L - info.R, x_final));
		y_final = max((double)info.R, min((double)info.W - info.R, y_final));
		
		cout << fixed << setprecision(2) << x_final << " " << y_final << endl;
	}
	return 0;
}
