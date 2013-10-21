;/*
; * mdct.c ASM version for the XingMPG Decoder
; * Include hybrid and FreqInvert functions
; *
; * Copyright (c) 1999, Jean-Michel HERVE
; *
; * This is hardcore coding, really! But should work pretty fine. If you find
; * a bug mmmmh... tell me where :)
; *
; * Code : TuO / StG
; * Date : 05/04/99
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
; * $Id: mp3_mdct.asm,v 1.1 2007/07/30 02:03:53 huchangyin Exp $
; */

  BITS 32

SECTION .data USE32

GLOBAL _imdct18_asm
GLOBAL _imdct6_3_asm
GLOBAL _hybrid_asm
GLOBAL _FreqInvert_asm

EXTERN _mdct18w
EXTERN _mdct18w2
EXTERN _mdct6_3v
EXTERN _mdct6_3v2
EXTERN _coef
EXTERN _coef87
EXTERN _win
EXTERN _band_limit_nsb

WINSIZE equ 144

;/* temp table */
tab_a dd 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
tab_b dd 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0

My_FPU_Half dd 0.5

SaveReturn dd 0

SECTION .text USE32

;/*
; * imdct18 ASM Version
; */
_imdct18_asm: ;// PROC f[]
              ;//      ebp
        push   ebp
        lea    ebp,[esp+8]
        pushad

        mov    ebp,[ebp]               ;/* Save arg */
        mov    edi,_mdct18w
        mov    esi,_mdct18w2
        mov    eax,tab_a
        mov    ebx,tab_b
        mov    ecx,_coef
;/* Unroll the loop because it is a known-value count */
%assign p 0                            ;/* Use compiler var as counter */
%rep 4
        ;/* First part - compute ap and bp */
        fld    dword [edi+p*4]         ;/* w[p] */
        fmul   dword [ebp+p*4]         ;/* g1 */
        fld    dword [edi+(17-p)*4]    ;/* w[17-p] g1 */
        fmul   dword [ebp+(17-p)*4]    ;/* g2 g1 */
        fld    st0                     ;/* g2 g2 g1 */
        fadd   st2                     ;/* ap g2 g1 */
        fxch   st1                     ;/* g2 ap g1 */
        fsubp  st2,st0                 ;/* ap g1-g2 */
        fxch   st1                     ;/* g1-g2 ap */
        fmul   dword [esi+p*4]         ;/* bp ap */

        ;/* Second part - compute a8p and b8p */
        fld    dword [edi+(8-p)*4]     ;/* w[8-p] bp ap */
        fmul   dword [ebp+(8-p)*4]     ;/* g1 bp ap */
        fld    dword [edi+(9+p)*4]     ;/* w[9+p] g1 bp ap */
        fmul   dword [ebp+(9+p)*4]     ;/* g2 g1 bp ap */
        fld    st0                     ;/* g2 g2 g1 bp ap */
        fadd   st2                     ;/* a8p g2 g1 bp ap */
        fxch   st1                     ;/* g2 a8p g1 bp ap */
        fsubp  st2,st0                 ;/* a8p g1-g2 bp ap */
        fxch   st1                     ;/* g1-g2 a8p bp ap */
        fmul   dword [esi+(8-p)*4]     ;/* b8p a8p bp ap */

        ;/* Final part - finalize computing and store values */
        fld    st3                     ;/* ap b8p a8p bp ap */
        fadd   st2                     ;/* ap+a8p b8p a8p bp ap */
        fstp   dword [eax+p*4]         ;/* b8p a8p bp ap */
        fxch   st1                     ;/* a8p b8p bp ap */
        fsubp  st3,st0                 ;/* b8p bp ap-a8p */
        fxch   st2                     ;/* ap-a8p bp b8p */
        fstp   dword [eax+(p+5)*4]     ;/* bp b8p */
        fxch   st1                     ;/* b8p bp */
        fld    st0                     ;/* b8p b8p bp */
        fadd   st2                     ;/* b8p+bp b8p bp */
        fstp   dword [ebx+p*4]         ;/* b8p bp */
        fsubp  st1,st0                 ;/* bp-b8p */
        fstp   dword [ebx+(p+5)*4]     ;/* - */
%assign p p+1
%endrep

%assign p 4
        ;/* Last - finalize array */
        fld    dword [edi+p*4]         ;/* w[p] */
        fmul   dword [ebp+p*4]         ;/* g1 */
        fld    dword [edi+(17-p)*4]    ;/* w[17-p] g1 */
        fmul   dword [ebp+(17-p)*4]    ;/* g2 g1 */
        fld    st0                     ;/* g2 g2 g1 */
        fadd   st2                     ;/* ap g2 g1 */
        fstp   dword [eax+p*4]         ;/* g2 g1 */
        fsubp  st1,st0                 ;/* g1-g2 */
        fmul   dword [esi+p*4]         ;/* bp */
        fstp   dword [ebx+p*4]         ;/* - */

;/* Now the huge and boring part */
;//   f[0] = 0.5f * (a[0] + a[1] + a[2] + a[3] + a[4]);
;// TO DO : avoid reload of a[4] and b[4]
        fld    dword [eax+0*4]         ;/* a[0] */
        fadd   dword [eax+1*4]         ;/* a[0]+a[1] */
        fadd   dword [eax+2*4]         ;/* a[0]+a[1]+a[2] */
        fadd   dword [eax+3*4]         ;/* a[0]+a[1]+a[2]+a[3] */
        fadd   dword [eax+4*4]         ;/* a[0]+a[1]+a[2]+a[3]+a[4] */
        fmul   dword [My_FPU_Half]       ;/* 0.5*(a[0]+a[1]+a[2]+a[3]+a[4]) */
        fst    dword [ebp+0*4]         ;/* f[0] */
;//   f[1] = 0.5f * (b[0] + b[1] + b[2] + b[3] + b[4]);
        fld    dword [ebx+0*4]         ;/* b[0] f[0] */
        fadd   dword [ebx+1*4]         ;/* b[0]+b[1] f[0] */
        fadd   dword [ebx+2*4]         ;/* b[0]+b[1]+b[2] f[0] */
        fadd   dword [ebx+3*4]         ;/* b[0]+b[1]+b[2]+b[3] f[0] */
        fadd   dword [ebx+4*4]         ;/* b[0]+b[1]+b[2]+b[3]+b[4] f[0] */
        fmul   dword [My_FPU_Half]       ;/* f[1]' f[0] */
;//   f[2] = coef[1][0]*a[5]+coef[1][1]*a[6]+coef[1][2]*a[7]+coef[1][3]*a[8];
        fld    dword [ecx+1*16+0*4]    ;/* coef[1][0]  f[1]' f[0] */
        fld    dword [ecx+1*16+1*4]    ;/* coef[1][1]  coef[1][0] f[1]' f[0] */
        fld    dword [ecx+1*16+2*4]    ;/* coef[1][2]  coef[1][1] coef[1][0] f[1]' f[0] */
        fld    dword [ecx+1*16+3*4]    ;/* coef[1][3]  coef[1][2] coef[1][1] coef[1][0] f[1]' f[0] */
        fld    st3                     ;/* coef[1][0]  coef[1][3] coef[1][2] coef[1][1] coef[1][0] f[1]' f[0] */
        fmul   dword [eax+5*4]         ;/* v0          coef[1][3] coef[1][2] coef[1][1] coef[1][0] f[1]' f[0] */
        fld    st3                     ;/* coef[1][1]  v0         coef[1][3] coef[1][2] coef[1][1] coef[1][0] f[1]' f[0] */
        fmul   dword [eax+6*4]         ;/* v1          v0         coef[1][3] coef[1][2] coef[1][1] coef[1][0] f[1]' f[0] */
        faddp  st1,st0                 ;/* v1+v0       coef[1][3] coef[1][2] coef[1][1] coef[1][0] f[1]' f[0] */
        fld    st2                     ;/* coef[1][2]  v1+v0      coef[1][3] coef[1][2] coef[1][1] coef[1][0] f[1]' f[0] */
        fmul   dword [eax+7*4]         ;/* v2 v1+v0    coef[1][3] coef[1][2] coef[1][1] coef[1][0] f[1]' f[0] */
        faddp  st1,st0                 ;/* v2+v1+v0    coef[1][3] coef[1][2] coef[1][1] coef[1][0] f[1]' f[0] */
        fld    st1                     ;/* coef[1][3]  v2+v1+v0   coef[1][3] coef[1][2] coef[1][1] coef[1][0] f[1]' f[0] */
        fmul   dword [eax+8*4]         ;/* v3 v2+v1+v0 coef[1][3] coef[1][2] coef[1][1] coef[1][0] f[1]' f[0] */
        faddp  st1,st0                 ;/* f[2]'       coef[1][3] coef[1][2] coef[1][1] coef[1][0] f[1]' f[0] */
;//   f[3] = coef[1][0]*b[5]+coef[1][1]*b[6]+coef[1][2]*b[7]+coef[1][3]*b[8]-f[1];
        fxch   st4                     ;/* coef[1][0]  coef[1][3] coef[1][2] coef[1][1] f[2]'      f[1]' f[0] */
        fmul   dword [ebx+5*4]         ;/* v0          coef[1][3] coef[1][2] coef[1][1] f[2]'      f[1]' f[0] */
        fxch   st3                     ;/* coef[1][1]  coef[1][3] coef[1][2] v0         f[2]'      f[1]' f[0] */
        fmul   dword [ebx+6*4]         ;/* v1          coef[1][3] coef[1][2] v0         f[2]'      f[1]' f[0] */
        faddp  st3,st0                 ;/* coef[1][3]  coef[1][2] v0+v1      f[2]'      f[1]' f[0] */
        fxch   st1                     ;/* coef[1][2]  coef[1][3] v0+v1      f[2]'      f[1]' f[0] */
        fmul   dword [ebx+7*4]         ;/* v2          coef[1][3] v0+v1      f[2]'      f[1]' f[0] */
        faddp  st2,st0                 ;/* coef[1][3]  v0+v1+v2   f[2]'      f[1]' f[0] */
        fmul   dword [ebx+8*4]         ;/* v3          v0+v1+v2   f[2]'      f[1]' f[0] */
        faddp  st1,st0                 ;/* v0+v1+v2+v3 f[2]'      f[1]' f[0] */
        fsub   st0,st2                 ;/* f[3]'       f[2]'      f[1]' f[0] */
;//   f[1] = f[1] - f[0];
        fxch   st3                     ;/* f[0]        f[2]'      f[1]' f[3]' */
        fsubp  st2,st0                 ;/* f[2]'       f[1]       f[3]' */
        fxch   st1                     ;/* f[1]        f[2]'      f[3]' */
        fst    dword [ebp+1*4]         ;/* f[1]        f[2]'      f[3]' */
;//   f[2] = f[2] - f[1];
        fsubp  st1,st0                 ;/* f[2]        f[3]' */
        fst    dword [ebp+2*4]         ;/* f[2]        f[3]' */
        fxch   st1                     ;/* f[3]'       f[2] */
;//   f[4]=coef[2][0]*a[0]+coef[2][1]*a[1]+coef[2][2]*a[2]+coef[2][3]*a[3]-a[4];
        fld    dword [ecx+2*16+0*4]    ;/* c[2,0] f[3]' f[2] */
        fld    dword [ecx+2*16+1*4]    ;/* c[2,1] c[2,0] f[3]' f[2] */
        fld    dword [ecx+2*16+2*4]    ;/* c[2,2] c[2,1] c[2,0] f[3]' f[2] */
        fld    dword [ecx+2*16+3*4]    ;/* c[2,3] c[2,2] c[2,1] c[2,0] f[3]' f[2] */
        fld    st3                     ;/* c[2,0] c[2,3] c[2,2] c[2,1] c[2,0] f[3]' f[2] */
        fmul   dword [eax+0*4]         ;/* v0     c[2,3] c[2,2] c[2,1] c[2,0] f[3]' f[2] */
        fld    st3                     ;/* c[2,1] v0     c[2,3] c[2,2] c[2,1] c[2,0] f[3]' f[2] */
        fmul   dword [eax+1*4]         ;/* v1     v0     c[2,3] c[2,2] c[2,1] c[2,0] f[3]' f[2] */
        faddp  st1,st0                 ;/* v1+v0  c[2,3] c[2,2] c[2,1] c[2,0] f[3]' f[2] */
        fld    st2                     ;/* c[2,2] v1+v0  c[2,3] c[2,2] c[2,1] c[2,0] f[3]' f[2] */
        fmul   dword [eax+2*4]         ;/* v2     v1+v0  c[2,3] c[2,2] c[2,1] c[2,0] f[3]' f[2] */
        faddp  st1,st0                 ;/* v0v1v2 c[2,3] c[2,2] c[2,1] c[2,0] f[3]' f[2] */
        fld    st1                     ;/* c[2,3] v0v1v2 c[2,3] c[2,2] c[2,1] c[2,0] f[3]' f[2] */
        fmul   dword [eax+3*4]         ;/* v3     v0v1v2 c[2,3] c[2,2] c[2,1] c[2,0] f[3]' f[2] */
        faddp  st1,st0                 ;/* f[4]'  c[2,3] c[2,2] c[2,1] c[2,0] f[3]' f[2] */
        fsub   dword [eax+4*4]         ;/* f[4]'  c[2,3] c[2,2] c[2,1] c[2,0] f[3]' f[2] */
        fxch   st4                     ;/* c[2,0] c[2,3] c[2,2] c[2,1] f[4]'  f[3]' f[2] */
