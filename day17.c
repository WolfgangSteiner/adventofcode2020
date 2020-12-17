#include <stdio.h>
#include "unity.h"
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "util.h"


void setUp()
{
}


void tearDown()
{
}


typedef struct
{
    int x_min, x_max;
    int y_min, y_max;
    int z_min, z_max;
    int w_min, w_max;

    size_t x_size;
    size_t y_size;
    size_t z_size;
    size_t w_size;
    
    size_t num_cells;
    char* data;
} grid_t;


grid_t* grid_initialize(int x_size, int y_size, int z_size, int w_size)
{
    grid_t* g = calloc(1, sizeof(grid_t));
    g->x_min = -(x_size/2);
    g->x_max =  x_size/2;
    g->y_min = -(y_size/2);
    g->y_max =  y_size/2;
    g->z_min = -(z_size/2);
    g->z_max = z_size/2;
    g->w_min = -(w_size/2);
    g->w_max = w_size/2;

    g->x_size = x_size;
    g->y_size = y_size; 
    g->z_size = z_size; 
    g->w_size = w_size; 

    g->num_cells = g->x_size * g->y_size * g->z_size * g->w_size;
    g->data = calloc(g->num_cells, sizeof(char));

    return g;
}


char* grid_get_cell(grid_t* g, int x, int y, int z, int w)
{
   size_t x_offset = x - g->x_min;
   size_t y_offset = y - g->y_min;
   size_t z_offset = z - g->z_min;
   size_t w_offset = w - g->w_min;
   return g->data + ((w_offset * g->z_size + z_offset) * g->y_size + y_offset) * g->x_size + x_offset;
}


grid_t* copy_grid(grid_t* grid)
{
    grid_t* copy = malloc(sizeof(grid_t));
    memcpy(copy, grid, sizeof(grid_t));
    memcpy(copy->data, grid->data, grid->num_cells);
    return copy;
}


void grid_free(grid_t* grid)
{
    free(grid->data);
    free(grid);
}


size_t count_active_cells(grid_t* grid)
{
    size_t result = 0;
    for (int w = grid->w_min; w <= grid->w_max; w++)
    {
        for (int z = grid->z_min; z <= grid->z_max; z++)
        {
            for (int y = grid->y_min; y <= grid->y_max; y++)
            {
                for (int x = grid->x_min; x <= grid->x_max; x++)
                {
                    result += *grid_get_cell(grid, x, y, z, w);
                }
            }
        } 
    }
    return result;
}


size_t grid_count_neighbors(grid_t* grid, int i_x, int i_y, int i_z, int i_w)
{
    int x_start = MAX(grid->x_min, i_x - 1);
    int x_end   = MIN(grid->x_max, i_x + 1);
    int y_start = MAX(grid->y_min, i_y - 1);
    int y_end   = MIN(grid->y_max, i_y + 1);
    int z_start = MAX(grid->z_min, i_z - 1);
    int z_end   = MIN(grid->z_max, i_z + 1);
    int w_start = MAX(grid->w_min, i_w - 1);
    int w_end   = MIN(grid->w_max, i_w + 1);

    size_t result = 0;

    for (int w = w_start; w <= w_end; ++w)
    {
        for (int z = z_start; z <= z_end; ++z)
        {
            for (int y = y_start; y <= y_end; ++y)
            {
                for (int x = x_start; x <= x_end; ++x)
                {
                    if (x == i_x && y == i_y && z == i_z && w == i_w) continue;
                    result += *grid_get_cell(grid, x, y, z, w);
                }
            }
        }
    }
    return result;
}


