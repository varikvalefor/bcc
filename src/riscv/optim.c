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

#include <string.h>
#include <stdio.h>
#include "riscv/cpu.h"
#include "optim.h"
#include "error.h"

// Turn IR_MUL to IR_IFCALL (if no M extension)
// TODO: implement the functions in libbcc first
static bool mul_to_func(ir_node_t** n) {
   if (riscv_cpu.has_mult)
      return false;
   bool success = false;
   ir_node_t* cur;
   for (cur = *n; cur; cur = cur->next) {
      const char* type;
      char sign;
      switch (cur->type) {
      case IR_IMUL:
         type = "mul";
         sign = 's';
         break;
      case IR_UMUL:
         type = "mul";
         sign = 'u';
         break;
      case IR_IDIV:
         type = "div";
         sign = 's';
         break;
      case IR_UDIV:
         type = "div";
         sign = 'u';
         break;
      case IR_IMOD:
         type = "mod";
         sign = 's';
         break;
      case IR_UMOD:
         type = "mod";
         sign = 'u';
         break;
      default:
         continue;
      }
      char name[] = "__mulxi2";
      memcpy(name + 2, type, 3);
      name[5] = sign;

      ir_node_t fc;
      fc.type = IR_IFCALL;
      fc.ifcall.name = strint(name);
      fc.ifcall.dest = cur->binary.dest;
      fc.ifcall.params = NULL;

      ir_node_t* tmp = new_node(IR_NOP);
      if (cur->binary.a.type == IRT_REG) {
         tmp->type = IR_MOVE;
         tmp->move.dest = fc.ifcall.dest;
         tmp->move.src = cur->binary.a.reg;
         tmp->move.size = cur->binary.size;
      } else if (cur->binary.a.type == IRT_UINT) {
         tmp->type = IR_LOAD;
         tmp->load.dest = fc.ifcall.dest;
         tmp->load.size = cur->binary.size;
         tmp->load.value = cur->binary.a.uVal;
      } else panic("unreachable reached");
      buf_push(fc.ifcall.params, tmp);

      tmp = new_node(IR_NOP);
      if (cur->binary.b.type == IRT_REG) {
         tmp->type = IR_MOVE;
         tmp->move.dest = fc.ifcall.dest;
         tmp->move.src = cur->binary.b.reg;
         tmp->move.size = cur->binary.size;
      } else if (cur->binary.b.type == IRT_UINT) {
         tmp->type = IR_LOAD;
         tmp->load.dest = fc.ifcall.dest;
         tmp->load.size = cur->binary.size;
         tmp->load.value = cur->binary.b.uVal;
      } else panic("unreachable reached");
      buf_push(fc.ifcall.params, tmp);

      *cur = fc;
      success = true;
   }
   return success;
}

static bool copy_to_memcpy(ir_node_t** n) {
   bool success = false;
   for (ir_node_t* cur = *n; cur; cur = cur->next) {
      if (cur->type != IR_COPY)
         continue;
      ir_node_t fcall;
      fcall.type = IR_FCALL;
      fcall.prev = cur->prev;
      fcall.next = cur->next;
      fcall.func = cur->func;

      fcall.ifcall.name = strint("memcpy");
      fcall.ifcall.dest = cur->copy.dest;
      fcall.ifcall.params = NULL;
      
      ir_node_t* param = new_node(IR_MOVE);
      param->move.dest = fcall.ifcall.dest;
      param->move.src  = cur->copy.dest;
      param->move.size = IRS_PTR;
      buf_push(fcall.ifcall.params, param);

      param = new_node(IR_MOVE);
      param->move.dest = fcall.ifcall.dest;
      param->move.src  = cur->copy.src;
      param->move.size = IRS_PTR;
      buf_push(fcall.ifcall.params, param);

      param = new_node(IR_LOAD);
      param->load.dest = fcall.ifcall.dest;
      param->load.value = cur->copy.len;
      param->load.value = IRS_PTR;
      buf_push(fcall.ifcall.params, param);
      *cur = fcall;
      success = true;
   }
   return success;
}

// TODO: implement target-specific IR optimizations
bool target_optim_ir(ir_node_t** n) {
   bool success = false;
   while (copy_to_memcpy(n))
      success = true;
   return success;
}

bool target_post_optim_ir(ir_node_t** n) {
   bool success = false;
   while (mul_to_func(n))
      success = true;
   return success;
}
