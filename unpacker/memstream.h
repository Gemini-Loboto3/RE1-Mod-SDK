#pragma once

#define MEMFILE_ALLOC		32*1024
#define GLIB_API

typedef struct tagMemStream
{
	u8 *data;
	int size, alloc;
	int pos;
} MEM_STREAM;

/* release buffer and close stream */
GLIB_API void MemStreamClose(MEM_STREAM *str);
/* allocate buffer and create a new stream */
GLIB_API void MemStreamCreate(MEM_STREAM *str);
/* flush the contents of the buffer to a physical file */
GLIB_API void MemStreamFlush(MEM_STREAM *str, LPCTSTR name);
/* allocate buffer and creare a stream from an existing file */
GLIB_API u8* MemStreamOpen(MEM_STREAM *str, LPCTSTR name);
/* allocate buffer and create a stream from a memory buffer */
GLIB_API u8* MemStreamOpen(MEM_STREAM *str, u8* data, int size);

/* seek around the stream */
GLIB_API int MemStreamSeek(MEM_STREAM *str, int pos, int mode);
/* return current seek position */
GLIB_API inline int MemStreamTell(MEM_STREAM *str) { return str->pos; }

/* return a byte from the stream (EOF when the stream is terminated) */
GLIB_API int MemStreamReadByte(MEM_STREAM *str);
/* read a sequence of bytes */
GLIB_API int MemStreamRead(MEM_STREAM *str, void *read, int size);
/* write a byte to the stream and reallocate buffer if it's necessary */
GLIB_API void MemStreamWriteByte(MEM_STREAM *str, u8 val);
/* write a sequence of bytes, internally uses MemStreamWriteByte */
GLIB_API int MemStreamWrite(MEM_STREAM *str, void *write, int size);

class CMemStream
{
public:
	operator u8*() const { return str.data; }

	CMemStream();
	CMemStream(int prealloc);
	~CMemStream();

	void WriteQuick(void *data, size_t size);

	void Write(void *data, size_t size);
	void Write(u16 val);
	void Seek(size_t pos, int mode);
	size_t Tell();
	void Reset();
	void Trim();
	u8 *Data();

	MEM_STREAM str;
};
