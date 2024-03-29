// rdumper.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <string>
#include "eyestep.h"
#include "eyestep_util.h"

using namespace eyestep::exeutil;
uint32_t at = 0;

//
// resizearray:
// 55 8B EC 8B 55 0C 8B ?? ?? 8D ?? ?? 03 C1 3D ?? ?? ?? ?? 75
// 
// luaM_realloc_ XREF and luaH_new:
// 6A 08 6A ?? 6A 00 6A 00 ?? E8
//
void log(const char* name, uint32_t func) {
	at = func;
	std::string spaces = "";
	for (int i=lstrlenA(name); i<34; i++) spaces+=0x20;
	if (func < 256) {
		printf("%s%s\t0x%02X\n", name, spaces.c_str(), static_cast<uint8_t>(func));
	} else {
		printf("%s%s\t0x%08X\t%s\n", name, spaces.c_str(), eyestep::unaslr(func), getsconv(getconv(func)));
	}
}

int main() {
	DWORD max = 250;
	DWORD key;
	GetVolumeInformation(NULL, NULL, NULL, &key, &max, NULL, NULL, NULL);
	/*char c_key[100];
	sprintf_s(c_key, "%lu", key);
	printf("Your key: %s.\n", c_key);*/

	void* h = nullptr;
	HWND hWnd = FindWindowA(NULL, "Roblox");
	if (hWnd == 0)
		printf("No window. Closing...\n");
	else {
		DWORD pid = 0;
		GetWindowThreadProcessId(hWnd, &pid);
		if (pid != 0) {
			h = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
		}
	}
	if (h == nullptr) {
		printf("No process found\n");
		Sleep(3000);
		return 0;
	}
	eyestep::use(h);

	/*uint32_t addr = 0x12000000;
	addr += eyestep::write(addr, "push ebp").len;
	addr += eyestep::write(addr, "mov ebp,esp").len;
	addr += eyestep::write(addr, "mov [00000040],ecx").len;
	addr += eyestep::write(addr, "add [edx+0C],40").len;
	addr += eyestep::write(addr, "sbb ebx,40").len;
	addr += eyestep::write(addr, "pop ebp").len;
	addr += eyestep::write(addr, "retn").len;
	system("PAUSE");*/

	/* luaH_resizearray:
	B801000000D3????FF75????FF75??E8
	5DC38A????B801000000
	3D????????75??33C0??FF75
	8B????8D????????3D????????75??33 (OR:)
	8B????8B????8D????????3D????????75??33

	luaH_set (previous function is luaH_resizearray (use to check luaH_resizearray))
	C6????003D????????75??8B????85??74
	74??83????75??F20F????83????0F
	*/

	std::vector<uint32_t> results = scan("8B????8D????????3D????????75??33", ".xx.xxxx.xxxx.x.");
	if (results.size() == 0) {
		printf("[!] NO RESULT -- demand Threaded#5538 to update this\n");
		Sleep(5000);
		return 0;
	}

	log("luaH_resizearray",		nextprologue(results[0], behind, false));
	log("resize",				nextcall(at, ahead));
	std::vector<uint32_t>		calls_resize = getcalls(at);

	log("resume",				nextprologue(at, ahead));
	std::vector<uint32_t>		calls_resume = getcalls(at);
	log("luaD_precall",			calls_resume[0]);
	log("luaD_poscall",			calls_resume[1]);

	std::vector<uint32_t>		calls_precall = getcalls(calls_resume[0]);
	log("tryfuncTM",			calls_precall[0]);
	log("luaD_reallocstack",	calls_precall[1]);
	log("adjust_varargs",		calls_precall[2]);
	log("luaD_callhook",		calls_precall[3]);
	//log("luaD_reallocstack",	calls_precall[4]);
	//log("luaD_callhook",		calls_precall[5]);
	//log("luaD_poscall",		calls_precall[6]);
	log("callrethooks",			nextcall(calls_resume[1], ahead));

	log("setarrayvector",		calls_resize[0]);
	log("luaD_rawrunprotected", calls_resize[1]);
	log("luaH_setnum",			calls_resize[2]);
	log("luaM_realloc_",		calls_resize[3]);
	log("luaH_set",				calls_resize[4]);
	log("luaM_realloc_",		calls_resize[5]);
	log("setarrayvector",		calls_resize[6]);
	log("luaD_throw",			calls_resize[7]);
	log("luaG_runerror",		calls_resize[8]);

	uint32_t addr_luaH_getn;
	std::vector<uint32_t>		calls_luaH_set = getcalls(calls_resize[4]);
	log("luaH_get",				calls_luaH_set[0]);
	log("luaopen_base",			calls_luaH_set[1]);
	log("newkey",				calls_luaH_set[2]);
	log("luaG_runerror",		calls_luaH_set[3]);
	std::vector<uint32_t>		calls_luaH_get = getcalls(calls_luaH_set[0]);
	log("luaH_getstr",			calls_luaH_get[0]);
	//log("luaopen_base",		calls_luaH_get[1]);
	log("luaH_getnum",			calls_luaH_get[2]);
	log("mainposition",			calls_luaH_get[3]);
	log("luaH_getn",			addr_luaH_getn = nextprologue(calls_luaH_set[0], ahead));
	log("luaH_new",				nextprologue(calls_luaH_get[0], ahead));
	log("luaH_next",			nextprologue(nextprologue(calls_luaH_get[0], ahead), ahead));
	std::vector<uint32_t>		calls_newkey = getcalls(calls_luaH_set[2]);
	log("mainposition",			calls_newkey[0]);
	log("rehash",				calls_newkey[1]);
	std::vector<uint32_t>		calls_mainposition = getcalls(calls_luaH_get[3]);
	log("hashnum",				calls_mainposition[0]);
	std::vector<uint32_t>		calls_rehash = getcalls(calls_newkey[1]);
	log("numusearray",			calls_rehash[0]);
	log("numhasharray",			calls_rehash[1]);
	log("luaopen_base",			calls_rehash[2]);
	log("computesizes",			calls_rehash[3]);

	std::vector<uint32_t>		calls_luaD_throw = getcalls(calls_resize[7]);
	log("resetstack",			calls_luaD_throw[0]);
	std::vector<uint32_t>		calls_resetstack = getcalls(calls_luaD_throw[0]);
	log("luaF_close",			calls_resetstack[0]);
	log("luaD_seterrorobj",		calls_resetstack[1]);
	log("luaD_reallocCI",		calls_resetstack[2]);
	log("luaS_newlstr",			nextcall(calls_resetstack[1], ahead));
	std::vector<uint32_t>		calls_luaG_runerror = getcalls(calls_resize[8]);
	log("luaO_pushvfstring",	calls_luaG_runerror[0]);
	log("addinfo",				calls_luaG_runerror[1]);
	log("luaG_errormsg",		calls_luaG_runerror[2]);
	std::vector<uint32_t>		calls_addinfo = getcalls(calls_luaG_runerror[1]);
	log("currentline",			calls_addinfo[0]);
	log("luaO_chunkid",			calls_addinfo[1]);
	log("luaO_pushfstring",		calls_addinfo[2]);

	log("pushstr",				nextcall(calls_luaG_runerror[0], ahead));
	std::vector<uint32_t>		calls_pushstr = getcalls(at);
	log("luaS_newlstr",			calls_pushstr[0]);
	log("luaD_reallocstack",	calls_pushstr[1]);

	log("gmatch_aux",			nextprologue(calls_mainposition[0], behind));
	std::vector<uint32_t>		calls_gmatch_aux = getcalls(at);
	log("push_captures",		nextcall(nextprologue(at, ahead), behind));
	log("lua_tolstring",		calls_gmatch_aux[0]);
	log("lua_tolstring",		calls_gmatch_aux[1]);
	log("luaO_str2d",			calls_gmatch_aux[2]);
	log("match",				calls_gmatch_aux[3]);
	log("lua_pushinteger",		calls_gmatch_aux[4]);
	std::vector<uint32_t>		calls_push_captures = getcalls(calls_gmatch_aux[6]);
	log("lua_checkstack",		calls_push_captures[0]);
	log("push_onecapture",		calls_push_captures[1]);
	log("lua_call",				nextprologue(calls_push_captures[0], behind));
	log("luaD_checkstack",		nextcall(calls_push_captures[0], ahead));
	
	log("lua_close",			nextprologue(calls_push_captures[0], ahead));
	std::vector<uint32_t>		calls_lua_close = getcalls(at);
	log("lua_cpcall",			nextprologue(at, ahead));
	log("lua_createtable",		nextprologue(at, ahead));
	log("luaF_close",			calls_lua_close[0]);
	
	std::vector<uint32_t>		behind_lua_tolstring = getprologues(calls_gmatch_aux[0], behind, 5);
	log("lua_tointeger",		behind_lua_tolstring[0]);
	log("lua_toboolean",		behind_lua_tolstring[1]);
	log("lua_setupvalue",		behind_lua_tolstring[2]);
	log("lua_settop",			behind_lua_tolstring[3]);
	log("lua_settable",			behind_lua_tolstring[4]);
	std::vector<uint32_t>		after_lua_tolstring = getprologues(calls_gmatch_aux[0], ahead, 4);
	log("lua_tonumber",			after_lua_tolstring[0]);
	log("lua_topointer",		after_lua_tolstring[1]);
	log("lua_tostring",			after_lua_tolstring[2]);
	log("lua_tothread",			after_lua_tolstring[3]);
	std::vector<uint32_t>		behind_luaopen_base = getprologues(calls_luaH_set[1], behind, 6);
	log("lua_yield",			behind_luaopen_base[0]);
	log("lua_xmove",			behind_luaopen_base[1]);
	log("lua_typename",			behind_luaopen_base[2]);
	log("lua_type",				behind_luaopen_base[3]);
	log("lua_touserdata(3 args)",behind_luaopen_base[4]);
	log("lua_touserdata",		behind_luaopen_base[5]);

	// function after luaG_runerror
	log("luaG_typeerror",		nextprologue(calls_luaH_set[3], ahead));
	std::vector<uint32_t>		calls_luaG_typeerror = getcalls(at);
	for (uint32_t x : getcalls(at)){
		if (x != calls_luaH_set[3]){
			log("getobjname", x);
			at = x;
			break;
		}
	}
	std::vector<uint32_t>		calls_getobjname = getcalls(at);
	log("currentpc",			calls_getobjname[0]);
	log("luaF_getlocalname",	calls_getobjname[1]);
	log("symbexec",				calls_getobjname[2]);
	log("kname",				calls_getobjname[3]);
	std::vector<uint32_t>		calls_symbexec = getcalls(calls_getobjname[2]);
	log("precheck",				calls_symbexec[0]);
	log("checkArgMode",			calls_symbexec[1]);

	uint32_t					addr_luaV_gettable5;
	uint32_t					addr_luaV_lessthan;
	uint32_t					addr_luaV_settable;
	std::vector<uint32_t>		calls_tolstring = getcalls(calls_gmatch_aux[0]);
	log("index2addr",			calls_tolstring[0]);
	log("luaC_step",			calls_tolstring[2]);
	log("luaV_tostring",		calls_tolstring[1]);
	std::vector<uint32_t>		behind_luaV_tostring = getprologues(calls_tolstring[1], behind, 4);
	log("luaV_tonumber",		behind_luaV_tostring[0]);
	log("luaV_settable",		addr_luaV_settable = behind_luaV_tostring[1]);
	log("luaV_lessthan",		addr_luaV_lessthan = behind_luaV_tostring[2]);
	log("luaV_concat",			behind_luaV_tostring[3]);
	/*
		luaV_execute			.text	0x8497F0	00002A32	0000017C	00000000	R	.	.	.	.	.	.
		luaV_gettable			.text	0x84C5D0	0000001B	00000004	00000010	R	.	..B	.	.
		luaV_gettable_4_Args	.text	0x84C5F0	00000192	00000014	00000011	R	.	..B	.	.
		luaV_concat				.text	0x84C790	000000BC	0000001C	0000000C	R	.	..B	.	.
		luaV_lessthan			.text	0x84C850	00000099	0000001C	0000000C	R	.	..B	.	.
		^ check function in lua_lessthan (above lua_load)
		luaV_settable			.text	0x84C8F0	000001DB	00000028	00000010	R	.	..B	.	.
		luaV_tonumber			.text	0x84CAD0	00000056	0000000C	00000008	R	.	..B	.	.
		luaV_tostring			.text	0x84CB30	00000070	00000030	00000008	R	.	..B	.	.
	*/
	
	// skip to luaV_execute and get luaV funcs
	// scan for the indirect table AOB:  05 05 05 01   05 05 05 05    05 05 02 05    05 05 03 05
	while (readui(at+0) != 0x01050505 && readui(at+4) != 0x05050505 && readui(at+8) != 0x05020505 && readui(at+12)!= 0x05030505) at--;
	log("luaV_execute",			nextprologue(at, behind, false));
	log("luaV_gettable",		nextprologue(at, ahead, false));
	log("luaV_gettable(5 args)",addr_luaV_gettable5 = nextprologue(at, ahead, false));
	std::vector<uint32_t>		calls_luaV_settable = getcalls(addr_luaV_settable);
	log("luaH_set",				calls_luaV_settable[0]);
	log("luaT_gettm",			calls_luaV_settable[1]);
	log("callTM",				calls_luaV_settable[2]);
	log("callTMres",			nextprologue(calls_luaV_settable[2], ahead));
	log("luaG_runerror",		calls_luaV_settable[3]);
	log("luaG_typeerror",		nextcall(nextprologue(addr_luaV_settable, ahead), behind));
	std::vector<uint32_t>		calls_luaV_lessthan = getcalls(addr_luaV_lessthan);
	log("luai_numlt",			calls_luaV_lessthan[0]);
	log("l_strcmp",				calls_luaV_lessthan[1]);
	log("call_orderTM",			calls_luaV_lessthan[2]);
	log("luaG_ordererror",		calls_luaV_lessthan[3]);
	std::vector<uint32_t>		calls_call_orderTM = getcalls(calls_luaV_lessthan[2]);
	log("luaT_gettmbyobj",		calls_call_orderTM[0]);
	//log("luaT_gettmbyobj",	calls_call_orderTM[1]);
	log("luaO_rawequalObj",		calls_call_orderTM[2]);

	log("lua_objlen",			nextxref(calls_gmatch_aux[4], behind, addr_luaH_getn, true)); // lua_pushinteger
	std::vector<uint32_t>		behind_lua_objlen = getprologues(at, behind, 5);
	std::vector<uint32_t>		after_lua_objlen = getprologues(at, ahead, 3);
	log("lua_next",				behind_lua_objlen[0]);
	log("lua_newuserdata",		behind_lua_objlen[1]);
	log("lua_newthread",		behind_lua_objlen[2]);
	log("lua_pcall",			after_lua_objlen[0]);
	log("lua_pushboolean",		after_lua_objlen[1]);
	log("lua_pushcclosure",		after_lua_objlen[2]);

	std::vector<uint32_t>		after_lua_pushinteger = getprologues(calls_gmatch_aux[4], ahead, 6);
	log("lua_pushlightuserdata",after_lua_pushinteger[0]);
	log("lua_pushlstring",		after_lua_pushinteger[1]);
	log("lua_pushnil",			after_lua_pushinteger[2]);
	log("lua_pushnumber",		after_lua_pushinteger[3]);
	log("lua_pushstring",		after_lua_pushinteger[4]);
	log("lua_pushthread",		after_lua_pushinteger[5]);
	log("lua_rawget",			nextxref(at, ahead, calls_luaH_set[0], true)); // luaH_get
	log("lua_rawequal",			nextprologue(at, behind));
	log("lua_pushvalue",		nextprologue(at, behind));
	log("lua_rawgeti",			nextxref(at, ahead, calls_luaH_get[2], true)); // luaH_getnum
	log("lua_rawset",			nextxref(at, ahead, calls_resize[4], true)); // luaH_set
	log("lua_rawseti",			nextxref(at, ahead, calls_luaH_get[2], true)); // luaH_getnum
	
	log("lua_setfield",			nextxref(at, ahead, addr_luaV_settable, true));
	std::vector<uint32_t>		behind_lua_setfield = getprologues(at, behind, 4);
	std::vector<uint32_t>		after_lua_setfield = getprologues(at, ahead, 3);
	log("lua_setfenv",			behind_lua_setfield[0]);
	log("lua_resume",			behind_lua_setfield[1]);
	log("lua_replace",			behind_lua_setfield[2]);
	log("lua_remove",			behind_lua_setfield[3]);
	log("lua_sethook",			after_lua_setfield[0]);
	log("lua_setlocal",			after_lua_setfield[1]);
	log("lua_setmetatable",		after_lua_setfield[2]);

	log("lua_lessthan",			nextxref(behind_lua_objlen[2], behind, addr_luaV_lessthan, true));
	std::vector<uint32_t>		behind_lua_lessthan = getprologues(at, behind, 5);
	std::vector<uint32_t>		after_lua_lessthan = getprologues(at, ahead, 2);
	log("lua_load",				after_lua_lessthan[0]);
	log("lua_newstate",			after_lua_lessthan[1]);
	log("lua_isnumber",			behind_lua_lessthan[0]);
	log("lua_isstring",			behind_lua_lessthan[1]);
	log("lua_iscfunction",		behind_lua_lessthan[2]);
	log("lua_insert",			behind_lua_lessthan[3]);
	log("lua_getupvalue",		behind_lua_lessthan[4]);

	log("lua_gettable",			nextxref(behind_lua_lessthan[4], behind, addr_luaV_gettable5, true));
	std::vector<uint32_t>		behind_lua_gettable = getprologues(at, behind, 4);
	log("lua_gettop",			nextprologue(at, ahead));
	log("lua_getstack",			behind_lua_gettable[0]);
	log("lua_getmetatable",		behind_lua_gettable[1]);
	log("lua_getlocal",			behind_lua_gettable[2]);
	log("lua_getinfo",			behind_lua_gettable[3]);
	
	log("lua_getfield",			nextxref(behind_lua_gettable[3], behind, addr_luaV_gettable5, true));
	std::vector<uint32_t>		behind_lua_getfield = getprologues(at, behind, 3);
	log("lua_gethook",			nextprologue(at, ahead));
	log("lua_getfenv",			behind_lua_gettable[0]);
	log("lua_gc",				behind_lua_gettable[1]);
	
	printf("\n\nPulled Struct offsets:\n\n");
	int d;

	// luaG_errormsg
	for (eyestep::inst i : eyestep::read(calls_luaG_runerror[2], 20)) {
		if (strcmp(i.opcode, "test") == 0 && i.flags & Fl_src_imm8) {
			log("L->errfunc", i.src.imm8);
			break;
		}
	}

	// lua_settable
	d = 0;
	for (eyestep::inst i : eyestep::read(behind_lua_tolstring[4], 40)) {
		if (i.flags & Fl_dest_imm8 && i.dest.r32 != eyestep::reg_32::ebp) {
			if (d == 0) log("L->base", i.dest.imm8);
			if (d == 2) log("L->top", i.dest.imm8);
			if (d == 3) printf("size of r_TValue\t\t\t0x%02X\n", (0xFF+1)-i.dest.imm8);
			d++;
			if (d > 3) break;
		}
	}

	// lua_getmetatable
	d = 0;
	for (eyestep::inst i : eyestep::read(behind_lua_gettable[1], 40)) {
		if (strcmp(i.opcode, "add") == 0 && i.src.r32 == 0 && i.flags & Fl_dest_disp8) {
			if (i.dest.disp8 < 0x7F && i.dest.disp8 > 0) {
				// uvalue(obj)->metatable ((obj->value.gc)->u->uv->metatable)
				if (d == 0) log("obj->value.u", i.dest.disp8);
				if (d == 1) log("obj->value.h", i.dest.disp8);
				d++;
			}
		}
	}

	uint32_t start = calls_symbexec[0]; // precheck
	const char* proto_names_a[] = {
			"p->maxstacksize",
			"p->is_vararg",
			"p->numparams",
			"p->nups",
			"p->sizeupvalues",
			"p->sizelineinfo",
			"p->sizecode"
	};
	for (int i=0,j=0; j<7; i++){
		eyestep::inst x = eyestep::read(start);
		if (x.dest.r32 != eyestep::reg_32::ebp && x.flags & Fl_dest_imm8){
			log(proto_names_a[j], x.dest.imm8);
			j++;
		}
		start += x.len; // move onto next instruction
	}

	system("PAUSE");
	return 0;
}
