;/*
; * Antialias ASM version for the XingMPG Decoder
; *
; * Copyright (c) 1999, Jean-Michel HERVE
; *
; * Code : TuO / StG
; * Date : 24/02/99
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
; * $Id: mp3_msis.asm,v 1.1 2007/07/30 02:03:53 huchangyin Exp $
; */

  BITS 32

SECTION .data USE32

GLOBAL _antialias_asm

EXTERN _csa

SECTION .text USE32

;/*
; * Antialias ASM Version
; */
_antialias_asm: ;// PROC x, n :  DWORD
                ;//      ebp ebp+4
        push   ebp
        lea    ebp,[esp+8]
        pushad

;//   for (k = 0; k < n; k++)
;//   {
        mov    ecx,[ebp+4]

        cmp    ecx,0
        je     near .Skip

        mov    ebp,[ebp]
      .Loop:
;//      for (i = 0; i < 8; i++)
;//      {
%assign i 0
%rep 8
;//        a = x[17 - i];
        fld    dword [ebp+(17-i)*4]    ;/* a */
;//        b = x[18 + i];
        fld    dword [ebp+(18+i)*4]    ;/* b a */
;//        x[17 - i] = a * csa[i][0] - b * csa[i][1];
        fld    st1                     ;/* a b a */
        fmul   dword [_csa+i*8]        ;/* a*csa[i][0] b a */
        fld    st1                     ;/* b a*csa[i][0] b a */
        fmul   dword [_csa+i*8+4]      ;/* b*csa[i][1] a*csa[i][0] b a */
        fsubp  st1,st0                 ;/* x[17-i] b a */
        fstp   dword [ebp+(17-i)*4]    ;/* b a */
;//        x[18 + i] = b * csa[i][0] + a * csa[i][1];
        fmul   dword [_csa+i*8]        ;/* b*csa[i][0] a */
        fxch   st1                     ;/* a b*csa[i][0] */
        fmul   dword [_csa+i*8+4]      ;/* a*csa[i][1] b*csa[i][0] */
        faddp  st1,st0                 ;/* x[18+i] */
        fstp   dword [ebp+(18+i)*4]    ;/* - */
;//      }
%assign i i+1
%endrep
;//      x += 18;
        add    ebp,18*4
;//   }
        dec    ecx
        jnz    near .Loop

      .Skip:

        popad
        pop    ebp
        ret

end
