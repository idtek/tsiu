#include "TUI_Module.h"
#include "TUI_Object.h"
#include "TRender_Renderer.h"
#include "TMath_Matrix4.h"
#include "TRender_Enum.h"
#include "TUI_DisplayStack.h"
#include "TUI_Modifier.h"


namespace TsiU
{

	UIObject::UIObject(): mColor(0xFFFFFFFF),mRegion(0,0, 100, 100)
	{
	}

	void UIObject::Draw()
	{
		BeforeDraw();		

		DrawSelf();

		AfterDraw();
	}

	void UIObject::DrawSelf()
	{
		UIDisplayStack * dspStack = spUIModule->GetDisplayStack();
		Mat4 curMatrix = dspStack->GetCurrentMatrix();

		
		short l, t, w, h;
		UIModifier * mod = dspStack->GetCurrentModifier();
		if(mod->mPointModifiers.find("MaterialPosition") != mod->mPointModifiers.end())
		{
			l = mod->mPointModifiers["MaterialPosition"].GetX();
			t = mod->mPointModifiers["MaterialPosition"].GetY();
		}
		else
		{
			l =  mMaterialPosition.GetX();
			t =	 mMaterialPosition.GetY();
		}

		if(mod->mPointModifiers.find("MaterialSize") != mod->mPointModifiers.end())
		{
			w = mod->mPointModifiers["MaterialSize"].GetX();
			h = mod->mPointModifiers["MaterialSize"].GetY();
		}
		else
		{
			w = mMaterial.GetWdith();
			h = mMaterial.GetHeight();
		}
		Mat4 m(
			l,		t,		1.f, 1.f,
			l,		t + h,	1.f, 1.f,
			l+w,	t + h,	1.f, 1.f,
			l+w,	t,		1.f, 1.f);

		m = m * curMatrix;

		Mat4 uv;
		if(mod->mTextureAddressModifiers.find("TextureU0V0") != mod->mTextureAddressModifiers.end())
		{
			uv[0][0] = mod->mTextureAddressModifiers["TextureU0V0"].mU;
			uv[0][1] = mod->mTextureAddressModifiers["TextureU0V0"].mV;
			uv[0][2] = mod->mTextureAddressModifiers["TextureU0V0"].mU;
			uv[0][3] = mod->mTextureAddressModifiers["TextureU0V0"].mV;			
		}
		else
		{
			uv[0][0] = mMaterial.GetTopLeftU();
			uv[0][1] = mMaterial.GetTopLeftV();
			uv[0][2] = mMaterial.GetTopLeftU();
			uv[0][3] = mMaterial.GetTopLeftV();
		}
		
		if(mod->mTextureAddressModifiers.find("TextureU1V1") != mod->mTextureAddressModifiers.end())
		{
			uv[1][0] = mod->mTextureAddressModifiers["TextureU1V1"].mU;
			uv[1][1] = mod->mTextureAddressModifiers["TextureU1V1"].mV;
			uv[1][2] = mod->mTextureAddressModifiers["TextureU1V1"].mU;
			uv[1][3] = mod->mTextureAddressModifiers["TextureU1V1"].mV;			
		}
		else
		{
			uv[1][0] = mMaterial.GetTopLeftU();
			uv[1][1] = mMaterial.GetBottomRightV();
			uv[1][2] = mMaterial.GetTopLeftU();
			uv[1][3] = mMaterial.GetBottomRightV();
		}
		
		if(mod->mTextureAddressModifiers.find("TextureU2V2") != mod->mTextureAddressModifiers.end())
		{
			uv[2][0] = mod->mTextureAddressModifiers["TextureU2V2"].mU;
			uv[2][1] = mod->mTextureAddressModifiers["TextureU2V2"].mV;
			uv[2][2] = mod->mTextureAddressModifiers["TextureU2V2"].mU;
			uv[2][3] = mod->mTextureAddressModifiers["TextureU2V2"].mV;			
		}
		else
		{
			uv[2][0] = mMaterial.GetBottomRightU();
			uv[2][1] = mMaterial.GetBottomRightV();
			uv[2][2] = mMaterial.GetBottomRightU();
			uv[2][3] = mMaterial.GetBottomRightV();
		}
		
		if(mod->mTextureAddressModifiers.find("TextureU3V3") != mod->mTextureAddressModifiers.end())
		{
			uv[3][0] = mod->mTextureAddressModifiers["TextureU3V3"].mU;
			uv[3][1] = mod->mTextureAddressModifiers["TextureU3V3"].mV;
			uv[3][2] = mod->mTextureAddressModifiers["TextureU3V3"].mU;
			uv[3][3] = mod->mTextureAddressModifiers["TextureU3V3"].mV;			
		}
		else
		{
			uv[3][0] = mMaterial.GetBottomRightU();
			uv[3][1] = mMaterial.GetTopLeftV();
			uv[3][2] = mMaterial.GetBottomRightU();
			uv[3][3] = mMaterial.GetTopLeftV();
		}
		
		unsigned int color;
		if(mod->mColorModifiers.find("TextureColor") != mod->mColorModifiers.end())
		{
			color = mod->mColorModifiers["TextureColor"];
		}
		else
		{
			color = mColor;
		}

		UITexture * texture;
		if(mod->mTextureModifiers.find("Texture") != mod->mTextureModifiers.end())
		{
			texture = mod->mTextureModifiers["Texture"];
		}
		else
		{
			texture = mMaterial.GetTexture();
		}
		
		UIObject::spUIModule->GetRenderer()->Draw2DWithTexture(texture, color, (EBlendMode) 0,(EAddressMode) 0, m, uv);

	}

	void UIObject::Tick(float delta)
	{
	}

	bool UIObject::AcceptInput(const UIInput *)
	{
		return false;
	}

	void UIObject::BeforeDraw()
	{
	}

	void UIObject::AfterDraw()
	{
	}


	UIModule *			UIObject::spUIModule = NULL;
}