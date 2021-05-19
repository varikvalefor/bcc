#include <stdlib.h>
#include "parser.h"
#include "optim.h"
#include "lex.h"
#include "ir.h"

struct cunit cunit = { NULL};

static void add_enum(const struct value_type* type) {
   if (type->venum->name && type->venum->is_definition) {
      if (unit_get_enum(type->venum->name))
         parse_error(&type->begin, "'enum %s' already defined", type->venum->name);
      buf_push(cunit.enums, copy_enum(type->venum));
   }

   // add all entries as constants
   for (size_t i = 0; i < buf_len(type->venum->entries); ++i) {
      const struct enum_entry* e = &type->venum->entries[i];
      if (find_constant(e->name, NULL))
         parse_error(&type->begin, "constant '%s' already defined", e->name);
      else if (unit_get_var(e->name) || unit_get_func(e->name))
         parse_error(&type->begin, "'%s' already declared", e->name);
      buf_push(cunit.constants, *e);
   }

}
static void add_struct(const struct value_type* type) {
   if (type->vstruct->name && type->vstruct->is_definition) {
      if (unit_get_struct(type->vstruct->name))
         parse_error(&type->begin, "'struct %s' already defined", type->vstruct->name);
      buf_push(cunit.structs, copy_struct(type->vstruct));
   }
}
static void add_union(const struct value_type* type) {
   if (type->vstruct->name && type->vstruct->is_definition) {
      if (unit_get_struct(type->vstruct->name))
         parse_error(&type->begin, "'union %s' already defined", type->vstruct->name);
      buf_push(cunit.unions, copy_struct(type->vstruct));
   }
}

void parse_unit(void) {
   buf_free(cunit.funcs);
   while (!lexer_match(TK_EOF)) {
      if (lexer_matches(KW_TYPEDEF)) {
         struct typerename alias;
         alias.begin = lexer_next().begin;
         alias.type = parse_value_type(NULL);
         if (!alias.type)
            parse_error(&alias.end, "failed to parse type");
         switch (alias.type->type) {
         case VAL_ENUM:
            add_enum(alias.type);
            break;
         case VAL_STRUCT:
            add_struct(alias.type);
            break;
         case VAL_UNION:
            add_union(alias.type);
            break;
         default:
            break;
         }
         alias.name = lexer_expect(TK_NAME).str;
         alias.end = lexer_expect(TK_SEMICOLON).end;
         buf_push(cunit.aliases, alias);
         continue;
      }

      bool has_begin = false;
      unsigned attrs = 0;
      struct source_pos begin;

      while (lexer_matches(KW_EXTERN) || lexer_matches(KW_STATIC)) {
         const struct token tk = lexer_next();
         switch (tk.type) {
         case KW_EXTERN:
            attrs |= ATTR_EXTERN;
            break;
         case KW_STATIC:
            attrs |= ATTR_STATIC;
            break;
         default: parse_error(&tk.begin, "invalid storage specifier");
         }
         if (!has_begin) {
            begin = tk.begin;
            has_begin = true;
         }
      }

      if ((attrs & (ATTR_EXTERN | ATTR_STATIC)) == (ATTR_EXTERN | ATTR_STATIC))
         parse_error(&begin, "variable cannot be static and extern at the same time");

      struct value_type* type = parse_value_type(NULL);
      if (!type)
         parse_error(&begin, "failed to parse type");

      if (type->type == VAL_ENUM) {
         add_enum(type);
         if (lexer_match(TK_SEMICOLON)) continue;
      } else if (type->type == VAL_STRUCT) {
         add_struct(type);
         if (lexer_match(TK_SEMICOLON)) continue;
      } else if (type->type == VAL_UNION) {
         add_union(type);
         if (lexer_match(TK_SEMICOLON)) continue;
      }

      const struct token name_tk = lexer_expect(TK_NAME);
      istr_t name = name_tk.str;

      if (!has_begin) begin = type->begin;
      
      if (lexer_matches(TK_LPAREN)) {
         struct function* func = parse_func_part(type, name);
         {
            struct function* f = unit_get_func(name);
            if (f) {
               if (f->scope)
                  parse_error(&begin, "function '%s' already defined", name);
            }
         }
         func->begin = begin;
         func->attrs = attrs;
         buf_push(cunit.funcs, func);
         if (func->scope) {
            if (func->attrs & ATTR_EXTERN)
               parse_warn(&begin, "function definition shall not be extern");
            func->ir_code = optim_ir_nodes(irgen_func(func));
         }
      } else {
         bool first = true;
         do {
            struct value_type* vt = copy_value_type(type);
            struct variable var;
            struct source_pos name_end;
            var.type = vt;
            if (first) {
               var.name = name;
               name_end = name_tk.end;
               first = false;
            } else {
               const struct token tk = lexer_expect(TK_NAME);
               var.begin = tk.begin;
               var.name = tk.str;
               name_end = tk.end;
            }
            var.begin = begin;
            var.attrs = attrs;
            {
               struct variable* v = unit_get_var(var.name);
               if (v) {
                  if (!(v->attrs & ATTR_EXTERN))
                     parse_error(&begin, "global variable '%s' already declared", name);
                  if (!value_type_equal(v->type, var.type))
                     parse_error(&begin, "incompatible types");
               }
            }
            if (unit_get_func(name))
               parse_error(&begin, "function with name '%s' already exists", name);
            if (lexer_match(TK_LBRACK)) {
               struct expression* expr = parse_expr(NULL);
               struct value val;
               if (!try_eval_expr(expr, &val))
                  parse_error(&expr->begin, "gloal VLAs are not allowed.");
               else if (val.type->type != VAL_INT)
                  parse_error(&expr->begin, "size of array must be an integer.");
               else if (!val.type->integer.is_unsigned && val.iVal < 0)
                  parse_error(&expr->begin, "size of array must be positive.");
               var.type = make_array_vt(val.type);
               var.type->pointer.array.has_const_size = true;
               var.type->pointer.array.size = val.uVal;
               lexer_expect(TK_RBRACK);
            }
            if (lexer_match(TK_EQ)) {
               var.init = parse_expr_no_comma(NULL);
               if (var.attrs & ATTR_EXTERN)
                  parse_error(&var.init->begin, "cannot initialize extern variable");
               if (!try_eval_expr(var.init, &var.const_init))
                  parse_error(&var.init->begin, "global variables may only be initialized with a constant value");
               var.has_const_value = true;
            } else var.init = NULL;
            var.end = var.init ? var.init->end : name_end;
            buf_push(cunit.vars, var);
         } while (lexer_match(TK_COMMA));
         lexer_expect(TK_SEMICOLON);
      }
   }
}