;//   f[5]=coef[2][0]*b[0]+coef[2][1]*b[1]+coef[2][2]*b[2]+coef[2][3]*b[3]-b[4]-f[3];
        fmul   dword [ebx+0*4]         ;/* v0     c[2,3] c[2,2] c[2,1] f[4]'  f[3]' f[2] */
        fxch   st3                     ;/* c[2,1] c[2,3] c[2,2] v0     f[4]'  f[3]' f[2] */
        fmul   dword [ebx+1*4]         ;/* v1     c[2,3] c[2,2] v0     f[4]'  f[3]' f[2] */
        faddp  st3,st0                 ;/* c[2,3] c[2,2] v0+v1  f[4]'  f[3]' f[2] */
        fxch   st1                     ;/* c[2,2] c[2,3] v0+v1  f[4]'  f[3]' f[2] */
        fmul   dword [ebx+2*4]         ;/* v2     c[2,3] v0+v1  f[4]'  f[3]' f[2] */
        faddp  st2,st0                 ;/* c[2,3] v0v1v2 f[4]'  f[3]' f[2] */
        fmul   dword [ebx+3*4]         ;/* v3     v0v1v2 f[4]'  f[3]' f[2] */
        faddp  st1,st0                 ;/* f[5]'' f[4]'  f[3]' f[2] */
        fsub   dword [ebx+4*4]         ;/* f[5]'' f[4]'  f[3]' f[2] */
        fsub   st2                     ;/* f[5]'  f[4]'  f[3]' f[2] */
;//   f[3] = f[3] - f[2];
        fxch   st3                     ;/* f[2]   f[4]'  f[3]' f[5]' */
        fsubp  st2,st0                 ;/* f[4]'  f[3]   f[5]' */
        fxch   st1                     ;/* f[3]   f[4]'  f[5]' */
        fst    dword [ebp+3*4]         ;/* f[3]   f[4]'  f[5]' */
;//   f[4] = f[4] - f[3];
        fsubp  st1,st0                 ;/* f[4]   f[5]' */
        fst    dword [ebp+4*4]         ;/* f[4]   f[5]' */
        fxch   st1                     ;/* f[5]'  f[4] */
;//   f[6] = coef[3][0] * (a[5] - a[7] - a[8]);
        fld    dword [eax+5*4]         ;/* a[5] f[5]' f[4] */
        fsub   dword [eax+7*4]         ;/* a[5]-a[7] f[5]' f[4] */
        fsub   dword [eax+8*4]         ;/* a[5]-a[7]-a[8] f[5]' f[4] */
        fmul   dword [ecx+3*16+0*4]    ;/* f[6]' f[5]' f[4] */
;//   f[7] = coef[3][0] * (b[5] - b[7] - b[8]) - f[5];
        fld    dword [ebx+5*4]         ;/* b[5] f[6]' f[5]' f[4] */
        fsub   dword [ebx+7*4]         ;/* b[5]-b[7] f[6]' f[5]' f[4] */
        fsub   dword [ebx+8*4]         ;/* b[5]-b[7]-b[8] f[6]' f[5]' f[4] */
        fmul   dword [ecx+3*16+0*4]    ;/* c[3,0]*(b5-b7-b8) f[6]' f[5]' f[4] */
        fsub   st2                     ;/* f[7]' f[6]' f[5]' f[4] */
;//   f[5] = f[5] - f[4];
        fxch   st3                     ;/* f[4] f[6]' f[5]' f[7]' */
        fsubp  st2,st0                 ;/* f[6]' f[5] f[7]' */
        fxch   st1                     ;/* f[5] f[6]' f[7]' */
        fst    dword [ebp+5*4]         ;/* f[5] f[6]' f[7]' */
;//   f[6] = f[6] - f[5];
        fsubp  st1,st0                 ;/* f[6] f[7]' */
        fst    dword [ebp+6*4]         ;/* f[6] f[7]' */
        fxch   st1                     ;/* f[7]' f[6] */
;//   f[8] = coef[4][0]*a[0]+coef[4][1]*a[1]+coef[4][2]*a[2]+coef[4][3]*a[3]+a[4];
        fld    dword [ecx+4*16+0*4]    ;/* c[4,0] f[7]' f[6] */
        fld    dword [ecx+4*16+1*4]    ;/* c[4,1] c[4,0] f[7]' f[6] */
        fld    dword [ecx+4*16+2*4]    ;/* c[4,2] c[4,1] c[4,0] f[7]' f[6] */
        fld    dword [ecx+4*16+3*4]    ;/* c[4,3] c[4,2] c[4,1] c[4,0] f[7]' f[6] */
        fld    st3                     ;/* c[4,0] c[4,3] c[4,2] c[4,1] c[4,0] f[7]' f[6] */
        fmul   dword [eax+0*4]         ;/* v0     c[4,3] c[4,2] c[4,1] c[4,0] f[7]' f[6] */
        fld    st3                     ;/* c[4,1] v0     c[4,3] c[4,2] c[4,1] c[4,0] f[7]' f[6] */
        fmul   dword [eax+1*4]         ;/* v1     v0     c[4,3] c[4,2] c[4,1] c[4,0] f[7]' f[6] */
        faddp  st1,st0                 ;/* v1+v0  c[4,3] c[4,2] c[4,1] c[4,0] f[7]' f[6] */
        fld    st2                     ;/* c[4,2] v1+v0  c[4,3] c[4,2] c[4,1] c[4,0] f[7]' f[6] */
        fmul   dword [eax+2*4]         ;/* v2     v1+v0  c[4,3] c[4,2] c[4,1] c[4,0] f[7]' f[6] */
        faddp  st1,st0                 ;/* v0v1v2 c[4,3] c[4,2] c[4,1] c[4,0] f[7]' f[6] */
        fld    st1                     ;/* c[4,3] v0v1v2 c[4,3] c[4,2] c[4,1] c[4,0] f[7]' f[6] */
        fmul   dword [eax+3*4]         ;/* v3     v0v1v2 c[4,3] c[4,2] c[4,1] c[4,0] f[7]' f[6] */
        faddp  st1,st0                 ;/* f[8]'  c[4,3] c[4,2] c[4,1] c[4,0] f[7]' f[6] */
        fadd   dword [eax+4*4]         ;/* f[8]'  c[4,3] c[4,2] c[4,1] c[4,0] f[7]' f[6] */        ;// CHECK HERE
        fxch   st4                     ;/* c[4,0] c[4,3] c[4,2] c[4,1] f[8]'  f[7]' f[6] */
;//   f[9] = coef[4][0]*b[0]+coef[4][1]*b[1]+coef[4][2]*b[2]+coef[4][3]*b[3]+b[4]-f[7];
        fmul   dword [ebx+0*4]         ;/* v0     c[4,3] c[4,2] c[4,1] f[8]'  f[7]' f[6] */
        fxch   st3                     ;/* c[4,1] c[4,3] c[4,2] v0     f[8]'  f[7]' f[6] */
        fmul   dword [ebx+1*4]         ;/* v1     c[4,3] c[4,2] v0     f[8]'  f[7]' f[6] */
        faddp  st3,st0                 ;/* c[4,3] c[4,2] v0+v1  f[8]'  f[7]' f[6] */
        fxch   st1                     ;/* c[4,2] c[4,3] v0+v1  f[8]'  f[7]' f[6] */
        fmul   dword [ebx+2*4]         ;/* v2     c[4,3] v0+v1  f[8]'  f[7]' f[6] */
        faddp  st2,st0                 ;/* c[4,3] v0v1v2 f[8]'  f[7]' f[6] */
        fmul   dword [ebx+3*4]         ;/* v3     v0v1v2 f[8]'  f[7]' f[6] */
        faddp  st1,st0                 ;/* f[9]'' f[8]'  f[7]' f[6] */
        fadd   dword [ebx+4*4]         ;/* f[9]'' f[8]'  f[7]' f[6] */  ;// CHECK HERE
        fsub   st2                     ;/* f[9]'  f[8]'  f[7]' f[6] */
;//   f[7] = f[7] - f[6];
        fxch   st3                     ;/* f[6]   f[8]'  f[7]' f[9]' */
        fsubp  st2,st0                 ;/* f[8]'  f[7]   f[9]' */
        fxch   st1                     ;/* f[7]   f[8]'  f[9]' */
        fst    dword [ebp+7*4]         ;/* f[7]   f[8]'  f[9]' */
;//   f[8] = f[8] - f[7];
        fsubp  st1,st0                 ;/* f[8]   f[9]' */
        fst    dword [ebp+8*4]         ;/* f[8]   f[9]' */
        fxch   st1                     ;/* f[9]'  f[8] */
;//   f[10] = coef[5][0]*a[5]+coef[5][1]*a[6]+coef[5][2]*a[7]+coef[5][3]*a[8];
        fld    dword [ecx+5*16+0*4]    ;/* coef[5][0]  f[9]' f[8] */
        fld    dword [ecx+5*16+1*4]    ;/* coef[5][1]  coef[5][0] f[9]' f[8] */
        fld    dword [ecx+5*16+2*4]    ;/* coef[5][2]  coef[5][1] coef[5][0] f[9]' f[8] */
        fld    dword [ecx+5*16+3*4]    ;/* coef[5][3]  coef[5][2] coef[5][1] coef[5][0] f[9]' f[8] */
        fld    st3                     ;/* coef[5][0]  coef[5][3] coef[5][2] coef[5][1] coef[5][0] f[9]' f[8] */
        fmul   dword [eax+5*4]         ;/* v0          coef[5][3] coef[5][2] coef[5][1] coef[5][0] f[9]' f[8] */
        fld    st3                     ;/* coef[5][1]  v0         coef[5][3] coef[5][2] coef[5][1] coef[5][0] f[9]' f[8] */
        fmul   dword [eax+6*4]         ;/* v1          v0         coef[5][3] coef[5][2] coef[5][1] coef[5][0] f[9]' f[8] */
        faddp  st1,st0                 ;/* v1+v0       coef[5][3] coef[5][2] coef[5][1] coef[5][0] f[9]' f[8] */
        fld    st2                     ;/* coef[5][2]  v1+v0      coef[5][3] coef[5][2] coef[5][1] coef[5][0] f[9]' f[8] */
        fmul   dword [eax+7*4]         ;/* v2 v1+v0    coef[5][3] coef[5][2] coef[5][1] coef[5][0] f[9]' f[8] */
        faddp  st1,st0                 ;/* v2+v1+v0    coef[5][3] coef[5][2] coef[5][1] coef[5][0] f[9]' f[8] */
        fld    st1                     ;/* coef[5][3]  v2+v1+v0   coef[5][3] coef[5][2] coef[5][1] coef[5][0] f[9]' f[8] */
        fmul   dword [eax+8*4]         ;/* v3 v2+v1+v0 coef[5][3] coef[5][2] coef[5][1] coef[5][0] f[9]' f[8] */
        faddp  st1,st0                 ;/* f[10]'      coef[5][3] coef[5][2] coef[5][1] coef[5][0] f[9]' f[8] */
;//   f[11] = coef[5][0]*b[5]+coef[5][1]*b[6]+coef[5][2]*b[7]+coef[5][3]*b[8]-f[9];
        fxch   st4                     ;/* coef[5][0]  coef[5][3] coef[5][2] coef[5][1] f[10]'     f[9]' f[8] */
        fmul   dword [ebx+5*4]         ;/* v0          coef[5][3] coef[5][2] coef[5][1] f[10]'     f[9]' f[8] */
        fxch   st3                     ;/* coef[5][1]  coef[5][3] coef[5][2] v0         f[10]'     f[9]' f[8] */
        fmul   dword [ebx+6*4]         ;/* v1          coef[5][3] coef[5][2] v0         f[10]'     f[9]' f[8] */
        faddp  st3,st0                 ;/* coef[5][3]  coef[5][2] v0+v1      f[10]'     f[9]' f[8] */
        fxch   st1                     ;/* coef[5][2]  coef[5][3] v0+v1      f[10]'     f[9]' f[8] */
        fmul   dword [ebx+7*4]         ;/* v2          coef[5][3] v0+v1      f[10]'     f[9]' f[8] */
        faddp  st2,st0                 ;/* coef[5][3]  v0+v1+v2   f[10]'     f[9]' f[8] */
        fmul   dword [ebx+8*4]         ;/* v3          v0+v1+v2   f[10]'     f[9]' f[8] */
        faddp  st1,st0                 ;/* v0+v1+v2+v3 f[10]'     f[9]' f[8] */
        fsub   st0,st2                 ;/* f[11]'      f[10]'     f[9]' f[8] */
