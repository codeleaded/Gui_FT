//#include "C:/Wichtig/System/Static/Library/WindowEngine.h"
#include "/home/codeleaded/System/Static/Library/WindowEngine.h"
#include "/home/codeleaded/System/Static/Library/Random.h"
#include "/home/codeleaded/System/Static/Library/FS.h"

#define MAX_VALUES	10000
#define MAX_LENGTH	1000
#define MAX_ITERMAX	100

float actime;
float offset;
TransformedView tv;

FS fs;
Vector vVales;


Polar_Pair FS_WierdWave(int k){
	const int n = k * 2 + 1;
	const float a = n * actime;
	const float r = (4.0f / (F32_PI * (float)n));
	return (Polar_Pair){ r,a };
}
Polar_Pair FS_SquareWave(int k){
	const float n = k % 2 == 0 ? 0.0f : (1.0f / (float)k);
	const float a = k * actime;
	const float r = (4.0f / F32_PI * (float)n);
	return (Polar_Pair){ r,a };
}

void Setup(AlxWindow* w){
	tv = TransformedView_Make(
		(Vec2){ GetWidth(),GetHeight() },
		(Vec2){ 0.0f,0.0f },
		(Vec2){ 0.1f,0.1f },
		(float)GetWidth() / (float)GetHeight()
	);

	fs = FS_Func(MAX_ITERMAX,FS_SquareWave);

	actime = 0.0f;
	offset = 4.0f;
	vVales = Vector_New(sizeof(float));
}
void Update(AlxWindow* w){
	TransformedView_HandlePanZoom(&tv,window.Strokes,GetMouse());
	Rect r = TransformedView_ScreenWorldRect(&tv,GetScreenRect());

	actime += w->ElapsedTime;

	Clear(BLACK);

	FS_Render(WINDOW_STD_ARGS,&tv,0.0f,0.0f,&fs,0.0f,actime);
	
	const Vec2 out = FS_Calc(&fs,0.0,actime);
	Vector_Add(&vVales,(float*)&out.y,0);

	if(vVales.size>MAX_VALUES)
		Vector_PopTop(&vVales);

	Vec2 ppS = TransformedView_WorldScreenPos(&tv,out);
	Vec2 ptS = TransformedView_WorldScreenPos(&tv,(Vec2){ offset,*(float*)Vector_Get(&vVales,0) });
	Line_RenderX(WINDOW_STD_ARGS,ppS,ptS,WHITE,1.0f);

	for(int i = 1;i<vVales.size;i++){
		float v0 = *(float*)Vector_Get(&vVales,i-1);
		float v1 = *(float*)Vector_Get(&vVales,i);

		Vec2 pS = TransformedView_WorldScreenPos(&tv,(Vec2){ offset + (float)(i-1) / (float)MAX_LENGTH,v0 });
		Vec2 tS = TransformedView_WorldScreenPos(&tv,(Vec2){ offset + (float)(i) 	/ (float)MAX_LENGTH,v1 });
		Line_RenderX(WINDOW_STD_ARGS,pS,tS,WHITE,1.0f);
	}

	String str = String_Format("MI: %d",MAX_ITERMAX);
	CStr_RenderSizeAlxFont(WINDOW_STD_ARGS,&window.font,str.Memory,str.size,0.0f,0.0f,WHITE);
	String_Free(&str);
}
void Delete(AlxWindow* w){
	Vector_Free(&vVales);
}

int main(){
    if(Create("FTransform",1200,1200,1,1,Setup,Update,Delete))
        Start();
    return 0;
}