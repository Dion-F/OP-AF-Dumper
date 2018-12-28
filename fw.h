// Don't forget to give credit to static and PuddingMuggs.
// He inspired me to get off my bum and finish making this.
// He is a huge help to work with, and an excellent dev,
// but mostly he is the nicest most respectable friend I have
// ever met on here.
// Anyway, heres a gift to everybody. Enjoy.
//
#ifndef STATICS_FUNCTION_WRAPPER
#define STATICS_FUNCTION_WRAPPER
#define arg32	int32_t	/* 32 bits */
#define arg64	int64_t	/* 64 bits */
#include <string>
#include <vector>
#define string	std::string
#define byte	unsigned char
#define dword	unsigned long

// FUNCTION WRAP
namespace fwrap {
	// byte identifier for call types
	const byte	t_stdcall	= 0x00,
				t_cdecl		= 0x01,
				t_fastcall	= 0x02,
				t_thiscall	= 0x03;

	// convert any type into a function arg
	struct par { 
		int size;
		arg32 data32;
		arg64 data64;
		string type;
		par(double d)	{ data64=(arg64)d; size=sizeof(double);	type="64"; }
		par(long d)		{ data32=(arg32)d; size=sizeof(long);	type="32"; }
		par(void* d)	{ data32=(arg32)d; size=sizeof(void*);	type="32"; }
		par(dword d)	{ data32=(arg32)d; size=sizeof(dword);	type="32"; }
		par(char* d)	{ data32=(arg32)d; size=sizeof(char*);	type="32"; }
		par(float d)	{ data32=(arg32)d; size=sizeof(float);	type="32"; }
		par(int d)		{ data32=(arg32)d; size=sizeof(int);	type="32"; }
		par(short d)	{ data32=(arg32)d; size=sizeof(short);	type="32"; }
		par(byte d)		{ data32=(arg32)d; size=sizeof(byte);	type="32"; }
	};

	struct function {
	public:
		string name;
		dword addr;
		string conv;

		function() {
			name="";
			addr=0;
			conv="";
		};

		function(dword Addr, string Conv) {
			name="";
			addr=Addr;
			conv=Conv;
		};

		function(string Name, dword Addr, string Conv) {
			name=Name;
			addr=Addr;
			conv=Conv;
		};

		dword call(par);
		dword call(par,par);
		dword call(par,par,par);
		dword call(par,par,par,par);
		dword call(par,par,par,par,par);
		dword call(par,par,par,par,par,par);
	};

	// -------------------------------------------------------
	// ------------------ ACCESSIBILITY ----------------------
	// -------------------------------------------------------
	std::vector<function>wrapped_functions;

	void fregister(function f) {
		wrapped_functions.push_back(f);
	}

	void fregister(string name, dword addr, string conv) {
		wrapped_functions.push_back(function(name, addr, conv));
	}

	function fget(string fname) {
		for (function f : wrapped_functions)
			if (f.name == fname) return f;
		return function();
	}