;//   f[9] = f[9] - f[8];
        fxch   st3                     ;/* f[8]        f[10]'     f[9]' f[11]' */
        fsubp  st2,st0                 ;/* f[10]'      f[9]       f[11]' */
        fxch   st1                     ;/* f[9]        f[10]'     f[11]' */
        fst    dword [ebp+9*4]         ;/* f[9]        f[10]'     f[11]' */
;//   f[10] = f[10] - f[9];
        fsubp  st1,st0                 ;/* f[10]       f[11]' */
        fst    dword [ebp+10*4]        ;/* f[10]       f[11]' */
        fxch   st1                     ;/* f[11]'      f[10]  */
;//   f[12] = 0.5f * (a[0] + a[2] + a[3]) - a[1] - a[4];
        fld    dword [eax+0*4]         ;/* a[0] f[11]' f[10]  */
        fadd   dword [eax+2*4]         ;/* a[0]+a[2] f[11]' f[10]  */
        fadd   dword [eax+3*4]         ;/* a[0]+a[2]+a[3] f[11]' f[10]  */
        fmul   dword [My_FPU_Half]       ;/* 0.5*(a[0]+a[2]+a[3]) f[11]' f[10]  */
        fsub   dword [eax+1*4]         ;/* 0.5*(a[0]+a[2]+a[3])-a[1] f[11]' f[10]  */
        fsub   dword [eax+4*4]         ;/* f[12]' f[11]' f[10]  */
;//   f[13] = 0.5f * (b[0] + b[2] + b[3]) - b[1] - b[4] - f[11];
        fld    dword [ebx+0*4]         ;/* b[0] f[12]' f[11]' f[10]  */
        fadd   dword [ebx+2*4]         ;/* b[0]+b[2] f[12]' f[11]' f[10]  */
        fadd   dword [ebx+3*4]         ;/* b[0]+b[2]+b[3] f[12]' f[11]' f[10]  */
        fmul   dword [My_FPU_Half]       ;/* 0.5*(b[0]+b[2]+b[3]) f[12]' f[11]' f[10]  */
        fsub   dword [ebx+1*4]         ;/* 0.5*(b[0]+b[2]+b[3])-b[1] f[12]' f[11]' f[10]  */
        fsub   dword [ebx+4*4]         ;/* 0.5*(b[0]+b[2]+b[3])-b[1]-b[4] f[12]' f[11]' f[10]  */
        fsub   st2                     ;/* f[13]' f[12]' f[11]' f[10]  */
;//   f[11] = f[11] - f[10];
        fxch   st3                     ;/* f[10]  f[12]' f[11]' f[13]' */
        fsubp  st2,st0                 ;/* f[12]' f[11]  f[13]' */
        fxch   st1                     ;/* f[11]  f[12]' f[13]' */
        fst    dword [ebp+11*4]        ;/* f[11]  f[12]' f[13]' */
;//   f[12] = f[12] - f[11];
        fsubp  st1,st0                 ;/* f[12]       f[13]' */
        fst    dword [ebp+12*4]        ;/* f[12]       f[13]' */
        fxch   st1                     ;/* f[13]'      f[12]  */
;//   f[14] = coef[7][0]*a[5]+coef[7][1]*a[6]+coef[7][2]*a[7]+coef[7][3]*a[8];
        fld    dword [ecx+7*16+0*4]    ;/* coef[7][0]  f[13]' f[12] */
        fld    dword [ecx+7*16+1*4]    ;/* coef[7][1]  coef[7][0] f[13]' f[12] */
        fld    dword [ecx+7*16+2*4]    ;/* coef[7][2]  coef[7][1] coef[7][0] f[13]' f[12] */
        fld    dword [ecx+7*16+3*4]    ;/* coef[7][3]  coef[7][2] coef[7][1] coef[7][0] f[13]' f[12] */
        fld    st3                     ;/* coef[7][0]  coef[7][3] coef[7][2] coef[7][1] coef[7][0] f[13]' f[12] */
        fmul   dword [eax+5*4]         ;/* v0          coef[7][3] coef[7][2] coef[7][1] coef[7][0] f[13]' f[12] */
        fld    st3                     ;/* coef[7][1]  v0         coef[7][3] coef[7][2] coef[7][1] coef[7][0] f[13]' f[12] */
        fmul   dword [eax+6*4]         ;/* v1          v0         coef[7][3] coef[7][2] coef[7][1] coef[7][0] f[13]' f[12] */
        faddp  st1,st0                 ;/* v1+v0       coef[7][3] coef[7][2] coef[7][1] coef[7][0] f[13]' f[12] */
        fld    st2                     ;/* coef[7][2]  v1+v0      coef[7][3] coef[7][2] coef[7][1] coef[7][0] f[13]' f[12] */
        fmul   dword [eax+7*4]         ;/* v2 v1+v0    coef[7][3] coef[7][2] coef[7][1] coef[7][0] f[13]' f[12] */
        faddp  st1,st0                 ;/* v2+v1+v0    coef[7][3] coef[7][2] coef[7][1] coef[7][0] f[13]' f[12] */
        fld    st1                     ;/* coef[7][3]  v2+v1+v0   coef[7][3] coef[7][2] coef[7][1] coef[7][0] f[13]' f[12] */
        fmul   dword [eax+8*4]         ;/* v3 v2+v1+v0 coef[7][3] coef[7][2] coef[7][1] coef[7][0] f[13]' f[12] */
        faddp  st1,st0                 ;/* f[14]'      coef[7][3] coef[7][2] coef[7][1] coef[7][0] f[13]' f[12] */
;//   f[15] = coef[7][0]*b[5]+coef[7][1]*b[6]+coef[7][2]*b[7]+coef[7][3]*b[8]-f[13];
        fxch   st4                     ;/* coef[7][0]  coef[7][3] coef[7][2] coef[7][1] f[14]'     f[13]' f[12] */
        fmul   dword [ebx+5*4]         ;/* v0          coef[7][3] coef[7][2] coef[7][1] f[14]'     f[13]' f[12] */
        fxch   st3                     ;/* coef[7][1]  coef[7][3] coef[7][2] v0         f[14]'     f[13]' f[12] */
        fmul   dword [ebx+6*4]         ;/* v1          coef[7][3] coef[7][2] v0         f[14]'     f[13]' f[12] */
        faddp  st3,st0                 ;/* coef[7][3]  coef[7][2] v0+v1      f[14]'     f[13]' f[12] */
        fxch   st1                     ;/* coef[7][2]  coef[7][3] v0+v1      f[14]'     f[13]' f[12] */
        fmul   dword [ebx+7*4]         ;/* v2          coef[7][3] v0+v1      f[14]'     f[13]' f[12] */
        faddp  st2,st0                 ;/* coef[7][3]  v0+v1+v2   f[14]'     f[13]' f[12] */
        fmul   dword [ebx+8*4]         ;/* v3          v0+v1+v2   f[14]'     f[13]' f[12] */
        faddp  st1,st0                 ;/* v0+v1+v2+v3 f[14]'     f[13]' f[12] */
        fsub   st0,st2                 ;/* f[15]'      f[14]'     f[13]' f[12] */
;//   f[13] = f[13] - f[12];
        fxch   st3                     ;/* f[12]  f[14]' f[13]' f[15]' */
        fsubp  st2,st0                 ;/* f[14]' f[13]  f[15]' */
        fxch   st1                     ;/* f[13]  f[14]' f[15]' */
        fst    dword [ebp+13*4]        ;/* f[13]  f[14]' f[15]' */
;//   f[14] = f[14] - f[13];
        fsubp  st1,st0                 ;/* f[14]       f[15]' */
        fst    dword [ebp+14*4]        ;/* f[14]       f[15]' */
        fxch   st1                     ;/* f[15]'      f[14]  */
;//   f[16] = coef[8][0]*a[0]+coef[8][1]*a[1]+coef[8][2]*a[2]+coef[8][3]*a[3]+a[4];
        fld    dword [ecx+8*16+0*4]    ;/* c[8,0] f[15]' f[14] */
        fld    dword [ecx+8*16+1*4]    ;/* c[8,1] c[8,0] f[15]' f[14] */
        fld    dword [ecx+8*16+2*4]    ;/* c[8,2] c[8,1] c[8,0] f[15]' f[14] */
        fld    dword [ecx+8*16+3*4]    ;/* c[8,3] c[8,2] c[8,1] c[8,0] f[15]' f[14] */
        fld    st3                     ;/* c[8,0] c[8,3] c[8,2] c[8,1] c[8,0] f[15]' f[14] */
        fmul   dword [eax+0*4]         ;/* v0     c[8,3] c[8,2] c[8,1] c[8,0] f[15]' f[14] */
        fld    st3                     ;/* c[8,1] v0     c[8,3] c[8,2] c[8,1] c[8,0] f[15]' f[14] */
        fmul   dword [eax+1*4]         ;/* v1     v0     c[8,3] c[8,2] c[8,1] c[8,0] f[15]' f[14] */
        faddp  st1,st0                 ;/* v1+v0  c[8,3] c[8,2] c[8,1] c[8,0] f[15]' f[14] */
        fld    st2                     ;/* c[8,2] v1+v0  c[8,3] c[8,2] c[8,1] c[8,0] f[15]' f[14] */
        fmul   dword [eax+2*4]         ;/* v2     v1+v0  c[8,3] c[8,2] c[8,1] c[8,0] f[15]' f[14] */
        faddp  st1,st0                 ;/* v0v1v2 c[8,3] c[8,2] c[8,1] c[8,0] f[15]' f[14] */
        fld    st1                     ;/* c[8,3] v0v1v2 c[8,3] c[8,2] c[8,1] c[8,0] f[15]' f[14] */
        fmul   dword [eax+3*4]         ;/* v3     v0v1v2 c[8,3] c[8,2] c[8,1] c[8,0] f[15]' f[14] */
        faddp  st1,st0                 ;/* f[8]'  c[8,3] c[8,2] c[8,1] c[8,0] f[15]' f[14] */
        fadd   dword [eax+4*4]         ;/* f[8]'  c[8,3] c[8,2] c[8,1] c[8,0] f[15]' f[14] */        ;// CHECK HERE
        fxch   st4                     ;/* c[8,0] c[8,3] c[8,2] c[8,1] f[16]' f[15]' f[14] */
;//   f[17] = coef[8][0]*b[0]+coef[8][1]*b[1]+coef[8][2]*b[2]+coef[8][3]*b[3]+b[4]-f[15];
        fmul   dword [ebx+0*4]         ;/* v0     c[8,3] c[8,2] c[8,1] f[16]'  f[15]' f[14] */
        fxch   st3                     ;/* c[8,1] c[8,3] c[8,2] v0     f[16]'  f[15]' f[14] */
        fmul   dword [ebx+1*4]         ;/* v1     c[8,3] c[8,2] v0     f[16]'  f[15]' f[14] */
        faddp  st3,st0                 ;/* c[8,3] c[8,2] v0+v1  f[16]'  f[15]' f[14] */
        fxch   st1                     ;/* c[8,2] c[8,3] v0+v1  f[16]'  f[15]' f[14] */
        fmul   dword [ebx+2*4]         ;/* v2     c[8,3] v0+v1  f[16]'  f[15]' f[14] */
        faddp  st2,st0                 ;/* c[8,3] v0v1v2 f[16]'  f[15]' f[14] */
        fmul   dword [ebx+3*4]         ;/* v3     v0v1v2 f[16]'  f[15]' f[14] */
        faddp  st1,st0                 ;/* f[17]'' f[16]'  f[15]' f[14] */
        fadd   dword [ebx+4*4]         ;/* f[17]'' f[16]'  f[15]' f[14] */  ;// CHECK HERE
        fsub   st0,st2                 ;/* f[17]'  f[16]'  f[15]' f[14] */
;//   f[15] = f[15] - f[14];
        fxch   st3                     ;/* f[14]  f[16]' f[15]' f[17]' */
        fsubp  st2,st0                 ;/* f[16]' f[15]  f[17]' */
        fxch   st1                     ;/* f[15]  f[16]' f[17]' */
        fst    dword [ebp+15*4]        ;/* f[15]  f[16]' f[17]' */
;//   f[16] = f[16] - f[15];
        fsubp  st1,st0                 ;/* f[16]       f[17]' */
        fst    dword [ebp+16*4]        ;/* f[16]       f[17]' */
