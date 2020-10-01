#pragma once

#define EXE_DIFF		0x400000

template<typename OUTSTUFF, typename INSTUFF>
OUTSTUFF ForceCast(INSTUFF in)
{
	union
	{
		INSTUFF  in;
		OUTSTUFF out;
	}
	u = { in };

	return u.out;
};

#pragma pack(push, 1)
typedef struct {
	BYTE opCode;	// must be 0xE9;
	DWORD offset;	// jump offset
} JMP;

typedef struct
{
	BYTE opCode0;	// must be 0xE8
	DWORD offset0;	// call offset
	BYTE opCode1;	// must be 0xE9
	DWORD offset1;	// reroute offset
} CALLX;

#pragma pack(pop)

#define INJECT(from,to) { \
	((JMP*)(from))->opCode = 0xE9; \
	((JMP*)(from))->offset = (DWORD)(to) - ((DWORD)(from) + sizeof(JMP)); \
}

#define INJECT_EXT(from,to) (*(DWORD*)(from)) = (DWORD)(to)

#define INJECT_CALL(from,to,size)	{\
	memset((from), 0x90, size); \
	((JMP*)(from))->opCode = 0xE8; \
	((JMP*)(from))->offset = (DWORD)(to) - ((DWORD)(from) + sizeof(JMP)); \
}

#define INJECT_CALLX(from,fnc,skp)	{\
	((CALLX*)(from))->opCode0 = 0xE8; \
	((CALLX*)(from))->offset0 = (DWORD)(fnc) - ((DWORD)(from) + sizeof(JMP)); \
	((CALLX*)(from))->opCode1 = 0xE9; \
	((CALLX*)(from))->offset1 = (DWORD)(skp) - ((DWORD)(from) + sizeof(CALLX)); \
}\


#define NAKED __declspec(naked)

enum InjectType
{
	IT_JUMP,
	IT_CALL,
	IT_EXTERN,
	IT_NOP
};
