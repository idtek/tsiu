#ifndef __SD_WINDOWMSG_CALLBACK_H__
#define __SD_WINDOWMSG_CALLBACK_H__

#include "SDGlobalDef.h"

class SDWindowMsgCallBack : public RenderWindowMsgListener
{
public:
	SDWindowMsgCallBack();

	virtual void OnMouseLDown(s32 x, s32 y);
	virtual void OnWindowResize(s32 width, s32 height);
	virtual void OnMouseLDrag(s32 x, s32 y, s32 relX, s32 relY);
	virtual void OnKeyDown(u32 key, s64 info);
	virtual void OnKeyUp(u32 key, s64 info);

private:
	bool	m_bIsAltHeld;
	bool	m_bIsControlHeld;
};

#endif