grid_t* play_round(grid_t* g)
{
    grid_t* new_grid = grid_initialize(g->x_size, g->y_size, g->z_size, g->w_size);

    for (int w = g->w_min; w <= g->w_max; w++)
    {
        for (int z = g->z_min; z <= g->z_max; z++)
        {
            for (int y = g->y_min; y <= g->y_max; y++)
            {
                for (int x = g->x_min; x <= g->x_max; x++)
                {
                    const size_t num_neighbors = grid_count_neighbors(g, x, y, z, w);
                    char* cell = grid_get_cell(g, x, y, z, w);
                    char* new_cell = grid_get_cell(new_grid, x, y, z, w);
                    if (*cell == 1 && (num_neighbors == 2 || num_neighbors == 3))
                    {
                        *new_cell = 1;
                    }
                    else if (*cell == 0 && num_neighbors == 3)
                    {
                        *new_cell = 1;
                    }

                }
            }
        }
    }
    return new_grid;
}


grid_t* play_game(grid_t* grid, int num_cycles)
{
    for (int i = 0; i < num_cycles; ++i)
    {
        grid_t* new_grid = play_round(grid);
        grid_free(grid);
        grid = new_grid;
    }

    return grid;
}


void read_input(char* file_name, grid_t* grid)
{
    FILE* fp = fopen(file_name, "r");
    char line[64];
    size_t width = 0;
    int x, y, z, w;
    while(!feof(fp))
    {
        if (fgets(line, 64, fp))
        {
            chomp(line);
            if (width == 0)
            {
                width = strlen(line);
                x = -(width/2);
                y = -(width/2);
                z = 0;
                w = 0;
            }
            else
            {
                x = -(width/2);
                y++;
            }

            char* pos = line;
            while(*pos)
            {
                if (*pos == '#')
                {
                    char* cell = grid_get_cell(grid, x, y, z, w);
                    *cell = 1;
                }
                x++;
                pos++;
            }
        }
    }
}


void test_read_input()
{
    grid_t* g = grid_initialize(19, 19, 19, 1);
    read_input("day17_test.txt", g);
    TEST_ASSERT_EQUAL(5, count_active_cells(g));
    grid_free(g);
}


void test_count_neighbors()
{
    grid_t* g = grid_initialize(19, 19, 19, 1);
    read_input("day17_test.txt", g);
    TEST_ASSERT_EQUAL(5, grid_count_neighbors(g, 0, 0, 0, 0));
    TEST_ASSERT_EQUAL(1, grid_count_neighbors(g, -1, -1, 0, 0));
    TEST_ASSERT_EQUAL(2, grid_count_neighbors(g,  1, 1, 0, 0));
    grid_free(g);
}


void test_play_game()
{
    grid_t* g = grid_initialize(19, 19, 19, 1);
    read_input("day17_test.txt", g);
    g = play_game(g, 1);
    TEST_ASSERT_EQUAL(11, count_active_cells(g));
    g = play_game(g, 1);
    TEST_ASSERT_EQUAL(21, count_active_cells(g));
    g = play_game(g, 1);
    TEST_ASSERT_EQUAL(38, count_active_cells(g));
    g = play_game(g, 3);
    TEST_ASSERT_EQUAL(112, count_active_cells(g));
    grid_free(g);
}

void test_play_game_part_two()
{
    grid_t* g = grid_initialize(19, 19, 19, 19);
    read_input("day17_test.txt", g);
    g = play_game(g, 6);
    TEST_ASSERT_EQUAL(848, count_active_cells(g));
    grid_free(g);
}

int main(int argc, char** argv)
{
    MUnused(argc);
    MUnused(argv);

    UNITY_BEGIN();
    RUN_TEST(test_read_input);
    RUN_TEST(test_count_neighbors);
    RUN_TEST(test_play_game);
    RUN_TEST(test_play_game_part_two);
    UNITY_END();

    grid_t* g = grid_initialize(19, 19, 19, 1);
    read_input("day17.txt", g);
    g = play_game(g, 6);
    size_t result_part_one = count_active_cells(g);
    printf("Result Part 1: %ld\n", result_part_one);
    grid_free(g);

    g = grid_initialize(19, 19, 19, 19);
    read_input("day17.txt", g);
    g = play_game(g, 6);
    size_t result_part_two = count_active_cells(g);
    printf("Result Part 2: %ld\n", result_part_two);
    grid_free(g);

    return 0;
}

