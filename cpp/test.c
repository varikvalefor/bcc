int printf(const char*, ...);

#define text "Hello World"

/*
 * this is a comment 
 * */

// this is a single-line comment

#undef text

int main(int argc, char** argv) {
   printf(text);
}