	// --------------------------------------------------------
	// ----------------- FUNCTION WRAPPER ---------------------
	// --------------------------------------------------------
	dword function::call(par arg1) {
		dword retv=0, func=addr, cleanup=0;
		byte is_arg1_64=(arg1.type == "64");
		arg64 d1_64=arg1.data64; arg32 d1_32=arg1.data32;

		// Size in bytes of all args to be cleaned up
		// we will simply cleanup 4byte or 8byte args, since shorts can be stored in 32bits
		cleanup += ((is_arg1_64)?8:4);

		// byte values that are easier to work with
		byte is_stdcall	 = (conv=="stdcall"	|| conv=="__stdcall");
		byte is_cdecl	 = (conv=="cdecl"	|| conv=="__cdecl");
		byte is_fastcall = (conv=="fastcall"|| conv=="__fastcall");
		byte is_thiscall = (conv=="thiscall"|| conv=="__thiscall");

		goto load; // determine the calling convention to use

		// ------- STDCALL AND CDECL --------
		l_standard:__asm {
			mov al, is_arg1_64
			test al, al
			jz l_standard_arg1_32
			movd xmm0, QWORD PTR d1_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp fcall
		} l_standard_arg1_32:__asm {
			push d1_32
			jmp fcall
		}

		// --------- FASTCALL ----------
		l_fastcall:__asm {
			mov al, is_arg1_64
			test al, al
			jz l_fastcall_arg1_32
			movd xmm0, QWORD PTR d1_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp fcall
		} l_fastcall_arg1_32:__asm {
			mov ecx, d1_32
			jmp fcall
		}

		// --------- THISCALL ----------
		l_thiscall:__asm {
			mov al, is_arg1_64
			test al, al
			jz l_thiscall_arg1_32
			// Sorry :c
			jmp fcall
		} l_thiscall_arg1_32:__asm {
			mov ecx, d1_32
			jmp fcall
		}

	load:
			if (is_stdcall)		goto l_standard;
			if (is_cdecl)		goto l_standard;
			if (is_fastcall)	goto l_fastcall;
			if (is_thiscall)	goto l_thiscall;
			goto fend;

	fcall:__asm {
			call func
			mov al, is_cdecl
			test al, al
			jz fend
			add esp, cleanup
		}

	fend:__asm { // store results in EAX
			mov retv, eax
			pop eax
		}

		return retv;
	}

	dword function::call(par arg1,par arg2) {
		dword retv=0, func=addr, cleanup=0;
		byte is_arg1_64=(arg1.type == "64");
		byte is_arg2_64=(arg2.type == "64");
		arg64 d1_64=arg1.data64; arg32 d1_32=arg1.data32;
		arg64 d2_64=arg2.data64; arg32 d2_32=arg2.data32;

		// Size in bytes of all args to be cleaned up
		// we will simply cleanup 4byte or 8byte args, since shorts can be stored in 32bits
		cleanup += ((is_arg1_64)?8:4);
		cleanup += ((is_arg2_64)?8:4);

		// byte values that are easier to work with
		byte is_stdcall	 = (conv=="stdcall"	|| conv=="__stdcall");
		byte is_cdecl	 = (conv=="cdecl"	|| conv=="__cdecl");
		byte is_fastcall = (conv=="fastcall"|| conv=="__fastcall");
		byte is_thiscall = (conv=="thiscall"|| conv=="__thiscall");

		goto load; // determine the calling convention to use

		// ------- STDCALL AND CDECL --------
		l_standard:__asm {
			mov al, is_arg2_64
			test al, al
			jz l_standard_arg2_32
			movd xmm0, QWORD PTR d2_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp l_standard_next1
		} l_standard_arg2_32:__asm push d2_32
		l_standard_next1:__asm {
			mov al, is_arg1_64
			test al, al
			jz l_standard_arg1_32
			movd xmm0, QWORD PTR d1_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp fcall
		} l_standard_arg1_32:__asm push d1_32
			__asm jmp fcall

		// --------- FASTCALL ----------
		l_fastcall:__asm {
			mov al, is_arg1_64
			test al, al
			jz l_fastcall_arg1_32
			movd xmm0, QWORD PTR d1_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp l_fastcall_next1
		} l_fastcall_arg1_32:__asm mov ecx, d1_32
		l_fastcall_next1:__asm {
			mov al, is_arg2_64
			test al, al
			jz l_fastcall_arg2_32
			movd xmm0, QWORD PTR d2_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp fcall
		} l_fastcall_arg2_32:__asm mov edx, d2_32
			__asm jmp fcall

		// --------- THISCALL ----------
		l_thiscall:__asm {
			mov al, is_arg1_64
			test al, al
			jz l_thiscall_arg1_32
			// Sorry :c
			jmp l_thiscall_next1
		} l_thiscall_arg1_32:__asm mov ecx, d1_32
		l_thiscall_next1:__asm {
			mov al, is_arg2_64
			test al, al
			jz l_thiscall_arg2_32
			movd xmm0, QWORD PTR d2_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp fcall
		} l_thiscall_arg2_32:__asm push d2_32
			__asm jmp fcall

	load:
			if (is_stdcall)		goto l_standard;
			if (is_cdecl)		goto l_standard;
			if (is_fastcall)	goto l_fastcall;
			if (is_thiscall)	goto l_thiscall;
			goto fend;

	fcall:__asm {
			call func
			mov al, is_cdecl
			test al, al
			jz fend
			add esp, cleanup
		}

	fend:__asm { // store results in EAX
			mov retv, eax
			pop eax
		}

		return retv;
	}
				
