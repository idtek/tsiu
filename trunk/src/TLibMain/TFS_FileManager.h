#ifndef __TFILESYSTEM_FILEMANAGER__
#define __TFILESYSTEM_FILEMANAGER__

#include "TCore_Types.h"
#include "TUtility_Singleton.h"
#include "TFS_File.h"

namespace TsiU
{
	class FileManager : public Singleton<FileManager>
	{
	public:
		File* OpenFile(const Char* _strPath, u32 _eOpenMode);
		void CloseFile(File* _poFile);
		Bool IsFileExisted(const Char* _strPath);

	private:
		const Char* _FillPath(const Char* _strPath);
	};
}

#endif