#ifndef FILE_UNIT_H
#define FILE_UNIT_H
#include "func.h"

struct typerename {
   struct source_pos begin, end;
   struct value_type* type;
   istr_t name;
};

struct cunit {
   struct function** funcs;
   struct variable* vars;
   struct typerename* renames;
   struct enumeration** enums;
   struct enum_entry* constants;
};

extern struct cunit cunit;

void parse_unit(void);
void print_unit(FILE*);
void print_ir_unit(FILE*);
struct function* unit_get_func(const char*);
struct variable* unit_get_var(const char*);
struct typerename* unit_get_typedef(const char*);
void free_unit(void);
bool find_constant(const char*, intmax_t*);

#endif /* FILE_UNIT_H */
