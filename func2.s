SECTION .TEXT
	GLOBAL filter_image

; zachowanie wartosci funkcji wywolujacej
filter_image:
	push rbp
	mov rbp, rsp
	push r12
	push r13
	push r14
	push r15	

	;the first six parameters go into rdi, rsi, rdx, rcx, r8, and r9.
	;rsi -> input_buffer to process
	;rdi -> output_buffer to save into
	;rcx  
	;rax 
	;rbx
	;dl min RBG
	;dh temporary RGB 
	;r15
	;r14 height
	;r13 mask shift
	;r12 current column
	;r11 current row
	;r10 iterator 
	;r9 width
	;r8 przejscie
	xor r14, r14
	lea r14, [r8]  ; height
	;saving the mask shift
	xor r13, r13
	lea r13, [rdx] 
	dec r13 
	shr r13, 1

	xor r9, r9
	; calculating number of byte we will have to process
	;mov r9, rcx
	;imul ecx, 3
	lea r9, [ecx + ecx*2] ; width
	;lea r10, [ecx + ecx*2];width in bytes
	;lea eax, [r8d]
	;mul r10d
	;saving the number into rcx - after mul value is in two registers
	;mov ecx, eax
	;shl rcx, 32
	;shrd rcx, rdx, 32

	xor r12, r12 
	xor r11, r11

loop_row: 
	
	inc r11
	cmp r11, r14
	jg 	epilog   ; end of program

	xor r12, r12

loop_column: 

	inc r12
	cmp r12, r9
	jg loop_row

	xor r10, r10  ; restarting min value 
	mov r10d, 255
	
	;row 
	xor rcx, rcx
	mov rcx, r11
	sub rcx, r13

	dec rcx

filter_loop_row:

	inc rcx
	cmp rcx, 0
	jle filter_loop_row

	xor rbx, rbx 
	mov rbx, r11
	add rbx, r13
	cmp rcx, rbx
	jg save
	cmp rcx, r14
	jg save

	;column
	xor rax, rax
	mov rax, r12 
	xor r15, r15
	mov r15, r13
	imul r15, 3
	sub rax, r15
	sub rax, 3

filter_loop_column:

	add rax, 3
	cmp rax, 0
	jle filter_loop_column
	cmp rax, r9
	jg filter_loop_row

	xor rbx, rbx
	mov rbx, r12
	xor r15, r15 
	mov r15, r13
	imul r15, 3
	add rbx, r15
	cmp rax, rbx
	jg filter_loop_row


	xor rbx, rbx 
	mov rbx, rcx
	sub rbx, r11
	imul rbx, r9  ; przesuniecie w pionie

	xor r15, r15
	mov r15, rax
	sub r15, r12

	add rbx, r15 
	xor r15, r15
	mov r15, rsi ; zapisanie rsi
	
	add rsi, rbx
	xor rdx, rdx
	mov dl, [rsi]
	mov rsi, r15
	cmp edx, r10d
	jg filter_loop_column
	xor r10, r10
	mov r10b, dl

	jmp filter_loop_column


save: 

	mov [rdi], r10b
	dec rdi
	dec rsi
	jmp loop_column
	

;przywrocenie wartosci funkcji wywolujacej
epilog:
	pop r15
	pop r14
	pop r13
	pop r12
	mov rsp, rbp
	pop rbp
	ret