	dword function::call(par arg1,par arg2,par arg3) {
		dword retv=0, func=addr, cleanup=0;
		byte is_arg1_64=(arg1.type == "64");
		byte is_arg2_64=(arg2.type == "64");
		byte is_arg3_64=(arg3.type == "64");
		arg64 d1_64=arg1.data64; arg32 d1_32=arg1.data32;
		arg64 d2_64=arg2.data64; arg32 d2_32=arg2.data32;
		arg64 d3_64=arg3.data64; arg32 d3_32=arg3.data32;

		// Size in bytes of all args to be cleaned up
		// we will simply cleanup 4byte or 8byte args, since shorts can be stored in 32bits
		cleanup += ((is_arg1_64)?8:4);
		cleanup += ((is_arg2_64)?8:4);
		cleanup += ((is_arg3_64)?8:4);

		// byte values that are easier to work with
		byte is_stdcall	 = (conv=="stdcall"	|| conv=="__stdcall");
		byte is_cdecl	 = (conv=="cdecl"	|| conv=="__cdecl");
		byte is_fastcall = (conv=="fastcall"|| conv=="__fastcall");
		byte is_thiscall = (conv=="thiscall"|| conv=="__thiscall");

		goto load; // determine the calling convention to use

		// ------- STDCALL AND CDECL --------
		l_standard:__asm {
			mov al, is_arg3_64
			test al, al
			jz l_standard_arg3_32
			movd xmm0, QWORD PTR d3_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp l_standard_next1
		} l_standard_arg3_32:__asm push d3_32
		l_standard_next1:__asm {
			mov al, is_arg2_64
			test al, al
			jz l_standard_arg2_32
			movd xmm0, QWORD PTR d2_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp l_standard_next2
		} l_standard_arg2_32:__asm push d2_32
		l_standard_next2:__asm {
			mov al, is_arg1_64
			test al, al
			jz l_standard_arg1_32
			movd xmm0, QWORD PTR d1_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp fcall
		} l_standard_arg1_32:__asm push d1_32
			__asm jmp fcall


		// --------- FASTCALL ----------
		l_fastcall:__asm {
			mov al, is_arg1_64
			test al, al
			jz l_fastcall_arg1_32
			movd xmm0, QWORD PTR d1_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp l_fastcall_next1
		} l_fastcall_arg1_32:__asm mov ecx, d1_32
		l_fastcall_next1:__asm {
			mov al, is_arg2_64
			test al, al
			jz l_fastcall_arg2_32
			movd xmm0, QWORD PTR d2_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp l_fastcall_next2
		} l_fastcall_arg2_32:__asm mov edx, d2_32
		l_fastcall_next2:__asm {
			mov al, is_arg3_64
			test al, al
			jz l_fastcall_arg3_32
			movd xmm0, QWORD PTR d3_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp fcall
		} l_fastcall_arg3_32:__asm push d3_32
			__asm jmp fcall


		// --------- THISCALL ----------
		l_thiscall:__asm {
			mov al, is_arg1_64
			test al, al
			jz l_thiscall_arg1_32
			// Sorry :c
			jmp l_thiscall_next1
		} l_thiscall_arg1_32:__asm mov ecx, d1_32
		l_thiscall_next1:__asm {
			mov al, is_arg3_64
			test al, al
			jz l_thiscall_arg3_32
			movd xmm0, QWORD PTR d3_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp l_thiscall_next2
		} l_thiscall_arg3_32:__asm push d3_32
		l_thiscall_next2:__asm {
			mov al, is_arg2_64
			test al, al
			jz l_thiscall_arg2_32
			movd xmm0, QWORD PTR d2_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp fcall
		} l_thiscall_arg2_32:__asm push d2_32
			__asm jmp fcall

	load:
			if (is_stdcall)		goto l_standard;
			if (is_cdecl)		goto l_standard;
			if (is_fastcall)	goto l_fastcall;
			if (is_thiscall)	goto l_thiscall;
			goto fend;

	fcall:__asm {
			call func
			mov al, is_cdecl
			test al, al
			jz fend
			add esp, cleanup
		}

	fend:__asm { // store results in EAX
			mov retv, eax
			pop eax
		}

		return retv;
	}

