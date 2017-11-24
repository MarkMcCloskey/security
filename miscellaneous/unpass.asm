segment .text
global _check
_check:
	enter 0,0
	
	mov ecx, 4
	lea esi, [un]
	lea edi, [ebp + 8]
	rep cmpsb
	jne fail
	
	lea esi, [pass]
	lea edi, [ebp + 12]
	rep cmpsb
	jne fail
	
	mov eax, 1
	jmp end



	;push un
	;push [ebp + 8]
	;call strcmp
	;cmp eax, 0
	;jne fail
	;push pass
	;push [ebp + 12]
	;call strcmp
	;cmp eax, 0
	;jne fail
	;mov eax, 1
	;jmp end


fail:
	mov eax, 0
	jmp end

end:
	leave
	ret



segment .data
	un: db 0x72, 0x6f, 0x6f, 0x74, 0
	pass: db 0x74, 0x6f, 0x6f, 0x72, 0
	;un .string "root"
	;pass .string "toor"
