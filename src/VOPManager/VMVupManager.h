#ifndef __VM_VUPMANAGER_H__
#define __VM_VUPMANAGER_H__

#include "VMCommand.h"

class VMVup;

class VMVupManager : Singleton<VMVupManager>
{
	typedef std::map<s32, VMVup*>					VUPMap;
	typedef std::map<s32, VMVup*>::iterator			VUPMapIterator;
	typedef std::map<s32, VMVup*>::const_iterator	VUPMapConstIterator;

public:
	static s32 AddVup(const VMCommandParamHolder& _p1, const VMCommandParamHolder& _p2);
	static s32 UpdateVup(const VMCommandParamHolder& _p1, const VMCommandParamHolder& _p2);

	virtual void Create();
	virtual void Tick(f32 _fDeltaTime){};

	Bool			AddVup(VMVup* _newVUP);
	VMVup*			FindVup(s32 _id);
	const VMVup*	FindVup(s32 _id) const;

private:
	 VUPMap m_poVupMap;
};

#endif