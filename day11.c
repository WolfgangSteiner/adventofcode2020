#include <string.h>
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "unity.h"
#include "util.h"



typedef struct
{
    char* rows[512];
    int numCols;
    int numRows;
} Grid;


Grid* grid_init()
{
    Grid* grid = calloc(1, sizeof(Grid));
    return grid;
}



void grid_delete(Grid* grid)
{
    for (int i = 0; i < grid->numRows; ++i)
    {
        free(grid->rows[i]);
    }

    free(grid);
}


Grid* grid_copy(const Grid* grid)
{
    Grid* out = grid_init();
    out->numRows = grid->numRows;
    out->numCols = grid->numCols;

    for (int i = 0; i < grid->numRows; ++i)
    {
        out->rows[i] = malloc(grid->numCols);
        strncpy(out->rows[i], grid->rows[i], grid->numRows);
    }

    return out;
}


bool grid_equal(const Grid* a, const Grid* b)
{
    if (a->numRows != b->numRows) return false;
    if (a->numCols != b->numCols) return false;

    
    for (int i = 0; i < a->numRows; ++i)
    {
        if (strncmp(a->rows[i], b->rows[i], a->numCols))
        {
            return false;
        }
    }

    return true;
}


char grid_get_state(const Grid* grid, int row, int col)
{
    return grid->rows[row][col];
}


char grid_set_state(Grid* grid, int row, int col, char state)
{
    grid->rows[row][col] = state;
}


void grid_print(const Grid* grid)
{
    for (int row = 0; row < grid->numRows; ++row)
    {
        for (int col = 0; col < grid->numCols; ++col)
        {
            putchar(grid_get_state(grid, row, col));
        }

        putchar('\n');
    }
    putchar('\n');
    putchar('\n');
}


int grid_num_occupied_neighbors(const Grid* grid, int row, int col)
{
    int result = 0;

    const int x1 = MAX(0, col - 1);
    const int x2 = MIN(col + 1, grid->numCols - 1);
    const int y1 = MAX(0, row - 1);
    const int y2 = MIN(row + 1, grid->numRows - 1);

    for (int y = y1; y <= y2; y++)
    {
        for (int x = x1; x <= x2; x++)
        {
            if (x == col && y == row) continue;
            if (grid_get_state(grid, y, x) == '#') result++;
        }
    }

    return result;
}


int grid_num_occupied_visible_neighbors_direction(const Grid* grid, int row, int col, int dy, int dx)
{
    int x = col;
    int y = row;
    int result = 0;

    while (true)
    {
        x += dx;
        y += dy;

        if (x < 0 || x >= grid->numCols || y < 0 || y >= grid->numRows) break;
        char state = grid_get_state(grid, y, x);
        if (state == 'L') return 0;
        if (state == '#') return 1;
    }

    return 0;
}


int grid_num_occupied_visible_neighbors(const Grid* grid, int row, int col)
{
    int result = 0;
    result += grid_num_occupied_visible_neighbors_direction(grid, row, col, -1,  0);
    result += grid_num_occupied_visible_neighbors_direction(grid, row, col, -1,  1);
    result += grid_num_occupied_visible_neighbors_direction(grid, row, col,  0,  1);
    result += grid_num_occupied_visible_neighbors_direction(grid, row, col,  1,  1);
    result += grid_num_occupied_visible_neighbors_direction(grid, row, col,  1,  0);
    result += grid_num_occupied_visible_neighbors_direction(grid, row, col,  1, -1);
    result += grid_num_occupied_visible_neighbors_direction(grid, row, col,  0, -1);
    result += grid_num_occupied_visible_neighbors_direction(grid, row, col, -1, -1);
    return result;
}


int grid_num_occupied_seats(const Grid* grid)
{
    int result = 0;

    for (int row = 0; row < grid->numRows; ++row)
    {
        for (int col = 0; col < grid->numCols; ++col)
        {
            if (grid_get_state(grid, row, col) == '#') result++;
        }
    }

    return result;
}


