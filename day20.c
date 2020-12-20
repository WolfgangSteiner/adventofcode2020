#include <stdio.h>
#include "unity.h"
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "util.h"
#include <math.h>
#include "dynarr.h"
#include <stdint.h>
#include "basetypes.h"
#include "bitmap.h"
#include "util.h"

char* final_image_str =
".#.#..#.##...#.##..#####"
"###....#.#....#..#......"
"##.##.###.#.#..######..."
"###.#####...#.#####.#..#"
"##.#....#.##.####...#.##"
"...########.#....#####.#"
"....#..#...##..#.#.###.."
".####...#..#.....#......"
"#..#.##..#..###.#.##...."
"#.####..#.####.#.#.###.."
"###.#.#...#.######.#..##"
"#.####....##..########.#"
"##..##.#...#...#.#.#.#.."
"...#..#..#.#.##..###.###"
".#.#....#.##.#...###.##."
"###.#...#..#.##.######.."
".#.#.###.##.##.#..#.##.."
".####.###.#...###.#..#.#"
"..#.#..#..#.#.#.####.###"
"#..####...#.#.#.###.###."
"#####..#####...###....##"
"#.##..#..#...#..####...#"
".#.###..##..##..####.##."
"...###...##...#...#..###";

char* monster_str = 
"                  # "
"#    ##    ##    ###"
" #  #  #  #  #  #   ";

typedef struct
{
    u32 id;
    u32 left;
    u32 right;
    u32 top;
    u32 bottom;

    bool left_unique;
    bool right_unique;
    bool top_unique;
    bool bottom_unique;

    bitmap_t* img;
}tile_t;


tile_t* tile_init()
{
    tile_t* t = calloc(1, sizeof(tile_t));
    t->img = bitmap_init(10, 10);
    return t;
}

void tile_free(void* ptr)
{
    tile_t* t = ptr; 
    bitmap_free(t->img);
    free(t);
}


u32 reverse(u32 a)
{
    u32 result = 0u;

    for (int i = 0; i < 10; ++i)
    {
        u32 bit = (a >> i) & 0x1;
        result |= (bit << (9 - i));
    }

    return result;
}


void tile_flip_x(tile_t* t)
{
    swap_u32(&t->left, &t->right);
    swap_bool(&t->left_unique, &t->right_unique);
    t->top = reverse(t->top);
    t->bottom = reverse(t->bottom);
    bitmap_flip_x(t->img);
}


void tile_flip_y(tile_t* t)
{
    swap_u32(&t->top, &t->bottom);
    swap_bool(&t->top_unique, &t->bottom_unique);
    t->left = reverse(t->left);
    t->right = reverse(t->right);
    bitmap_flip_y(t->img);
}


void tile_flip_diag(tile_t* t)
{
    u32 top = t->top;
    t->top = reverse(t->right);
    t->right = reverse(top);
    u32 bottom = t->bottom;
    t->bottom = reverse(t->left);
    t->left = reverse(bottom);
    bitmap_flip_diag(t->img);

    swap_bool(&t->top_unique, &t->right_unique);
    swap_bool(&t->left_unique, &t->bottom_unique);
}

void tile_rotate(tile_t* t)
{
    u32 top = t->top;
    t->top = t->right;
    t->right = reverse(t->bottom);
    t->bottom = t->left;
    t->left = reverse(top);

    bool top_unique = t->top_unique;
    t->top_unique = t->right_unique;
    t->right_unique = t->bottom_unique;
    t->bottom_unique = t->left_unique;
    t->left_unique = top_unique;

    bitmap_rotate(t->img);
}

bool tile_is_corner(tile_t *t)
{
    return (t->left_unique && t->top_unique)
        || (t->left_unique && t->bottom_unique)
        || (t->right_unique && t->top_unique)
        || (t->right_unique && t->bottom_unique);
}

u32 parse_num(char* str)
{
    u32 result = 0;
    for (int i = 0; i < 10; ++i)
    {
        u32 bit = str[i] == '#' ? 1 : 0; 
        result |= bit << (9 - i);
    }
    
    return result;
}


