include win32.inc
	
extern printf:near

	.data
kernel32Addr	dword	0
exportDirTAddr	dword	0
loadLibAAddr	dword	0
getProcAAddr	dword	0
msg			byte	"helloworld",13,10,0
msgtitle	byte	"HelloWorld",0
content		byte	"My name is hs, and I like computer, and this is my first MessageBox program",0
loadLibA	byte	"LoadLibraryA",0
getProcA	byte	"GetProcAddress",0
	
	.code
start:
	;获取imageBase偏移
	call	getOffset
getOffset:
	pop 	ebx
	sub		ebx,offset getOffset
	
	;查找kernel32.dll基地址
	mov		eax,[esp]
	and		eax,0ffff0000h
l1:
	cmp		word ptr [eax],'ZM'
	je		l2
	sub		eax,010000h
	jmp 	l1
l2:
	;存储kernel32.dll基地址
	mov		[ebx + offset kernel32Addr],eax
	
	;查找引出目录表地址
	mov		eax,[eax + 3ch]
	add		eax,78h
	add		eax,[ebx + offset kernel32Addr]
	mov		eax,[eax]
	add		eax,[ebx + offset kernel32Addr]
	
	;存储引出目录表地址
	mov		[ebx + offset exportDirTAddr],eax
	
	;查找LoadLibraryA,GetProcAddress函数地址&&存储
	lea		ecx,[ebx + offset loadLibA]
	push	ecx
	push	dword ptr [ebx + offset exportDirTAddr]
	call	fun_GetFunA
	mov		[ebx + offset loadLibAAddr],eax
	
	lea		ecx,[ebx + offset getProcA]
	push	ecx
	push	dword ptr [ebx + offset exportDirTAddr]
	call	fun_GetFunA
	mov		[ebx + offset getProcAAddr],eax
	
	
	
	lea eax,[ebx + offset msg]
	push eax
	call printf
	push 40h+1000h
	lea eax,[ebx + offset msgtitle]
	push eax
	lea eax, [ebx + offset content]
	push eax
	push 0
	call MessageBox
	push 0
	call ExitProcess

;para1	export directory table virtual address
;para2:	search function name string
;ret:	function virtual address
fun_GetFunA:
	push	ebp
	mov		ebp,esp
	sub		esp,010h
	;export address table
	mov		eax,[ebp+8h]
	mov		eax,[eax + 1ch]
	add		eax,[ebx + offset kernel32Addr]
	mov		[ebp-4h],eax
	;export name table
	mov		eax,[ebp+8h]
	mov		eax,[eax + 20h]
	add		eax,[ebx + offset kernel32Addr]
	mov		[ebp-8h],eax
	;export ordinal table
	mov		eax,[ebp+8h]
	mov		eax,[eax + 24h]
	add		eax,[ebx + offset kernel32Addr]
	mov		[ebp-0ch],eax
	;i
	mov		dword ptr [ebp-010h],0h
	
fun_GetFunA_loop:
	mov		eax,[ebp-8h]
	mov		edx,[ebp-010h]
	mov		ecx,[eax + edx * 4]
	add		ecx,[ebx + offset kernel32Addr]
	push	ecx
	push	[ebp+0ch]
	call	fun_strcmp
	cmp		eax,1h
	je		fun_GetFunA_ret
	inc 	dword ptr [ebp-010h]
	jmp		fun_GetFunA_loop
fun_GetFunA_ret:
	mov		eax,[ebp-0ch]
	mov		edx,[ebp-010h]
	xor		ecx,ecx
	mov		cx,[eax + edx * 2]
	mov 	eax,[ebp-4h]
	mov		eax,[eax + ecx * 4]
	add 	eax,[ebx + offset kernel32Addr]
	leave
	ret 8h
	
;para1：string1
;para2：string2
;ret:	equal=1,not equal=0
fun_strcmp:
	push	ebp
	mov		ebp,esp

	mov 	eax,[ebp+0ch]
	mov 	ecx,[ebp+8h]
fun_strcmp_loop:
	mov		dl,[eax]
	cmp		dl,[ecx]
	jne		fun_strcmp_l_ret0
	add		dl,[ecx]
	jz		fun_strcmp_l_ret1
	inc		eax
	inc		ecx
	jmp		fun_strcmp_loop

fun_strcmp_l_ret0:
	mov		eax,0
	jmp		fun_strcmp_ret
fun_strcmp_l_ret1:
	mov 	eax,1
fun_strcmp_ret:
	leave
	ret 8h
	
	end start
	
	
	
	
	
	
	
	
	
	
	
	
	
	