int grid_simulate(Grid* out, const Grid* in)
{
    int numChanges = 0;
    for (int y = 0; y < in->numRows; ++y)
    {
        for (int x = 0; x < in->numCols; ++x)
        {
            const int numOccupiedNeighbors = grid_num_occupied_neighbors(in, y, x);
            char state = grid_get_state(in, y, x);
             
            if (state == 'L' && numOccupiedNeighbors == 0)
            {
                state = '#';
                numChanges++;
            }
            else if (state == '#' && numOccupiedNeighbors >= 4)
            {
                state = 'L';
                numChanges++;
            }

            grid_set_state(out, y, x, state);
        }
    }

    return numChanges;
}

int grid_simulate2(Grid* out, const Grid* in)
{
    int numChanges = 0;
    for (int y = 0; y < in->numRows; ++y)
    {
        for (int x = 0; x < in->numCols; ++x)
        {
            const int numOccupiedNeighbors = grid_num_occupied_visible_neighbors(in, y, x);
            char state = grid_get_state(in, y, x);
             
            if (state == 'L' && numOccupiedNeighbors == 0)
            {
                state = '#';
                numChanges++;
            }
            else if (state == '#' && numOccupiedNeighbors >= 5)
            {
                state = 'L';
                numChanges++;
            }

            grid_set_state(out, y, x, state);
        }
    }

    return numChanges;
}

Grid* grid_read(const char* fileName)
{
    FILE* fp = fopen(fileName, "r");
    char line[512];

    Grid* grid = grid_init();

    while (!feof(fp))
    {
        if (fgets(line, 512, fp))
        {
            const int width = strlen(line) - 1;
            grid->rows[grid->numRows] = malloc(width);
            strncpy(grid->rows[grid->numRows], line, width);
            MImplies(grid->numCols, width == grid->numCols);
            grid->numCols = width;
            grid->numRows++;
        }
    }

    return grid;
}

Grid* test00;
Grid* test01;
Grid* test02;
Grid* test03;
Grid* test04;
Grid* test05;
Grid* test12;
Grid* test13;
Grid* test14;
Grid* test15;
Grid* test16;


void setUp()
{
    test00 = grid_read("day11_test00.txt");
    test01 = grid_read("day11_test01.txt");
    test02 = grid_read("day11_test02.txt");
    test03 = grid_read("day11_test03.txt");
    test04 = grid_read("day11_test04.txt");
    test05 = grid_read("day11_test05.txt");
    test12 = grid_read("day11_test12.txt");
    test13 = grid_read("day11_test13.txt");
    test14 = grid_read("day11_test14.txt");
    test15 = grid_read("day11_test15.txt");
    test16 = grid_read("day11_test16.txt");
}

void tearDown() 
{
    grid_delete(test00);
    grid_delete(test01);
    grid_delete(test02);
    grid_delete(test03);
    grid_delete(test04);
    grid_delete(test05);
    grid_delete(test12);
    grid_delete(test13);
    grid_delete(test14);
    grid_delete(test15);
    grid_delete(test16);
}

void test_grid_size()
{
    TEST_ASSERT_EQUAL(10, test00->numCols);
    TEST_ASSERT_EQUAL(10, test00->numRows);
    TEST_ASSERT_EQUAL(10, test01->numCols);
    TEST_ASSERT_EQUAL(10, test01->numRows);
    TEST_ASSERT_EQUAL(10, test02->numCols);
    TEST_ASSERT_EQUAL(10, test02->numRows);
    TEST_ASSERT_EQUAL(10, test03->numCols);
    TEST_ASSERT_EQUAL(10, test03->numRows);
    TEST_ASSERT_EQUAL(10, test04->numCols);
    TEST_ASSERT_EQUAL(10, test04->numRows);
}

void test_grid_equal()
{
    Grid* b = grid_read("day11_test00.txt");
    TEST_ASSERT_EQUAL(10, b->numCols);
    TEST_ASSERT_EQUAL(10, b->numRows);
    TEST_ASSERT_EQUAL(true, grid_equal(test00, b));
    grid_delete(b);
}

void test_grid_copy()
{
    Grid* b = grid_copy(test00);
    TEST_ASSERT_EQUAL(10, b->numCols);
    TEST_ASSERT_EQUAL(10, b->numRows);
    TEST_ASSERT(grid_equal(test00, b));
    grid_delete(b);
}

void test_min_max()
{
    TEST_ASSERT_EQUAL(4, MIN(4, 5));
    TEST_ASSERT_EQUAL(5, MAX(4, 5));
}

