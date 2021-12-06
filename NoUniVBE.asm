; NoUniVBE 0.4
; Small DOS TSR that tries to hinder (in-game) UniVBE from loading
;
; USAM (http://www.terraspace.co.uk/uasm.html#p2) or MASM
; uasm.exe NoUniVBE.asm

; ulink (ftp://ftp.styx.cabel.net/pub/UniLink)
; ulink.exe NoUniVBE.obj, NoUniVBE.exe, NoUniVBE.map
; or 16bit link.exe from microsoft

Far_pointer struc
  offs dw 0
  segm dw 0
Far_pointer ends

VESA_STR EQU <"VESA">

VBE_info_block struc
  VbeSignature db VESA_STR
  VbeVersion dw 0
  OemStringPtr Far_pointer <>
  Capabilities dd 0
  VideoModePtr Far_pointer <>
  TotalMemory dw 0
; VBE 2.x  
  OemSoftwareRev dw 0
  OemVendorNamePtr Far_pointer <>
  OemProductNamePtr Far_pointer <>
  OemProductRevPtr Far_pointer <>
  Reserved1 db 222 dup(0)
  OemData db 256 dup(0)
VBE_info_block ends

TSR_ID_STR EQU <"NoUniVBE 0.4", 0>

TSR_info_t struc
  signature db TSR_ID_STR
  version dw 0
  load_segment dw 0
  old_int10h Far_pointer<>
  old_int2fh Far_pointer<>
TSR_info_t ends

ON EQU 0
OFF EQU 1

LICENSE_LITE_BUNDLE EQU 1 ; "UniVBE(tm)/Lite - Universal VESA BIOS Extension (VBE) - Bundle Copy"
LICENSE_PRO_BUNDLE EQU 2 ; "UniVBE(tm)/Pro - Universal VESA BIOS Extension (VBE) - Bundle Copy"
LICENSE_ULTRA_BUNDLE EQU 3 ; "UniVBE(tm)/Ultra - Universal VESA BIOS Extension (VBE) - Bundle Copy"
LICENSE_SHAREWARE EQU 4 ; "UniVBE(tm)/Shareware - Universal VESA BIOS Extension (VBE)"
LICENSE_PRO EQU 5 ; "UniVBE(tm)/Pro - Universal VESA BIOS Extension (VBE)" ; Single-License
LICENSE_PRO_MULTI_SITE EQU 6 ; "UniVBE(tm)/Pro - Universal VESA BIOS Extension (VBE)" ; Multi-Site-License
;>6 Bundle copy license

UniVBE_53a_data_t struc
  card_info db 'S3 SuperVGA (Trio64) with 4 Mb',(80-30) dup (0) ; full sizeof needs to be 80
  dac_info db 'S3 Trio Internal 24 bit DAC', (50-27) dup(0) ; full sizeof needs to be 50
  clock_info db 'S3 Trio Internal Clock',(50-22) dup(0) ; full sizeof needs to be 50
  
  ciphered_registered_user db 0CAh,0D5h,08Fh,0D3h,0A9h,099h,082h,086h,0FFh,09Eh,0B3h,0BAh,0BBh,0A2h,0A8h,0ADh, ; '5.x Free Edition'
                              (62-16) dup(0) ; full sizeof needs to be 62
  ciphered_serial_no db 0CFh,0CBh,0C7h,0C3h,0DFh,0C6h,0DFh,0D2h,0E9h,09Eh,096h,097h,0FCh,0FBh,0EAh,0F1h,08Fh,08Bh,085h,083h,0 ; '00000-816EAD30-20020' , full sizeof needs to be 21
  
  version_major_minor db '53'
  release_date db 'Jan 24 1997',4 dup(0) ; full sizeof needs to be 15

  version dw LICENSE_PRO ; : 118h
  unknown2 dw 3B6Ah ; : 11Ah ; ?????
  
  enabled dw ON
  
  vbe2_support dw ON ; 11Eh  ; "VBE20"
  native_mode_centering dw ON ; : 120h
  lfb_support dw ON ; : 122h ; "LFB"
  vbe2_palette_functions dw ON ; : 124h ; "PAL"
  vbe2_32bit_interface dw ON ; : 126h ; "PM32"
  directx2_compatiblity_mode dw ON ; : 128h "DX2"
UniVBE_53a_data_t ends

UniVBE_SIGNATURE_STR EQU <"Universal VESA VBE">

.model medium

resident_seg segment para use16 
resident_begin:

signature db UniVBE_SIGNATURE_STR
 
oem_string db 'Universal VESA VBE 5.3a',0
OEM_SOFTWARE_REV = 0503h
oem_vendor_name db 'SciTech Software',0
oem_product_name db 'UniVBE(tm)',0
oem_prodcut_rev db '5.3',0

; resident code

tsr_info TSR_info_t<>

univbe_53a_data UniVBE_53a_data_t<>

detoured_int10h: ; this needs to be int10h.vector.offset
  cmp ax,4F00h ; VESA INFO?
  je fake_00

  cmp ax,4F0Fh ; UniVBE 5.3(a) special function
  je fake_0f

  ; let original code handle the request
  jmp cs:tsr_info.old_int10h

fake_00:
  pushf
  call cs:tsr_info.old_int10h
  
  cmp ax,004Fh ; result OK?
  jne exit_intr ; calling app should handle errors
 
  ; overload oem string pointer with UniVBE string
  mov es:[di+VBE_info_block.OemStringPtr.segm],cs
  mov es:[di+VBE_info_block.OemStringPtr.offs],offset(oem_string)
  
  iret

fake_0f:
  
  mov dx,cs ; segment
  mov bx,offset(univbe_53a_data)
  mov ax,004Fh ; OK result
  
  iret

exit_intr:
  iret
  
detoured_int2fh:
  jmp cs:tsr_info.old_int2fh
  iret

resident_end:

resident_seg ends

configure_seg segment para use16

VESA db VESA_STR
UniVBE_SIGNATURE db UniVBE_SIGNATURE_STR

vbi VBE_info_block<>

no_vesa_info_error_msg db 'error: VESA info call failed! (no VESA support?)',10,13,'$'
no_vesa_signature_msg db 'error: VESA signature not detected #1!',10,13,'$'
no_univbe_msg db 'UniVBE not detected!',10,13,'$'
univbe_detected_msg db 'UniVBE oem-string detected!',10,13,'$'
vesa_detected_msg db 'VESA detected!',10,13,'$'
installed_msg db 'NoUniVBE 0.4 installed!',10,13,'$'
uninstalled_msg db 'NoUniVBE uninstalled!',10,13,'$'
org_univbe_installed_msg db 'error: original UniVBE already installed!',10,13,'$'
no_univbe_installed_msg db 'NoUniVBE already installed!',10,13,'$'
buggy_msg db 'error: something is wrong!',10,13,'$'
free_environment_failed_msg db 'error: freeing environment failed!',10,13,'$'
free_resident_failed_msg db 'error: freeing resident part failed!',10,13,'$'

PSP_BYTES = 100h
PARAGRAPH_BYTES = 16
RESIDENT_BYTES = resident_end - resident_begin
RESIDENT_PARAGRAPHS = (PSP_BYTES + RESIDENT_BYTES + (PARAGRAPH_BYTES-1))/PARAGRAPH_BYTES

VIDEO_BIOS_INT = 10h
; http://www.techhelpmanual.com/681-int_2fh__multiplex_interrupt.html
MUX_INT = 2fh

psp_seg dw 0

start:
  ; DS = PSP
  mov bx,ds
  mov ax,configure_seg
  mov ds,ax
  mov es,ax
  assume ds:configure_seg, es:configure_seg
  mov psp_seg,bx

  ; get VESA info
  mov ax,4F00h
  mov di,offset(vbi)
  push ds
  pop es
  int 10h
  
  ; call successfull?
  cmp ax,004Fh
  jne vesa_info_error

  ; vbi.VbeSignature == "VESA" ?
  cld
  mov cx,sizeof VBE_info_block.VbeSignature
  mov si,offset(vbi.VbeSignature)
  mov di,offset(VESA)
  repe cmpsb
  jne no_vesa_signature

  mov dx,offset(vesa_detected_msg)
  mov ah,9h
  int 21h  

  ; UniVBE installed?
  ; check if OemString starts with "Universal VESA..."
  mov ax,vbi.OemStringPtr.segm
  mov es,ax
  mov di,vbi.OemStringPtr.offs
  
  mov si,offset(UniVBE_SIGNATURE)
  mov cx,sizeof UniVBE_SIGNATURE

compare: 
  mov bl,es:[di] ; from VESA BIOS
  mov al,[si] ; compare string
  cmp al,bl
  jne no_univbe
  
  inc di
  inc si
  
  dec cx
  cmp cx,0
  jnz compare
  
  ; UniVBE found !
  mov dx,offset(univbe_detected_msg)
  mov ah,9h
  int 21h  

  ; is it the original UniVBE or NoUniVBE?
  mov ah,35h
  mov al,VIDEO_BIOS_INT
  int 21h
  ; es:bx = vector ptr

  ; ignore bx offset
  ; es:offset(tsr_info)
  mov di,offset tsr_info
  
  ; exists TSR info signature?
  cld
  mov cx, sizeof TSR_info_t.signature
  mov ax,resident_seg
  push ds
  mov ds,ax
  assume ds:resident_seg
  mov si, offset(tsr_info.signature)
  repe cmpsb
  pop ds
  jne org_univbe  

  mov dx,offset(no_univbe_installed_msg)
  mov ah,9h
  int 21h  

  jmp uninstall
  
org_univbe:    
  mov dx,offset(org_univbe_installed_msg)
  jmp error
  
no_univbe:
  mov dx,offset(no_univbe_msg)
  mov ah,9h
  int 21h  
  jmp install

no_vesa_signature:
  mov dx,offset(no_vesa_signature_msg)
  jmp error
  
vesa_info_error:
  mov dx,offset(no_vesa_info_error_msg)
  jmp error
  
free_environment_failed:  
  mov dx,offset(free_environment_failed_msg)
  jmp error
  
free_resident_failed:
  mov dx,offset(free_resident_failed_msg)
  jmp error
  
error:
  mov ah,9h
  int 21h  
  jmp exit
  
install:
  mov ax,resident_seg
  mov ds,ax
  assume ds:resident_seg
  
  ; safe old VIDEO BIOS interrupt handler ptr
  mov ah,35h
  mov al,VIDEO_BIOS_INT
  int 21h
  ; es:bx = vector
  mov [tsr_info.old_int10h.segm],es
  mov [tsr_info.old_int10h.offs],bx
  
  ; safe old interrupt Mux handler ptr
  mov ah,35h
  mov al,MUX_INT
  int 21h
  ; es:bx = vector
  mov [tsr_info.old_int2fh.segm],es
  mov [tsr_info.old_int2fh.offs],bx  

  ; install new handler
  mov ah,25h
  mov al,VIDEO_BIOS_INT
  ; ds = resident_seg
  mov dx,offset(detoured_int10h)
  int 21h

  ; install new handler
  mov ah,25h
  mov al,MUX_INT
  ; ds = resident_seg
  mov dx,offset(detoured_int2fh)
  int 21h
  
  push ds
  pop es
  mov ax,configure_seg
  mov ds,ax
  assume ds:configure_seg

  ; safe load segment of TSR
  mov ax,psp_seg
  mov es:[tsr_info.load_segment],ax

  mov dx,offset(installed_msg)
  mov ah,9h
  int 21h

  ; free environment
  push es
  mov ax,psp_seg
  mov es,ax
  mov ah,49h
  int 21h
  pop es
  jc free_environment_failed

  ; stay resident
  mov dx,RESIDENT_PARAGRAPHS
  mov ah,31h
  mov al,0
  int 21h 
  
uninstall:
  mov ah,35h
  mov al,VIDEO_BIOS_INT
  int 21h
  ; es:bx = vector ptr

  ; ignore bx offset
  ; es:offset(tsr_info)
  mov di,offset tsr_info

  ; install old 10h handler
  push ds
  lds dx,es:[di+TSR_info_t.old_int10h]
  mov ah,25h
  mov al,VIDEO_BIOS_INT
  int 21h
  pop ds

  ; install old 2fh handler
  push ds
  lds dx,es:[di+TSR_info_t.old_int2fh]
  mov ah,25h
  mov al,MUX_INT
  int 21h
  pop ds

  mov ax,es:[di+TSR_info_t.load_segment]
  mov es,ax
  mov ah,49h
  int 21h
  jc free_resident_failed

  mov dx,offset(uninstalled_msg)
  mov ah,9h
  int 21h  

exit:
  mov ax,4c00h
  int 21h

configure_seg ends

stack_seg segment use16 stack
  db 256 dup (?)
stack_seg ends

end start