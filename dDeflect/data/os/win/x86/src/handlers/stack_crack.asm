[bits 32]
; stack

			add		esp, 512
			xor		[esp], edx
			xor		[esp + 4], esi
			or		[esp + 8], edi
			and		[esp + 12], esp
			