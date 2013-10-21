;/*
; * DCT functions ASM version for the XingMPG Decoder
; *
; * Copyright (c) 1999, Jean-Michel HERVE
; *
; * Code : TuO / StG
; * Date : 08/04/99
; *
; * WARNING : only _fdct32_* has been tested. The other ones are made from
; *           this function, but hasn't been tested at all. Should check it.
; * NOTE : I don't think that forward_bf and back_bf macros can be more
; *        optimized (except maybe in changing algo)
; *
; * This program is free software; you can redistribute it and/or modify
; * it under the terms of the GNU General Public License as published by
; * the Free Software Foundation; either version 2 of the License, or
; * (at your option) any later version.
; * 
; * This program is distributed in the hope that it will be useful,
; * but WITHOUT ANY WARRANTY; without even the implied warranty of
; * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; * GNU General Public License for more details.
; * 
; * You should have received a copy of the GNU General Public License
; * along with this program; if not, write to the Free Software
; * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
; * 
; * $Id: mp3_fdct.asm,v 1.1 2007/07/30 02:03:53 huchangyin Exp $
; */

  BITS 32

SECTION .data USE32

GLOBAL _forward_bf_asm
GLOBAL _back_bf_asm

;/* fdct function exportation */
GLOBAL _fdct32_asm
GLOBAL _fdct32_dual_asm

EXTERN _coef32

ValC    dd 0
Val4N   dd 0
ValN    dd 0
ValN2   dd 0
ValN21  dd 0
ValX    dd 0
ValF    dd 0
ValCoef dd 0
ValP0   dd 0

;/* 32 floats tables */
tab_a   dd 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
tab_b   dd 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

SECTION .text USE32

;/*
; * forward_bf ASM Version
; */
_forward_bf_asm: ;// PROC m,  n,    x[],  f[],   coef[] :  DWORD
                 ;//      ebp ebp+4 ebp+8 ebp+12 ebp+16
        push   ebp
        lea    ebp,[esp+8]
        pushad

        ;/* Initialization part */
        mov    eax,[ebp+4]
        mov    [ValN],eax
        shr    eax,1
        mov    [ValN2],eax             ;/* n2 is eax */
        mov    esi,[ebp+8]             ;/* x[0] is esi */
        mov    ebx,[ebp+12]            ;/* f[0] is ebx */
        lea    edx,[ebx+eax*4]         ;/* f[n2] is edx */
        mov    edi,[ebp+16]            ;/* coef[] is edi */
        mov    [ValCoef],edi

        mov    dword [ValP0],0

        mov    ecx,[ebp]               ;/* m-controled loop */
      .BigLoop:
        push   ecx

        mov    edi,[ValCoef]           ;/* coef is edi */
        mov    ecx,[ValP0]             ;/* p is ecx */
        mov    ebp,ecx
        add    ebp,[ValN2]
        dec    ebp                     ;/* q-n2 is ebp */
        lea    ebp,[esi+ebp*4]         ;/* x[q-n2] is ebp */
                                       ;/* This way we can use the loop */
                                       ;/* counter to index the table */
                                       ;/* it save 1 dec per loop */
        mov    eax,[ValN2]             ;/* n2-controled loop */
      .SmallLoop:
        fld    dword [esi+ecx*4]       ;/* Load x[p] */
        fld    st0                     ;/* Duplicate x[p] */
        fld    dword [ebp+eax*4]       ;/* Load x[q] */
        fld    st0                     ;/* Duplicate x[q] */
        faddp  st2,st0                 ;/* x[p]+x[q] */
        fxch   st0,st1                 ;/* put value to the top */
        fstp   dword [ebx+ecx*4]       ;/* Store value */
        fsubp  st1,st0                 ;/* x[p]-x[q] */
        fmul   dword [edi]             ;/* coef[k] * (x[p]-x[q]) */
        fstp   dword [edx+ecx*4]

        add    edi,4                   ;/* Update coef[k] */
        inc    ecx                     ;/* Update p */
        dec    eax                     ;/* Update loop counter */
        jnz    .SmallLoop

        mov    eax,[ValN]
        add    [ValP0],eax             ;/* Update p0 */

        pop    ecx
        dec    ecx
        jnz    .BigLoop

        popad
        pop    ebp
        ret

