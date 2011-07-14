#ifndef __TUI_MODIFIER_H__
#define __TUI_MODIFIER_H__

#include <string>
#include <map>

#include "TUtility_UIUtility.h"

namespace TsiU
{
	enum EModifierType
	{
		EModifierType_None = 0,
		EModifierType_Position,
		EModifierType_Color,
		EModifierType_Texture,
		EModifierType_TextureAddress,
		EModifierType_Max,
	};

	class UITexture;

	struct UIModifier
	{
	public:
		void clear()
		{
			mColorModifiers.clear();
			mPointModifiers.clear();
			mTextureModifiers.clear();
			mTextureAddressModifiers.clear();
		}

		std::map<std::string, unsigned int>	mColorModifiers;
		std::map<std::string, UIPoint>		mPointModifiers;
		std::map<std::string, UITexture *>	mTextureModifiers;
		std::map<std::string, UITextureUV> mTextureAddressModifiers;



	};
}



#endif

