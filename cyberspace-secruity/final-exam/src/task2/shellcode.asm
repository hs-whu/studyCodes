global _start

section .text
_start:
	xor		edx,edx
	push	edx
	push	word "xt"
	push	"hs.t"
	mov		ebx,esp
	xor		ecx,ecx
	mov		cx,01q | 0100q | 01000q
	mov		dx,0666q
	xor		eax,eax
	mov		al,05h
	int		80h
	
	xor		edx,edx
	push	edx
	mov		ebx,eax
	mov		al,"9"
	push	ax
	push "0001"
	push "3015"
	push "2017"
	mov		ecx,esp
	mov		dl,13
	xor		eax,eax
	mov		al,04h
	int		80h

	xor		eax,eax
	mov al,1
	int 80h
