int  main(int argc, char* argv[])
{
	_asm{
		jmp JMP1;
		_emit 0x20;
	JMP1:
		mov eax, 50;
		jmp JMP2;
		_emit 0x56;
		_emit 0x78;
	JMP2:
		mov eax, 100;
		jmp JMP3;
		_emit 0x0F;
		_emit 0xF2;
	JMP3:
		mov ebx, 200;

	}
	return 0;
}
