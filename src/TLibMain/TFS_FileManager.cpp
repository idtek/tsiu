#include "TFS_FileManager.h"
#include "TCore_LibSettings.h"

namespace TsiU
{
	File* FileManager::OpenFile(const Char* _strPath, u32 _eOpenMode)
	{
		File* poFile = CallCreator<File>(E_CreatorType_File);

		if(!poFile->Open(_FillPath(_strPath), _eOpenMode))
		{
			CloseFile(poFile);
			return NULL;
		}
		else
			return poFile;
	}

	void FileManager::CloseFile(File* _poFile)
	{
		if(_poFile)
		{
			_poFile->Close();
			delete _poFile;
			_poFile = NULL;
		}
	}
	Bool FileManager::IsFileExisted(const Char* _strPath)
	{
		File* poFile = CallCreator<File>(E_CreatorType_File);
		Bool hasFile = poFile->IsExisted(_FillPath(_strPath));
		delete poFile;
		return hasFile;
	}

	const Char* FileManager::_FillPath(const Char* _strPath)
	{
		return _strPath;
	}
}