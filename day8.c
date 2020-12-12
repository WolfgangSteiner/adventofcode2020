#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "unity.h"
#include "util.h"

void setUp() {}
void tearDown() {}




typedef struct 
{
    enum { NOP, ACC, JMP } type;
    int value;
    int count;
} Instruction;


void init_instruction(Instruction* instruction, const char* instructionName, const char* valueString)
{
    instruction->type = !strcmp(instructionName, "nop") ? NOP : !strcmp(instructionName, "acc") ? ACC : JMP;
    instruction->value = atoi(valueString);
    instruction->count = 0;
}

struct sCpu
{
    int pc;
    int acc;
    enum { RUN, INF, END } status;
    Instruction* instructions;
    int numInstructions;
    void (*callbacks[3])(struct sCpu*, int value); 
};

typedef struct sCpu Cpu;


void cpu_execute_nop(Cpu* cpu, int value)
{
    cpu->pc++;
}

void cpu_execute_acc(Cpu* cpu, int value)
{
    cpu->acc += value;
    cpu->pc++;
}

void cpu_execute_jmp(Cpu* cpu, int value)
{
    cpu->pc += value;
}


void cpu_init(Cpu* cpu, Instruction* instructions, int numInstructions)
{
    cpu->pc = 0;
    cpu->acc = 0;
    cpu->status = RUN;
    cpu->instructions = instructions;
    cpu->numInstructions = numInstructions;
    cpu->callbacks[NOP] = cpu_execute_nop;
    cpu->callbacks[ACC] = cpu_execute_acc;
    cpu->callbacks[JMP] = cpu_execute_jmp;

    for (int i = 0; i < numInstructions; ++i)
    {
        instructions[i].count = 0;
    }
}

void cpu_execute(Cpu* cpu)
{
    while (cpu->pc < cpu->numInstructions)
    {
        Instruction* nextInstruction = &(cpu->instructions[cpu->pc]);
        if (nextInstruction->count > 0)
        {
            cpu->status = INF;
            return;
            
        }

        cpu->callbacks[nextInstruction->type](cpu, nextInstruction->value);
        nextInstruction->count++;
    }

    cpu->status = END;
}

int mutate_code(Instruction* dst, const Instruction* src, int count, int start)
{
    memcpy(dst, src, sizeof(Instruction) * count);
    
    for (int i = start; i < count; ++i)
    {
        Instruction* inst = &dst[i];

        if (inst->type == NOP && inst->value != 0)
        {
            inst->type = JMP;
            return i + 1;
        }
        else if (inst->type == JMP)
        {
            inst->type = NOP;
            return i + 1;
        }
    }
    return count;
}


int main(int argc, char** argv)
{
    UNITY_BEGIN();
    UNITY_END();
    
    int count = 0;
    char line[512];
    char instruction[64];
    char value[64];
    char fileName[] = "day8.txt";
    Instruction instructions[1024];
    Instruction mutated[1024];

    if (argc == 2)
    {
        strcpy(fileName, argv[1]);
    }

    FILE* fp = fopen(fileName, "r");

    while (!feof(fp))
    {
        if (fgets(line, 512, fp))
        {
            const char* pos = copy_word(instruction, line);
            pos = copy_word(value, pos);
            init_instruction(&instructions[count++], instruction, value);
        }
    }

    Cpu cpu;
    cpu_init(&cpu, instructions, count);
    cpu_execute(&cpu); 

    printf("Result: %d\n", cpu.acc);


    int start = 0;
    int trial = 0;
    
    while (start < count)
    {
        printf("%d\n", trial++);
        start = mutate_code(mutated, instructions, count, start);
        cpu_init(&cpu, mutated, count);
        cpu_execute(&cpu);
        if (cpu.status == END)
        {
            printf("Fixed program! acc = %d\n", cpu.acc);
            return 0;
        }
    }

    printf("Could not fix program...\n");

    return 0;
}