tile_t* parse_tile(FILE* fp)
{
    char line[64];
    tile_t* t = tile_init();

    fgets(line, 64, fp);
    t->id = atoi(line + 5);
    for (int i = 0; i < 10; ++i)
    {
        fgets(line, 64, fp);
        chomp(line);
        if (i == 0) t->top = parse_num(line);
        else if (i == 9) t->bottom = parse_num(line);
        u32 left_bit = line[0] == '#' ? 1 : 0;
        u32 right_bit = line[9] == '#' ? 1 : 0;
        t->left |= left_bit << (9 - i);
        t->right |= right_bit << (9 - i);
        bitmap_parse_line(t->img, line, i);
    } 

    //printf("Tile %d\n", t->id);
    //bitmap_print(t->img);
    //printf("\n\n");

    fgets(line, 64, fp);

    return t;
}


dynarr_t* read_input(char* file_name)
{
    FILE* fp = fopen(file_name, "r");
    dynarr_t* arr = dynarr_init();

    while(!feof(fp))
    {
        tile_t* t = parse_tile(fp);
        dynarr_push(arr, t);
    }

    //fclose(fp);
    return arr;
}


bool edge_matchable(u32 edge, tile_t* b)
{
    u32 rev = reverse(edge);
    return edge == b->left || edge == b->right || edge == b->top || edge == b->bottom
         || rev == b->left || rev  == b->right || rev  == b->top || rev  == b->bottom;
} 


void mark_unique_edges(dynarr_t* arr)
{
    for (size_t i = 0; i < arr->size; ++i)
    {
        tile_t* a = dynarr_at(tile_t, arr, i);
        a->left_unique = true;
        a->right_unique = true;
        a->top_unique = true;
        a->bottom_unique = true;

        for (size_t j = 0 ; j < arr->size; ++j)
        {
            if (i == j) continue;

            tile_t* b = dynarr_at(tile_t, arr, j);
            if (edge_matchable(a->left, b))
            {
                //printf("Left edge of tile %d matches with tile %d.\n", a->id, b->id);
                a->left_unique = false;
            } 
            if (edge_matchable(a->right, b))
            {
                //printf("Right edge of tile %d matches with tile %d.\n", a->id, b->id);
                a->right_unique = false; 
            }
            if (edge_matchable(a->top, b))
            {
                //printf("Top edge of tile %d matches with tile %d.\n", a->id, b->id);
                a->top_unique = false;
            } 
            if (edge_matchable(a->bottom, b))
            {
                //printf("Bottom edge of tile %d matches with tile %d.\n", a->id, b->id);
                a->bottom_unique = false;
            } 
        }
    }
}


size_t find_first_corner_tile(dynarr_t* arr)
{
    for (size_t i = 0; i < arr->size; ++i)
    {
        tile_t* a = dynarr_at(tile_t, arr, i);
        if (tile_is_corner(a)) return i;
    }

    return 0;
}


void tile_rotate_to_top_left_corner(tile_t* t)
{
    assert(tile_is_corner(t));
    while(!t->left_unique)
    {
        tile_rotate(t);
    }

    if (!t->top_unique)
    {
        tile_flip_y(t);
    }
}


bool edges_can_be_matched(u32 a, u32 b)
{
    return a == b || a == reverse(b);
}  


void tile_align_to_right(tile_t* a, tile_t* b)
{
    while (!edges_can_be_matched(a->right, b->left))
    {
        tile_rotate(b);
    }

    if (a->right != b->left)
    {
        tile_flip_y(b);
    }
}


void tile_align_to_bottom(tile_t* a, tile_t* b)
{
    //printf("Aligning %d and %d\n", a->id, b->id);
    //printf("Bottom: %x\n", a->bottom);
    while (!edges_can_be_matched(a->bottom, b->top))
    {
        tile_rotate(b);
    }

    if (a->bottom != b->top)
    {
        tile_flip_x(b);
        assert(a->bottom == b->top);
    }
    //printf("Top: %x\n", b->top);
    //printf("Right: %x\n", b->right); 
    assert(b->left_unique);
}


size_t puzzle_compute_width(dynarr_t* arr)
{
    return (size_t)(sqrtf((float) arr->size) + 0.1);
}


