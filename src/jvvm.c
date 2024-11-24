#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "jvvm.h"
#include "utils.h"

Jvvm jvvm = {0};

Err jvvm_run_instance(Jvvm *vm)
{
  if ((int)vm->pc > (int)vm->program_size)
  {
    printf("PC: %ld\n", vm->pc);
    printf("PROGRAM SIZE: %ld\n", vm->program_size);
    return ERR_SEGMENTATION_FAULT;
  }

  Inst inst = vm->program[vm->pc];

  switch (inst.inst)
  {
  case INST_NOP:
    vm->pc++;
    break;

  case INST_PUSH:
    if (vm->stack_size >= STACK_CAPACITY)
    {
      return ERR_STACK_OVERFLOW;
    }

    vm->stack[vm->stack_size++] = inst.operand;
    vm->pc++;
    break;

  case INST_ADD:
    if (vm->stack_size >= STACK_CAPACITY)
    {
      return ERR_STACK_OVERFLOW;
    }
    if (vm->stack_size < 2)
    {
      return ERR_STACK_UNDERFLOW;
    }

    vm->stack[vm->stack_size - 2] += vm->stack[vm->stack_size - 1];
    vm->stack_size--;
    vm->pc++;
    break;

  case INST_MINUS:
    if (vm->stack_size >= STACK_CAPACITY)
    {
      return ERR_STACK_OVERFLOW;
    }
    if (vm->stack_size < 2)
    {
      return ERR_STACK_UNDERFLOW;
    }

    vm->stack[vm->stack_size - 2] -= vm->stack[vm->stack_size - 1];
    vm->stack_size--;
    vm->pc++;
    break;

  case INST_MULT:
    if (vm->stack_size < 2)
    {
      return ERR_STACK_UNDERFLOW;
    }

    vm->stack[vm->stack_size - 2] *= vm->stack[vm->stack_size - 1];
    vm->stack_size--;
    vm->pc++;
    break;

  case INST_DIV:

    if (vm->stack[vm->stack_size - 1] == 0)
    {
      return ERR_DIVISION_BY_ZERO;
    }
    if (vm->stack_size < 2)
    {
      return ERR_STACK_UNDERFLOW;
    }

    vm->stack[vm->stack_size - 2] /= vm->stack[vm->stack_size - 1];
    vm->stack_size--;

    vm->pc++;
    break;

  case INST_JMP:
    vm->pc = inst.operand;
    break;

  case INST_EQ:
    if (vm->stack_size < 2)
    {
      return ERR_STACK_UNDERFLOW;
    }

    int is_eq = vm->stack[vm->stack_size - 2] == vm->stack[vm->stack_size - 1];
    vm->stack[vm->stack_size - 2] = is_eq;
    vm->stack_size--;
    vm->pc++;
    break;
  case INST_JMP_IF:
    if (vm->program_size < 1)
    {
      return ERR_STACK_UNDERFLOW;
    }

    if (vm->stack[vm->stack_size - 1])
    {
      vm->stack_size--;
      vm->pc = inst.operand;
    }
    else
    {
      vm->pc++;
    }
    break;
  case INST_PRINT_DBG:
    if (vm->program_size < 1)
    {
      return ERR_STACK_UNDERFLOW;
    }

    printf("%ld", vm->stack[vm->stack_size - 1]);
    vm->stack_size--;
    vm->pc++;
    break;

  case INST_DUP:
    if (vm->stack_size >= STACK_CAPACITY)
    {
      return ERR_STACK_OVERFLOW;
    }
    if (vm->stack_size - inst.operand <= 0)
    {
      return ERR_STACK_UNDERFLOW;
    }
    if (inst.operand < 0)
    {
      return ERR_ILLEGAL_OPERAND;
    }

    vm->stack[vm->stack_size] = vm->stack[vm->stack_size - 1 - inst.operand];

    vm->stack_size++;
    vm->pc++;
    break;
  case INST_HALT:
    vm->halt = 1;
    break;
  default:
    return ERR_UNKNOWN_OPCODE;
  }

  return ERR_OK;
}

