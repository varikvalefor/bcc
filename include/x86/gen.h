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

#ifndef FILE_X86_GEN_H
#define FILE_X86_GEN_H
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "strint.h"
#include "target.h"
#include "config.h"
#include "ctype.h"
#include "strdb.h"
#include "math.h"
#include "regs.h"
#include "bcc.h"
#include "ir.h"

struct stack_alloc_entry {
   bool is_const;
   size_t sz; // if !is_const it is [ebp - sz]
};

static const struct function* cur_func;
static uintreg_t esp = 0;
static istr_t* unresolved = NULL;
static istr_t* defined = NULL;
static struct stack_alloc_entry** stack_alloc;
static struct stack_alloc_entry* stack_cur_alloc;
struct machine_option mach_opts[] = {
   { "stack-check", "Perform stack alignment checking on every function entry", 0, .bVal = false },
};
const size_t num_mach_opts = arraylen(mach_opts);
bool emit_prepare(void) { return true; }

void emit_externs(void);

static const char* nasm_size(enum ir_value_size s) {
   switch (s) {
   case IRS_BYTE:
   case IRS_CHAR:    return "byte";
   case IRS_SHORT:   return "word";

   case IRS_PTR:
#if BITS == 64
   case IRS_LONG:    return "qword";
#endif
   case IRS_INT:     return "dword";
   default:          panic("unsupported operand size '%s'", ir_size_str[s]);
   }
}
static size_t x86_sizeof_value(const struct value_type* vt) {
   if (vt->type == VAL_POINTER && !vt->pointer.array.has_const_size)
      return REGSIZE * 2;
   else return sizeof_value(vt, false);
}
static size_t sizeof_scope(const struct scope* scope) {
   size_t num = 0;
   for (size_t i = 0; i < buf_len(scope->vars); ++i) {
      const size_t sz = x86_sizeof_value(scope->vars[i].type);
      num += sz;
   }
   size_t max_child = 0;
   for (size_t i = 0; i < buf_len(scope->children); ++i) {
      const size_t sz = sizeof_scope(scope->children[i]);
      if (sz > max_child) max_child = sz;
   }
   return num + max_child;
}
static void assign_scope(struct scope* scope, size_t* addr) {
   for (size_t i = 0; i < buf_len(scope->vars); ++i) {
      *addr += x86_sizeof_value(scope->vars[i].type);
      scope->vars[i].addr = *addr;
   }
   for (size_t i = 0; i < buf_len(scope->children); ++i) {
      assign_scope(scope->children[i], addr);
   }
}
static ir_node_t* emit_ir(const ir_node_t* n);
static void emit_begin(void) {
   strdb_init();
   if (unresolved) buf_free(unresolved);
   if (defined) buf_free(defined);
   emit("default rel");
   emit("section .text");
   emit_externs();
   for (size_t i = 0; i < buf_len(cunit.vars); ++i) {
      const struct variable* v = &cunit.vars[i];
      if (!(v->attrs & ATTR_EXTERN))
         buf_push(defined, v->name);
      else buf_push(unresolved, v->name);
   }
}
static void emit_global_init(const struct value_type* vt, const struct value* val);
static void emit_end(void) {
   for (size_t i = 0; i < buf_len(unresolved); ++i) {
      bool is_defined = false;
      for (size_t j = 0; j < buf_len(defined); ++j) {
         if (unresolved[i] == defined[j]) {
            is_defined = true;
            break;
         }
      }
      if (!is_defined) emit("extern %s", unresolved[i]);
   }

   asm_indent = 0;
   for (size_t i = 0; i < num_builtin_funcs; ++i) {
      if (builtin_funcs[i].requested)
         emit("%s:\n%s", builtin_funcs[i].name, builtin_funcs[i].code);
   }

   if (strdb) {
      emit("\nsection .rodata\n__strings:");
      size_t i = 0;
      while (i < buf_len(strdb)) {
         if (!strdb[i]) {
            emit("db 0");
            ++i;
            continue;
         }
         emitraw("db ");
         while (strdb[i]) {
            if (isprint(strdb[i])) {
               emitraw("\"");
               while (isprint(strdb[i])) {
                  emitraw("%c", strdb[i++]);
               }
               emitraw("\"");
            } else {
               emitraw("%u", strdb[i++]);
            }
            emitraw(", ");
         }
         emit("0");
         ++i;
      }
   }

   buf_free(unresolved);
   buf_free(defined);

   if (cunit.vars) {
      emit("section .data");
      for (size_t i = 0; i < buf_len(cunit.vars); ++i) {
         const struct variable* var = &cunit.vars[i];
         const struct value_type* type = var->type;
         if (var->attrs & ATTR_EXTERN)
            continue;
         if (!(var->attrs & ATTR_STATIC))
            emit("global %s", var->name);
         emit("%s:", var->name);
         emit_global_init(var->type, var->has_const_value ? &var->const_init : NULL);
      }
   }
}