;//   f[17] = f[17] - f[16];
        fsubp  st1,st0                 ;/* f[17] */
        fstp   dword [ebp+17*4]        ;/* - */

        popad
        pop    ebp
        ret

;/*
; * imdct6_3 ASM Version
; */
_imdct6_3_asm: ;// PROC f[]
               ;//      ebp
        push   ebp
        lea    ebp,[esp+8]
        pushad

        mov    ebp,[ebp]               ;/* Save arg */
        mov    edi,_mdct6_3v
        mov    esi,_mdct6_3v2
        mov    eax,tab_a
;/* Unroll the loop because it is a known-value count */
%assign p 0
%rep 3
;//      g1 = v[0] * f[3 * 0];
        fld    dword [ebp+(p+(3*0))*4] ;/* f[3*0] */
        fmul   dword [edi+0*4]         ;/* g1 */
;//      g2 = v[5] * f[3 * 5];
        fld    dword [ebp+(p+(3*5))*4] ;/* f[3*5] g1 */
        fmul   dword [edi+5*4]         ;/* g2 g1 */
;//      a[0] = g1 + g2;
        fld    st0                     ;/* g2 g2 g1 */
        fadd   st2                     ;/* a[0] g2 g1 */
        fstp   dword [eax+(p*6+0)*4]   ;/* g2 g1 */
;//      a[3 + 0] = v2[0] * (g1 - g2);
        fsubp  st1,st0                 ;/* g1-g2 */
        fmul   dword [esi+0*4]         ;/* a[3+0] */
        fstp   dword [eax+(p*6+3)*4]   ;/* - */

;//      g1 = v[1] * f[3 * 1];
        fld    dword [ebp+(p+(3*1))*4] ;/* f[3*1] */
        fmul   dword [edi+1*4]         ;/* g1 */
;//      g2 = v[4] * f[3 * 4];
        fld    dword [ebp+(p+(3*4))*4] ;/* f[3*4] g1 */
        fmul   dword [edi+4*4]         ;/* g2 g1 */
;//      a[1] = g1 + g2;
        fld    st0                     ;/* g2 g2 g1 */
        fadd   st2                     ;/* a[1] g2 g1 */
        fstp   dword [eax+(p*6+1)*4]   ;/* g2 g1 */
;//      a[3 + 1] = v2[1] * (g1 - g2);
        fsubp  st1,st0                 ;/* g1-g2 */
        fmul   dword [esi+1*4]         ;/* a[3+1] */
        fstp   dword [eax+(p*6+3+1)*4] ;/* - */

;//      g1 = v[2] * f[3 * 2];
        fld    dword [ebp+(p+(3*2))*4] ;/* f[3*2] */
        fmul   dword [edi+2*4]         ;/* g1 */
;//      g2 = v[3] * f[3 * 3];
        fld    dword [ebp+(p+(3*3))*4] ;/* f[3*3] g1 */
        fmul   dword [edi+3*4]         ;/* g2 g1 */
;//      a[2] = g1 + g2;
        fld    st0                     ;/* g2 g2 g1 */
        fadd   st2                     ;/* a[2] g2 g1 */
        fstp   dword [eax+(p*6+2)*4]   ;/* g2 g1 */
;//      a[3 + 2] = v2[2] * (g1 - g2);
        fsubp  st1,st0                 ;/* g1-g2 */
        fmul   dword [esi+2*4]         ;/* a[3+2] */
        fstp   dword [eax+(p*6+3+2)*4] ;/* - */
%assign p p+1
%endrep

;/* Unroll the loop because it is a known-value count */
%assign p 0
%rep 3
;//      a02 = (a[0] + a[2]);
        fld    dword [eax+(p*6+0)*4]   ;/* a[0] */
        fadd   dword [eax+(p*6+2)*4]   ;/* a02 */
;//      b02 = (a[3 + 0] + a[3 + 2]);
        fld    dword [eax+(p*6+3+0)*4] ;/* a[3+0] a02 */
        fadd   dword [eax+(p*6+3+2)*4] ;/* b02 a02 */
;//      c[0] = a02 + a[1];
        fld    st1                     ;/* a02 b02 a02 */
        fadd   dword [eax+(p*6+1)*4]   ;/* c[0] b02 a02 */
        fst    dword [ebp+(p*6+0)*4]   ;/* c[0] b02 a02 */
;//      c[1] = b02 + a[3 + 1];
        fld    st1                     ;/* b02 c[0] b02 a02 */
        fadd   dword [eax+(p*6+3+1)*4] ;/* c[1]' c[0] b02 a02 */
;//      c[2] = coef87 * (a[0] - a[2]);
        fld    dword [eax+(p*6+0)*4]   ;/* a[0] c[1]' c[0] b02 a02 */
        fsub   dword [eax+(p*6+2)*4]   ;/* a[0]-a[2] c[1]' c[0] b02 a02 */
        fmul   dword [_coef87]         ;/* c[2]' c[1]' c[0] b02 a02 */
;//      c[3] = coef87 * (a[3 + 0] - a[3 + 2]) - c[1];
        fld    dword [eax+(p*6+3)*4]   ;/* a[3] c[2]' c[1]' c[0] b02 a02 */
        fsub   dword [eax+(p*6+3+2)*4] ;/* a[3]-a[5] c[2]' c[1]' c[0] b02 a02 */
        fmul   dword [_coef87]         ;/* c[3]'' c[2]' c[1]' c[0] b02 a02 */
        fsub   st2                     ;/* c[3]' c[2]' c[1]' c[0] b02 a02 */
;//      c[1] = c[1] - c[0];
        fxch   st3                     ;/* c[0] c[2]' c[1]' c[3]' b02 a02 */
        fsubp  st2,st0                 ;/* c[2]' c[1] c[3]' b02 a02 */
        fxch   st1                     ;/* c[1] c[2]' c[3]' b02 a02 */
        fst    dword [ebp+(p*6+1)*4]   ;/* c[1] c[2]' c[3]' b02 a02 */
;//      c[2] = c[2] - c[1];
        fsubp  st1,st0                 ;/* c[2] c[3]' b02 a02 */
        fst    dword [ebp+(p*6+2)*4]   ;/* c[2] c[3]' b02 a02 */
;//      c[4] = a02 - a[1] - a[1];
        fxch   st3                     ;/* a02 c[3]' b02 c[2] */
        fsub   dword [eax+(p*6+1)*4]   ;/* a02-a[1] c[3]' b02 c[2] */
        fsub   dword [eax+(p*6+1)*4]   ;/* c[4]' c[3]' b02 c[2] */
;//      c[5] = b02 - a[3 + 1] - a[3 + 1] - c[3];
        fxch   st2                     ;/* b02 c[3]' c[4]' c[2] */
        fsub   dword [eax+(p*6+3+1)*4] ;/* b02-a[3+1] c[3]' c[4]' c[2] */
        fsub   dword [eax+(p*6+3+1)*4] ;/* c[5]'' c[3]' c[4]' c[2] */
        fsub   st0,st1                 ;/* c[5]' c[3]' c[4]' c[2] */
;//      c[3] = c[3] - c[2];
        fxch   st3                     ;/* c[2] c[3]' c[4]' c[5]' */
        fsubp  st1,st0                 ;/* c[3] c[4]' c[5]' */
        fst    dword [ebp+(p*6+3)*4]   ;/* c[3] c[4]' c[5]' */
;//      c[4] = c[4] - c[3];
        fsubp  st1,st0                 ;/* c[4] c[5]' */
        fst    dword [ebp+(p*6+4)*4]   ;/* c[4] c[5]' */
;//      c[5] = c[5] - c[4];
        fsubp  st1,st0                 ;/* c[5] */
        fstp   dword [ebp+(p*6+5)*4]   ;/* - */
%assign p p+1
%endrep

        popad
        pop    ebp
        ret

;/*
; * imdct18 ASM MACRO Version
; */
%macro imdct18 1
        push   ebp

        mov    ebp,%1                  ;/* Save arg */
;/* Unroll the loop because it is a known-value count */
%assign p 0                            ;/* Use compiler var as counter */
%rep 4
        ;/* First part - compute ap and bp */
        fld    dword [_mdct18w+p*4]    ;/* w[p] */
        fmul   dword [ebp+p*4]         ;/* g1 */
        fld    dword [_mdct18w+(17-p)*4] ;/* w[17-p] g1 */
        fmul   dword [ebp+(17-p)*4]    ;/* g2 g1 */
        fld    st0                     ;/* g2 g2 g1 */
        fadd   st2                     ;/* ap g2 g1 */
        fxch   st1                     ;/* g2 ap g1 */
        fsubp  st2,st0                 ;/* ap g1-g2 */
        fxch   st1                     ;/* g1-g2 ap */
        fmul   dword [_mdct18w2+p*4]   ;/* bp ap */

        ;/* Second part - compute a8p and b8p */
        fld    dword [_mdct18w+(8-p)*4];/* w[8-p] bp ap */
        fmul   dword [ebp+(8-p)*4]     ;/* g1 bp ap */
        fld    dword [_mdct18w+(9+p)*4];/* w[9+p] g1 bp ap */
        fmul   dword [ebp+(9+p)*4]     ;/* g2 g1 bp ap */
        fld    st0                     ;/* g2 g2 g1 bp ap */
        fadd   st2                     ;/* a8p g2 g1 bp ap */
        fxch   st1                     ;/* g2 a8p g1 bp ap */
        fsubp  st2,st0                 ;/* a8p g1-g2 bp ap */
        fxch   st1                     ;/* g1-g2 a8p bp ap */
        fmul   dword [_mdct18w2+(8-p)*4] ;/* b8p a8p bp ap */

        ;/* Final part - finalize computing and store values */
        fld    st3                     ;/* ap b8p a8p bp ap */
        fadd   st2                     ;/* ap+a8p b8p a8p bp ap */
        fstp   dword [tab_a+p*4]       ;/* b8p a8p bp ap */
        fxch   st1                     ;/* a8p b8p bp ap */
        fsubp  st3,st0                 ;/* b8p bp ap-a8p */
        fxch   st2                     ;/* ap-a8p bp b8p */
        fstp   dword [tab_a+(p+5)*4]   ;/* bp b8p */
        fxch   st1                     ;/* b8p bp */
        fld    st0                     ;/* b8p b8p bp */
        fadd   st2                     ;/* b8p+bp b8p bp */
        fstp   dword [tab_b+p*4]       ;/* b8p bp */
        fsubp  st1,st0                 ;/* bp-b8p */
        fstp   dword [tab_b+(p+5)*4]   ;/* - */
%assign p p+1
%endrep

%assign p 4
        ;/* Last - finalize array */
        fld    dword [_mdct18w+p*4]    ;/* w[p] */
        fmul   dword [ebp+p*4]         ;/* g1 */
        fld    dword [_mdct18w+(17-p)*4] ;/* w[17-p] g1 */
        fmul   dword [ebp+(17-p)*4]    ;/* g2 g1 */
        fld    st0                     ;/* g2 g2 g1 */
        fadd   st2                     ;/* ap g2 g1 */
        fstp   dword [tab_a+p*4]       ;/* g2 g1 */
        fsubp  st1,st0                 ;/* g1-g2 */
        fmul   dword [_mdct18w2+p*4]   ;/* bp */
        fstp   dword [tab_b+p*4]       ;/* - */

;/* Now the huge and boring part */
;//   f[0] = 0.5f * (a[0] + a[1] + a[2] + a[3] + a[4]);
;// TO DO : avoid reload of a[4] and b[4]
        fld    dword [tab_a+0*4]       ;/* a[0] */
        fadd   dword [tab_a+1*4]       ;/* a[0]+a[1] */
        fadd   dword [tab_a+2*4]       ;/* a[0]+a[1]+a[2] */
        fadd   dword [tab_a+3*4]       ;/* a[0]+a[1]+a[2]+a[3] */
        fadd   dword [tab_a+4*4]       ;/* a[0]+a[1]+a[2]+a[3]+a[4] */
        fmul   dword [My_FPU_Half]       ;/* 0.5*(a[0]+a[1]+a[2]+a[3]+a[4]) */
        fst    dword [ebp+0*4]         ;/* f[0] */
;//   f[1] = 0.5f * (b[0] + b[1] + b[2] + b[3] + b[4]);
        fld    dword [tab_b+0*4]       ;/* b[0] f[0] */
        fadd   dword [tab_b+1*4]       ;/* b[0]+b[1] f[0] */
        fadd   dword [tab_b+2*4]       ;/* b[0]+b[1]+b[2] f[0] */
        fadd   dword [tab_b+3*4]       ;/* b[0]+b[1]+b[2]+b[3] f[0] */
        fadd   dword [tab_b+4*4]       ;/* b[0]+b[1]+b[2]+b[3]+b[4] f[0] */
        fmul   dword [My_FPU_Half]       ;/* f[1]' f[0] */