size_t find_matching_tile_for_edge(dynarr_t* arr, u32 edge, size_t start_idx)
{
    for (size_t i = start_idx; i < arr->size; ++i)
    {
        tile_t* t = dynarr_at(tile_t, arr, i);
        if (edge_matchable(edge, t))
        {
           return i;
        }
    }

    return 0;
} 


size_t solve_puzzle(dynarr_t* arr)
{
    size_t width = puzzle_compute_width(arr);
    tile_t* current_tile;

    for (size_t y = 0; y < width; ++y)
    {
        if (y == 0) 
        {
            size_t first_corner_idx = find_first_corner_tile(arr);
            tile_t* first_corner_tile = dynarr_at(tile_t, arr, first_corner_idx);
            tile_rotate_to_top_left_corner(first_corner_tile);
            dynarr_swap(arr, 0, first_corner_idx);
            current_tile = first_corner_tile;
            //printf("Top left corner: %d\n", first_corner_tile->id);
        }
        for (size_t x = 0; x < width; ++x)
        {
            size_t idx = y * width + x;
            //printf("Index: %lu\n", idx);
            
            if (x == 0)
            {
                if (y > 0)
                {
                    tile_t* top_tile = dynarr_at(tile_t, arr, idx - width);
                    size_t next_tile_idx = find_matching_tile_for_edge(arr, top_tile->bottom, idx);
                    assert(next_tile_idx);
                    tile_t* next_tile = dynarr_at(tile_t, arr, next_tile_idx);
                    //printf("Next tile: %d\n", next_tile->id);
                    tile_align_to_bottom(top_tile, next_tile);
                    dynarr_swap(arr, next_tile_idx, idx);
                    current_tile = next_tile;
                } 
            }
            else
            {
                size_t next_tile_idx = find_matching_tile_for_edge(arr, current_tile->right, idx);
                //printf("Current Tile: %d  \n", current_tile->id);
                //printf("Right Edge: %x \n", current_tile->right);
                assert(next_tile_idx);
                tile_t* next_tile = dynarr_at(tile_t, arr, next_tile_idx);
                //printf("Next tile: %d\n", next_tile->id);
                tile_align_to_right(current_tile, next_tile);
                dynarr_swap(arr, next_tile_idx, idx);
                current_tile = next_tile;
            }
        }
    }

    size_t result = dynarr_at(tile_t, arr, 0)->id;
    result *= dynarr_at(tile_t, arr, width - 1)->id;
    result *= dynarr_at(tile_t, arr, width * (width - 1))->id;
    result *= dynarr_at(tile_t, arr, width * width - 1)->id;

    return result;
}


bitmap_t* construct_final_bitmap(dynarr_t* tiles)
{
    size_t puzzle_width = puzzle_compute_width(tiles);
    size_t bitmap_width = puzzle_width * 8;

    bitmap_t* b = bitmap_init(bitmap_width, bitmap_width);

    for (size_t row = 0; row < puzzle_width; row++)
    {
        for (size_t col = 0; col < puzzle_width; col++)
        {
            size_t tile_idx = row * puzzle_width + col;
            tile_t* tile = dynarr_at(tile_t, tiles, tile_idx);
            //printf("Tile %d\n", tile->id);
            //bitmap_print(tile->img);
            //printf("\n\n"); 
            rect_t src_rect = { 1, 1, 9, 9 };
            rect_t dst_rect = { col * 8, row * 8, (col + 1) * 8, (row + 1) * 8 };
            bitmap_blit(b, dst_rect, tile->img, src_rect);
        }
    }

    return b;
}    


size_t count_monsters(bitmap_t* b, bitmap_t* monster)
{
    size_t num_monster_pixels = bitmap_count_active(monster);
    size_t result = 0;
    
    for (size_t y = 0; y < b->h - monster->h + 1; y++)
    {
        for (size_t x = 0; x < b->w - monster->w + 1; x++)
        {
            size_t activation = bitmap_apply_filter(b, monster, x, y);
            result += (activation == num_monster_pixels);
        }
    }

    return result;
} 

