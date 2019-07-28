//#include <stdafx.h>
#include <stdafx.h>
#include <stdio.h>
#include "memstream.h"
#include "FileHandle.h"

u8* MemStreamOpen(MEM_STREAM *str, LPCTSTR name)
{
	CFile f;
	if (f.Open(name))
	{
		str->size = str->alloc = f.GetSize();
		str->data = new u8[str->size];
		f.Read(str->data, str->size);
		f.Close();
	}
	else
	{
		str->size = str->alloc = 0;
		str->data = NULL;
	}

	str->pos=0;
	return str->data;
}

u8* MemStreamOpen(MEM_STREAM *str, u8* data, int size)
{
	str->size = str->alloc = size;
	str->data = new u8[size];
	memcpy(str->data, data, size);
	str->pos = 0;

	return str->data;
}

void MemStreamFlush(MEM_STREAM *str, LPCTSTR name)
{
	//FlushFile(name,str->data,str->size);
}

void MemStreamCreate(MEM_STREAM *str)
{
	str->size = 0;
	str->alloc = MEMFILE_ALLOC;
	str->data = new u8[MEMFILE_ALLOC];
	str->pos = 0;
}

int MemStreamReadByte(MEM_STREAM *str)
{
	if (str->pos >= str->size) return EOF;
	return str->data[str->pos++];
}

int MemStreamRead(MEM_STREAM *str, void *read, int size)
{
	int ret;

	if (str->pos + size >= str->size)
	{
		size = str->size - str->pos;
		ret = EOF;
	}
	else ret = size;

	memcpy(read, &str->data[str->pos], size);
	str->pos += size;

	return size;
}

__inline void MemStreamWriteByte(MEM_STREAM *str, u8 val)
{
	if (str->pos + 1 >= str->alloc)
	{
		u8* temp = new u8[str->alloc + MEMFILE_ALLOC];
		//u8* temp=new u8[str->alloc*2];	// using a different growth method, less fragmentation expected
		// copy old data to new buffer
		memcpy(temp, str->data, str->alloc);
		delete[] str->data;
		// assign new buffer and size
		str->data = temp;
		str->alloc += MEMFILE_ALLOC;
		//str->alloc*=2;					// again, new allocation growth
	}
	str->data[str->pos++] = val;
	if (str->pos>str->size) str->size = str->pos;
}

int MemStreamWrite(MEM_STREAM *str, void *write, int size)
{
	u8 *src = (u8*)write;
	for (int i = 0; i<size; i++, src++) MemStreamWriteByte(str, *src);

	return size;
}

int MemStreamSeek(MEM_STREAM *str, int pos, int mode)
{
	switch (mode)
	{
	case SEEK_SET:
		str->pos = pos;
		if (pos>str->size) return EOF;
		break;
	case SEEK_CUR:
		str->pos += pos;
		if (str->pos>str->size) return EOF;
		break;
	case SEEK_END:
		str->pos = str->size - pos;
		if (str->pos<0) { str->pos = 0; return EOF; }
		break;
	default:	// just in case
		return EOF;
	}

	return str->pos;
}

void MemStreamClose(MEM_STREAM *str)
{
	delete[] str->data;
}

/////////////////////////////////////
CMemStream::CMemStream(int prealloc)
{
	str.data = new u8[prealloc];
	str.alloc = prealloc;
	str.pos = 0;
	str.size = 0;
}

CMemStream::CMemStream()
{
	MemStreamCreate(&str);
}

CMemStream::~CMemStream()
{
	MemStreamClose(&str);
}

void CMemStream::WriteQuick(void *data, size_t size)
{
	// try some quick reallocation by doubling the allocated size
	if ((int)size + str.pos > str.alloc)
	{
		u8 *temp = new u8[str.alloc * 2];
		memcpy(temp, str.data, str.alloc);
		delete[] str.data;
		str.data = temp;
		str.alloc *= 2;
	}

	memcpy(&str.data[str.pos], data, size);
	str.pos += size;
	if (str.pos > str.size)
		str.size = str.pos;
}

void CMemStream::Write(void *data, size_t size)
{
	MemStreamWrite(&str, data, size);
}

void CMemStream::Write(u16 val)
{
	MemStreamWrite(&str, &val, 2);
}

void CMemStream::Seek(size_t pos, int mode)
{
	MemStreamSeek(&str, pos, mode);
}

size_t CMemStream::Tell()
{
	return MemStreamTell(&str);
}

void CMemStream::Reset()
{
	str.pos = 0;
	str.size = 0;
}

void CMemStream::Trim()
{
	u8 *_old = str.data;
	u8 *_new = new u8[str.size];
	memcpy(_new, _old, str.size);
	delete[] str.data;
	str.data = _new;
	str.alloc = str.size;
}

u8 *CMemStream::Data() { return str.data; }
