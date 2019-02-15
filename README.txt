This provides a way to call virtually any function using only an address,
and a calling convention as a string.
It allows an infinite number of args, of nearly every type possible.
This can hugely assist in making an exploit auto updating, if it uses functions whos convention may change often.
It can be used for more than just exploiting games.
Enjoy.

Usage:

// Easily accessible
f_add(function("r_getfield","stdcall",0x7D7480));
f_get("r_getfield").call({rL, -10002, "game"});
f_get("r_getfield").call({rL, -1, "Players"});
f_get("r_getfield").call({rL, -1, "LocalPlayer"});
f_get("r_getfield").call({rL, -1, "Character"});
f_get("r_getfield").call({rL, -1, "Humanoid"});
int t = reinterpret_cast<int>(function("r_type","cdecl",0x7D9C20)).call({rL,-1});
printf("Type of field \"Humanoid\": %i.\n", t);


// Variable-style
function r_pushnumber = function("cdecl",0x7D8740);
r_pushnumber.call({rL, 250.0});


// Make it into an elegant function
f_add(function("r_setfield","stdcall",0x7D91E0));
auto r_setfield = [](UINT_PTR L,int id,const char* s){
  f_get("r_setfield").call({L,id,s});
};

r_setfield(rL, -2, "WalkSpeed");







