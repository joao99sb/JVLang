#ifndef JVVM
#define JVVM

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "utils.h"
#define STACK_CAPACITY 1024
#define JVVM_PROGRAM_CAPACITY 1024

typedef int64_t Word;

typedef enum
{
  INST_NOP = 0,
  INST_PUSH,
  INST_DUP,
  INST_ADD,
  INST_MINUS,
  INST_MULT,
  INST_DIV,
  INST_JMP,
  INST_JMP_IF,
  INST_EQ,
  INST_HALT,
  INST_PRINT_DBG
} Minemonic;

typedef struct
{
  Minemonic inst;
  Word operand;
} Inst;

typedef struct
{
  Word stack[STACK_CAPACITY];
  size_t stack_size;

  Word pc;
  Inst program[JVVM_PROGRAM_CAPACITY];
  size_t program_size;

  char *memory;

  int halt;

} Jvvm;

const char *inst_str(Minemonic inst);

#define MAKE_OPC_PUSH(val) {.inst = INST_PUSH, .operand = val}
#define MAKE_OPC_PLUS {.inst = INST_ADD}
#define MAKE_OPC_MINUS {.inst = INST_MINUS}
#define MAKE_OPC_MULT {.inst = INST_MULT}
#define MAKE_OPC_DIV {.inst = INST_DIV}
#define MAKE_OPC_JMP(addr) {.inst = INST_JMP, .operand = (addr)}
#define MAKE_OPC_HALT {.inst = INST_HALT}
#define MAKE_OPC_JMP_IF(addr) {.inst = INST_JMP_IF, .operand = (addr)}
#define MAKE_OPC_EQ {.inst = INST_EQ}
#define MAKE_OPC_DUP(index) {.inst = INST_DUP, .operand = (index)}
#define MAKE_OPC_NOP {.inst = INST_NOP}

typedef enum
{
  ERR_OK,
  ERR_STACK_OVERFLOW,
  ERR_STACK_UNDERFLOW,
  ERR_DIVISION_BY_ZERO,
  ERR_UNKNOWN_OPCODE,
  ERR_SEGMENTATION_FAULT,
  ERR_ILLEGAL_OPERAND
} Err;

Err jvvm_run_instance(Jvvm *vm);
void jvvm_push_inst(Jvvm *vm, Inst inst);
void jvvm_load_program_from_mem(Jvvm *vm, Inst *program, size_t program_size);
void jvvm_save_program_to_file(Inst *program, size_t program_size, const char *file_path);
void jvvm_load_program_from_file(Jvvm *vm, const char *file_path);
size_t jvvm_translate_source(StringView source, Inst *program, size_t program_capacity);
Inst jvvm_translate_line(StringView line);

StringView slurp_file(const char *file_path);

void jvvm_dump_stack(FILE *stream, const Jvvm *vm);
const char *err_str(Err err);

#endif
