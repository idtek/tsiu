#ifndef __TENGINE_MODULE__
#define __TENGINE_MODULE__

namespace TsiU
{
	class IModule
	{
		virtual void Init() = 0;
		virtual void RunOneFrame(float p_fDeltaTime) = 0;
		virtual void UnInit() = 0;
	};
}

#endif