#ifndef __VC_EXPORT_HEADER__
#define __VC_EXPORT_HEADER__

#ifdef VUPCLIENT_EXPORTS
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif

#include <map>
#pragma warning(disable:4251)
#include "..\\VUPManager\\VMProtocal.h"

extern "C"
{
	class DLL_EXPORT WatchedValueBase
	{
	public:
		WatchedValueBase(unsigned char _type)
			:m_AssociateType(_type)
		{}

		virtual bool		HasChanged() const = 0;
		virtual void		Flush() = 0;
		virtual const void*	GetValue() const = 0;

	protected:
		unsigned char	m_AssociateType;
	};

	typedef void (*UdpPackHandler)(const UDP_PACK& _pInPack);

	class DLL_EXPORT VUPClientAdapter
	{
		typedef std::map<std::string,	WatchedValueBase*>					WatchedNameValueMap;
		typedef std::map<std::string,	WatchedValueBase*>::iterator		WatchedNameValueMapIterator;
		typedef std::map<unsigned char, WatchedNameValueMap*>				WatchedValueMap;
		typedef std::map<unsigned char, WatchedNameValueMap*>::iterator		WatchedValueMapIterator;

		typedef void (VUPClientAdapter::*Pack_Func)(UDP_PACK* outPack, WatchedNameValueMap& nameValue);

	public:
		VUPClientAdapter();
		virtual ~VUPClientAdapter();

		bool Init(unsigned int _uiPassport);
		bool RegisterMe();
		bool Tick();
		
		bool HasConnectToManager() const{
			return m_HasConnectedToManager;
		}
		void RegisterUDPPackHandler(unsigned char _uiType, UdpPackHandler _pPackHandler);
		void Watch(unsigned char _uiKey, const char* _zName, const int* _watchedvalue);
		void ReachRDVPoint(unsigned short _uiRDVPointID, unsigned short _uiExpected, unsigned short _uiTimeout);

	private:
		void _HandleRecvPack();
		void _HandleWatchedValue();

		//PACK func
		void _PACK_ReportClientRunningStatus(UDP_PACK* outPack, WatchedNameValueMap& nameValue);
		void _PACK_ReportClientTesingPhase(UDP_PACK* outPack, WatchedNameValueMap& nameValue);

	private:

		unsigned int	m_uiPassport;
		unsigned short	m_uiPort;
		unsigned char	m_uiStatus;
		unsigned char	m_uiTestPhase;

		WatchedValueMap m_WatchValues;
		Pack_Func		m_PackFunctions[EPT_Num];
		UdpPackHandler	m_UDPPackHandler[EPT_Num];

		bool			m_HasConnectedToManager;
	};
}
template<typename T>
class WatchedValue : public WatchedValueBase
{
public:
	WatchedValue(unsigned char _type)
		:WatchedValueBase(_type)
		,m_CurrentValue(0)
		,m_LastValue()
	{}
	virtual bool HasChanged() const{
		return (*m_CurrentValue != m_LastValue);
	}
	virtual void Flush(){
		m_LastValue = *m_CurrentValue;
	}
	virtual const void* GetValue() const{
		return (const void*)m_CurrentValue;
	}
	void Set(const T* _pValue)
	{
		m_CurrentValue = _pValue;
		m_LastValue = *_pValue;
	}
protected:
	const T*		m_CurrentValue;
	T				m_LastValue;
};


#endif