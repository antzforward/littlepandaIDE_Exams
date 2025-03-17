#include <iostream>
#include <iomanip>
#define _USE_MATH_DEFINES
#include <cstring>
#include <cmath>
using namespace std;

struct Info{
	int L,W;//长宽[100,10^5]
	int x,y;//球的初始位置[R,L-R] [R,W-R]
	int R;//球的半径[1,5]
	int a;//x正半轴逆时针的角度，就是普通角度了[0,360]
	int v;//速度
	int s;//多少时间单位之后，在什么地方 ,[1,10^5]
	Info(){
		memset(this, 0, sizeof(Info));
	}
};

Info zero;
bool operator==(const Info& lhs, const Info& rhs){
	return lhs.L == rhs.L
	&& lhs.W == rhs.W
	&& lhs.x == rhs.x
	&& lhs.y == rhs.y
	&& lhs.R == rhs.R
	&& lhs.a == rhs.a
	&& lhs.v == rhs.v
	&& lhs.s == rhs.s
	;
}
int main(int argc, char** argv) {
	Info info;
	while(1){
		cin>>info.L>>info.W>>info.x>>info.y>>info.R>>info.a>>info.v>>info.s;
		if( info == zero )
			break;
		double x=info.x, y=info.y;
		double radian = info.a * M_PI /180.0 ;
		double vx=info.v*cos(radian),vy = info.v*sin(radian);//水平与垂直的速度
		double llx = vx*info.s,lly = vy*info.s;//x轴的长度，y轴的长度
		double resultx = llx + info.x, resulty = lly + info.y;//在虚拟的连续空间中的目标点。
		double W = info.W - 2*info.R;//活动区域的范围，Y轴方面
		double L = info.L - 2*info.R;//活动区域的范围，X轴方面
		int timesX = (int)(resultx/L), timesY = (int)(resulty/W);
		resultx -= timesX * L;if(resultx <0) resultx += L;
		resulty -= timesY * W;if(resulty <0) resulty += W;
		if( resultx < info.R ) resultx = L - resultx;
		if( resultx > (info.L - info.R))  resultx = L + (info.L - info.R) - resultx;
		if( resulty < info.R ) resulty = W - resulty;
		if( resulty > (info.W - info.R))  resulty = W + (info.W - info.R) - resulty;
		cout<<fixed<<setprecision(2)<<resultx<<" "<<resulty<<endl;
		
	}
	return 0;
}
