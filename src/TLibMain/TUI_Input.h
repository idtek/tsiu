#ifndef __TEGINE_UIINPUT_H__
#define __TEGINE_UIINPUT_H__

namespace TsiU
{
	// Input Type
	enum EMouseInput
	{
		EMouseInput_Base = 0x8000,
		EMouseInput_Mouse,
		EMouseInput_Axis0,
		EMouseInput_Axis1,
		EMouseInput_Axis2,
		EMouseInput_Axis3,

		EMouseInput_Max = 0x9000,
	};

	enum EKeyInput
	{
		EKeyInput_Char =0,		

		// Ascii compatable
		EKeyInput_Enter = 13,
		EKeyInput_Esc = 27,
		EKeyInput_Space = 32,

		// Others
		EKeyinput_OthersBase = 0x0100,
		EKeyInput_Up,
		EKeyInput_Down,
		EKeyInput_Left,
		EKeyInput_Right,
		EKeyInput_ScrollDown,
		EKeyInput_ScrollUp,

		EKeyInput_ConsoleBase = 0x1000,
		EKeyInput_Max = 0x8000,
	};

	enum EKeyInputXBox
	{
		EKeyInputXBox_Base = 0x1000,
		EKeyInput_XBoxA,
		EKeyInput_XBoxB,
		EKeyInput_XBoxX,
		EKeyInput_XBoxY,
	};

	enum EKeyInputPS3
	{
		EKeyInput_PS3Base = 0x1000,
		EKeyInput_PS3Cross,
		EKeyInput_PS3Circle,
		EKeyInput_PS3Square,
		EKeyInput_PS3Triange,
	};

	// Input State
	enum EMouseInputState
	{
		EMouseInputState_MouseMove = 0,
		EMouseInputState_LeftDown = 1,
		EMouseInputState_LeftUp = 2,
		EMouseInputState_RightDown = 3,
		EMouseInputState_RightUp = 4,
	};

	enum EKeyInputState
	{
		EKeyInputState_KeyDown = 0,
		EKeyInputState_KeyUp = 1,
		EKeyInputState_KeyValue = 2,
	};

	enum EInputIndex
	{
		EInputIndex_All = -1,		
		EInputIndex_None = 0,
		EInputIndex_1 = 1,
		EInputIndex_2 = 2,
		EInputIndex_3 = 4,
		EInputIndex_4 = 8,
		EInputIndex_5 = 16,
		EInputIndex_6 = 32,
		EInputIndex_7 = 64,
		EInputIndex_8 = 128,
		EInputIndex_9 = 256,
		EInputIndex_10= 512,
	};

	class UIInput
	{
	public:

		short		mInputIndex;
		short		mInputType;
		short		mInputState;

		union 
		{
			float floatValue;
			int intValue;
			wchar_t charValue;
			struct
			{
				short x, y;
			} pointValue;

		} mValue;

	};
	


}

#endif