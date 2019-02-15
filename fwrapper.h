#ifndef C_FUNCTION_WRAPPER
#define C_FUNCTION_WRAPPER
#include <string>
#include <vector>

namespace FWRAP {
	typedef int32_t arg32; /* 32 bits */
	typedef int64_t arg64; /* 64 bits */
	enum CTYPE { none, c_stdcall, c_cdecl, c_fastcall, c_thiscall };

	CTYPE tconv(const char* s_convention) {
		if (strcmp(s_convention,"cdecl")==0 ||
			strcmp(s_convention,"__cdecl")==0)
			return CTYPE::c_cdecl;
		if (strcmp(s_convention,"stdcall")==0 ||
			strcmp(s_convention,"__stdcall")==0)
			return CTYPE::c_stdcall;
		if (strcmp(s_convention,"fastcall")==0 ||
			strcmp(s_convention,"__fastcall")==0)
			return CTYPE::c_fastcall;
		if (strcmp(s_convention,"thiscall")==0 ||
			strcmp(s_convention,"__thiscall")==0)
			return CTYPE::c_thiscall;
		return CTYPE::none;
	}

	// Convert any basic value to one specific type
	// that can be passed to a function
	// 
	// Add your own types if you have to, or reinterpret_cast.
	//
	struct par { 
		arg32 data32;
		arg64 data64;
		int size;
		char t[2];
		par(double d)		{ data64=(arg64)d; size=sizeof(double);			strcpy(t,"64\0"); }
		par(char* d)		{ data32=(arg32)d; size=sizeof(char*);			strcpy(t,"32\0"); }
		par(void* d)		{ data32=(arg32)d; size=sizeof(void*);			strcpy(t,"32\0"); }
		par(const char* d)	{ data32=(arg32)d; size=sizeof(const char*);	strcpy(t,"32\0"); }
		par(const void* d)	{ data32=(arg32)d; size=sizeof(const void*);	strcpy(t,"32\0"); }
		par(unsigned long d){ data32=(arg32)d; size=sizeof(unsigned long);	strcpy(t,"32\0"); }
		par(unsigned int d)	{ data32=(arg32)d; size=sizeof(unsigned int);	strcpy(t,"32\0"); }
		par(float d)		{ data32=(arg32)d; size=sizeof(float);			strcpy(t,"32\0"); }
		par(long d)			{ data32=(arg32)d; size=sizeof(long);			strcpy(t,"32\0"); }
		par(int d)			{ data32=(arg32)d; size=sizeof(int);			strcpy(t,"32\0"); }
		par(short d)		{ data32=(arg32)d; size=sizeof(short);			strcpy(t,"32\0"); }
		par(unsigned char d){ data32=(arg32)d; size=sizeof(unsigned char);	strcpy(t,"32\0"); }
	};

	UINT_PTR __stdcall f__call(UINT_PTR func,CTYPE t,std::vector<par>args){
		int stack_size = 0;
		unsigned char cleanup = (t==CTYPE::c_cdecl),
			isfc = (t==CTYPE::c_fastcall),
			istc = (t==CTYPE::c_thiscall),
			use_ecx_arg,
			use_edx_arg;
		
		for (char j=args.size()-1;j>=0;j--){
			arg32 a32=args[j].data32;
			arg64 a64=args[j].data64;
			if (args[j].t[0]==0x36 && args[j].t[1]==0x34){
				stack_size += sizeof(arg64);
				__asm {
					movd xmm0, QWORD PTR a64
					cvtdq2pd xmm0, xmm0
					sub esp, 8
					movsd qword ptr[esp], xmm0
				}
			} else {
				stack_size += sizeof(arg32);
				if (j==1 && (isfc))
					use_edx_arg=1;
				else if (j==0 && (isfc||istc))
					use_ecx_arg=1;
				else
					__asm push a32
			}
		}
		if (use_ecx_arg){ // set ECX as first arg
			arg32 a32=args[0].data32;
			__asm mov ecx,a32
		}
		if (use_edx_arg){ // set EDX as second arg
			arg32 a32=args[1].data32;
			__asm mov edx,a32
		}
		__asm {
			call func
			mov al,cleanup
			test al,al
			jz eof
			add esp,stack_size
			eof:
		}
	};

	struct function {
		char _name[100];
		CTYPE _type;
		UINT_PTR _func;

		function()				{ _name[0]='\0',_type = CTYPE::none,_func = 0; }
		function(UINT_PTR func)	{ _name[0]='\0',_type=CTYPE::c_stdcall,_func=func; }
		function(CTYPE type, UINT_PTR func)			{ _name[0]='\0',_type=type,_func=func; }
		function(const char* type, UINT_PTR func)	{ _name[0]='\0',_type=tconv(type),_func=func; }
		function(const char* name, CTYPE type, UINT_PTR func){
			_name[0]='\0';
			strcpy(_name,name);
			_type = type;
			_func = func;
		}
		function(const char* name, const char* type, UINT_PTR func) {
			_name[0]='\0';
			strcpy(_name,name);
			_type = tconv(type);
			_func = func;
		}

		UINT_PTR __stdcall call(std::vector<par>args) {
			return f__call(_func,_type,args);
		}
	};

	std::vector<function>_functions;

	void f_add(function f){ _functions.push_back(f); }
	void f_add(const char* name, CTYPE type, UINT_PTR func)		 { _functions.push_back(function(name,type,func)); }
	void f_add(const char* name, const char* type, UINT_PTR func){ _functions.push_back(function(name,type,func)); }

	function f_get(const char* name) {
		for (function f : FWRAP::_functions)
			if (strcmp(f._name, name) == 0)
				return f;
		return function();
	}
}

#endif

