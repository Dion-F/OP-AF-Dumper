This is a function-wrapping module for DLL's, that works by producing the function call in assembly.
This way, you can call virtually any function with only its address, and optionally the calling convention.
If you want, it will automatically determine the calling convention
(and allows you to call the function with up to 12 args).
This eliminates tedious code such as:
typedef int(__cdecl *functiontype1)(void* data1, int data2);
functiontype1 function1 = (functiontype1)0x6290B0;

replacing it with a much simpler:
fregister("function1","6290B0","cdecl");

and allows you to call it like so:
fcall("function1", (void*)0, 1000);

And for the exploiting community, this means fully auto-updatable exploits,
as you can retrieve the calling convention and address as a string from your web server.

This project is already complete.
I will not release this yet, but am planning to, if the idea is supported.
This could completely revolutionize DLL's and exploiting.
Lua C addresses will even be supplied from my own web server, to allow for creation of lua
exploit development for everyone.

Contact me at static#1942


