#include <stdio.h>
#include "unity.h"
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "util.h"
#include "hash_map_int.h"

void setUp()
{
}


void tearDown()
{
}

void parse_mask(const char* mask_string, size_t* and_mask, size_t* or_mask, size_t* floating_mask)
{
    assert(strstr(mask_string, "mask = "));

    const char* pos = mask_string + 7;

    *and_mask = 0;
    *or_mask = 0;
    *floating_mask = 0;
    size_t one = 1;
    const int mask_length = 36;
    
    for (int i = 0; i < mask_length; ++i)
    {
        const int shift = mask_length - i - 1;
        if (*pos == '1')
        {
            *or_mask |= (one << shift);
        }
        else if (*pos == '0')
        {
            *and_mask |= (one << shift);
        }
        else
        {
            assert(*pos == 'X');
            *floating_mask |= (one << shift);
        }

        pos++;
    }

    *and_mask = ~*and_mask;
}

void parse_mem(const char* mem_string, size_t* address, size_t* value)
{
    assert(strstr(mem_string, "mem["));
    const char* pos = mem_string + 4;

    *address = atoi(pos);

    pos = strstr(mem_string, "]") + 4;

    *value = atoi(pos);
}

typedef void(*TMemOpcodeCallback)(HashMap_int*, size_t, size_t, size_t, size_t, size_t);

HashMap_int* process_input(const char* file_name, TMemOpcodeCallback callback) 
{
    FILE* fp = fopen(file_name, "r");
    int line_size = 512;
    char* line = malloc(line_size); 
    size_t and_mask = 0;
    size_t or_mask = 0;
    size_t address = 0;
    size_t floating_mask = 0;
    size_t value = 0;
    //HashMap_int* mem = hash_map_init_int(1048576);
    HashMap_int* mem = hash_map_init_int(65535 << 2);

    while (!feof(fp))
    {
        if (fgets(line, line_size, fp))
        {
            chomp(line);
            if (strstr(line, "mask"))
            {
                parse_mask(line, &and_mask, &or_mask, &floating_mask);
            }
            else if (strstr(line, "mem"))
            {
                parse_mem(line, &address, &value);
                callback(mem, address, value, and_mask, or_mask, floating_mask);
            }

        }    
    }

    free(line);

    printf("Hash capacity: %f\n", (float)mem->size / mem->capacity * 100.0f);
    printf("Hash depth: %lu\n", mem->max_depth);
    return mem;
}

void update_address(HashMap_int* mem, size_t address, size_t value)
{
    if (hash_map_has_key_int(mem, address))
    {
        hash_map_update_value_int(mem, address, (void*) value);
    }
    else
    {
        hash_map_insert_int(mem, address, (void*) value);
    }
}


void part_one_mem_callback(
    HashMap_int* mem,
    size_t address,
    size_t value,
    size_t and_mask,
    size_t or_mask,
    size_t floating_mask)
{
    value &= and_mask;
    value |= or_mask;
    update_address(mem, address, value);
}


int count_num_set_bits(size_t value)
{
    int result = 0;

    for (int i = 0; i < 64; i++)
    {
        result += value & 0x1;
        value = value >> 1;
    }

    return result;
}


int* build_permutation_map(size_t mask)
{
    int* map = calloc(64, sizeof(int));
    int idx = 0;
    size_t shifted_mask = mask;

    for (int i = 0; i < 64; ++i)
    {
        if (shifted_mask & 0x1)
        {
            map[idx] = i;
            idx++;
        }

        shifted_mask = shifted_mask >> 1;    
    }

    assert(idx == count_num_set_bits(mask));
    return map;
}


size_t permutate_mask(int* permutation_map, int num_set_bits, int index)
{
    size_t mask = 0ull;

    for (int i = 0; i < num_set_bits; ++i)
    {
        if (index & (0x1ull << i))
        {
            const int bit_position_in_mask = permutation_map[i];
            mask |= (1ull << bit_position_in_mask);
        }
    }

    return mask;
}


void part_two_mem_callback(
    HashMap_int* mem,
    size_t address,
    size_t value,
    size_t and_mask,
    size_t or_mask,
    size_t floating_mask)
{
    address |= or_mask;
    address &= (~floating_mask);
    const int num_floating_bits = count_num_set_bits(floating_mask);
    const int num_permutations = 1ull << num_floating_bits;
    const int num_set_bits = count_num_set_bits(floating_mask);
    int* permutation_map = build_permutation_map(floating_mask);

    for (int i = 0; i < num_permutations; ++i)
    {
        size_t current_mask = permutate_mask(permutation_map, num_set_bits, i);
        size_t current_address = address | current_mask;
        update_address(mem, current_address, value);
    }

    free(permutation_map);

}


HashMap_int* process_part1(const char* file_name)
{
    return process_input(file_name, part_one_mem_callback);
}