	dword function::call(par arg1,par arg2,par arg3,par arg4) {
		dword retv=0, func=addr, cleanup=0;
		byte is_arg1_64=(arg1.type == "64");
		byte is_arg2_64=(arg2.type == "64");
		byte is_arg3_64=(arg3.type == "64");
		byte is_arg4_64=(arg4.type == "64");
		arg64 d1_64=arg1.data64; arg32 d1_32=arg1.data32;
		arg64 d2_64=arg2.data64; arg32 d2_32=arg2.data32;
		arg64 d3_64=arg3.data64; arg32 d3_32=arg3.data32;
		arg64 d4_64=arg4.data64; arg32 d4_32=arg4.data32;

		// Size in bytes of all args to be cleaned up
		// we will simply cleanup 4byte or 8byte args, since shorts can be stored in 32bits
		cleanup += ((is_arg1_64)?8:4);
		cleanup += ((is_arg2_64)?8:4);
		cleanup += ((is_arg3_64)?8:4);
		cleanup += ((is_arg4_64)?8:4);

		// byte values that are easier to work with
		byte is_stdcall	 = (conv=="stdcall"	|| conv=="__stdcall");
		byte is_cdecl	 = (conv=="cdecl"	|| conv=="__cdecl");
		byte is_fastcall = (conv=="fastcall"|| conv=="__fastcall");
		byte is_thiscall = (conv=="thiscall"|| conv=="__thiscall");

		goto load; // determine the calling convention to use

		// ------- STDCALL AND CDECL --------
		l_standard:__asm {
			mov al, is_arg4_64
			test al, al
			jz l_standard_arg4_32
			movd xmm0, QWORD PTR d4_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp l_standard_next1
		} l_standard_arg4_32:__asm push d4_32
		l_standard_next1:__asm {
			mov al, is_arg3_64
			test al, al
			jz l_standard_arg3_32
			movd xmm0, QWORD PTR d3_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp l_standard_next2
		} l_standard_arg3_32:__asm push d3_32
		l_standard_next2:__asm {
			mov al, is_arg2_64
			test al, al
			jz l_standard_arg2_32
			movd xmm0, QWORD PTR d2_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp l_standard_next3
		} l_standard_arg2_32:__asm push d2_32
		l_standard_next3:__asm {
			mov al, is_arg1_64
			test al, al
			jz l_standard_arg1_32
			movd xmm0, QWORD PTR d1_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp fcall
		} l_standard_arg1_32:__asm push d1_32
			__asm jmp fcall


		// --------- FASTCALL ----------
		l_fastcall:__asm {
			mov al, is_arg1_64
			test al, al
			jz l_fastcall_arg1_32
			movd xmm0, QWORD PTR d1_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp l_fastcall_next1
		} l_fastcall_arg1_32:__asm mov ecx, d1_32
		l_fastcall_next1:__asm {
			mov al, is_arg2_64
			test al, al
			jz l_fastcall_arg2_32
			movd xmm0, QWORD PTR d2_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp l_fastcall_next2
		} l_fastcall_arg2_32:__asm mov edx, d2_32
		l_fastcall_next2:__asm {
			mov al, is_arg4_64
			test al, al
			jz l_fastcall_arg4_32
			movd xmm0, QWORD PTR d4_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp l_fastcall_next3
		} l_fastcall_arg4_32:__asm push d4_32
		l_fastcall_next3:__asm {
			mov al, is_arg3_64
			test al, al
			jz l_fastcall_arg3_32
			movd xmm0, QWORD PTR d3_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp fcall
		} l_fastcall_arg3_32:__asm push d3_32
			__asm jmp fcall


		// --------- THISCALL ----------
		l_thiscall:__asm {
			mov al, is_arg1_64
			test al, al
			jz l_thiscall_arg1_32
			// Sorry :c
			jmp l_thiscall_next1
		} l_thiscall_arg1_32:__asm mov ecx, d1_32
		l_thiscall_next1:__asm {
			mov al, is_arg4_64
			test al, al
			jz l_thiscall_arg4_32
			movd xmm0, QWORD PTR d4_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp l_thiscall_next2
		} l_thiscall_arg4_32:__asm push d4_32
		l_thiscall_next2:__asm {
			mov al, is_arg3_64
			test al, al
			jz l_thiscall_arg3_32
			movd xmm0, QWORD PTR d3_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp l_thiscall_next3
		} l_thiscall_arg3_32:__asm push d3_32
		l_thiscall_next3:__asm {
			mov al, is_arg2_64
			test al, al
			jz l_thiscall_arg2_32
			movd xmm0, QWORD PTR d2_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp fcall
		} l_thiscall_arg2_32:__asm push d2_32
			__asm jmp fcall

	load:
			if (is_stdcall)		goto l_standard;
			if (is_cdecl)		goto l_standard;
			if (is_fastcall)	goto l_fastcall;
			if (is_thiscall)	goto l_thiscall;
			goto fend;

	fcall:__asm {
			call func
			mov al, is_cdecl
			test al, al
			jz fend
			add esp, cleanup
		}

	fend:__asm { // store results in EAX
			mov retv, eax
			pop eax
		}

		return retv;
	}
				
