#include <iostream>
#include <algorithm>
using namespace std;

/**
 * 街道分n段从1..n，居民给出m个建议
 * 建议为，第i到第j的路段，至少要中tt棵树
 * 问题：如果要满足所有建议，至少要种多少棵树。
 * 
 * 思路：还是贪心算法
 */


struct node{
	int begin;
	int end;
	int trees;
}sugg[5001];

bool cmp( node& x, node& y){
	return x.end < y.end;
}

int main(int argc, char** argv) {
	
	int n,h;
	cin>>n>>h;
	for(int i=1;i<=h;i++){
		cin>>sugg[i].begin>>sugg[i].end>>sugg[i].trees;
	}
	
	sort( sugg+1, sugg+1+h, cmp);
	
	return 0;
}