void test_grid_get_state()
{
    TEST_ASSERT_EQUAL('#', grid_get_state(test01, 0, 0));
    TEST_ASSERT_EQUAL('.', grid_get_state(test01, 0, 1));
}

void test_grid_occupied_neighbors()
{
    Grid* g = grid_read("day11_test_small.txt");
    TEST_ASSERT_EQUAL(2, grid_num_occupied_neighbors(g, 0, 0));
    TEST_ASSERT_EQUAL(4, grid_num_occupied_neighbors(g, 0, 1));
    TEST_ASSERT_EQUAL(2, grid_num_occupied_neighbors(g, 0, 2));
    TEST_ASSERT_EQUAL(4, grid_num_occupied_neighbors(g, 1, 0));
    TEST_ASSERT_EQUAL(8, grid_num_occupied_neighbors(g, 1, 1));
    TEST_ASSERT_EQUAL(4, grid_num_occupied_neighbors(g, 1, 2));
    TEST_ASSERT_EQUAL(2, grid_num_occupied_neighbors(g, 2, 0));
    TEST_ASSERT_EQUAL(4, grid_num_occupied_neighbors(g, 2, 1));
    TEST_ASSERT_EQUAL(2, grid_num_occupied_neighbors(g, 2, 2));
    TEST_ASSERT_EQUAL(2, grid_num_occupied_neighbors(test01, 0, 0));
    grid_delete(g);
}

void test_grid_simulate()
{
    Grid* b = grid_copy(test00);
    
    grid_simulate(b, test00);
    TEST_ASSERT(grid_equal(b, test01));

    grid_simulate(b, test01);
    TEST_ASSERT(grid_equal(b, test02));

    grid_simulate(b, test02);
    TEST_ASSERT(grid_equal(b, test03));
    
    grid_simulate(b, test03);
    TEST_ASSERT(grid_equal(b, test04));

    grid_simulate(b, test04);
    TEST_ASSERT(grid_equal(b, test05));

    int num_changes = grid_simulate(b, test05);
    TEST_ASSERT(grid_equal(b, test05));
    TEST_ASSERT_EQUAL(num_changes, 0);
    TEST_ASSERT_EQUAL(37, grid_num_occupied_seats(b));

    grid_delete(b);
}

void test_grid_simulate2()
{
    Grid* b = grid_copy(test00);
    
    grid_simulate2(b, test00);
    TEST_ASSERT(grid_equal(b, test01));

    grid_simulate2(b, test01);
    TEST_ASSERT(grid_equal(b, test12));

    grid_simulate2(b, test12);
    TEST_ASSERT(grid_equal(b, test13));
    
    grid_simulate2(b, test13);
    TEST_ASSERT(grid_equal(b, test14));

    grid_simulate2(b, test14);
    TEST_ASSERT(grid_equal(b, test15));

    grid_simulate2(b, test15);
    TEST_ASSERT(grid_equal(b, test16));

    int num_changes = grid_simulate(b, test16);
    TEST_ASSERT(grid_equal(b, test16));
    TEST_ASSERT_EQUAL(num_changes, 0);
    TEST_ASSERT_EQUAL(26, grid_num_occupied_seats(b));

    grid_delete(b);
}

int main(int argc, char** argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_min_max);
    RUN_TEST(test_grid_size);
    RUN_TEST(test_grid_equal);
    RUN_TEST(test_grid_copy);
    RUN_TEST(test_grid_get_state);
    RUN_TEST(test_grid_occupied_neighbors);
    RUN_TEST(test_grid_simulate);
    RUN_TEST(test_grid_simulate2);
    UNITY_END();

    Grid* a = grid_read("day11.txt");
    Grid* b = grid_copy(a);

    while (grid_simulate(b, a))
    {
        Grid* c = a;
        a = b;
        b = c;
    }

    const int numOccupiedSeats = grid_num_occupied_seats(b);
    printf("Num occupied seats Part 1: %d\n", numOccupiedSeats);

    a = grid_read("day11.txt");

    while (grid_simulate2(b, a))
    {
        Grid* c = a;
        a = b;
        b = c;
    }

    const int numOccupiedSeats2 = grid_num_occupied_seats(b);
    printf("Num occupied seats Part 2: %d\n", numOccupiedSeats2);

    return 0;
}

