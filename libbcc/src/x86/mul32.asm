;  Copyright (C) 2021 Benjamin Stürz
;  
;  This program is free software: you can redistribute it and/or modify
;  it under the terms of the GNU General Public License as published by
;  the Free Software Foundation, either version 3 of the License, or
;  (at your option) any later version.
;  
;  This program is distributed in the hope that it will be useful,
;  but WITHOUT ANY WARRANTY; without even the implied warranty of
;  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;  GNU General Public License for more details.
;  
;  You should have received a copy of the GNU General Public License
;  along with this program.  If not, see <https://www.gnu.org/licenses/>.

; unsigned

global __mului32
__mului32:
mov eax, dword [esp + 8]
mul eax, dword [esp + 4]
ret

global __mului16
__mului16:
xor eax, eax
mov ax, word [esp + 6]
mul ax, word [esp + 4]
ret

global __mului8
__mului8:
xor eax, eax
mov al, byte [esp + 5]
mul al, byte [esp + 4]
ret

; signed

global __mulsi32
__mulsi32:
mov eax, dword [esp + 8]
imul eax, dword [esp + 4]
ret

global __mulsi16
__mulsi16:
xor eax, eax
mov ax, word [esp + 6]
imul ax, word [esp + 4]
ret

global __mulsi8
__mulsi8:
xor eax, eax
mov al, byte [esp + 5]
imul al, byte [esp + 4]
ret
