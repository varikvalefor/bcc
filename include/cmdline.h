//  Copyright (C) 2021 Benjamin Stürz
//  
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef FILE_CMDLINE_H
#define FILE_CMDLINE_H
#include <stdbool.h>
#include <stdio.h>

struct cmdline_arg {
   char option;
   const char* arg;
};

extern bool verbose;

#define verbose_execl(prog, arg0, ...) \
   vexecl_print(prog, __VA_ARGS__); \
   execlp(prog, arg0, __VA_ARGS__)

void vexecl_print(const char* name, ...);

#endif /* FILE_CMDLINE_H */