;//   f[2] = coef[1][0]*a[5]+coef[1][1]*a[6]+coef[1][2]*a[7]+coef[1][3]*a[8];
        fld    dword [_coef+1*16+0*4]  ;/* coef[1][0]  f[1]' f[0] */
        fld    dword [_coef+1*16+1*4]  ;/* coef[1][1]  coef[1][0] f[1]' f[0] */
        fld    dword [_coef+1*16+2*4]  ;/* coef[1][2]  coef[1][1] coef[1][0] f[1]' f[0] */
        fld    dword [_coef+1*16+3*4]  ;/* coef[1][3]  coef[1][2] coef[1][1] coef[1][0] f[1]' f[0] */
        fld    st3                     ;/* coef[1][0]  coef[1][3] coef[1][2] coef[1][1] coef[1][0] f[1]' f[0] */
        fmul   dword [tab_a+5*4]       ;/* v0          coef[1][3] coef[1][2] coef[1][1] coef[1][0] f[1]' f[0] */
        fld    st3                     ;/* coef[1][1]  v0         coef[1][3] coef[1][2] coef[1][1] coef[1][0] f[1]' f[0] */
        fmul   dword [tab_a+6*4]       ;/* v1          v0         coef[1][3] coef[1][2] coef[1][1] coef[1][0] f[1]' f[0] */
        faddp  st1,st0                 ;/* v1+v0       coef[1][3] coef[1][2] coef[1][1] coef[1][0] f[1]' f[0] */
        fld    st2                     ;/* coef[1][2]  v1+v0      coef[1][3] coef[1][2] coef[1][1] coef[1][0] f[1]' f[0] */
        fmul   dword [tab_a+7*4]       ;/* v2 v1+v0    coef[1][3] coef[1][2] coef[1][1] coef[1][0] f[1]' f[0] */
        faddp  st1,st0                 ;/* v2+v1+v0    coef[1][3] coef[1][2] coef[1][1] coef[1][0] f[1]' f[0] */
        fld    st1                     ;/* coef[1][3]  v2+v1+v0   coef[1][3] coef[1][2] coef[1][1] coef[1][0] f[1]' f[0] */
        fmul   dword [tab_a+8*4]       ;/* v3 v2+v1+v0 coef[1][3] coef[1][2] coef[1][1] coef[1][0] f[1]' f[0] */
        faddp  st1,st0                 ;/* f[2]'       coef[1][3] coef[1][2] coef[1][1] coef[1][0] f[1]' f[0] */
;//   f[3] = coef[1][0]*b[5]+coef[1][1]*b[6]+coef[1][2]*b[7]+coef[1][3]*b[8]-f[1];
        fxch   st4                     ;/* coef[1][0]  coef[1][3] coef[1][2] coef[1][1] f[2]'      f[1]' f[0] */
        fmul   dword [tab_b+5*4]       ;/* v0          coef[1][3] coef[1][2] coef[1][1] f[2]'      f[1]' f[0] */
        fxch   st3                     ;/* coef[1][1]  coef[1][3] coef[1][2] v0         f[2]'      f[1]' f[0] */
        fmul   dword [tab_b+6*4]       ;/* v1          coef[1][3] coef[1][2] v0         f[2]'      f[1]' f[0] */
        faddp  st3,st0                 ;/* coef[1][3]  coef[1][2] v0+v1      f[2]'      f[1]' f[0] */
        fxch   st1                     ;/* coef[1][2]  coef[1][3] v0+v1      f[2]'      f[1]' f[0] */
        fmul   dword [tab_b+7*4]       ;/* v2          coef[1][3] v0+v1      f[2]'      f[1]' f[0] */
        faddp  st2,st0                 ;/* coef[1][3]  v0+v1+v2   f[2]'      f[1]' f[0] */
        fmul   dword [tab_b+8*4]       ;/* v3          v0+v1+v2   f[2]'      f[1]' f[0] */
        faddp  st1,st0                 ;/* v0+v1+v2+v3 f[2]'      f[1]' f[0] */
        fsub   st0,st2                 ;/* f[3]'       f[2]'      f[1]' f[0] */
;//   f[1] = f[1] - f[0];
        fxch   st3                     ;/* f[0]        f[2]'      f[1]' f[3]' */
        fsubp  st2,st0                 ;/* f[2]'       f[1]       f[3]' */
        fxch   st1                     ;/* f[1]        f[2]'      f[3]' */
        fst    dword [ebp+1*4]         ;/* f[1]        f[2]'      f[3]' */
;//   f[2] = f[2] - f[1];
        fsubp  st1,st0                 ;/* f[2]        f[3]' */
        fst    dword [ebp+2*4]         ;/* f[2]        f[3]' */
        fxch   st1                     ;/* f[3]'       f[2] */
;//   f[4]=coef[2][0]*a[0]+coef[2][1]*a[1]+coef[2][2]*a[2]+coef[2][3]*a[3]-a[4];
        fld    dword [_coef+2*16+0*4]  ;/* c[2,0] f[3]' f[2] */
        fld    dword [_coef+2*16+1*4]  ;/* c[2,1] c[2,0] f[3]' f[2] */
        fld    dword [_coef+2*16+2*4]  ;/* c[2,2] c[2,1] c[2,0] f[3]' f[2] */
        fld    dword [_coef+2*16+3*4]  ;/* c[2,3] c[2,2] c[2,1] c[2,0] f[3]' f[2] */
        fld    st3                     ;/* c[2,0] c[2,3] c[2,2] c[2,1] c[2,0] f[3]' f[2] */
        fmul   dword [tab_a+0*4]       ;/* v0     c[2,3] c[2,2] c[2,1] c[2,0] f[3]' f[2] */
        fld    st3                     ;/* c[2,1] v0     c[2,3] c[2,2] c[2,1] c[2,0] f[3]' f[2] */
        fmul   dword [tab_a+1*4]       ;/* v1     v0     c[2,3] c[2,2] c[2,1] c[2,0] f[3]' f[2] */
        faddp  st1,st0                 ;/* v1+v0  c[2,3] c[2,2] c[2,1] c[2,0] f[3]' f[2] */
        fld    st2                     ;/* c[2,2] v1+v0  c[2,3] c[2,2] c[2,1] c[2,0] f[3]' f[2] */
        fmul   dword [tab_a+2*4]       ;/* v2     v1+v0  c[2,3] c[2,2] c[2,1] c[2,0] f[3]' f[2] */
        faddp  st1,st0                 ;/* v0v1v2 c[2,3] c[2,2] c[2,1] c[2,0] f[3]' f[2] */
        fld    st1                     ;/* c[2,3] v0v1v2 c[2,3] c[2,2] c[2,1] c[2,0] f[3]' f[2] */
        fmul   dword [tab_a+3*4]       ;/* v3     v0v1v2 c[2,3] c[2,2] c[2,1] c[2,0] f[3]' f[2] */
        faddp  st1,st0                 ;/* f[4]'  c[2,3] c[2,2] c[2,1] c[2,0] f[3]' f[2] */
        fsub   dword [tab_a+4*4]       ;/* f[4]'  c[2,3] c[2,2] c[2,1] c[2,0] f[3]' f[2] */
        fxch   st4                     ;/* c[2,0] c[2,3] c[2,2] c[2,1] f[4]'  f[3]' f[2] */
;//   f[5]=coef[2][0]*b[0]+coef[2][1]*b[1]+coef[2][2]*b[2]+coef[2][3]*b[3]-b[4]-f[3];
        fmul   dword [tab_b+0*4]       ;/* v0     c[2,3] c[2,2] c[2,1] f[4]'  f[3]' f[2] */
        fxch   st3                     ;/* c[2,1] c[2,3] c[2,2] v0     f[4]'  f[3]' f[2] */
        fmul   dword [tab_b+1*4]       ;/* v1     c[2,3] c[2,2] v0     f[4]'  f[3]' f[2] */
        faddp  st3,st0                 ;/* c[2,3] c[2,2] v0+v1  f[4]'  f[3]' f[2] */
        fxch   st1                     ;/* c[2,2] c[2,3] v0+v1  f[4]'  f[3]' f[2] */
        fmul   dword [tab_b+2*4]       ;/* v2     c[2,3] v0+v1  f[4]'  f[3]' f[2] */
        faddp  st2,st0                 ;/* c[2,3] v0v1v2 f[4]'  f[3]' f[2] */
        fmul   dword [tab_b+3*4]       ;/* v3     v0v1v2 f[4]'  f[3]' f[2] */
        faddp  st1,st0                 ;/* f[5]'' f[4]'  f[3]' f[2] */
        fsub   dword [tab_b+4*4]       ;/* f[5]'' f[4]'  f[3]' f[2] */
        fsub   st2                     ;/* f[5]'  f[4]'  f[3]' f[2] */
;//   f[3] = f[3] - f[2];
        fxch   st3                     ;/* f[2]   f[4]'  f[3]' f[5]' */
        fsubp  st2,st0                 ;/* f[4]'  f[3]   f[5]' */
        fxch   st1                     ;/* f[3]   f[4]'  f[5]' */
        fst    dword [ebp+3*4]         ;/* f[3]   f[4]'  f[5]' */
;//   f[4] = f[4] - f[3];
        fsubp  st1,st0                 ;/* f[4]   f[5]' */
        fst    dword [ebp+4*4]         ;/* f[4]   f[5]' */
        fxch   st1                     ;/* f[5]'  f[4] */
;//   f[6] = coef[3][0] * (a[5] - a[7] - a[8]);
        fld    dword [tab_a+5*4]       ;/* a[5] f[5]' f[4] */
        fsub   dword [tab_a+7*4]       ;/* a[5]-a[7] f[5]' f[4] */
        fsub   dword [tab_a+8*4]       ;/* a[5]-a[7]-a[8] f[5]' f[4] */
        fmul   dword [_coef+3*16+0*4]  ;/* f[6]' f[5]' f[4] */
;//   f[7] = coef[3][0] * (b[5] - b[7] - b[8]) - f[5];
        fld    dword [tab_b+5*4]       ;/* b[5] f[6]' f[5]' f[4] */
        fsub   dword [tab_b+7*4]       ;/* b[5]-b[7] f[6]' f[5]' f[4] */
        fsub   dword [tab_b+8*4]       ;/* b[5]-b[7]-b[8] f[6]' f[5]' f[4] */
        fmul   dword [_coef+3*16+0*4]  ;/* c[3,0]*(b5-b7-b8) f[6]' f[5]' f[4] */
        fsub   st2                     ;/* f[7]' f[6]' f[5]' f[4] */
;//   f[5] = f[5] - f[4];
        fxch   st3                     ;/* f[4] f[6]' f[5]' f[7]' */
        fsubp  st2,st0                 ;/* f[6]' f[5] f[7]' */
        fxch   st1                     ;/* f[5] f[6]' f[7]' */
        fst    dword [ebp+5*4]         ;/* f[5] f[6]' f[7]' */
;//   f[6] = f[6] - f[5];
        fsubp  st1,st0                 ;/* f[6] f[7]' */
        fst    dword [ebp+6*4]         ;/* f[6] f[7]' */
        fxch   st1                     ;/* f[7]' f[6] */
;//   f[8] = coef[4][0]*a[0]+coef[4][1]*a[1]+coef[4][2]*a[2]+coef[4][3]*a[3]+a[4];
        fld    dword [_coef+4*16+0*4]  ;/* c[4,0] f[7]' f[6] */
        fld    dword [_coef+4*16+1*4]  ;/* c[4,1] c[4,0] f[7]' f[6] */
        fld    dword [_coef+4*16+2*4]  ;/* c[4,2] c[4,1] c[4,0] f[7]' f[6] */
        fld    dword [_coef+4*16+3*4]  ;/* c[4,3] c[4,2] c[4,1] c[4,0] f[7]' f[6] */
        fld    st3                     ;/* c[4,0] c[4,3] c[4,2] c[4,1] c[4,0] f[7]' f[6] */
        fmul   dword [tab_a+0*4]       ;/* v0     c[4,3] c[4,2] c[4,1] c[4,0] f[7]' f[6] */
        fld    st3                     ;/* c[4,1] v0     c[4,3] c[4,2] c[4,1] c[4,0] f[7]' f[6] */
        fmul   dword [tab_a+1*4]       ;/* v1     v0     c[4,3] c[4,2] c[4,1] c[4,0] f[7]' f[6] */
        faddp  st1,st0                 ;/* v1+v0  c[4,3] c[4,2] c[4,1] c[4,0] f[7]' f[6] */
        fld    st2                     ;/* c[4,2] v1+v0  c[4,3] c[4,2] c[4,1] c[4,0] f[7]' f[6] */
        fmul   dword [tab_a+2*4]       ;/* v2     v1+v0  c[4,3] c[4,2] c[4,1] c[4,0] f[7]' f[6] */
        faddp  st1,st0                 ;/* v0v1v2 c[4,3] c[4,2] c[4,1] c[4,0] f[7]' f[6] */
        fld    st1                     ;/* c[4,3] v0v1v2 c[4,3] c[4,2] c[4,1] c[4,0] f[7]' f[6] */
        fmul   dword [tab_a+3*4]       ;/* v3     v0v1v2 c[4,3] c[4,2] c[4,1] c[4,0] f[7]' f[6] */
        faddp  st1,st0                 ;/* f[8]'  c[4,3] c[4,2] c[4,1] c[4,0] f[7]' f[6] */
        fadd   dword [tab_a+4*4]       ;/* f[8]'  c[4,3] c[4,2] c[4,1] c[4,0] f[7]' f[6] */        ;// CHECK HERE
        fxch   st4                     ;/* c[4,0] c[4,3] c[4,2] c[4,1] f[8]'  f[7]' f[6] */