	dword function::call(par arg1,par arg2,par arg3,par arg4,par arg5) {
		dword retv=0, func=addr, cleanup=0;
		byte is_arg1_64=(arg1.type == "64");
		byte is_arg2_64=(arg2.type == "64");
		byte is_arg3_64=(arg3.type == "64");
		byte is_arg4_64=(arg4.type == "64");
		byte is_arg5_64=(arg5.type == "64");
		arg64 d1_64=arg1.data64; arg32 d1_32=arg1.data32;
		arg64 d2_64=arg2.data64; arg32 d2_32=arg2.data32;
		arg64 d3_64=arg3.data64; arg32 d3_32=arg3.data32;
		arg64 d4_64=arg4.data64; arg32 d4_32=arg4.data32;
		arg64 d5_64=arg5.data64; arg32 d5_32=arg5.data32;

		// Size in bytes of all args to be cleaned up
		// we will simply cleanup 4byte or 8byte args, since shorts can be stored in 32bits
		cleanup += ((is_arg1_64)?8:4);
		cleanup += ((is_arg2_64)?8:4);
		cleanup += ((is_arg3_64)?8:4);
		cleanup += ((is_arg4_64)?8:4);
		cleanup += ((is_arg5_64)?8:4);

		// byte values that are easier to work with
		byte is_stdcall	 = (conv=="stdcall"	|| conv=="__stdcall");
		byte is_cdecl	 = (conv=="cdecl"	|| conv=="__cdecl");
		byte is_fastcall = (conv=="fastcall"|| conv=="__fastcall");
		byte is_thiscall = (conv=="thiscall"|| conv=="__thiscall");

		goto load; // determine the calling convention to use

		// ------- STDCALL AND CDECL --------
		l_standard:__asm {
			mov al, is_arg5_64
			test al, al
			jz l_standard_arg5_32
			movd xmm0, QWORD PTR d5_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp l_standard_next1
		} l_standard_arg5_32:__asm push d5_32
		l_standard_next1:__asm {
			mov al, is_arg4_64
			test al, al
			jz l_standard_arg4_32
			movd xmm0, QWORD PTR d4_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp l_standard_next2
		} l_standard_arg4_32:__asm push d4_32
		l_standard_next2:__asm {
			mov al, is_arg3_64
			test al, al
			jz l_standard_arg3_32
			movd xmm0, QWORD PTR d3_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp l_standard_next3
		} l_standard_arg3_32:__asm push d3_32
		l_standard_next3:__asm {
			mov al, is_arg2_64
			test al, al
			jz l_standard_arg2_32
			movd xmm0, QWORD PTR d2_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp l_standard_next4
		} l_standard_arg2_32:__asm push d2_32
		l_standard_next4:__asm {
			mov al, is_arg1_64
			test al, al
			jz l_standard_arg1_32
			movd xmm0, QWORD PTR d1_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp fcall
		} l_standard_arg1_32:__asm push d1_32
			__asm jmp fcall


		// --------- FASTCALL ----------
		l_fastcall:__asm {
			mov al, is_arg1_64
			test al, al
			jz l_fastcall_arg1_32
			movd xmm0, QWORD PTR d1_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp l_fastcall_next1
		} l_fastcall_arg1_32:__asm mov ecx, d1_32
		l_fastcall_next1:__asm {
			mov al, is_arg2_64
			test al, al
			jz l_fastcall_arg2_32
			movd xmm0, QWORD PTR d2_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp l_fastcall_next2
		} l_fastcall_arg2_32:__asm mov edx, d2_32
		l_fastcall_next2:__asm {
			mov al, is_arg5_64
			test al, al
			jz l_fastcall_arg5_32
			movd xmm0, QWORD PTR d5_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp l_fastcall_next3
		} l_fastcall_arg5_32:__asm push d5_32
		l_fastcall_next3:__asm {
			mov al, is_arg4_64
			test al, al
			jz l_fastcall_arg4_32
			movd xmm0, QWORD PTR d4_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp l_fastcall_next4
		} l_fastcall_arg4_32:__asm push d4_32
		l_fastcall_next4:__asm {
			mov al, is_arg3_64
			test al, al
			jz l_fastcall_arg3_32
			movd xmm0, QWORD PTR d3_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp fcall
		} l_fastcall_arg3_32:__asm push d3_32
			__asm jmp fcall


		// --------- THISCALL ----------
		l_thiscall:__asm {
			mov al, is_arg1_64
			test al, al
			jz l_thiscall_arg1_32
			jmp l_thiscall_next1
		} l_thiscall_arg1_32:__asm mov ecx, d1_32
		l_thiscall_next1:__asm {
			mov al, is_arg5_64
			test al, al
			jz l_thiscall_arg5_32
			movd xmm0, QWORD PTR d5_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp l_thiscall_next2
		} l_thiscall_arg5_32:__asm push d5_32
		l_thiscall_next2:__asm {
			mov al, is_arg4_64
			test al, al
			jz l_thiscall_arg4_32
			movd xmm0, QWORD PTR d4_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp l_thiscall_next3
		} l_thiscall_arg4_32:__asm push d4_32
		l_thiscall_next3:__asm {
			mov al, is_arg3_64
			test al, al
			jz l_thiscall_arg3_32
			movd xmm0, QWORD PTR d3_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp l_thiscall_next4
		} l_thiscall_arg3_32:__asm push d3_32
		l_thiscall_next4:__asm {
			mov al, is_arg2_64
			test al, al
			jz l_thiscall_arg2_32
			movd xmm0, QWORD PTR d2_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp fcall
		} l_thiscall_arg2_32:__asm push d2_32
			__asm jmp fcall

	load:
			if (is_stdcall)		goto l_standard;
			if (is_cdecl)		goto l_standard;
			if (is_fastcall)	goto l_fastcall;
			if (is_thiscall)	goto l_thiscall;
			goto fend;

	fcall:__asm {
			call func
			mov al, is_cdecl
			test al, al
			jz fend
			add esp, cleanup
		}

	fend:__asm { // store results in EAX
			mov retv, eax
			pop eax
		}

		return retv;
	}