void jvvm_push_inst(Jvvm *vm, Inst inst)
{
  assert(vm->program_size < JVVM_PROGRAM_CAPACITY);
  vm->program[vm->program_size++] = inst;
}

void jvvm_load_program_from_mem(Jvvm *vm, Inst *program, size_t program_size)
{
  assert(program_size <= JVVM_PROGRAM_CAPACITY);
  memcpy(vm->program, program, sizeof(program[0]) * program_size);

  vm->program_size = program_size;
  vm->pc = 0;
}

void jvvm_save_program_to_file(Inst *program, size_t program_size, const char *file_path)
{
  FILE *f = fopen(file_path, "wb");
  if (f == NULL)
  {
    fprintf(stderr, "ERROR: Could not open file \"%s\": %s", file_path, strerror(errno));
    exit(1);
  }
  fwrite(program, sizeof(program[0]), program_size, f);

  if (ferror(f))
  {
    fprintf(stderr, "ERROR: Could not open file \"%s\": %s", file_path, strerror(errno));
    exit(1);
  }
  fclose(f);
}

void jvvm_load_program_from_file(Jvvm *vm, const char *file_path)
{
  FILE *f = fopen(file_path, "rb");

  if (f == NULL)
  {
    fprintf(stderr, "ERROR: Could not open file \"%s\": %s", file_path, strerror(errno));
    exit(1);
  }

  if (fseek(f, 0, SEEK_END) < 0)
  {
    fprintf(stderr, "ERROR: Could not open file \"%s\": %s", file_path, strerror(errno));
    exit(1);
  }

  long m = ftell(f);

  if (m < 0)
  {
    fprintf(stderr, "ERROR: Could not open file \"%s\": %s", file_path, strerror(errno));
    exit(1);
  }

  assert(m % sizeof(vm->program[0]) == 0);
  assert((size_t)m <= JVVM_PROGRAM_CAPACITY * sizeof(vm->program[0]));

  if (fseek(f, 0, SEEK_SET) < 0)
  {
    fprintf(stderr, "ERROR: Could not open file \"%s\": %s", file_path, strerror(errno));
    exit(1);
  }

  vm->program_size = fread(vm->program, sizeof(vm->program[0]), m / sizeof(vm->program[0]), f);

  if (ferror(f))
  {
    fprintf(stderr, "ERROR: Could not open file \"%s\": %s", file_path, strerror(errno));
    exit(1);
  }
  fclose(f);
}

size_t jvvm_translate_source(StringView source, Inst *program, size_t program_capacity)
{

  size_t program_size = 0;
  while (source.count > 0)
  {
    assert(program_size < program_capacity);
    StringView line = sv_chop(&source, '\n');
    if (line.count > 0)
    {
      program[program_size++] = jvvm_translate_line(line);
    }
  }

  return program_size;
}

Inst jvvm_translate_line(StringView line)
{
  line = sv_trim_left(line);
  StringView inst_name = sv_chop(&line, ' ');
  StringView inst_name_lower = sv_to_lower_case(inst_name);

  if (sv_eq(inst_name_lower, cstr_to_sv("push")))
  {
    int operand = sv_to_int(sv_trim_right(line));

    sv_free(inst_name_lower);
    return (Inst){
        .inst = INST_PUSH,
        .operand = operand};
  }
  else if (sv_eq(inst_name_lower, cstr_to_sv("dup")))
  {
    int operand = sv_to_int(sv_trim_right(line));

    sv_free(inst_name_lower);
    return (Inst){
        .inst = INST_DUP,
        .operand = operand,
    };
  }
  else if (sv_eq(inst_name_lower, cstr_to_sv("plus")))
  {
    sv_free(inst_name_lower);
    return (Inst){
        .inst = INST_JMP,
    };
  }
  else if (sv_eq(inst_name_lower, cstr_to_sv("jmp")))
  {
    int operand = sv_to_int(sv_trim_right(line));
    sv_free(inst_name_lower);
    return (Inst){
        .inst = INST_JMP,
        .operand = operand,
    };
  }
  else if (sv_eq(inst_name_lower, cstr_to_sv("add")))
  {
    return (Inst){
        .inst = INST_ADD,
    };
  }

  fprintf(stderr, "ERROR `%.*s` unknow instruction", (int)inst_name.count, inst_name.data);
  exit(1);
}

