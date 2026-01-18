#include <iostream>
using namespace std;

#include <rttr/registration>
using namespace rttr;

struct MyStruct { 
	MyStruct() {}; 
	void func(double) {}; 
	int data; 
};

RTTR_REGISTRATION
{
	registration::class_<MyStruct>("MyStruct")
	.constructor<>()
	.property("data", &MyStruct::data)
	.method("func", &MyStruct::func);
}


int main(int argc, char** argv) {
	return 0;
}
