#ifndef __VC_EXPORT_HEADER__
#define __VC_EXPORT_HEADER__

#ifdef VUPCLIENT_EXPORTS
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif

extern "C"
{
	class DLL_EXPORT VUPClientAdapter
	{
	public:
		VUPClientAdapter();
		virtual ~VUPClientAdapter();

		bool Init(unsigned int _uiPassport);
		bool RegisterMe();
		bool Tick();

	private:
		unsigned int	m_uiPassport;
		unsigned short	m_uiPort;
		unsigned char	m_uiStatus;
	};
}

#endif