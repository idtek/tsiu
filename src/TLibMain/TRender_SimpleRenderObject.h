#ifndef __TRENDER_SIMPLE_RENDER_OBJECT__
#define __TRENDER_SIMPLE_RENDER_OBJECT__


#include "TRender_Color.h"
#include "TEngine_Object.h"

namespace TsiU
{
	class Texture;

	enum
	{
		e_SimpleRenderObject_Type_1,
		e_SimpleRenderObject_Type_2,
		e_SimpleRenderObject_Type_1_WithTex
	};
	
	class SimpleRenderObject : public DrawableObject
	{
	public:
		virtual ~SimpleRenderObject(){}
		
		virtual void Create(){};
		virtual void Tick(f32 _fDeltaTime){};
		virtual void Draw(){}

		u32 m_ulType;
	};

	class SimpleRenderObjectUtility
	{
	public:
		//for 3d
		virtual void DrawObject(SimpleRenderObject* _poObj){};
		virtual SimpleRenderObject* CreateCube(f32 width, f32 height, f32 depth, D_Color color){
			return NULL;
		};
		virtual SimpleRenderObject* CreateCubeWithTex(f32 _fWidth, f32 _fHeight, f32 _fDepth, D_Color _Color, Texture* _poTex){
			return NULL;
		};
		virtual SimpleRenderObject* CreateSphere(f32 _fRadius, D_Color _Color){
			return NULL;
		};

		//for 2d
		virtual void DrawString(f32 x, f32 y, const Char* str, D_Color clr){};
		virtual void DrawStringEx(f32 x, f32 y, f32 width, f32 height, f32 size, const Char* str, const Char* fontName, D_Color clr){};
		virtual void DrawPixel(f32 x, f32 y, D_Color clr){};
		virtual void DrawLine(f32 x1, f32 y1, f32 x2, f32 y2, D_Color clr, f32 lineWidth){};
		virtual void DrawCircle(f32 x, f32 y, f32 radius, D_Color clr, f32 lineWidth){};
		virtual void DrawRectangle(f32 x, f32 y, f32 width, f32 height, D_Color clr, f32 lineWidth){};
		virtual void DrawFillRectangle(f32 x, f32 y, f32 width, f32 height, D_Color clr, D_Color outclr){};
		virtual void DrawFillCircle(f32 x, f32 y, f32 radius, D_Color clr, D_Color outclr){};
	};
}

#endif