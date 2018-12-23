This is a function wrapping module for DLL's, that works by simulating the calls in x86 ASM.
This way, you can call virtually any function with only its address, and calling convention
as a string, in an elegant api.
This eliminates tedious code such as:
typedef int(__cdecl *functiontype1)(void* data1, int data2);
functiontype1 function1 = (functiontype1)0x6290B0;

replacing it with a much simpler:
fregister("function1","6290B0","cdecl");

and allows you to call it like so:
fcall("function1", (void*)0, 1000);

I will not release this yet, but am planning to, if this idea is supported..