;//   f[9] = coef[4][0]*b[0]+coef[4][1]*b[1]+coef[4][2]*b[2]+coef[4][3]*b[3]+b[4]-f[7];
        fmul   dword [tab_b+0*4]       ;/* v0     c[4,3] c[4,2] c[4,1] f[8]'  f[7]' f[6] */
        fxch   st3                     ;/* c[4,1] c[4,3] c[4,2] v0     f[8]'  f[7]' f[6] */
        fmul   dword [tab_b+1*4]       ;/* v1     c[4,3] c[4,2] v0     f[8]'  f[7]' f[6] */
        faddp  st3,st0                 ;/* c[4,3] c[4,2] v0+v1  f[8]'  f[7]' f[6] */
        fxch   st1                     ;/* c[4,2] c[4,3] v0+v1  f[8]'  f[7]' f[6] */
        fmul   dword [tab_b+2*4]       ;/* v2     c[4,3] v0+v1  f[8]'  f[7]' f[6] */
        faddp  st2,st0                 ;/* c[4,3] v0v1v2 f[8]'  f[7]' f[6] */
        fmul   dword [tab_b+3*4]       ;/* v3     v0v1v2 f[8]'  f[7]' f[6] */
        faddp  st1,st0                 ;/* f[9]'' f[8]'  f[7]' f[6] */
        fadd   dword [tab_b+4*4]       ;/* f[9]'' f[8]'  f[7]' f[6] */  ;// CHECK HERE
        fsub   st2                     ;/* f[9]'  f[8]'  f[7]' f[6] */
;//   f[7] = f[7] - f[6];
        fxch   st3                     ;/* f[6]   f[8]'  f[7]' f[9]' */
        fsubp  st2,st0                 ;/* f[8]'  f[7]   f[9]' */
        fxch   st1                     ;/* f[7]   f[8]'  f[9]' */
        fst    dword [ebp+7*4]         ;/* f[7]   f[8]'  f[9]' */
;//   f[8] = f[8] - f[7];
        fsubp  st1,st0                 ;/* f[8]   f[9]' */
        fst    dword [ebp+8*4]         ;/* f[8]   f[9]' */
        fxch   st1                     ;/* f[9]'  f[8] */
;//   f[10] = coef[5][0]*a[5]+coef[5][1]*a[6]+coef[5][2]*a[7]+coef[5][3]*a[8];
        fld    dword [_coef+5*16+0*4]  ;/* coef[5][0]  f[9]' f[8] */
        fld    dword [_coef+5*16+1*4]  ;/* coef[5][1]  coef[5][0] f[9]' f[8] */
        fld    dword [_coef+5*16+2*4]  ;/* coef[5][2]  coef[5][1] coef[5][0] f[9]' f[8] */
        fld    dword [_coef+5*16+3*4]  ;/* coef[5][3]  coef[5][2] coef[5][1] coef[5][0] f[9]' f[8] */
        fld    st3                     ;/* coef[5][0]  coef[5][3] coef[5][2] coef[5][1] coef[5][0] f[9]' f[8] */
        fmul   dword [tab_a+5*4]       ;/* v0          coef[5][3] coef[5][2] coef[5][1] coef[5][0] f[9]' f[8] */
        fld    st3                     ;/* coef[5][1]  v0         coef[5][3] coef[5][2] coef[5][1] coef[5][0] f[9]' f[8] */
        fmul   dword [tab_a+6*4]       ;/* v1          v0         coef[5][3] coef[5][2] coef[5][1] coef[5][0] f[9]' f[8] */
        faddp  st1,st0                 ;/* v1+v0       coef[5][3] coef[5][2] coef[5][1] coef[5][0] f[9]' f[8] */
        fld    st2                     ;/* coef[5][2]  v1+v0      coef[5][3] coef[5][2] coef[5][1] coef[5][0] f[9]' f[8] */
        fmul   dword [tab_a+7*4]       ;/* v2 v1+v0    coef[5][3] coef[5][2] coef[5][1] coef[5][0] f[9]' f[8] */
        faddp  st1,st0                 ;/* v2+v1+v0    coef[5][3] coef[5][2] coef[5][1] coef[5][0] f[9]' f[8] */
        fld    st1                     ;/* coef[5][3]  v2+v1+v0   coef[5][3] coef[5][2] coef[5][1] coef[5][0] f[9]' f[8] */
        fmul   dword [tab_a+8*4]       ;/* v3 v2+v1+v0 coef[5][3] coef[5][2] coef[5][1] coef[5][0] f[9]' f[8] */
        faddp  st1,st0                 ;/* f[10]'      coef[5][3] coef[5][2] coef[5][1] coef[5][0] f[9]' f[8] */
;//   f[11] = coef[5][0]*b[5]+coef[5][1]*b[6]+coef[5][2]*b[7]+coef[5][3]*b[8]-f[9];
        fxch   st4                     ;/* coef[5][0]  coef[5][3] coef[5][2] coef[5][1] f[10]'     f[9]' f[8] */
        fmul   dword [tab_b+5*4]       ;/* v0          coef[5][3] coef[5][2] coef[5][1] f[10]'     f[9]' f[8] */
        fxch   st3                     ;/* coef[5][1]  coef[5][3] coef[5][2] v0         f[10]'     f[9]' f[8] */
        fmul   dword [tab_b+6*4]       ;/* v1          coef[5][3] coef[5][2] v0         f[10]'     f[9]' f[8] */
        faddp  st3,st0                 ;/* coef[5][3]  coef[5][2] v0+v1      f[10]'     f[9]' f[8] */
        fxch   st1                     ;/* coef[5][2]  coef[5][3] v0+v1      f[10]'     f[9]' f[8] */
        fmul   dword [tab_b+7*4]       ;/* v2          coef[5][3] v0+v1      f[10]'     f[9]' f[8] */
        faddp  st2,st0                 ;/* coef[5][3]  v0+v1+v2   f[10]'     f[9]' f[8] */
        fmul   dword [tab_b+8*4]       ;/* v3          v0+v1+v2   f[10]'     f[9]' f[8] */
        faddp  st1,st0                 ;/* v0+v1+v2+v3 f[10]'     f[9]' f[8] */
        fsub   st0,st2                 ;/* f[11]'      f[10]'     f[9]' f[8] */
;//   f[9] = f[9] - f[8];
        fxch   st3                     ;/* f[8]        f[10]'     f[9]' f[11]' */
        fsubp  st2,st0                 ;/* f[10]'      f[9]       f[11]' */
        fxch   st1                     ;/* f[9]        f[10]'     f[11]' */
        fst    dword [ebp+9*4]         ;/* f[9]        f[10]'     f[11]' */
;//   f[10] = f[10] - f[9];
        fsubp  st1,st0                 ;/* f[10]       f[11]' */
        fst    dword [ebp+10*4]        ;/* f[10]       f[11]' */
        fxch   st1                     ;/* f[11]'      f[10]  */
;//   f[12] = 0.5f * (a[0] + a[2] + a[3]) - a[1] - a[4];
        fld    dword [tab_a+0*4]       ;/* a[0] f[11]' f[10]  */
        fadd   dword [tab_a+2*4]       ;/* a[0]+a[2] f[11]' f[10]  */
        fadd   dword [tab_a+3*4]       ;/* a[0]+a[2]+a[3] f[11]' f[10]  */
        fmul   dword [My_FPU_Half]       ;/* 0.5*(a[0]+a[2]+a[3]) f[11]' f[10]  */
        fsub   dword [tab_a+1*4]       ;/* 0.5*(a[0]+a[2]+a[3])-a[1] f[11]' f[10]  */
        fsub   dword [tab_a+4*4]       ;/* f[12]' f[11]' f[10]  */
;//   f[13] = 0.5f * (b[0] + b[2] + b[3]) - b[1] - b[4] - f[11];
        fld    dword [tab_b+0*4]       ;/* b[0] f[12]' f[11]' f[10]  */
        fadd   dword [tab_b+2*4]       ;/* b[0]+b[2] f[12]' f[11]' f[10]  */
        fadd   dword [tab_b+3*4]       ;/* b[0]+b[2]+b[3] f[12]' f[11]' f[10]  */
        fmul   dword [My_FPU_Half]       ;/* 0.5*(b[0]+b[2]+b[3]) f[12]' f[11]' f[10]  */
        fsub   dword [tab_b+1*4]       ;/* 0.5*(b[0]+b[2]+b[3])-b[1] f[12]' f[11]' f[10]  */
        fsub   dword [tab_b+4*4]       ;/* 0.5*(b[0]+b[2]+b[3])-b[1]-b[4] f[12]' f[11]' f[10]  */
        fsub   st2                     ;/* f[13]' f[12]' f[11]' f[10]  */
;//   f[11] = f[11] - f[10];
        fxch   st3                     ;/* f[10]  f[12]' f[11]' f[13]' */
        fsubp  st2,st0                 ;/* f[12]' f[11]  f[13]' */
        fxch   st1                     ;/* f[11]  f[12]' f[13]' */
        fst    dword [ebp+11*4]        ;/* f[11]  f[12]' f[13]' */
;//   f[12] = f[12] - f[11];
        fsubp  st1,st0                 ;/* f[12]       f[13]' */
        fst    dword [ebp+12*4]        ;/* f[12]       f[13]' */
        fxch   st1                     ;/* f[13]'      f[12]  */
;//   f[14] = coef[7][0]*a[5]+coef[7][1]*a[6]+coef[7][2]*a[7]+coef[7][3]*a[8];
        fld    dword [_coef+7*16+0*4]  ;/* coef[7][0]  f[13]' f[12] */
        fld    dword [_coef+7*16+1*4]  ;/* coef[7][1]  coef[7][0] f[13]' f[12] */
        fld    dword [_coef+7*16+2*4]  ;/* coef[7][2]  coef[7][1] coef[7][0] f[13]' f[12] */
        fld    dword [_coef+7*16+3*4]  ;/* coef[7][3]  coef[7][2] coef[7][1] coef[7][0] f[13]' f[12] */
        fld    st3                     ;/* coef[7][0]  coef[7][3] coef[7][2] coef[7][1] coef[7][0] f[13]' f[12] */
        fmul   dword [tab_a+5*4]       ;/* v0          coef[7][3] coef[7][2] coef[7][1] coef[7][0] f[13]' f[12] */
        fld    st3                     ;/* coef[7][1]  v0         coef[7][3] coef[7][2] coef[7][1] coef[7][0] f[13]' f[12] */
        fmul   dword [tab_a+6*4]       ;/* v1          v0         coef[7][3] coef[7][2] coef[7][1] coef[7][0] f[13]' f[12] */
        faddp  st1,st0                 ;/* v1+v0       coef[7][3] coef[7][2] coef[7][1] coef[7][0] f[13]' f[12] */
        fld    st2                     ;/* coef[7][2]  v1+v0      coef[7][3] coef[7][2] coef[7][1] coef[7][0] f[13]' f[12] */
        fmul   dword [tab_a+7*4]       ;/* v2 v1+v0    coef[7][3] coef[7][2] coef[7][1] coef[7][0] f[13]' f[12] */
        faddp  st1,st0                 ;/* v2+v1+v0    coef[7][3] coef[7][2] coef[7][1] coef[7][0] f[13]' f[12] */
        fld    st1                     ;/* coef[7][3]  v2+v1+v0   coef[7][3] coef[7][2] coef[7][1] coef[7][0] f[13]' f[12] */
        fmul   dword [tab_a+8*4]       ;/* v3 v2+v1+v0 coef[7][3] coef[7][2] coef[7][1] coef[7][0] f[13]' f[12] */
        faddp  st1,st0                 ;/* f[14]'      coef[7][3] coef[7][2] coef[7][1] coef[7][0] f[13]' f[12] */
