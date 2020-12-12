#include <stdio.h>
#include "unity.h"
#include "util.h"
#include <string.h>
#include <stdbool.h>

int chomp(char* str)
{
    int length = strlen(str);
    while (length && str[length - 1] == '\n')
    {
        str[length - 1] = '\0';
        length--;
    }

    return length;
}

typedef struct
{
    char opcode;
    int value;
} Instruction;


typedef struct
{
    Instruction* instr;
    size_t size;
    size_t alloc_size;
} Program;


typedef struct
{
    int x;
    int y;
    int wp_x;
    int wp_y;
} State;


Program* program_init(size_t alloc_size)
{
    Program* p = calloc(1, sizeof(Program));
    p->alloc_size = alloc_size;
    p->instr = calloc(alloc_size, sizeof(Instruction));
    return p;
}


Program* program_read(char* fileName)
{
    FILE* fp = fopen(fileName, "r");
    Program* p = program_init(1);
    char line[16];

    while(!feof(fp))
    {
        if (fgets(line, 16, fp))
        {
            if (p->size == p->alloc_size)
            {
                p->alloc_size *= 2;
                p->instr = realloc(p->instr, p->alloc_size * sizeof(Instruction));
            }

            p->instr[p->size] = (Instruction){ line[0], atoi(line + 1) };
            p->size++;
        }
    }

    return p;
}


void program_print(Program* p)
{
    for (int i = 0; i < p->size; ++i)
    {
        printf("%c %d\n", p->instr[i].opcode, p->instr[i].value);
    }
}


void program_free(Program *p)
{
    free(p->instr);
    free(p);
}



int cosi(int degrees)
{
    degrees = (degrees + 720) % 360;

    if (degrees == 0)
    {
        return 1;
    }
    else if (degrees == 90 || degrees == 270)
    {
        return 0;
    }
    else if (degrees == 180)
    {
        return -1;
    }

    assert(false);
    return 0;
}

int sini(int degrees)
{
    degrees = (degrees + 720) % 360;

    if (degrees == 0 || degrees == 180)
    {
        return 0;
    }
    else if (degrees == 90)
    {
        return 1;
    }
    else if (degrees == 270)
    {
        return -1;
    }

    assert(false);
    return 0;
}


int absi(int val)
{
    return val < 0 ? -val : val;
}


void turn(State* s, int degrees)
{
    int wp_y = s->wp_y * cosi(degrees) - s->wp_x * sini(degrees);
    int wp_x = s->wp_y * sini(degrees) + s->wp_x * cosi(degrees);
    s->wp_x = wp_x;
    s->wp_y = wp_y;
}



void program_execute(const Program* p, State* s)
{
    s->x = 0;
    s->y = 0;
    s->wp_x = 10;
    s->wp_y = 1;

    for (int i = 0; i < p->size; ++i)
    {
        Instruction ins = p->instr[i];
        
        switch(ins.opcode)
        {
            case 'F':
                s->x += s->wp_x * ins.value;
                s->y += s->wp_y * ins.value;
                break;
            
            case 'N':
                s->wp_y += ins.value;
                break;
            
            case 'E':
                s->wp_x += ins.value;
                break;

            case 'S':
                s->wp_y -= ins.value;
                break;

            case 'W':
                s->wp_x -= ins.value;
                break;

            case 'R':
                turn(s, ins.value);
                break;

            case 'L':
                turn(s, -ins.value);
                break;

            default:
                assert(false);
        }
    }
}

int manhatten_distance(State* s)
{
    return absi(s->x) + absi(s->y);
}


Program* testProgram;

void setUp()
{
    testProgram = program_read("day12_test.txt");
}


void tearDown() 
{
    program_free(testProgram);
};


void test_turn()
{
    State s;
    s.wp_x = 0;
    s.wp_y = 1;
    turn(&s, 90);
    TEST_ASSERT_EQUAL(1, s.wp_x);
    TEST_ASSERT_EQUAL(0, s.wp_y);

    turn(&s, 90);
    TEST_ASSERT_EQUAL(0, s.wp_x);
    TEST_ASSERT_EQUAL(-1, s.wp_y);

    turn(&s, 90);
    TEST_ASSERT_EQUAL(-1, s.wp_x);
    TEST_ASSERT_EQUAL(0, s.wp_y);

    turn(&s, 90);
    TEST_ASSERT_EQUAL(0, s.wp_x);
    TEST_ASSERT_EQUAL(1, s.wp_y);


    s.wp_x = 2;
    s.wp_y = 1;
    turn(&s, 90);
    TEST_ASSERT_EQUAL( 1, s.wp_x);
    TEST_ASSERT_EQUAL(-2, s.wp_y);

    turn(&s, 90);
    TEST_ASSERT_EQUAL(-2, s.wp_x);
    TEST_ASSERT_EQUAL(-1, s.wp_y);

    turn(&s, 90);
    TEST_ASSERT_EQUAL(-1, s.wp_x);
    TEST_ASSERT_EQUAL( 2, s.wp_y);

    turn(&s, 90);
    TEST_ASSERT_EQUAL(2, s.wp_x);
    TEST_ASSERT_EQUAL(1, s.wp_y);

}


void test_program()
{
    State s;
    program_execute(testProgram, &s);
    TEST_ASSERT_EQUAL(214, s.x);
    TEST_ASSERT_EQUAL(-72, s.y);
    TEST_ASSERT_EQUAL(286, manhatten_distance(&s));
}


int main(int argc, char** argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_turn);
    RUN_TEST(test_program);
    UNITY_END();

    State s;
    Program* p = program_read("day12.txt");
    program_execute(p, &s);
    printf("Final Manhatten distance: %d\n", manhatten_distance(&s));

    return 0;
}