;/*
; * back_bf ASM Version
; */
_back_bf_asm: ;// PROC m,  n,    x[],  f[],   :  DWORD
              ;//      ebp ebp+4 ebp+8 ebp+12
        push   ebp
        lea    ebp,[esp+8]
        pushad

        mov    edi,[ebp+8]             ;/* x[0] is esi */
        mov    esi,[ebp+12]            ;/* f[0] is esi */

        mov    eax,[ebp+4]
        mov    [ValN],eax
        shl    eax,2
        mov    [Val4N],eax
        shr    eax,3
        mov    [ValN2],eax

        lea    edx,[edi+eax*4]

        dec    eax
        mov    [ValN21],eax

        mov    ecx,[ebp]
        mov    ebp,edx
      .BigLoop:

        xor    ebx,ebx
        mov    eax,[ValN2]
      .SmallLoop1:
        mov    edx,[edi+ebx*4]
        mov    [esi+ebx*8],edx         ;/* f[p]=x[q] */
        inc    ebx
        dec    eax
        jnz    .SmallLoop1

        xor    ebx,ebx

        mov    eax,[ValN21]            ;/* n/2 -1 */
      .SmallLoop2:

        fld    dword [ebp+ebx*4]       ;/* x[q] */
        fadd   dword [ebp+ebx*4+4]     ;/* x[q]+x[q+1] */
        fstp   dword [esi+ebx*8+4]     ;/* f[p]=x[q]+x[q+1] */
        inc    ebx
        dec    eax
        jnz    .SmallLoop2

        mov    edx,[ebp+ebx*4]
        mov    [esi+ebx*8+4],edx         ;/* f[p]=x[q] */

        mov    eax,[Val4N]
        add    edi,eax                 ;/* Update p0 (edi) */
        add    ebp,eax                 ;/* Update p0 (edi) */
        add    esi,eax                 ;/* Update p0 (esi) */

        dec    ecx
        jnz    .BigLoop

        popad
        pop    ebp
        ret

;/*
; * forward_bf macro ASM Version
; */
%macro forward_bf_macro 5 ;// PROC m,  n,    x[],  f[],   coef[] :  DWORD
                          ;//      ebp ebp+4 ebp+8 ebp+12 ebp+16
        ;/* Initialization part */
        mov    ebx,%4                  ;/* f[0] is ebx */
        mov    esi,%3                  ;/* x[0] is esi */
        lea    edx,[ebx+((%2)/2)*4]    ;/* f[n2] is edx */

        xor    edi,edi                 ;/* edi is p0 */

        mov    ecx,%1                  ;/* m-controled loop */
      %%BigLoop:
;/* Hiho, unrolled loop */
%assign i 0
%assign j %2-1
%rep (%2)/2
        fld    dword [esi+edi*4+i*4]   ;/* Load x[p] */
        fld    st0                     ;/* Duplicate x[p] */
        fld    dword [esi+edi*4+j*4]   ;/* Load x[q] */
        fadd   st1,st0                 ;/* x[p]+x[q] */
        fxch   st1                     ;/* put value to the top */
        fstp   dword [ebx+edi*4+i*4]   ;/* Store value */
        fsubp  st1,st0                 ;/* x[p]-x[q] */
        fmul   dword [%5+i*4]          ;/* coef[k] * (x[p]-x[q]) */
        fstp   dword [edx+edi*4+i*4]
%assign i i+1
%assign j j-1
%endrep

        add    edi,%2                  ;/* Update p0 */

        dec    ecx
        jnz    near %%BigLoop
%endmacro

;/*
; * back_bf macro ASM Version
; */
%macro back_bf_macro 4 ;// PROC m,  n,    x[],  f[],   :  DWORD
                       ;//      ebp ebp+4 ebp+8 ebp+12
        mov    edi,%3                  ;/* x[0] is esi */
        mov    esi,%4                  ;/* f[0] is esi */
        mov    ebp,%3+((%2/2)*4)       ;/* x[n2] is ebp */
        xor    eax,eax                 ;/* p0 is eax */

;/* Main loop counter initialization (if needed!) */
%if (%1)>1
        mov    ecx,%1                  ;/* Avoid 1 step loop */
      %%BigLoop:
