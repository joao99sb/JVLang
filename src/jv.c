#include "jvvm.c"

int main(int argc, char **argv)
{

  if (argc < 2)
  {
    fprintf(stderr, "Usage: ./jv <input.jvb>\n");
    fprintf(stderr, "ERROR: expected input \n");
    exit(1);
  }

  const char *input_file = argv[1];

  jvvm_load_program_from_file(&jvvm, input_file);
  for (int i = 0; i < 69 && !jvvm.halt; i++)
  {

    Err err = jvvm_run_instance(&jvvm);
    if (err != ERR_OK)
    {
      fprintf(stderr, "ERROR: %s\n", err_str(err));

      jvvm_dump_stack(stderr, &jvvm);
      exit(1);
    }

    jvvm_dump_stack(stdout, &jvvm);
  }

  return 0;
}