HashMap_int* process_part2(const char* file_name)
{
    return process_input(file_name, part_two_mem_callback);
}

long long compute_result(const HashMap_int* mem)
{
    HashMapIterator_int* iter = hash_map_iterator_begin_int(mem);
    size_t result = 0;
    while (!hash_map_iterator_is_end_int(iter))
    {
        result += (size_t) hash_map_iterator_get_value_int(iter);
        hash_map_iterator_next_int(iter);
    }

    free(iter);

    return result;
}

long long evaluate_part1(const char* file_name)
{
    HashMap_int* mem = process_part1(file_name);
    return compute_result(mem);
}

long long evaluate_part2(const char* file_name)
{
    HashMap_int* mem = process_part2(file_name);
    return compute_result(mem);
}

void test_parse_mem()
{
    size_t address;
    size_t value;
    parse_mem("mem[8] = 11", &address, &value);
    TEST_ASSERT_EQUAL(8, address);
    TEST_ASSERT_EQUAL(11, value);

    parse_mem("mem[12344566] = 8989736", &address, &value);
    TEST_ASSERT_EQUAL(12344566, address);
    TEST_ASSERT_EQUAL(8989736, value);
}


void test_parse_mask()
{
    size_t and_mask;
    size_t or_mask;
    size_t floating_mask;
    parse_mask("mask = XXXXXXXXXXXXXXXXXXXXXXXXXXXXX1XXXX0X", &and_mask, &or_mask, &floating_mask);
    TEST_ASSERT_EQUAL(0xfffffffffffffffd, and_mask);
    TEST_ASSERT_EQUAL(0x0000000000000040, or_mask);

    parse_mask("mask = 1XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", &and_mask, &or_mask, &floating_mask);
    TEST_ASSERT_EQUAL(0xffffffffffffffff, and_mask);
    TEST_ASSERT_EQUAL(0b100000000000000000000000000000000000, or_mask);

    parse_mask("mask = 000000000000000000000000000000000X00", &and_mask, &or_mask, &floating_mask);
    TEST_ASSERT_EQUAL(0x4, floating_mask);

    parse_mask("mask = 0000000000000000000000000000XXXX0X00", &and_mask, &or_mask, &floating_mask);
    TEST_ASSERT_EQUAL(0xf4, floating_mask);
}


void test_count_num_set_bits()
{
    size_t mask = 0xf4;
    TEST_ASSERT_EQUAL(5, count_num_set_bits(mask));
}


void test_build_permutation_map()
{
    size_t mask = 0xf4;
    int* map = build_permutation_map(mask);

    TEST_ASSERT_EQUAL(2, map[0]);
    TEST_ASSERT_EQUAL(4, map[1]);
    TEST_ASSERT_EQUAL(5, map[2]);
    TEST_ASSERT_EQUAL(6, map[3]);
    TEST_ASSERT_EQUAL(7, map[4]);
    TEST_ASSERT_EQUAL(0, map[5]);

    free(map);
}

void test_permutate_mask()
{
    size_t mask = 0x1001001;
    int* map = build_permutation_map(mask);
    const int num_set_bits = count_num_set_bits(mask);
    TEST_ASSERT_EQUAL(0x0, permutate_mask(map, num_set_bits, 0));
    TEST_ASSERT_EQUAL(0x1, permutate_mask(map, num_set_bits, 1));
    TEST_ASSERT_EQUAL(0x1000, permutate_mask(map, num_set_bits, 2));
    TEST_ASSERT_EQUAL(0x1001, permutate_mask(map, num_set_bits, 3));
    TEST_ASSERT_EQUAL(0x1000000, permutate_mask(map, num_set_bits, 4));
    TEST_ASSERT_EQUAL(0x1000001, permutate_mask(map, num_set_bits, 5));
    TEST_ASSERT_EQUAL(0x1001000, permutate_mask(map, num_set_bits, 6));
    TEST_ASSERT_EQUAL(0x1001001, permutate_mask(map, num_set_bits, 7));
    free(map);
}

void test_evaluate_part1()
{
    TEST_ASSERT_EQUAL(165, evaluate_part1("day14_test.txt"));
}

void test_evaluate_part2()
{
    TEST_ASSERT_EQUAL(208, evaluate_part2("day14_test2.txt"));
}


int main(int argc, char** argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_parse_mem);
    RUN_TEST(test_parse_mask);
    RUN_TEST(test_evaluate_part1);
    RUN_TEST(test_count_num_set_bits);
    RUN_TEST(test_build_permutation_map);
    RUN_TEST(test_permutate_mask);
    RUN_TEST(test_evaluate_part2);
    UNITY_END();

    long long result1 = evaluate_part1("day14.txt");
    printf("Result Part 1: %lld\n", result1);

    long long result2 = evaluate_part2("day14.txt");
    printf("Result Part 2: %lld\n", result2);

    return 0;
}