;//   f[15] = coef[7][0]*b[5]+coef[7][1]*b[6]+coef[7][2]*b[7]+coef[7][3]*b[8]-f[13];
        fxch   st4                     ;/* coef[7][0]  coef[7][3] coef[7][2] coef[7][1] f[14]'     f[13]' f[12] */
        fmul   dword [tab_b+5*4]       ;/* v0          coef[7][3] coef[7][2] coef[7][1] f[14]'     f[13]' f[12] */
        fxch   st3                     ;/* coef[7][1]  coef[7][3] coef[7][2] v0         f[14]'     f[13]' f[12] */
        fmul   dword [tab_b+6*4]       ;/* v1          coef[7][3] coef[7][2] v0         f[14]'     f[13]' f[12] */
        faddp  st3,st0                 ;/* coef[7][3]  coef[7][2] v0+v1      f[14]'     f[13]' f[12] */
        fxch   st1                     ;/* coef[7][2]  coef[7][3] v0+v1      f[14]'     f[13]' f[12] */
        fmul   dword [tab_b+7*4]       ;/* v2          coef[7][3] v0+v1      f[14]'     f[13]' f[12] */
        faddp  st2,st0                 ;/* coef[7][3]  v0+v1+v2   f[14]'     f[13]' f[12] */
        fmul   dword [tab_b+8*4]       ;/* v3          v0+v1+v2   f[14]'     f[13]' f[12] */
        faddp  st1,st0                 ;/* v0+v1+v2+v3 f[14]'     f[13]' f[12] */
        fsub   st0,st2                 ;/* f[15]'      f[14]'     f[13]' f[12] */
;//   f[13] = f[13] - f[12];
        fxch   st3                     ;/* f[12]  f[14]' f[13]' f[15]' */
        fsubp  st2,st0                 ;/* f[14]' f[13]  f[15]' */
        fxch   st1                     ;/* f[13]  f[14]' f[15]' */
        fst    dword [ebp+13*4]        ;/* f[13]  f[14]' f[15]' */
;//   f[14] = f[14] - f[13];
        fsubp  st1,st0                 ;/* f[14]       f[15]' */
        fst    dword [ebp+14*4]        ;/* f[14]       f[15]' */
        fxch   st1                     ;/* f[15]'      f[14]  */
;//   f[16] = coef[8][0]*a[0]+coef[8][1]*a[1]+coef[8][2]*a[2]+coef[8][3]*a[3]+a[4];
        fld    dword [_coef+8*16+0*4]  ;/* c[8,0] f[15]' f[14] */
        fld    dword [_coef+8*16+1*4]  ;/* c[8,1] c[8,0] f[15]' f[14] */
        fld    dword [_coef+8*16+2*4]  ;/* c[8,2] c[8,1] c[8,0] f[15]' f[14] */
        fld    dword [_coef+8*16+3*4]  ;/* c[8,3] c[8,2] c[8,1] c[8,0] f[15]' f[14] */
        fld    st3                     ;/* c[8,0] c[8,3] c[8,2] c[8,1] c[8,0] f[15]' f[14] */
        fmul   dword [tab_a+0*4]       ;/* v0     c[8,3] c[8,2] c[8,1] c[8,0] f[15]' f[14] */
        fld    st3                     ;/* c[8,1] v0     c[8,3] c[8,2] c[8,1] c[8,0] f[15]' f[14] */
        fmul   dword [tab_a+1*4]       ;/* v1     v0     c[8,3] c[8,2] c[8,1] c[8,0] f[15]' f[14] */
        faddp  st1,st0                 ;/* v1+v0  c[8,3] c[8,2] c[8,1] c[8,0] f[15]' f[14] */
        fld    st2                     ;/* c[8,2] v1+v0  c[8,3] c[8,2] c[8,1] c[8,0] f[15]' f[14] */
        fmul   dword [tab_a+2*4]       ;/* v2     v1+v0  c[8,3] c[8,2] c[8,1] c[8,0] f[15]' f[14] */
        faddp  st1,st0                 ;/* v0v1v2 c[8,3] c[8,2] c[8,1] c[8,0] f[15]' f[14] */
        fld    st1                     ;/* c[8,3] v0v1v2 c[8,3] c[8,2] c[8,1] c[8,0] f[15]' f[14] */
        fmul   dword [tab_a+3*4]       ;/* v3     v0v1v2 c[8,3] c[8,2] c[8,1] c[8,0] f[15]' f[14] */
        faddp  st1,st0                 ;/* f[8]'  c[8,3] c[8,2] c[8,1] c[8,0] f[15]' f[14] */
        fadd   dword [tab_a+4*4]       ;/* f[8]'  c[8,3] c[8,2] c[8,1] c[8,0] f[15]' f[14] */        ;// CHECK HERE
        fxch   st4                     ;/* c[8,0] c[8,3] c[8,2] c[8,1] f[16]' f[15]' f[14] */
;//   f[17] = coef[8][0]*b[0]+coef[8][1]*b[1]+coef[8][2]*b[2]+coef[8][3]*b[3]+b[4]-f[15];
        fmul   dword [tab_b+0*4]       ;/* v0     c[8,3] c[8,2] c[8,1] f[16]'  f[15]' f[14] */
        fxch   st3                     ;/* c[8,1] c[8,3] c[8,2] v0     f[16]'  f[15]' f[14] */
        fmul   dword [tab_b+1*4]       ;/* v1     c[8,3] c[8,2] v0     f[16]'  f[15]' f[14] */
        faddp  st3,st0                 ;/* c[8,3] c[8,2] v0+v1  f[16]'  f[15]' f[14] */
        fxch   st1                     ;/* c[8,2] c[8,3] v0+v1  f[16]'  f[15]' f[14] */
        fmul   dword [tab_b+2*4]       ;/* v2     c[8,3] v0+v1  f[16]'  f[15]' f[14] */
        faddp  st2,st0                 ;/* c[8,3] v0v1v2 f[16]'  f[15]' f[14] */
        fmul   dword [tab_b+3*4]       ;/* v3     v0v1v2 f[16]'  f[15]' f[14] */
        faddp  st1,st0                 ;/* f[17]'' f[16]'  f[15]' f[14] */
        fadd   dword [tab_b+4*4]       ;/* f[17]'' f[16]'  f[15]' f[14] */  ;// CHECK HERE
        fsub   st0,st2                 ;/* f[17]'  f[16]'  f[15]' f[14] */
;//   f[15] = f[15] - f[14];
        fxch   st3                     ;/* f[14]  f[16]' f[15]' f[17]' */
        fsubp  st2,st0                 ;/* f[16]' f[15]  f[17]' */
        fxch   st1                     ;/* f[15]  f[16]' f[17]' */
        fst    dword [ebp+15*4]        ;/* f[15]  f[16]' f[17]' */
;//   f[16] = f[16] - f[15];
        fsubp  st1,st0                 ;/* f[16]       f[17]' */
        fst    dword [ebp+16*4]        ;/* f[16]       f[17]' */
;//   f[17] = f[17] - f[16];
        fsubp  st1,st0                 ;/* f[17] */
        fstp   dword [ebp+17*4]        ;/* - */

        pop    ebp
%endmacro

;/*
; * imdct6_3 ASM MACRO Version
; */
%macro imdct6_3 1 ;// PROC f[]
        push   ebp

        mov    ebp,%1                  ;/* Save arg */
;/* Unroll the loop because it is a known-value count */
%assign p 0
%rep 3
        fld    dword [ebp+(p+(3*0))*4] ;/* f[3*0] */
        fmul   dword [_mdct6_3v+0*4]   ;/* g1 */
        fld    dword [ebp+(p+(3*5))*4] ;/* f[3*5] g1 */
        fmul   dword [_mdct6_3v+5*4]   ;/* g2 g1 */
        fld    st0                     ;/* g2 g2 g1 */
        fadd   st2                     ;/* a[0] g2 g1 */
        fstp   dword [tab_a+(p*6+0)*4] ;/* g2 g1 */
        fsubp  st1,st0                 ;/* g1-g2 */
        fmul   dword [_mdct6_3v2+0*4]  ;/* a[3+0] */
        fstp   dword [tab_a+(p*6+3)*4] ;/* - */

        fld    dword [ebp+(p+(3*1))*4] ;/* f[3*1] */
        fmul   dword [_mdct6_3v+1*4]   ;/* g1 */
        fld    dword [ebp+(p+(3*4))*4] ;/* f[3*4] g1 */
        fmul   dword [_mdct6_3v+4*4]   ;/* g2 g1 */
        fld    st0                     ;/* g2 g2 g1 */
        fadd   st2                     ;/* a[1] g2 g1 */
        fstp   dword [tab_a+(p*6+1)*4] ;/* g2 g1 */
        fsubp  st1,st0                 ;/* g1-g2 */
        fmul   dword [_mdct6_3v2+1*4]  ;/* a[3+1] */
        fstp   dword [tab_a+(p*6+3+1)*4] ;/* - */

        fld    dword [ebp+(p+(3*2))*4] ;/* f[3*2] */
        fmul   dword [_mdct6_3v+2*4]   ;/* g1 */
        fld    dword [ebp+(p+(3*3))*4] ;/* f[3*3] g1 */
        fmul   dword [_mdct6_3v+3*4]   ;/* g2 g1 */
        fld    st0                     ;/* g2 g2 g1 */
        fadd   st2                     ;/* a[2] g2 g1 */
        fstp   dword [tab_a+(p*6+2)*4] ;/* g2 g1 */
        fsubp  st1,st0                 ;/* g1-g2 */
        fmul   dword [_mdct6_3v2+2*4]  ;/* a[3+2] */
        fstp   dword [tab_a+(p*6+3+2)*4] ;/* - */
%assign p p+1
%endrep

;/* Unroll the loop because it is a known-value count */
%assign p 0
%rep 3
        fld    dword [tab_a+(p*6+0)*4] ;/* a[0] */
        fadd   dword [tab_a+(p*6+2)*4] ;/* a02 */
        fld    dword [tab_a+(p*6+3+0)*4] ;/* a[3+0] a02 */
        fadd   dword [tab_a+(p*6+3+2)*4] ;/* b02 a02 */
        fld    st1                     ;/* a02 b02 a02 */
        fadd   dword [tab_a+(p*6+1)*4] ;/* c[0] b02 a02 */
        fst    dword [ebp+(p*6+0)*4]   ;/* c[0] b02 a02 */
        fld    st1                     ;/* b02 c[0] b02 a02 */
        fadd   dword [tab_a+(p*6+3+1)*4] ;/* c[1]' c[0] b02 a02 */
        fld    dword [tab_a+(p*6+0)*4] ;/* a[0] c[1]' c[0] b02 a02 */
        fsub   dword [tab_a+(p*6+2)*4] ;/* a[0]-a[2] c[1]' c[0] b02 a02 */
        fmul   dword [_coef87]         ;/* c[2]' c[1]' c[0] b02 a02 */
        fld    dword [tab_a+(p*6+3)*4] ;/* a[3] c[2]' c[1]' c[0] b02 a02 */
        fsub   dword [tab_a+(p*6+3+2)*4] ;/* a[3]-a[5] c[2]' c[1]' c[0] b02 a02 */
        fmul   dword [_coef87]         ;/* c[3]'' c[2]' c[1]' c[0] b02 a02 */
        fsub   st2                     ;/* c[3]' c[2]' c[1]' c[0] b02 a02 */
        fxch   st3                     ;/* c[0] c[2]' c[1]' c[3]' b02 a02 */
        fsubp  st2,st0                 ;/* c[2]' c[1] c[3]' b02 a02 */
        fxch   st1                     ;/* c[1] c[2]' c[3]' b02 a02 */
        fst    dword [ebp+(p*6+1)*4]   ;/* c[1] c[2]' c[3]' b02 a02 */
        fsubp  st1,st0                 ;/* c[2] c[3]' b02 a02 */
        fst    dword [ebp+(p*6+2)*4]   ;/* c[2] c[3]' b02 a02 */
        fxch   st3                     ;/* a02 c[3]' b02 c[2] */
        fsub   dword [tab_a+(p*6+1)*4] ;/* a02-a[1] c[3]' b02 c[2] */
        fsub   dword [tab_a+(p*6+1)*4] ;/* c[4]' c[3]' b02 c[2] */
        fxch   st2                     ;/* b02 c[3]' c[4]' c[2] */
        fsub   dword [tab_a+(p*6+3+1)*4] ;/* b02-a[3+1] c[3]' c[4]' c[2] */
        fsub   dword [tab_a+(p*6+3+1)*4] ;/* c[5]'' c[3]' c[4]' c[2] */
        fsub   st0,st1                 ;/* c[5]' c[3]' c[4]' c[2] */
        fxch   st3                     ;/* c[2] c[3]' c[4]' c[5]' */
        fsubp  st1,st0                 ;/* c[3] c[4]' c[5]' */
        fst    dword [ebp+(p*6+3)*4]   ;/* c[3] c[4]' c[5]' */
        fsubp  st1,st0                 ;/* c[4] c[5]' */
        fst    dword [ebp+(p*6+4)*4]   ;/* c[4] c[5]' */
        fsubp  st1,st0                 ;/* c[5] */
        fstp   dword [ebp+(p*6+5)*4]   ;/* - */
