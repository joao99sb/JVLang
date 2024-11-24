#include "jvvm.c"

int main(int argc, char **argv)
{
  if (argc < 3)
  {
    fprintf(stderr, "Usage: ./jvvm <input.jvasm> <output.jvb>\n");
    fprintf(stderr, "ERROR: expected input and output\n");
    exit(1);
  }

  const char *input_file_path = argv[1];
  const char *output_file_path = argv[2];

  StringView source = slurp_file(input_file_path);
  jvvm.program_size = jvvm_translate_source(source,
                                            jvvm.program, JVVM_PROGRAM_CAPACITY);

  jvvm_save_program_to_file(jvvm.program, jvvm.program_size, output_file_path);
  return 0;
}