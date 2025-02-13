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

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include "riscv/regs.h"
#include "riscv/cpu.h"
#include "cmdline.h"
#include "target.h"
#include "error.h"
#include "config.h"

struct machine_option mach_opts[] = {
   { "cpu", "The target CPU (default: " DEF_CPU ")", 2, .sVal = DEF_CPU },
   { "abi", "The target ABI (default: " DEF_ABI ")", 2, .sVal = DEF_ABI },
   { "stack-check", "Check the stack for alignment", 0, .bVal = false },
};

const size_t num_mach_opts = arraylen(mach_opts);
extern const char* gnu_as;

// TODO: add -mas=... option
int assemble(const char* source, const char* output) {
   const pid_t pid = fork();
   if (pid < 0) panic("failed to fork()");
   if (pid == 0) {
      char* mabi = malloc(100);
      if (!mabi) {
         perror("bcc: failed to allocate memory");
         _exit(1);
      }
      snprintf(mabi, 100, "-mabi=%s", get_mach_opt("abi")->sVal);
      verbose_execl(GNU_AS, GNU_AS, mabi, "-o", output, source, NULL);
      perror("bcc: failed to invoke assembler");
      _exit(1);
   } else {
      int wstatus;
      waitpid(pid, &wstatus, 0);
      if (WIFEXITED(wstatus)) return WEXITSTATUS(wstatus);
      panic("failed to wait for assembler");
   }
}

char* get_ld_abi(void) {
   char* abi = malloc(100);
   if (!abi)
      panic("failed to malloc()");
   snprintf(abi, 100, "-melf%dlriscv_%s", BITS, get_mach_opt("abi")->sVal);
   return abi;
}
