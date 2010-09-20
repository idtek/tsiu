#ifndef __TUTILITY_SINGLETON__
#define __TUTILITY_SINGLETON__

namespace TsiU
{
	template<typename T>
	class Singleton
	{
	public:
		static T& Get()		{ Create(); return (*self);   };
		static T* GetPtr()	{ Create(); return self;      };

		static void Create()
		{
			if( !self )
				self = new T;
		}

		static void Destroy()
		{
			if( self )
			{
				delete self;
				self = NULL;
			}
		}

	private:
		static T* self;
	};

	template<typename T> T* Singleton<T>::self(0);
}

#endif