%endif

;/* Unrolled loop */
%assign j 0
%rep (%2)/2
        mov    edx,[edi+eax+j*4]
        mov    [esi+eax+j*8],edx       ;/* f[p]=x[q] */
%assign j j+1
%endrep


;/* Another unrolled loop */
%assign j 0
%rep ((%2)/2)-1
        fld    dword [ebp+eax+j*4]     ;/* x[q] */
        fadd   dword [ebp+eax+j*4+4]   ;/* x[q]+x[q+1] */
        fstp   dword [esi+eax+j*8+4]   ;/* f[p]=x[q]+x[q+1] */
%assign j j+1
%endrep

        mov    edx,[ebp+eax+j*4]
        mov    [esi+eax+j*8+4],edx     ;/* f[p]=x[q] */

        add    eax,%2*4                ;/* Update p0 */

;/* Loop */
%if (%1)>1                             ;/* Avoid 1 step loop */
        dec    ecx
        jnz    near %%BigLoop
%endif
%endmacro

;/* Special case of fdct32 and fdct32_dual functions */
%macro fdct32_specialcase 1
        mov    edi,[ebp]               ;/* x[0] is edi */
;/* Unrolled loop */
%assign p 0
%assign q 31
%rep 16
        fld    dword [edi+p*4*%1]      ;/* Load x[p] */
        fld    st0
        fld    dword [edi+q*4*%1]      ;/* Load x[q] */
        fadd   st1,st0                 ;/* x[p]+x[q] */
        fxch   st1                     ;/* put value to the top */
        fstp   dword [tab_a+p*4]       ;/* Store value */
        fsubp  st1,st0                 ;/* x[p]-x[q] */
        fmul   dword [_coef32+p*4]     ;/* coef32[p] * (x[p]-x[q]) */
        fstp   dword [tab_a+p*4+16*4]
%assign p p+1
%assign q q-1
%endrep
%endmacro

;/*
; * fdct32 ASM Version
; */
_fdct32_asm: ;// PROC x[], c[] :  DWORD
             ;//      ebp  ebp+4
        push   ebp
        lea    ebp,[esp+8]
        pushad

        mov    eax,[ebp+4]
        mov    [ValC],eax

        ;/* Special first stage for single chan */
        fdct32_specialcase 1

        forward_bf_macro 2 , 16, tab_a, tab_b, _coef32 + (16)*4
        forward_bf_macro 4 , 8 , tab_b, tab_a, _coef32 + (16 + 8)*4
        forward_bf_macro 8 , 4 , tab_a, tab_b, _coef32 + (16 + 8 + 4)*4
        forward_bf_macro 16, 2 , tab_b, tab_a, _coef32 + (16 + 8 + 4 + 2)*4
        back_bf_macro    8 , 4 , tab_a, tab_b
        back_bf_macro    4 , 8 , tab_b, tab_a
        back_bf_macro    2 , 16, tab_a, tab_b
        back_bf_macro    1 , 32, tab_b, [ValC]

        popad
        pop    ebp
        ret

;/*
; * fdct32_dual ASM Version
; */
_fdct32_dual_asm: ;// PROC x[], c[] :  DWORD
                  ;//      ebp  ebp+4
        push   ebp
        lea    ebp,[esp+8]
        pushad

        mov    eax,[ebp+4]
        mov    [ValC],eax

        ;/* Special first stage for dual chan (interleaved x) */
        fdct32_specialcase 2

        forward_bf_macro 2 , 16, tab_a, tab_b, _coef32 + (16)*4
        forward_bf_macro 4 , 8 , tab_b, tab_a, _coef32 + (16 + 8)*4
        forward_bf_macro 8 , 4 , tab_a, tab_b, _coef32 + (16 + 8 + 4)*4
        forward_bf_macro 16, 2 , tab_b, tab_a, _coef32 + (16 + 8 + 4 + 2)*4
        back_bf_macro    8 , 4 , tab_a, tab_b
        back_bf_macro    4 , 8 , tab_b, tab_a
        back_bf_macro    2 , 16, tab_a, tab_b
        back_bf_macro    1 , 32, tab_b, [ValC]

        popad
        pop    ebp
        ret

end

