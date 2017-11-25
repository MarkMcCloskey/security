segment .text
global check
check:
	enter 0,0	;create stack frame with no locals and no nesting
	cld		;set forward direction flag
	mov ecx, 4	;going to compare 4 bytes
	lea esi, [un]	;load stored un into esi reg
	lea edi, [ebp + 8];get username arg into edi
	mov edi, [edi]	;resolve username arg pointer
	repe cmpsb	;compare 4 bytes breaking if not equal
	jne fail	;fail if the strings are not the same
	
	mov ecx, 4	;same as above except load pass and password arg
	lea esi, [pass]
	lea edi, [ebp + 12]
	mov edi, [edi]	
	repe cmpsb
	jne fail
	
	mov eax, 1	;if get here strings are the same so return 1
	jmp end

fail:
	mov eax, 0	;strings not the same return 0
	jmp end

end:
	leave
	ret



segment .data
	un: db 0x72, 0x6f, 0x6f, 0x74, 0 	;"root"
	pass: db 0x74, 0x6f, 0x6f, 0x72, 0 	;"toor"