void print_unit(FILE* file) {
   for (size_t i = 0; i < buf_len(cunit.funcs); ++i) {
      print_func(file, cunit.funcs[i]);
      fputc('\n', file);
   }
}
void print_ir_unit(FILE* file) {
   for (size_t i = 0; i < buf_len(cunit.funcs); ++i) {
      struct function* f = cunit.funcs[i];
      fprintf(file, "\n// function %s\n", f->name);
      print_ir_nodes(file, f->ir_code);
      fputc('\n', file);
   }
}

size_t unit_get_func_idx(istr_t name) {
   for (size_t i = 0; i < buf_len(cunit.funcs); ++i) {
      if (name == cunit.funcs[i]->name) return i;
   }
   return SIZE_MAX;
}
struct function* unit_get_func(istr_t name) {
   const size_t idx = unit_get_func_idx(name);
   return idx == SIZE_MAX ? NULL : cunit.funcs[idx];
}
void free_unit(void) {
   for (size_t i = 0; i < buf_len(cunit.funcs); ++i) {
      free_func(cunit.funcs[i]);
   }
   buf_free(cunit.funcs);
}
size_t unit_get_var_idx(istr_t name) {
   for (size_t i = 0; i < buf_len(cunit.vars); ++i) {
      if (name == cunit.vars[i].name)
         return i;
   }
   return SIZE_MAX;
}
struct variable* unit_get_var(istr_t name) {
   const size_t idx = unit_get_var_idx(name);
   return idx == SIZE_MAX ? NULL : &cunit.vars[idx];
}

size_t unit_get_typedef_idx(istr_t name) {
   for (size_t i = 0; i < buf_len(cunit.aliases); ++i) {
      if (name == cunit.aliases[i].name)
         return i;
   }
   return SIZE_MAX;
}
struct typerename* unit_get_typedef(istr_t name) {
   const size_t idx = unit_get_typedef_idx(name);
   return idx == SIZE_MAX ? NULL : &cunit.aliases[idx];
}
size_t unit_get_const_idx(istr_t name) {
   for (size_t i = 0; i < buf_len(cunit.constants); ++i) {
      if (name == cunit.constants[i].name)
         return i;
   }
   return SIZE_MAX;

}
bool find_constant(istr_t name, intmax_t* value) {
   const size_t idx = unit_get_const_idx(name);
   if (idx == SIZE_MAX) return false;
   if (value) *value = cunit.constants[idx].value;
   return true;
}
struct enumeration* unit_get_enum(istr_t name) {
   for (size_t i = 0; i < buf_len(cunit.enums); ++i) {
      if (name == cunit.enums[i]->name)
         return cunit.enums[i];
   }
   return NULL;
}
struct structure* unit_get_struct(istr_t name) {
   for (size_t i = 0; i < buf_len(cunit.structs); ++i) {
      if (name == cunit.structs[i]->name)
         return cunit.structs[i];
   }
   return NULL;
}
struct structure* unit_get_union(istr_t name) {
   for (size_t i = 0; i < buf_len(cunit.unions); ++i) {
      if (name == cunit.unions[i]->name)
         return cunit.unions[i];
   }
   return NULL;
}

bool unit_find(istr_t name, struct symbol* sym) {
   size_t idx;
   if ((idx = unit_get_var_idx(name)) != SIZE_MAX) {
      if (sym) {
         sym->type = SYM_VAR;
         sym->idx = idx;
      }
      return true;
   }
   if ((idx = unit_get_func_idx(name)) != SIZE_MAX) {
      if (sym) {
         sym->type = SYM_FUNC;
         sym->idx = idx;
      }
      return true;
   }
   if ((idx = unit_get_typedef_idx(name)) != SIZE_MAX) {
      if (sym) {
         sym->type = SYM_ALIAS;
         sym->idx = idx;
      }
      return true;
   }
   if ((idx = unit_get_const_idx(name)) != SIZE_MAX) {
      if (sym) {
         sym->type = SYM_CONST;
         sym->idx = idx;
      }
      return true;
   }
   return false;
}
