#include <iostream>

class blocked_range {
	int YourVar;
public:
	blocked_range(int _begin_, int _end_, size_t grainsize_=1):
		my_end(_end_),my_start(_begin_),my_grainsize(grainsize_)
	{
		
	}
	const int begin()const{return my_start;}
	const int end()const{return my_end;}
	const size_t grainsize()const{ return my_grainsize;}
	size_t size()const{
		return size_t(my_end - my_start);
	}
	
	virtual ~blocked_range() =default;
	void YourMethodOrFunction() {
		
	}
private:
	int my_end;
	int my_start;
	size_t my_grainsize;
};
int main(int argc, char** argv) {
	return 0;
}