%assign p p+1
%endrep

        pop    ebp
%endmacro

;/*
; * hybrid ASM Version
; * Uses imdct18 and imdct6_3 macros
; */
_hybrid_asm: ;// xin[] xprev[] y[18][32] btype  nlong  ntot   nprev
             ;// ebp   ebp+4   ebp+8     ebp+12 ebp+16 ebp+20 ebp+24
        push   ebp
        lea    ebp,[esp+8]
        pushad

        ;/* Check btype */
        mov    ebx,[ebp+12]
        cmp    ebx,2
        jne    .SkipBTYPE
        xor    ebx,ebx
      .SkipBTYPE:
        mov    eax,WINSIZE
        mul    ebx
        mov    ecx,eax                 ;/* Store win[btype] address */
        add    ecx,_win

        ;/* Compute number of dct's to do */
        mov    eax,[ebp+16]
        add    eax,17
        mov    ebx,18
        cdq
        div    ebx                     ;/* Number of dct's to do in eax */
        ;/* Initialize x and x0 */
        mov    edi,[ebp]               ;/* edi is x */
        mov    esi,[ebp+4]             ;/* esi is x0 */
        ;/* Initialize win[btype] */
        mov    edx,ecx                 ;/* edx is win[btype] */
        ;/* Initialize loop counter */
        xor    ecx,ecx                 ;/* ecx is the loop counter */
        mov    ebx,[ebp+8]             ;/* ebx is y */
        ;/* First check */
        cmp    ecx,eax
        jge    near .ExitLongBlockLoop

        ;/* Long block loop */
      .LongBlockLoop:
        ;/* call imdct 18 macro */
        imdct18 edi

        ;/* Unrool first J-controlled loop */
%assign j 0
%rep 9
        fld    dword [esi+j*4]         ;/* x0[j] */
        fld    dword [edx+j*4]         ;/* win[btype][j] x0[j] */
        fmul   dword [edi+(9+j)*4]     ;/* win[btype][j]*x[9+j] x0[j] */
        faddp  st1,st0                 ;/* y[j][i] */
        fstp   dword [ebx+ecx*4+j*128] ;/* - */

        fld    dword [esi+(j+9)*4]     ;/* x0[9+j] */
        fld    dword [edx+(j+9)*4]     ;/* win[btype][9+j] x0[9+j] */
        fmul   dword [edi+(17-j)*4]    ;/* win[btype][9+j]*x[17-+j] x0[9+j] */
        faddp  st1,st0
        fstp   dword [ebx+ecx*4+(j+9)*128] ;/* - */
%assign j j+1
%endrep

        ;/* Unrool second J-controlled loop */
%assign j 0
%rep 4
        fld    dword [edi+j*4]         ;/* xa */
        fld    dword [edi+(8-j)*4]     ;/* xb xa */
        fld    dword [edx+(18+j)*4]    ;/* win[btype][18+j] xb xa */
        fmul   st1                     ;/* x[j] xb xa */
        fstp   dword [edi+j*4]         ;/* xb xa */
        fld    dword [edx+((18+8)-j)*4];/* win[btype][(18+8)-j] xb xa */
        fmul   st2                     ;/* x[8-j] xb xa */
        fstp   dword [edi+(8-j)*4]     ;/* xb xa */
        fxch   st1                     ;/* xa xb */
        fmul   dword [edx+((18+9)+j)*4];/* x[9+j] xb */
        fstp   dword [edi+(9+j)*4]     ;/* xb */
        fmul   dword [edx+(18+17-j)*4] ;/* x[17-j] */
        fstp   dword [edi+(17-j)*4]    ;/* - */
%assign j j+1
%endrep
        ;/* Handle last special case */
        fld    dword [edi+j*4]         ;/* xa */
        fld    st0                     ;/* xa xa */
        fmul   dword [edx+(18+j)*4]    ;/* x[j] xa */
        fxch   st1                     ;/* xa x[j] */
        fmul   dword [edx+(18+9+j)*4]  ;/* x[9+j] x[j] */
        fxch   st1                     ;/* x[j] x[9+j] */
        fstp   dword [edi+j*4]         ;/* x[9+j] */
        fstp   dword [edi+(j+9)*4]     ;/* - */

        add    edi,18*4                ;/* Update x */
        add    esi,18*4                ;/* Update x0 */

        inc    ecx                     ;/* Increment counter */
        cmp    ecx,eax
        jl     near .LongBlockLoop
      .ExitLongBlockLoop:

        mov    eax,[ebp+20]
        add    eax,17
        mov    ebx,18
        cdq
        div    ebx                     ;/* Number of 6 pts dct's triples to do in eax */
        mov    edx,_win+(WINSIZE*2)    ;/* address of win[2] is in edx */
        mov    ebx,[ebp+8]             ;/* ebx is y */

        ;/* First check */
        cmp    ecx,eax
        jge    near .ExitShortBlockLoop
        ;/* Short block loop */
      .ShortBlockLoop:
        ;/* call imdct6_3 macro */
        imdct6_3 edi
        ;/* First unrolled loop */
%assign j 0
%rep 3
        fld    dword [esi+j*4]         ;/* x0[j] */
        fstp   dword [ebx+ecx*4+j*128] ;/* - */
        fld    dword [esi+(3+j)*4]     ;/* x0[3+j] */
        fstp   dword [ebx+ecx*4+(3+j)*128] ;/* - */

        fld    dword [edi+(3+j)*4]     ;/* x[3+j] */
        fld    dword [edx+j*4]         ;/* win[2][j] x[3+j] */
        fxch   st1                     ;/* x[3+j] win[2][j] */
        fmul   st0,st1                 ;/* win[2][j]*x[3+j] win[2][j] */
        fadd   dword [esi+(6+j)*4]     ;/* y[6+j][i] win[2][j] */
        fstp   dword [ebx+ecx*4+(6+j)*128] ;/* win[2][j] */

        fld    dword [edi+(5-j)*4]     ;/* x[5-j] win[2][j] */
        fld    dword [edx+(3+j)*4]     ;/* win[2][3+j] x[5-j] win[2][j] */
        fxch   st1                     ;/* x[5-j] win[2][3+j] win[2][j] */
        fmul   st0,st1                 ;/* win[2][3+j]*x[5-j] win[2][3+j] win[2][j] */
        fadd   dword [esi+(9+j)*4]     ;/* y[9+j][i] win[2][3+j] win[2][j] */
        fstp   dword [ebx+ecx*4+(9+j)*128] ;/* win[2][3+j] win[2][j] */
        fxch   st1                     ;/* win[2][j] win[2][3+j] */

        fld    dword [edx+(6+j)*4]     ;/* win[2][6+j] win[2][j] win[2][3+j] */
        fmul   dword [edi+(2-j)*4]     ;/* win[2][6+j]*x[2-j] win[2][j] win[2][3+j] */
        fxch   st1                     ;/* win[2][j] win[2][6+j]*x[2-j] win[2][3+j] */
        fmul   dword [edi+(9+j)*4]     ;/* win[2][j]*x[9+j] win[2][6+j]*x[2-j] win[2][3+j] */
        fadd   dword [esi+(12+j)*4]    ;/* x0[12+j]+win[2][j]*x[9+j] win[2][6+j]*x[2-j] win[2][3+j] */
        faddp  st1,st0                 ;/* y[12+j][i] win[2][3+j] */
        fstp   dword [ebx+ecx*4+(12+j)*128] ;/* win[2][3+j] */

        fld    dword [edx+(9+j)*4]     ;/* win[2][9+j] win[2][3+j] */
        fmul   dword [edi+j*4]         ;/* win[2][9+j]*x[j] win[2][3+j] */
        fxch   st1                     ;/* win[2][3+j] win[2][9+j]*x[j] */
        fmul   dword [edi+(11-j)*4]    ;/* win[2][3+j]*x[11-j] win[2][9+j]*x[j] */
        fadd   dword [esi+(15+j)*4]    ;/* x0[15+j]+win[2][3+j]*x[11-j] win[2][9+j]*x[j] */
        faddp  st1,st0                 ;/* y[15+j][i] */
        fstp   dword [ebx+ecx*4+(15+j)*128] ;/* - */
%assign j j+1
%endrep

        ;/* Second unrolled loop */
%assign j 0
%rep 3
        fld    dword [edx+(6+j)*4]     ;/* win[2][6+j] */
        fmul   dword [edi+(8-j)*4]     ;/* win[2][6+j]*x[8-j] */
        fld    dword [edx+j*4]         ;/* win[2][j] win[2][6+j]*x[8-j] */
        fmul   dword [edi+(15+j)*4]    ;/* win[2][j]*x[15+j] win[2][6+j]*x[8-j] */
        faddp  st1,st0                 ;/* x[j] */
        fstp   dword [edi+j*4]         ;/* - */

        fld    dword [edx+(9+j)*4]     ;/* win[2][9+j] */
        fmul   dword [edi+(6+j)*4]     ;/* win[2][9+j]*x[6+j] */
        fld    dword [edx+(3+j)*4]     ;/* win[2][3+j] win[2][9+j]*x[6+j] */
        fmul   dword [edi+(17-j)*4]    ;/* win[2][3+j]*x[17-j] win[2][9+j]*x[6+j] */
        faddp  st1,st0                 ;/* x[3+j] */
        fstp   dword [edi+(3+j)*4]     ;/* - */
%assign j j+1
%endrep

        ;/* Third unrolled loop */
%assign j 0
%rep 3
        fld    dword [edx+(6+j)*4]     ;/* win[2][6+j] */
        fmul   dword [edi+(14-j)*4]    ;/* win[2][6+j]*x[14-j] */
        fstp   dword [edi+(6+j)*4]     ;/* - */

        fld    dword [edx+(9+j)*4]     ;/* win[2][9+j] */
        fmul   dword [edi+(12+j)*4]    ;/* win[2][9+j]*x[12+j] */
        fstp   dword [edi+(9+j)*4]     ;/* - */
%assign j j+1
%endrep

        ;/* Fourth unrolled loop */
        fldz
%assign j 0
%rep 3
        fst    dword [edi+(12+j)*4];
        ;/* If it is the last one, pop value */
%if j==2
        fstp   dword [edi+(15+j)*4];
%else
        fst    dword [edi+(15+j)*4];
%endif
%assign j j+1
%endrep

        add    edi,18*4                ;/* Update x */
        add    esi,18*4                ;/* Update x0 */

        inc    ecx
        cmp    ecx,eax
        jl     near .ShortBlockLoop
      .ExitShortBlockLoop:

        mov    eax,[ebp+24]
        add    eax,17
        mov    ebx,18
        cdq
        div    ebx                     ;/* number of block is in eax */
        mov    ebx,[ebp+8]             ;/* ebx is y */

        ;/* First check */
        cmp    ecx,eax
        jge    near .ExitLastBlockLoop
        ;/* Short block loop */
      .LastBlockLoop:

%assign j 0
%rep 18
        mov    edx,[esi+j*4]
        mov    [ebx+ecx*4+j*128],edx
%assign j j+1
%endrep
        add    esi,18*4

        inc    ecx
        cmp    ecx,eax
        jl     near .LastBlockLoop
      .ExitLastBlockLoop:

        ;/* Compute return value */
        mov    eax,18
        mul    ecx
        mov    [SaveReturn],eax

        mov    eax,[_band_limit_nsb]
        cmp    ecx,eax
        jge    near .SkipFinalClear
      .FinalClear:
%assign j 0
%rep 18
        mov    dword [ebx+ecx*4+j*128],0
%assign j j+1
%endrep
        inc    ecx
        cmp    ecx,eax
        jl     near .FinalClear

      .SkipFinalClear:

        popad
        pop    ebp
        mov    eax,[SaveReturn]
        ret

end

;/* Small macro */
%macro InnerLoop 1
        xor    ecx,ecx

      .loop_%1:

        fld    dword [edi+ecx*8+4+(1+j)*128]
        fchs
        fstp   dword [edi+ecx*8+4+(1+j)*128]

        inc    ecx
        cmp    ecx,eax
        jl     .loop_%1
%endmacro
;/*
; * FreqInvert ASM Version
; */
_FreqInvert_asm:
        push   ebp
        lea    ebp,[esp+8]
        pushad

        mov    eax,[ebp+4]
        add    eax,17
        mov    ebx,18
        cdq
        div    ebx                     ;/* Number of blocks in eax */
        shr    eax,1

        mov    edi,[ebp]

        or     eax,eax
        je     near .ExitNow

%assign j 0
%rep 9
        InnerLoop j
%assign j j+2
%endrep
      .ExitNow:

        popad
        pop    ebp
        ret