	dword function::call(par arg1,par arg2,par arg3,par arg4,par arg5,par arg6) {
		dword retv=0, func=addr, cleanup=0;
		byte is_arg1_64=(arg1.type == "64");
		byte is_arg2_64=(arg2.type == "64");
		byte is_arg3_64=(arg3.type == "64");
		byte is_arg4_64=(arg4.type == "64");
		byte is_arg5_64=(arg5.type == "64");
		byte is_arg6_64=(arg6.type == "64");
		arg64 d1_64=arg1.data64; arg32 d1_32=arg1.data32;
		arg64 d2_64=arg2.data64; arg32 d2_32=arg2.data32;
		arg64 d3_64=arg3.data64; arg32 d3_32=arg3.data32;
		arg64 d4_64=arg4.data64; arg32 d4_32=arg4.data32;
		arg64 d5_64=arg5.data64; arg32 d5_32=arg5.data32;
		arg64 d6_64=arg6.data64; arg32 d6_32=arg6.data32;

		// Size in bytes of all args to be cleaned up
		// we will simply cleanup 4byte or 8byte args, since shorts can be stored in 32bits
		cleanup += ((is_arg1_64)?8:4);
		cleanup += ((is_arg2_64)?8:4);
		cleanup += ((is_arg3_64)?8:4);
		cleanup += ((is_arg4_64)?8:4);
		cleanup += ((is_arg5_64)?8:4);
		cleanup += ((is_arg6_64)?8:4);

		// byte values that are easier to work with
		byte is_stdcall	 = (conv=="stdcall"	|| conv=="__stdcall");
		byte is_cdecl	 = (conv=="cdecl"	|| conv=="__cdecl");
		byte is_fastcall = (conv=="fastcall"|| conv=="__fastcall");
		byte is_thiscall = (conv=="thiscall"|| conv=="__thiscall");

		goto load; // determine the calling convention to use

		// ------- STDCALL AND CDECL --------
		l_standard:__asm {
			mov al, is_arg6_64
			test al, al
			jz l_standard_arg6_32
			movd xmm0, QWORD PTR d6_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp l_standard_next1
		} l_standard_arg6_32:__asm push d6_32
		l_standard_next1:__asm {
			mov al, is_arg5_64
			test al, al
			jz l_standard_arg5_32
			movd xmm0, QWORD PTR d5_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp l_standard_next2
		} l_standard_arg5_32:__asm push d5_32
		l_standard_next2:__asm {
			mov al, is_arg4_64
			test al, al
			jz l_standard_arg4_32
			movd xmm0, QWORD PTR d4_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp l_standard_next3
		} l_standard_arg4_32:__asm push d4_32
		l_standard_next3:__asm {
			mov al, is_arg3_64
			test al, al
			jz l_standard_arg3_32
			movd xmm0, QWORD PTR d3_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp l_standard_next4
		} l_standard_arg3_32:__asm push d3_32
		l_standard_next4:__asm {
			mov al, is_arg2_64
			test al, al
			jz l_standard_arg2_32
			movd xmm0, QWORD PTR d2_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp l_standard_next5
		} l_standard_arg2_32:__asm push d2_32
		l_standard_next5:__asm {
			mov al, is_arg1_64
			test al, al
			jz l_standard_arg1_32
			movd xmm0, QWORD PTR d1_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp fcall
		} l_standard_arg1_32:__asm push d1_32
			__asm jmp fcall


		// --------- FASTCALL ----------
		l_fastcall:__asm {
			mov al, is_arg1_64
			test al, al
			jz l_fastcall_arg1_32
			movd xmm0, QWORD PTR d1_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp l_fastcall_next1
		} l_fastcall_arg1_32:__asm mov ecx, d1_32
		l_fastcall_next1:__asm {
			mov al, is_arg2_64
			test al, al
			jz l_fastcall_arg2_32
			movd xmm0, QWORD PTR d2_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp l_fastcall_next2
		} l_fastcall_arg2_32:__asm mov edx, d2_32
		l_fastcall_next2:__asm {
			mov al, is_arg6_64
			test al, al
			jz l_fastcall_arg6_32
			movd xmm0, QWORD PTR d6_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp l_fastcall_next3
		} l_fastcall_arg6_32:__asm push d6_32
		l_fastcall_next3:__asm {
			mov al, is_arg5_64
			test al, al
			jz l_fastcall_arg5_32
			movd xmm0, QWORD PTR d5_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp l_fastcall_next4
		} l_fastcall_arg5_32:__asm push d5_32
		l_fastcall_next4:__asm {
			mov al, is_arg4_64
			test al, al
			jz l_fastcall_arg4_32
			movd xmm0, QWORD PTR d4_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp l_fastcall_next5
		} l_fastcall_arg4_32:__asm push d4_32
		l_fastcall_next5:__asm {
			mov al, is_arg3_64
			test al, al
			jz l_fastcall_arg3_32
			movd xmm0, QWORD PTR d3_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp fcall
		} l_fastcall_arg3_32:__asm push d3_32
			__asm jmp fcall


		// --------- THISCALL ----------
		l_thiscall:__asm {
			mov al, is_arg1_64
			test al, al
			jz l_thiscall_arg1_32
			jmp l_thiscall_next1
		} l_thiscall_arg1_32:__asm mov ecx, d1_32
		l_thiscall_next1:__asm {
			mov al, is_arg6_64
			test al, al
			jz l_thiscall_arg6_32
			movd xmm0, QWORD PTR d6_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp l_thiscall_next2
		} l_thiscall_arg6_32:__asm push d6_32
		l_thiscall_next2:__asm {
			mov al, is_arg5_64
			test al, al
			jz l_thiscall_arg5_32
			movd xmm0, QWORD PTR d5_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp l_thiscall_next3
		} l_thiscall_arg5_32:__asm push d5_32
		l_thiscall_next3:__asm {
			mov al, is_arg4_64
			test al, al
			jz l_thiscall_arg4_32
			movd xmm0, QWORD PTR d4_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp l_thiscall_next4
		} l_thiscall_arg4_32:__asm push d4_32
		l_thiscall_next4:__asm {
			mov al, is_arg3_64
			test al, al
			jz l_thiscall_arg3_32
			movd xmm0, QWORD PTR d3_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp l_thiscall_next5
		} l_thiscall_arg3_32:__asm push d3_32
		l_thiscall_next5:__asm {
			mov al, is_arg2_64
			test al, al
			jz l_thiscall_arg2_32
			movd xmm0, QWORD PTR d2_64
			cvtdq2pd xmm0, xmm0
			sub esp, 8
			movsd qword ptr[esp], xmm0
			jmp fcall
		} l_thiscall_arg2_32:__asm push d2_32
			__asm jmp fcall

	load:
			if (is_stdcall)		goto l_standard;
			if (is_cdecl)		goto l_standard;
			if (is_fastcall)	goto l_fastcall;
			if (is_thiscall)	goto l_thiscall;
			goto fend;

	fcall:__asm {
			call func
			mov al, is_cdecl
			test al, al
			jz fend
			add esp, cleanup
		}

	fend:__asm { // store results in EAX
			mov retv, eax
			pop eax
		}

		return retv;
	}
}

#endif
