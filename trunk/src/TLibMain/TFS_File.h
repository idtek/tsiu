#ifndef __TFILESYSTEM_FILE__
#define __TFILESYSTEM_FILE__

#include "TCore_Types.h"

namespace TsiU
{
	enum FileOpenMode_t
	{
		E_FOM_Read		= 1 << 1,
		E_FOM_Write		= 1 << 2,
		E_FOM_Append	= 1 << 3,
		E_FOM_Binary	= 1 << 4,
		E_FOM_Text		= 1 << 5
	};
	enum FileCursor_t
	{
		E_FC_Begin,
		E_FC_Current,
		E_FC_End
	};

	class File
	{
	public:
		virtual Bool Open(const Char* _poFilePath, u32 _eOpenMode) = 0;
		virtual Bool Read(void* _poBuff, u32 _uiSize) = 0;
		virtual Bool Write(const void* _poBuff, u32 _uiSize) = 0;
		virtual Bool Seek(s32 _iOffset, FileCursor_t _ulPos) = 0;
		virtual Bool Close() = 0;
		virtual u32	 Size() = 0;
		virtual Bool IsExisted(const Char* _poFilePath) = 0;
	};

	class DefaultFile : public File
	{
	public:
		Bool Open(const Char* _poFilePath, u32 _eOpenMode)
		{
			Char option[3] = {0};
			if(_eOpenMode & E_FOM_Read)
				option[0] = 'r';
			else if(_eOpenMode & E_FOM_Write)
				option[0] = 'w';
			else if(_eOpenMode & E_FOM_Append)
				option[0] = 'a';

			if(_eOpenMode & E_FOM_Text)
				option[1] = 't';
			else if(_eOpenMode & E_FOM_Binary)
				option[1] = 'b';

			option[2] = 0;

			m_fp = fopen(_poFilePath, option);

			return m_fp != NULL;
		}
		Bool Read(void* _poBuff, u32 _uiSize)
		{
			return m_fp && (u32)fread(_poBuff, 1, _uiSize, m_fp) == _uiSize;
		}
		Bool Write(const void* _poBuff, u32 _uiSize)
		{
			return m_fp && (u32)fwrite(_poBuff, 1, _uiSize, m_fp) == _uiSize;;
		}
		Bool Seek(s32 _iOffset, FileCursor_t _ulPos)
		{
			s32 option;
			if(_ulPos == E_FC_Begin)
				option = SEEK_SET;
			else if(_ulPos == E_FC_Current)
				option = SEEK_CUR;
			else if(_ulPos == E_FC_End)
				option = SEEK_END;

			return m_fp && fseek(m_fp, _iOffset, option) == 0;
		}
		u32 Size()
		{
			if(!m_fp)
				return 0;

			fseek(m_fp, 0, SEEK_END);
			u32 fileSize = ftell(m_fp);
			fseek(m_fp, 0, SEEK_SET);

			return fileSize;
		}
		Bool Close()
		{
			if(m_fp)
				fclose(m_fp);
			return true;
		}
		Bool IsExisted(const Char* _poFilePath)
		{
			m_fp = fopen(_poFilePath, "r");
			if(m_fp != NULL)
			{
				fclose(m_fp);
				return true;
			}
			return false;
		}
	private:
		FILE *m_fp;
	};
}

#endif