StringView slurp_file(const char *file_path)
{
  FILE *f = fopen(file_path, "r");
  if (f == NULL)
  {
    fprintf(stderr, "ERROR: Could not read file %s: %s\n",
            file_path, strerror(errno));
    exit(1);
  }

  if (fseek(f, 0, SEEK_END) < 0)
  {
    fprintf(stderr, "ERROR: Could not open file \"%s\": %s", file_path, strerror(errno));
    exit(1);
  }

  long m = ftell(f);

  if (m < 0)
  {
    fprintf(stderr, "ERROR: Could not open file \"%s\": %s", file_path, strerror(errno));
    exit(1);
  }

  char *buffer = malloc(m);
  if (buffer == NULL)
  {
    fprintf(stderr, "ERROR: Could not allocate  memory for file: %s\n",
            strerror(errno));
  }

  if (fseek(f, 0, SEEK_SET) < 0)
  {
    fprintf(stderr, "ERROR: Could not open file \"%s\": %s", file_path, strerror(errno));
    exit(1);
  }

  size_t n = fread(buffer, 1, m, f);

  if (ferror(f))
  {
    fprintf(stderr, "ERROR: Could not open file \"%s\": %s", file_path, strerror(errno));
    exit(1);
  }

  fclose(f);

  return (StringView){
      .count = n,
      .data = buffer,
  };
}

/**
 * code utils
 * */
void jvvm_dump_stack(FILE *stream, const Jvvm *vm)
{
  fprintf(stream, "Stack:\n");

  if (vm->stack_size > 0)
  {

    for (uint64_t i = 0; i < vm->stack_size; ++i)
    {
      fprintf(stream, "%ld\n", vm->stack[i]);
    }

    return;
  }

  fprintf(stream, " [empty]\n");
}

const char *inst_str(Minemonic type)
{
  switch (type)
  {
  case INST_NOP:
    return "INST_NOP";
  case INST_PUSH:
    return "INST_PUSH";
  case INST_DUP:
    return "INST_DUP";
  case INST_ADD:
    return "INST_ADD";
  case INST_MINUS:
    return "INST_MINUS";
  case INST_MULT:
    return "INST_MULT";
  case INST_DIV:
    return "INST_DIV";
  case INST_JMP:
    return "INST_JMP";
  case INST_HALT:
    return "INST_HALT";
  case INST_JMP_IF:
    return "INST_JMP_IF";
  case INST_EQ:
    return "INST_EQ";
  case INST_PRINT_DBG:
    return "INST_PRINT_DBG";
  default:
    printf("%d\n", type);
    assert(0 && "INST_type_str: unreachable");
  }
}

const char *err_str(Err err)
{

  switch (err)
  {
  case ERR_OK:
    return "OK";
  case ERR_STACK_OVERFLOW:
    return "Stack overflow";
  case ERR_STACK_UNDERFLOW:
    return "Stack underflow";
  case ERR_DIVISION_BY_ZERO:
    return "Division by zero";
  case ERR_UNKNOWN_OPCODE:
    return "Unknown opcode";
  case ERR_SEGMENTATION_FAULT:
    return "Segmentation fault";

  case ERR_ILLEGAL_OPERAND:
    return "ERR_ILLEGAL_OPERAND";
  default:
    assert(0 && "err_str: unreachable");
  }
}
