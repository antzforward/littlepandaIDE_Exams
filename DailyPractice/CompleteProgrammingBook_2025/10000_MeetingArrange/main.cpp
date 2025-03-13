#include <iostream>
#include <algorithm>
using namespace std;

struct MeetTime
{
	int start;
	int end;
};

bool Compare(MeetTime& a, MeetTime& b )
{
	if( a.end < b.end)
		return true;
	return false;
}
int main(int argc, char** argv) {
	int n=0;
	cin>>n;
	MeetTime* mts = new MeetTime[n];
	for(int i=0;i<n;i++){
		cin>>(mts+i)->start>>(mts+i)->end;
	}
	sort(mts, mts+n, Compare);
	int endT = mts->end;
	int num = 1;
	for(int i=1;i<n;i++){
		if( endT <= (mts+i)->start )
		{
			endT = (mts+i)->end;
			num++;
		}
	}
	cout<<num;
	delete[] mts;
	return 0;
}
