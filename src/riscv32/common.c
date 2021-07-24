#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include "riscv/cpu.h"
#include "target.h"
#include "error.h"
#include "regs.h"
#include "as.h"

#if BITS == 32
#define ABI "ilp32"
#else
#define ABI "lp64"
#endif


struct machine_option mach_opts[] = {
   { "cpu", "The target CPU", 2, .sVal = DEF_MACH },
};

const size_t num_mach_opts = arraylen(mach_opts);
extern const char* gnu_as;

// TODO: add -mas=... option
int assemble(const char* source, const char* output) {
   const pid_t pid = fork();
   if (pid < 0) panic("failed to fork()");
   if (pid == 0) {
      execlp(gnu_as, gnu_as, "-mabi=" ABI, "-o", output, source, NULL);
      perror("bcc: failed to invoke assembler");
      _exit(1);
   } else {
      int wstatus;
      waitpid(pid, &wstatus, 0);
      if (WIFEXITED(wstatus)) return WEXITSTATUS(wstatus);
      panic("failed to wait for assembler");
   }
}