size_t find_final_orientation(bitmap_t* b)
{
    bitmap_t* monster = bitmap_parse_with_size(monster_str, 20, 3);
    size_t monster_count = 0; 
    size_t num_rotations = 0;

    while ((monster_count = count_monsters(b, monster)) == 0)
    {
        if (num_rotations < 4)
        {
            printf("Rotating final image... \n");
            bitmap_rotate(b);
            num_rotations++;
        }
        else if (num_rotations == 4)
        {
            printf("Flipping final image... \n");
            bitmap_flip_x(b);
            num_rotations = 0;
        }
    }

    printf("Found %lu monsters.\n", monster_count);
    bitmap_free(monster);

    return monster_count;
}


size_t solve_part_two(char* file_name)
{
    dynarr_t* arr = read_input(file_name);
    mark_unique_edges(arr);
    solve_puzzle(arr);
    bitmap_t* final = construct_final_bitmap(arr);
    size_t num_monsters = find_final_orientation(final);
    size_t num_pixels = bitmap_count_active(final);

    dynarr_free(arr, tile_free);
    bitmap_free(final);
    return num_pixels - num_monsters * 15u;
}


void setUp()
{
}


void tearDown()
{
}

void test_reverse()
{
    TEST_ASSERT_EQUAL(0b0110001000, reverse(0b0001000110));
}

void test_read_input()
{
    dynarr_t* arr = read_input("day20_test.txt");
    TEST_ASSERT_EQUAL(9, arr->size);
    tile_t* t = dynarr_at(tile_t, arr, 0);
    TEST_ASSERT_EQUAL(2311, t->id); 
    TEST_ASSERT_EQUAL(0b0011010010, t->top);
    TEST_ASSERT_EQUAL(0b0011100111, t->bottom);
    TEST_ASSERT_EQUAL(0b0111110010, t->left);
    TEST_ASSERT_EQUAL(0b0001011001, t->right);
    TEST_ASSERT_EQUAL(3, puzzle_compute_width(arr));
    dynarr_free(arr, tile_free);
}

void test_mark_unique_edges()
{
    dynarr_t* arr = read_input("day20_test.txt");
    mark_unique_edges(arr);

    tile_t* t = dynarr_at(tile_t, arr, 0);
    TEST_ASSERT_EQUAL(2311, t->id); 
    TEST_ASSERT_TRUE(t->bottom_unique);
    TEST_ASSERT_FALSE(t->top_unique);
    TEST_ASSERT_FALSE(t->left_unique);
    TEST_ASSERT_FALSE(t->right_unique);

    tile_t* t2 = dynarr_at(tile_t, arr, 1);
    TEST_ASSERT_EQUAL(1951, t2->id);
    TEST_ASSERT_TRUE(t2->bottom_unique);
    TEST_ASSERT_TRUE(t2->left_unique);
    TEST_ASSERT_FALSE(t2->right_unique);
    TEST_ASSERT_FALSE(t2->top_unique);

    dynarr_free(arr, tile_free);
}

void test_find_first_corner_tile()
{
    dynarr_t* arr = read_input("day20_test.txt");
    mark_unique_edges(arr);

    size_t idx = find_first_corner_tile(arr);
    tile_t* t2 = dynarr_at(tile_t, arr, idx);
    TEST_ASSERT_EQUAL(1951, t2->id);
    TEST_ASSERT_TRUE(t2->bottom_unique);
    TEST_ASSERT_TRUE(t2->left_unique);
    TEST_ASSERT_FALSE(t2->right_unique);
    TEST_ASSERT_FALSE(t2->top_unique);

    dynarr_free(arr, tile_free);
}

void test_transform()
{
    dynarr_t* arr = read_input("day20_test.txt");
    tile_t* t = dynarr_at(tile_t, arr, 0);
    TEST_ASSERT_EQUAL(2311, t->id); 

    tile_flip_x(t);

    TEST_ASSERT_EQUAL(0b0100101100, t->top);
    TEST_ASSERT_EQUAL(0b1110011100,  t->bottom);
    TEST_ASSERT_EQUAL(0b0111110010, t->right);
    TEST_ASSERT_EQUAL(0b0001011001, t->left);

    tile_flip_y(t);
    TEST_ASSERT_EQUAL(0b0100101100, t->bottom);
    TEST_ASSERT_EQUAL(0b1110011100, t->top);
    TEST_ASSERT_EQUAL(0b0100111110, t->right);
    TEST_ASSERT_EQUAL(0b1001101000, t->left);

    tile_rotate(t);
    TEST_ASSERT_EQUAL(reverse(0b0100101100), t->right);
    TEST_ASSERT_EQUAL(reverse(0b1110011100), t->left);
    TEST_ASSERT_EQUAL(0b0100111110, t->top);
    TEST_ASSERT_EQUAL(0b1001101000, t->bottom);

    dynarr_free(arr, tile_free);
}


