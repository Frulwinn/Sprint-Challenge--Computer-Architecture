#ifndef _CPU_H_
#define _CPU_H_

// Holds all information about the CPU
struct cpu {
  unsigned char PC;
  unsigned char registers[8];
  unsigned char ram[256];
  unsigned char flag;
};

// ALU operations
enum alu_op {
  ALU_MUL,
  ALU_ADD,
  ALU_CMP
  
};


// Instructions
// These use binary literals. If these aren't available with your compiler, hex
// literals should be used.
#define LDI   0b10000010
#define PRN   0b01000111
#define HLT   0b00000001
#define MUL   0b10100010
#define ADD   0b10100000
#define JMP   0b01010100
#define JEQ   0b01010101
#define JNE   0b01010110
#define CMP   0b10100111
#define CMP_L 0b00000100 //< 4
#define CMP_G 0b00000010 //> 2
#define CMP_E 0b00000001 //==1


// Function declarations

extern void cpu_load(char *filename, struct cpu *cpu);
extern void cpu_init(struct cpu *cpu);
extern void cpu_run(struct cpu *cpu);

#endif
