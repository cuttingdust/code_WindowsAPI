.386                           ; ָ�� 32 λģʽ
.model flat, stdcall
option casemap:none		       ; ָ����Сд����
; ### x64 ƽ̨��ʱ��ע�͵�

.code
	;��Ҫ��x64��������
	;�ֺű�ʾע��
	;������//��ע��


; ���ƽ̨
ifdef _WIN64                 ; ����� 64 λƽ̨
    ; 64 λ�ض�����
    mycall proc
        mov rax, 1111122h   ; 64 λ�Ĵ���
        ret
    mycall endp

    myadd1 proc ;proc��ʾһ�������Ŀ�ʼ{
		mov rax,12345678h
		ret ;return
	myadd1 endp ;endp��ʾһ�������Ľ���}

else      
    ; ������ 32 λƽ̨
    ; 32 λ�ض�����
    mycall proc
        mov eax, 1111122h   ; 32 λ�Ĵ���
        ret
    mycall endp

    myadd1 proc
        ;xor edx, edx             ; ��� edx �Ĵ���
	    mov eax, 12345678h
		ret
	myadd1 endp 
endif

end