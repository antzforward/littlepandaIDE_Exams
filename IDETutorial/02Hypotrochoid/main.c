#include <raylib.h>
#include <rdrawing.h>
#include <math.h>

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

void updateRadius(int baseL, int innerL,int pointL, int *pBaseR, int *pInnerR, int *pPointR) {
	int totalL=baseL;
	if (innerL>baseL) {
		if (innerL>pointL)
			totalL = (2*innerL-baseL);
		else {
			totalL = (innerL+pointL-baseL);
		}
	} else {
		if (pointL>innerL)
			totalL = baseL-innerL+pointL;
	} 
	int totalR = 320;
	int remainder = totalR % totalL;
	if (remainder!=0) {
		if (remainder < totalL / 2) {
			totalR -= remainder;
		} else {
			totalR += ( totalL - remainder);
		}
	}
	*pBaseR = totalR / totalL * baseL;
	*pInnerR = totalR / totalL * innerL;		
	*pPointR = totalR / totalL * pointL;
}


int main() {
	int baseL=6;
	int innerL=3;
	int pointL=6;
	int baseR,innerR,pointR;
	int cx=350,cy=350;
	int speed = 1;
	Color trackColor = BLUE;
	updateRadius(baseL, innerL, pointL, &baseR, &innerR, &pointR);
	InitWindow(1100,700,"Hypotrochoid");
	SetTraceLogLevel(LOG_WARNING);
	SetTargetFPS(60);
	GuiSetStyle(DEFAULT,TEXT_SIZE,20);
	
	Image trackImage=GenImageColor(700,700,WHITE);
	//border
	ImageFillRectangleEx(&trackImage,0,0,700,700,LIGHTGRAY);
	ImageFillRectangleEx(&trackImage,5,5,690,690,WHITE);
	
	Image circlesImage = GenImageColor(700,700,BLANK);
	float r=0;
	int lastx,lasty;
	bool skip=true;
	lasty=cy;
	lastx=cx+baseR;
	int frameCount = 0;
	while(!WindowShouldClose()) {
		//GUI
		int newInnerL = GuiSliderBar((Rectangle){ 70, 20, 200, 30 },"Inner",TextFormat("%i", (int)innerL), innerL, 1, 50);
		int newBaseL = GuiSliderBar((Rectangle){ 70, 60, 200, 30 },"Base",TextFormat("%i", (int)baseL), baseL, 1, 50);
		int newPointL = GuiSliderBar((Rectangle){ 70, 100, 200, 30 },"Point",TextFormat("%i", (int)pointL), pointL, 1, 50);
		speed = GuiSliderBar((Rectangle){ 70, 150, 200, 30 },"Speed",TextFormat("%i", (int)speed), speed, 1, 50);
		GuiLabel((Rectangle){ 20, 220, 200, 30 },TextFormat("Color: 0x%02X%02X%02X ",(int)(trackColor.r), (int)(trackColor.g),(int)(trackColor.b)));
		trackColor= GuiColorPicker((Rectangle){ 50, 250, 196, 192 }, NULL, trackColor);
		int doClear = GuiButton((Rectangle){ 120, 500, 80, 30 },"Clear");
		if (newInnerL!=innerL || newBaseL!=baseL || newPointL!=pointL) {
			if (newInnerL!=innerL) 
				pointL=newInnerL;
			else
				pointL=newPointL;
			innerL=newInnerL;
			baseL=newBaseL;
			updateRadius(baseL, innerL, pointL, &baseR, &innerR, &pointR);
			r=0;
			skip=true;
			ImageClearBackground(&trackImage,WHITE);
			ImageFillRectangleEx(&trackImage,0,0,700,700,LIGHTGRAY);
			ImageFillRectangleEx(&trackImage,5,5,690,690,WHITE);			
		} else if (doClear) {
			ImageClearBackground(&trackImage,WHITE);
			ImageFillRectangleEx(&trackImage,0,0,700,700,LIGHTGRAY);
			ImageFillRectangleEx(&trackImage,5,5,690,690,WHITE);						
		}
		//update datas
		float innerCX=cx+ (baseR-innerR)*cos(r);
		float innerCY=cy+ (baseR-innerR)*sin(r);
		int x,y;
		float theta;
		if (innerL<baseL) {
			theta = r * (baseL-innerL) / innerL;
			x=round(innerCX + pointR * cos(theta));
			y=round(innerCY - pointR * sin(theta));		
		} else { 
			theta = r * (innerL-baseL) / innerL;
			x=round(innerCX + pointR * cos(theta));
			y=round(innerCY + pointR * sin(theta));		
		}
		
		//update image (in CPU)
		//ImageClearBackground(&trackImage,WHITE);
		if (!skip)
			ImageDrawLineEx(&trackImage,lastx,lasty,x,y,3,trackColor);
		
		frameCount++;
		if (frameCount>=speed) {
			ImageClearBackground(&circlesImage,BLANK);
			//base circle
			ImageDrawCircleEx(&circlesImage,cx,cy,baseR,1,LIGHTRED);
			ImageDrawCircleEx(&circlesImage,innerCX,innerCY,innerR,1,LIGHTGRAY);
			ImageDrawLineEx(&circlesImage,innerCX,innerCY,cx,cy,1,LIGHTRED);
			ImageDrawLineEx(&circlesImage,innerCX,innerCY,x,y,1,LIGHTGRAY);
			ImageDrawPointEx(&circlesImage,x,y,7,RED);
			
			//Drawing in GPU
			Texture trackTexture = LoadTextureFromImage(trackImage);
			Texture circlesTexture = LoadTextureFromImage(circlesImage);
			BeginDrawing();
			ClearBackground(WHITE);
			DrawTexture(trackTexture,300,0,WHITE);
			DrawTexture(circlesTexture,300,0,WHITE);
			EndDrawing();
			UnloadTexture(circlesTexture);
			UnloadTexture(trackTexture);
			frameCount=0;			
		}
		lastx=x;
		lasty=y;
		skip=false;
		r+=0.01;
	}
	
	//Clean up
	UnloadImage(circlesImage);
	UnloadImage(trackImage);
	CloseWindow();
}
