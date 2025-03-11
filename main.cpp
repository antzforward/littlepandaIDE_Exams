#include <iostream>

int main(int argc, char** argv) {
	int data[10];
	int min_index = 0;
	for(int i=0; i<10;i++)
	{
		std::cin>>data[i];
		if( data[i] < data[min_index])
			min_index = i;
	}
	int temp = data[9];
	data[9] = data[min_index];
	data[min_index] = temp;
	for(int i=0; i<10;i++)
	{
		std::cout<<data[i]<<" ";
	}
		
	return 0;
}