void test_puzzle_start()
{
    dynarr_t* arr = read_input("day20_test.txt");
    mark_unique_edges(arr);
    
    size_t first_corner_idx = find_first_corner_tile(arr);
    tile_t* first_corner_tile = dynarr_at(tile_t, arr, first_corner_idx);
    tile_rotate_to_top_left_corner(first_corner_tile);
    dynarr_swap(arr, first_corner_idx, 0);
    
    tile_t* t = dynarr_at(tile_t, arr, 0);
    TEST_ASSERT_TRUE(t->left_unique);
    TEST_ASSERT_TRUE(t->top_unique);
    TEST_ASSERT_FALSE(t->right_unique);
    TEST_ASSERT_FALSE(t->bottom_unique);
     
    dynarr_free(arr, tile_free);
}


void test_solve_puzzle()
{
    dynarr_t* arr = read_input("day20_test.txt");
    mark_unique_edges(arr);
    size_t result = solve_puzzle(arr);
    TEST_ASSERT_EQUAL(20899048083289, result);
    
    dynarr_free(arr, tile_free);
}


void test_construct_final_bitmap()
{
    dynarr_t* arr = read_input("day20_test.txt");
    mark_unique_edges(arr);
    solve_puzzle(arr);
    bitmap_t* final = construct_final_bitmap(arr);
    bitmap_print(final);
    TEST_ASSERT(bitmap_equal_str(final, final_image_str));
    dynarr_free(arr, tile_free);
}

void test_find_monsters()
{
    dynarr_t* arr = read_input("day20_test.txt");
    mark_unique_edges(arr);
    solve_puzzle(arr);
    bitmap_t* final = construct_final_bitmap(arr);
    bitmap_rotate(final);
    bitmap_flip_y(final);
    bitmap_t* monster = bitmap_parse_with_size(monster_str, 20, 3);
    TEST_ASSERT_EQUAL(20, monster->w);
    TEST_ASSERT_EQUAL(3, monster->h);
    TEST_ASSERT_EQUAL(15, bitmap_count_active(monster));
    size_t monster_count = count_monsters(final, monster);
    TEST_ASSERT_EQUAL(1, count_monsters(monster, monster));
    TEST_ASSERT_EQUAL(2, monster_count);
    dynarr_free(arr, tile_free);
    bitmap_free(final);
    bitmap_free(monster);
}

void test_auto_find_monsters()
{
    dynarr_t* arr = read_input("day20_test.txt");
    mark_unique_edges(arr);
    solve_puzzle(arr);
    bitmap_t* final = construct_final_bitmap(arr);
    size_t monster_count = find_final_orientation(final);
    TEST_ASSERT_EQUAL(2, monster_count);
    dynarr_free(arr, tile_free);
    bitmap_free(final);
}

void test_solve_part_two()
{
    size_t result = solve_part_two("day20_test.txt");
    TEST_ASSERT_EQUAL(273, result);
}


int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_reverse);
    RUN_TEST(test_read_input);
    RUN_TEST(test_mark_unique_edges);
    RUN_TEST(test_find_first_corner_tile);
    RUN_TEST(test_transform);
    RUN_TEST(test_puzzle_start);
    RUN_TEST(test_solve_puzzle);
    RUN_TEST(test_construct_final_bitmap);
    RUN_TEST(test_find_monsters);
    RUN_TEST(test_auto_find_monsters);
    RUN_TEST(test_solve_part_two);
    UNITY_END();

#if 0
    dynarr_t* arr = read_input("day20.txt");
    mark_unique_edges(arr);
    size_t result = solve_puzzle(arr);
    printf("Result part one: %lu\n", result); 
#endif

    printf("Solution Part Two: %lu\n", solve_part_two("day20.txt"));


    return 0;
}

