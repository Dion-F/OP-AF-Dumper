This uses an older version of eyestep. update at your own risk.

Learn from the crappy code I wrote for this and have made 100x better(im not putting my best version out there)

Enjoy

please note, if you get an error at this part of the code:

uint32_t padding = (size - 1);
while (start < end){
	bool read = pmread(handle, vcast(start), buffer, chunksize, 0);
	if (read){
		__asm push edi
		__asm xor edi, edi
		__asm jmp L2
	L1: __asm inc edi

        
it's due to a formatting issue.
Just make sure that this:
__asm xor edi, edi
is NOT:
__asmxor edi, edi


also, running in release mode > debug mode, performance wise. jsyk