static void emit_global_init(const struct value_type* vt, const struct value* val) {
   switch (vt->type) {
   case VAL_INT:
      switch (vt->integer.size) {
      case INT_BYTE:
      case INT_CHAR:
         emitraw("db ");
         break;
      case INT_SHORT:
         emitraw("dw ");
         break;
      case INT_LONG:
#if BITS == 64
         emitraw("dq ");
         break;
#endif
      case INT_INT:
         emitraw("dd ");
         break;
      default:
         panic("invalid IR integer size");
      }
      if (val) {
         if (vt->integer.is_unsigned)
            emit("%ju", val->uVal);
         else emit("%jd", val->iVal);
      } else emit("0");
      break;
#if ENABLE_FP
      case VAL_FLOAT:
         switch (vt->fp.size) {
         case FP_FLOAT:
            emitraw("dd ");
            break;
         case FP_DOUBLE:
            emitraw("dq ");
            break;
         default:
            panic("invalid IR fp size");
         }
         if (val) {
            panic("initialization fo global floating-point variables is not supported");
         } else emit("0");
         break;
#endif
      case VAL_POINTER:
         if (vt->pointer.is_array) {
            if (val) {
               print_value_type(stderr, vt->pointer.type);
               if (!vt_is_array(val->type))
                  panic("val is not an array, val is %s", value_type_str[val->type->type]);
               for (size_t i = 0; i < buf_len(val->array); ++i) {
                  emit_global_init(vt->pointer.type, &val->array[i]);
               }
            } else {
               emit("resb %zu", sizeof_value(vt, false));
            }
            break;
         } else {
#if BITS == 32
            emitraw("dd ");
#else
            emitraw("dq ");
#endif
            if (val) {
               emit("%ju", val->uVal);
            } else emit("0");
         }
         break;
      case VAL_BOOL:
         emitraw("db ");
         if (val) {
            emit("%u", val->uVal ? 1 : 0);
         } else emit("0");
         break;
      default:
         panic("invalid variable type '%s'", value_type_str[vt->type]);
   }
}
static void emit_func(const struct function* func, const ir_node_t* n) {
   cur_func = func;
   esp = 0;
   while ((n = emit_ir(n)) != NULL);
}
void emit_unit(void) {
   emit_begin();
   for (size_t i = 0; i < buf_len(cunit.funcs); ++i) {
      const struct function* f = cunit.funcs[i];
      if (f->ir_code)
         emit_func(f, f->ir_code);
      //else emit("extern %s", f->name);
   }
   emit_end();
}

static size_t uslen(uintmax_t v) {
   return v == 0 ? 1 : (size_t)log10(v) + 1;
}
static const char* irv2str(const struct ir_value* v, const enum ir_value_size s) {
   if (v->type == IRT_REG) {
      const char* str;
      reg_op(str, v->reg, s);
      return str;
   } else if (v->type == IRT_UINT) {
      char buffer[uslen(v->uVal) + 1];
      snprintf(buffer, sizeof(buffer), "%ju", v->uVal);
      return strint(buffer);
   } else panic("invalid IR value type '%u'", v->type);
}
static void emit_clear(const char* reg) {
   if (optim_level < 1) emit("mov %s, 0", reg);
   else emit("xor %s, %s", reg, reg);
}

static size_t align_stack_size(size_t n) {
   return n & 15 ? (n & ~15) + 16 : n;
}

static void free_stack(void) {
   struct stack_alloc_entry* e = stack_cur_alloc;
   stack_cur_alloc = buf_pop(stack_alloc);

   size_t const_sz = 0;
   for (size_t i = 0; i < buf_len(e); ++i) {
      if (e[i].is_const)
         const_sz += e[i].sz;
      else {
         emit("add %s, %s [%s - %zu]", reg_sp, nasm_size(IRS_PTR), reg_bp, e[i].sz);
      }
   }
   if (const_sz) emit("add %s, %zu", reg_sp, const_sz);

   buf_free(e);
}
static bool has_mach_opt(const char* name) {
   for (size_t i = 0; i < arraylen(mach_opts); ++i) {
      if (!strcmp(name, mach_opts[i].name))
         return mach_opts[i].type != 0 || mach_opts[i].bVal;
   }
   return false;
}
static void add_unresolved(istr_t name) {
   for (size_t i = 0; i < buf_len(defined); ++i) {
      if (defined[i] == name) return;
   }
   for (size_t i = 0; i < buf_len(unresolved); ++i) {
      if (unresolved[i] == name) return;
   }
   struct builtin_func* f = get_builtin_func(name);
   if (f) {
      f->requested = true;
      return;
   }
   buf_push(unresolved, name);
}

static bool is_defined(istr_t s) {
   for (size_t i = 0; i < buf_len(cunit.funcs); ++i) {
      const struct function* f = cunit.funcs[i];
      if (s == f->name && f->scope) return true;
   }
   return get_builtin_func(s) != NULL;
}
#define has_struct_return() (is_struct(cur_func->type->type))
#endif /* FILE_X86_GEN_H */
