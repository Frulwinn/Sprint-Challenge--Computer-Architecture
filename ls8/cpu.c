#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cpu.h"


unsigned char cpu_ram_read(struct cpu *cpu, unsigned char index)
{
  return cpu->ram[index];
}

void cpu_ram_write(struct cpu *cpu, unsigned char value, unsigned char index)
{
  cpu->ram[index] = value;
}

void cpu_load(char *filename, struct cpu *cpu)
{
  FILE *fp = fopen(filename, "r");
  char line[1024];
  int address = 0;


  //open the source file
  if (fp == NULL) {
    fprintf(stderr, "Cannot open file %s\n", filename);
    exit(2);
  }

  //read all the lines and store them in RAM
  while (fgets(line, sizeof line, fp) != NULL) {

    //convert string to a number
    char *endptr;
    unsigned char value = strtol(line, &endptr, 2);

    //ignore lines from which no numbers were read
    if (endptr == line) {
      continue;
    }

    //store in ram
    cpu_ram_write(cpu, value, address++);
  }
  fclose(fp);
}

void alu(struct cpu *cpu, enum alu_op op, unsigned char regA, unsigned char regB)
{

  switch (op) {
    case ALU_MUL:
      cpu->registers[regA] *= cpu->registers[regB];
      break;

    case ALU_ADD:
      cpu->registers[regA] += cpu->registers[regB];
      break;

    case ALU_CMP:
      //Compare the values in two registers.
      //If they are equal, set the Equal E flag to 1, otherwise set it to 0.
      if (cpu->registers[regA] == cpu->registers[regB]) {
        cpu->flag = CMP_E;

      //If registerA is less than registerB, set the Less-than L flag to 1, 
      //otherwise set it to 0.
      } else if (cpu->registers[regA] < cpu->registers[regB]) {
        cpu->flag = CMP_L;
      //If registerA is greater than registerB, set the Greater-than G flag to 1, 
      //otherwise set it to 0.
      } else {
        cpu->flag = CMP_G;
      }
    break;
  }
}

void cpu_run(struct cpu *cpu)
{

  int running = 1; // True until we get a HLT instruction

  while (running) {
    unsigned char opA = 0;
    unsigned char opB = 0;
    unsigned char IR = cpu_ram_read(cpu, cpu->PC);
    unsigned int num_op = (IR >> 6);

    //bitwise-AND the result with 0xFF (255) to keep the register values in that range
    if (num_op == 2) {
      opA = cpu_ram_read(cpu, (cpu->PC + 1));
      opB = cpu_ram_read(cpu, (cpu->PC + 2));
    } else if (num_op == 1) {
      opA = cpu_ram_read(cpu, (cpu->PC + 1));
    } else {
      return;
    }

    //this line is shifting the instruction by 4 bits to access
    //then seeing if the bit is set to 0 or 1
    int instruction_set_pc = (IR >> 4) & 1;

    switch (IR) {
      case LDI:
        cpu->registers[opA] = opB;
        break;

      case PRN:
        printf("%d\n", cpu->registers[opA]);
        break;

      case MUL:
        alu(cpu, ALU_MUL, opA, opB);
        break;

      case ADD:
        alu(cpu, ALU_ADD, opA, opB);
        break;

      case CMP:
        alu(cpu, ALU_CMP, opA, opB);
        break;

      case JMP:
        //Jump to the address stored in the given register.
        //Set the PC to the address stored in the given register.
        cpu->PC = cpu->registers[opA];
        break;

      case JEQ:
        //If equal flag is set (true), jump to the address stored in the given register.
        if ((cpu->flag & CMP_E) == CMP_E) {
          cpu->PC = cpu->registers[opA];
         } else {
        // or just go to the next step
           cpu->PC += 2;
        }
        break;

      case JNE:
        //If E flag is clear (false, 0), jump to the address stored in the given register.
        if ((cpu->flag & CMP_E) != CMP_E) {
          cpu->PC = cpu->registers[opA];
        } else {
          //or just go to next step
          cpu->PC += 2;
        }
        break;

      case HLT:
        running = 0;
        break;

      default:
        fprintf(stderr, "PC %02x: unknown instruction %02x\n", cpu->PC, IR);
        exit(2);
    }

    if (!instruction_set_pc) {
      //increment PC by the number of arguments that were passed to the instruction we just executed
      cpu->PC += num_op + 1;
      
    }
  }
}

void cpu_init(struct cpu *cpu)
{
  //PC and FL registers are cleared to 0
  cpu->PC = 0;
  //cpu->flag = 0;
  //R7 is set to 0xF4
  cpu->registers[7] = 0xF4;

  //0: Timer interrupt. This interrupt triggers once per second.
  //1: Keyboard interrupt. This interrupt triggers when a key is pressed. 
  //The value of the key pressed is stored in address 0xF4.

  //zero registers and RAM
  memset(cpu->registers, 0, sizeof cpu->registers);
  memset(cpu->ram, 0, sizeof cpu->ram);

}
