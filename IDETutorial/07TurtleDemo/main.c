#include <rturtle.h>

int main() {
	int n;
	initWorld(800,600);

//  Press F2 toggle display of grids
//  Press F3 toggle display of the turtle
//  Press F5 capture screen

    setSpeed(500);
//  setRewind(true);
//  setImmediate(true);
    setPenSize(1);

	n=50;
	float delta = 1.0f/n;
	for (int i=0;i<n;i++) {
		Color color = {delta*255,delta*255,255,255};
		setPenColor( color );
		for (int j=0;j<4;j++) {
			fd(200);
			lt(90);
		}
				
		lt(360.0/n);
	}
	waitClick();

	return 0;
}
