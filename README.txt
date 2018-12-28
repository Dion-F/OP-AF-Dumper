This is a function-wrapping module for DLL's, that works by producing the function call in assembly.
This way, you can call virtually any function with only its address, and calling convention** as a string.

You can then call the function with up to 6 args of any type or value.
This eliminates tedious code such as:

typedef int(__cdecl *functiontype1)(void* data1, int data2);
functiontype1 function1 = (functiontype1)0x6290B0;

replacing it with a much simpler:
fregister("function1",unprotect(aslr(0xABCDEF)),"cdecl")

and you can now call it like so:
fget("function1").call((void*)0, 1000, (arg3), (arg4), (arg5), . . .);

Or call the entire function at an address in a single line:
function(unprotect(aslr(0xABCDEF)),"stdcall").call(arg1, arg2, arg3);

For the exploiting community, this means fully auto-updatable exploits,
as you can retrieve the calling convention and address as a string
from your web server.

**Soon I will add a function for auto-determining the calling convention.
This will be a bonus feature since I'm working on a very secure one.

