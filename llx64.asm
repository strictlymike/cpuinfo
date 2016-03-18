.code

; ----------------------------------------------------------------------
; SGDT function
; ----------------------------------------------------------------------
ll_sgdt PROC
  sub rsp, 28h

  sgdt [rcx]

  xor rax,rax

  add rsp, 28h
  ret
ll_sgdt ENDP

; ----------------------------------------------------------------------
; SMSW function
; ----------------------------------------------------------------------
ll_smsw PROC
  sub rsp, 28h

  smsw r10
  mov qword ptr [rcx], r10

  xor rax,rax

  add rsp, 28h
  ret
ll_smsw ENDP

; ----------------------------------------------------------------------
; SLDT function, work in progress, no workie right now
; ----------------------------------------------------------------------
ll_sldt PROC
  sub rsp, 28h

  xor eax,eax
  ; db 66h			; http://www.asmcommunity.net/forums/topic/?id=11487
  sldt ax
  mov eax, dword ptr [rcx]

  xor rax,rax

  add rsp, 28h
  ret
ll_sldt ENDP

End
