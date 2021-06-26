const char* help_options =
	"  -o                   Specify the output file (or '-' for standard output). By default the output file is derived from the source file, except if the -A option was specified, then it is standard output.\n"
	"  -c                   Compile and assemble the program. (This is the default)\n"
	"  -A                   Only emit the abstract syntax tree of the program, as interpreted by the parser.\n"
	"  -i                   Only emit the intermediate representation.\n"
	"  -S                   Emit the assembly output.\n"
	"  -O                   Specify the optimization level. (possible values: 0, 1, 2, 3)\n"
	"  -w                   Disable all warnings.\n"
	"  -V                   Print the version information.\n"
	"  -m                   Specify a target-dependend option.\n"
	"  -h                   Print a simple help message.\n"
	"  -dumpmachine         Print the target machine architectue.\n"
	"  -dumpversion         Print the version.\n"
;
