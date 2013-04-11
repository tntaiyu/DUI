#ifndef __DUI_FILE_H__
#define __DUI_FILE_H__

#include "DUIDef.h"
#include <stdio.h>

#pragma warning(disable:4996)

DUI_BGN_NAMESPCE

class  DUILIB_API IFileBase
{
public:
	IFileBase(void) { };
	virtual ~IFileBase() { };

	virtual bool	Close() = 0;
	virtual size_t	Read(void *buffer, size_t size, size_t count) = 0;
	virtual size_t	Write(const void *buffer, size_t size, size_t count) = 0;
	virtual bool	Seek(long offset, int origin) = 0;
	virtual long	Tell() = 0;
	virtual long	Size() = 0;
	virtual bool	Flush() = 0;
	virtual bool	Eof() = 0;
	virtual long	Error() = 0;
	virtual bool	PutC(unsigned char c)
	{
		// Default implementation
		size_t nWrote = Write(&c, 1, 1);
		return (bool)(nWrote == 1);
	}
	virtual long	GetC() = 0;
	virtual char *	GetS(char *string, int n) = 0;
	virtual long	Scanf(const char *format, void* output) = 0;
};


class  DUILIB_API CIOFile : public IFileBase
	{
public:
	CIOFile(FILE* fp = NULL)
	{
		m_fp = fp;
		m_bCloseFile = (bool)(fp==0);
	}

	~CIOFile()
	{
		Close();
	}
//////////////////////////////////////////////////////////
	bool Open(LPCTSTR filename, LPCTSTR mode)
	{
		if (m_fp) return false;	// Can't re-open without closing first

		m_fp = _tfopen(filename, mode);
		if (!m_fp) return false;

		m_bCloseFile = true;

		return true;
	}
//////////////////////////////////////////////////////////
	virtual bool Close()
	{
		int iErr = 0;
		if ( (m_fp) && (m_bCloseFile) ){ 
			iErr = fclose(m_fp);
			m_fp = NULL;
		}
		return (bool)(iErr==0);
	}
//////////////////////////////////////////////////////////
	virtual size_t	Read(void *buffer, size_t size, size_t count)
	{
		if (!m_fp) return 0;
		return fread(buffer, size, count, m_fp);
	}
//////////////////////////////////////////////////////////
	virtual size_t	Write(const void *buffer, size_t size, size_t count)
	{
		if (!m_fp) return 0;
		return fwrite(buffer, size, count, m_fp);
	}
//////////////////////////////////////////////////////////
	virtual bool Seek(long offset, int origin)
	{
		if (!m_fp) return false;
		return (bool)(fseek(m_fp, offset, origin) == 0);
	}
//////////////////////////////////////////////////////////
	virtual long Tell()
	{
		if (!m_fp) return 0;
		return ftell(m_fp);
	}
//////////////////////////////////////////////////////////
	virtual long	Size()
	{
		if (!m_fp) return -1;
		long pos,size;
		pos = ftell(m_fp);
		fseek(m_fp, 0, SEEK_END);
		size = ftell(m_fp);
		fseek(m_fp, pos,SEEK_SET);
		return size;
	}
//////////////////////////////////////////////////////////
	virtual bool	Flush()
	{
		if (!m_fp) return false;
		return (bool)(fflush(m_fp) == 0);
	}
//////////////////////////////////////////////////////////
	virtual bool	Eof()
	{
		if (!m_fp) return true;
		return (bool)(feof(m_fp) != 0);
	}
//////////////////////////////////////////////////////////
	virtual long	Error()
	{
		if (!m_fp) return -1;
		return ferror(m_fp);
	}
//////////////////////////////////////////////////////////
	virtual bool PutC(unsigned char c)
	{
		if (!m_fp) return false;
		return (bool)(fputc(c, m_fp) == c);
	}
//////////////////////////////////////////////////////////
	virtual long	GetC()
	{
		if (!m_fp) return EOF;
		return getc(m_fp);
	}
//////////////////////////////////////////////////////////
	virtual char *	GetS(char *string, int n)
	{
		if (!m_fp) return NULL;
		return fgets(string,n,m_fp);
	}
//////////////////////////////////////////////////////////
	virtual long	Scanf(const char *format, void* output)
	{
		if (!m_fp) return EOF;
		return fscanf(m_fp, format, output);
	}
//////////////////////////////////////////////////////////
protected:
	FILE *m_fp;
	bool m_bCloseFile;
};


class  DUILIB_API CMemFile : public IFileBase
{
public:
	CMemFile(BYTE* pBuffer = NULL, DWORD size = 0);
	~CMemFile();

	bool Open();
	BYTE* GetBuffer(bool bDetachBuffer = true);

	virtual bool	Close();
	virtual size_t	Read(void *buffer, size_t size, size_t count);
	virtual size_t	Write(const void *buffer, size_t size, size_t count);
	virtual bool	Seek(long offset, int origin);
	virtual long	Tell();
	virtual long	Size();
	virtual bool	Flush();
	virtual bool	Eof();
	virtual long	Error();
	virtual bool	PutC(unsigned char c);
	virtual long	GetC();
	virtual char *	GetS(char *string, int n);
	virtual long	Scanf(const char *format, void* output);

protected:
	bool	Alloc(DWORD nBytes);
	void	Free();

	BYTE*	m_pBuffer;
	DWORD	m_Size;
	bool	m_bFreeOnClose;
	long	m_Position;	//current position
	long	m_Edge;		//buffer size
};


DUI_END_NAMESPCE

#endif //__DUI_FILE_H__