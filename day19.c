#include <stdio.h>
#include "unity.h"
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "util.h"
#include "hash_map.h"
#include "ctype.h"
#include "strarr.h"

void setUp()
{
}


void tearDown()
{
}



typedef struct
{
    size_t size;
    size_t alloc_size;
    int* data;
}intarr_t;


intarr_t* intarr_init(size_t initial_alloc_size)
{
    intarr_t* arr = calloc(1, sizeof(intarr_t));
    arr->alloc_size = initial_alloc_size;
    arr->data = malloc(initial_alloc_size * sizeof(int));
    return arr;
}    

void intarr_free(intarr_t* arr)
{
    free(arr->data);
    free(arr);
}


void intarr_push(intarr_t* arr, int value)
{
    if (arr->size == arr->alloc_size)
    {
        arr->alloc_size *= 2;
        arr->data = realloc(arr->data, arr->alloc_size * sizeof(int));
    }

    arr->data[arr->size++] = value;
}

void intarr_clear(intarr_t* arr)
{
    arr->size = 0;
}


int* intarr_at(intarr_t* arr, size_t index)
{
    assert(index < arr->size);
    return &arr->data[index];
}


void intarr_print(intarr_t* arr)
{
    for (size_t i = 0; i < arr->size; ++i)
    {
        printf("%d ", *intarr_at(arr, i));
    }

    printf("\n");
}

typedef struct s_automaton_t
{
    size_t num_sub_nodes;
    int id;
    struct s_automaton_t* sub_nodes[2];
    struct s_automaton_t* parent;
    struct s_automaton_t* next_node;
    char match_char;
} automaton_t;


automaton_t* automaton_init()
{
    automaton_t* a = calloc(1, sizeof(automaton_t));
    return a;
}


char* automaton_match_char(automaton_t* a, char* str)
{
    return str[0] == a->match_char ? str + 1 : NULL;
}

char* automaton_check_string(automaton_t* a, char* str);


strarr_t* automaton_process_next(automaton_t* a, strarr_t* input)
{
    strarr_t* output = strarr_init();
    for (size_t i = 0; i < input->size; i++)
    {
        char* result = automaton_check_string(a->next_node, input->data[i]);
        if (result) strarr_push(output, result);
    } 

    strarr_free(input);
        
    return output;
}


char* automaton_check_string(automaton_t* a, char* str)
{
    strarr_t* results = strarr_init();
    strarr_free(results); 
    results = strarr_init();

    if (str == NULL)
    {
    }
    else if (a->match_char)
    {
        char* pos = automaton_match_char(a, str);
        if (pos) strarr_push(results, pos);
    }
    else
    {
        for (size_t i = 0; i < a->num_sub_nodes ; i++)
        {
            char* result = automaton_check_string(a->sub_nodes[i], str);
            if (result) strarr_push(results, result);
        }
    }

    if (a->next_node)
    {
        results = automaton_process_next(a, results);
    }

    char* result_str = NULL;

    for (size_t i = 0; i < results->size; i++)
    {
        if (results->data[i])
        {
            result_str = results->data[i];
            break;
        }
    }

    strarr_free(results);
    return result_str;
}

char* skip_whitespace(char* pos)
{
    if (*pos == 0) return NULL;
    while (isspace(*pos)) pos++;
    return (*pos == '\0') ? NULL : pos;
}

typedef struct 
{
    char match_char;
    int id;
    size_t num_sub_rules;
    intarr_t* sub_rules[0];
}rule_t;


rule_t* rule_init(char* str)
{
    rule_t* r = calloc(1, sizeof(rule_t));
    char* pos = str;

    r->id = atoi(pos);
    pos = strstr(pos, ": ");
    pos++;
     
    while (pos && *pos)
    {
        //if (pos) printf("%c", *pos);
        pos = skip_whitespace(pos);

        if (pos && isdigit(*pos))
        {
            //printf("%c ", *pos);
            int rule = strtol(pos, (char**)&pos, 10);
            if (r->num_sub_rules == 0)
            {
                r->sub_rules[r->num_sub_rules++] = intarr_init(8);
            }         

            intarr_push(r->sub_rules[r->num_sub_rules - 1], rule);
        }
        else if (pos && *pos == '|')
        {
            //printf("%c ", *pos);
            r->sub_rules[r->num_sub_rules++] = intarr_init(8);
            pos++;
        }
        else if (strchr(pos, '"'))
        {
            r->match_char = pos[1];
            assert(r->match_char == 'a' || r->match_char == 'b');
            break;
        }
    }

    return r;
}

void rule_free(rule_t* r)
{
    for (size_t i = 0; i < r->num_sub_rules; ++i)
    {
        intarr_free(r->sub_rules[i]);
    }
    free(r);
}


typedef struct 
{
    rule_t* rules[256];
    size_t num_rules;
    strarr_t* input;
} problem_t;


problem_t* problem_init()
{
    problem_t* p = calloc(1, sizeof(problem_t));
    p->input = strarr_init(); 
    return p;
}


void problem_free(problem_t* p)
{
    for (size_t i = 0; i < 256; ++i)
    {
        if (p->rules[i])
        {
            rule_free(p->rules[i]);
        }
    }

    strarr_free(p->input);
    free(p);
}


void problem_add_rule(problem_t* p, rule_t* rule)
{
    //printf("Adding rule: %d\n", rule->id);
    p->rules[rule->id] = rule;
    p->num_rules++; 
}


problem_t* parse_input(const char* file_name)
{
    FILE* fp = fopen(file_name, "r");
    char line[512];
    problem_t* p = problem_init(); 
    while (!feof(fp))
    {
        if (fgets(line, 512, fp))
        {
            chomp(line);
            if (strstr(line, ": "))
            {
                //printf("Parsing rule: %s\n", line);
                rule_t* r = rule_init(line);
                problem_add_rule(p, r);
            }
            else if (strlen(line))
            {
                //printf("Pushing input: %s\n", line);
                strarr_push(p->input, line);
            }
        }
    }

    return p;
}

size_t automaton_depth(automaton_t* a)
{
    size_t result = 1;

    while (a->parent && a->parent->id != 0)
    {
        result += 1;
        a = a->parent;
    }
    
    printf("automaton depth %d\n", (int)result);
    return result;
}


automaton_t* compile_regex(problem_t* p, rule_t* rule, automaton_t* parent)
{
    automaton_t* a = automaton_init();
    a->parent = parent;
    a->id = rule->id;
    if (rule->match_char)
    {
        a->match_char = rule->match_char;
    }
    else
    {
        a->num_sub_nodes = rule->num_sub_rules;
        for (size_t sub_match = 0; sub_match < rule->num_sub_rules; ++sub_match)
        {
            automaton_t** ptr = &a->sub_nodes[sub_match]; 

            for (size_t chain_idx = 0; chain_idx < rule->sub_rules[sub_match]->size; ++chain_idx)
            {
                size_t next_rule_id = *intarr_at(rule->sub_rules[sub_match], chain_idx);
                if (next_rule_id == 8 && a->id == 8)
                {
                    if (a->parent->id == 0)
                    {
                        *ptr = compile_regex(p, p->rules[8], a);
                    }
                    else
                    {
                        *ptr = a;
                    }
                }
                else if (next_rule_id == 11 && a->id == 11)
                {
                    if (a->parent->id == 0)
                    {
                        *ptr = compile_regex(p, p->rules[11], a);
                    }
                    else
                    {
                        *ptr = a;
                    }
                }
                else
                {
                    rule_t* next_rule = p->rules[next_rule_id]; 
                    *ptr = compile_regex(p, next_rule, a);
                }
                ptr = &((*ptr)->next_node);
            }
        }
    }

    return a;
} 


bool check_string(automaton_t* a, char* str)
{
    char* result = automaton_check_string(a, str);
    return result != NULL && *result == 0;
}


size_t count_valid_strings(problem_t* p)
{
    automaton_t* a = compile_regex(p, p->rules[0], NULL);

    size_t result = 0;

    for (size_t i = 0; i < p->input->size; ++i)
    {
        result += check_string(a, p->input->data[i]);
    }

    return result;
}


void update_problem_part_two(problem_t* p)
{
    rule_t* rule8 = p->rules[8];
    rule8->num_sub_rules = 2;
    rule8->sub_rules[1] = intarr_init(8);
    intarr_push(rule8->sub_rules[1], 42);
    intarr_push(rule8->sub_rules[1], 8);

    rule_t* rule11 = p->rules[11];
    rule11->num_sub_rules = 2;
    rule11->sub_rules[1] = intarr_init(8);
    intarr_push(rule11->sub_rules[1], 42);
    intarr_push(rule11->sub_rules[1], 11);
    intarr_push(rule11->sub_rules[1], 31);
}


char* copy_string(const char* str)
{
    char* copy = malloc(strlen(str) + 1);
    strcpy(copy, str);
    return copy;
}

size_t replace_char(char* str, char find, char repl)
{
    char* pos;
    size_t count = 0;
    while((pos = strchr(str, find)))
    {
        count++;
        *pos = repl;
    }   

    return count;
} 


void test_skip_whitespace()
{
    char* pos;
    TEST_ASSERT_EQUAL(NULL, pos = skip_whitespace(""));
    TEST_ASSERT_EQUAL(NULL, pos = skip_whitespace("   "));
    TEST_ASSERT_EQUAL('a', *(pos = skip_whitespace(" a")));
}



void test_parse_input()
{
    problem_t* p = parse_input("day19_test.txt");
    TEST_ASSERT_EQUAL(6, p->num_rules);
    TEST_ASSERT_EQUAL(5, p->input->size);

    rule_t* r = p->rules[0];
    TEST_ASSERT_EQUAL(0, r->id);
    TEST_ASSERT_EQUAL(0, r->match_char);
    TEST_ASSERT_EQUAL(1, r->num_sub_rules);
    TEST_ASSERT_EQUAL(3, r->sub_rules[0]->size);
    TEST_ASSERT_EQUAL(4, r->sub_rules[0]->data[0]);
    TEST_ASSERT_EQUAL(1, r->sub_rules[0]->data[1]);
    TEST_ASSERT_EQUAL(5, r->sub_rules[0]->data[2]);

    r = p->rules[1];
    TEST_ASSERT_EQUAL(1, r->id);
    TEST_ASSERT_EQUAL(0, r->match_char);
    TEST_ASSERT_EQUAL(2, r->num_sub_rules);
    TEST_ASSERT_EQUAL(2, r->sub_rules[0]->size);
    TEST_ASSERT_EQUAL(2, r->sub_rules[0]->data[0]);
    TEST_ASSERT_EQUAL(3, r->sub_rules[0]->data[1]);
    TEST_ASSERT_EQUAL(2, r->sub_rules[1]->size);
    TEST_ASSERT_EQUAL(3, r->sub_rules[1]->data[0]);
    TEST_ASSERT_EQUAL(2, r->sub_rules[1]->data[1]);

    r = p->rules[2];
    TEST_ASSERT_EQUAL(2, r->id);
    TEST_ASSERT_EQUAL(0, r->match_char);
    TEST_ASSERT_EQUAL(2, r->num_sub_rules);
    TEST_ASSERT_EQUAL(2, r->sub_rules[0]->size);
    TEST_ASSERT_EQUAL(4, r->sub_rules[0]->data[0]);
    TEST_ASSERT_EQUAL(4, r->sub_rules[0]->data[1]);
    TEST_ASSERT_EQUAL(2, r->sub_rules[1]->size);
    TEST_ASSERT_EQUAL(5, r->sub_rules[1]->data[0]);
    TEST_ASSERT_EQUAL(5, r->sub_rules[1]->data[1]);

    r = p->rules[3];
    TEST_ASSERT_EQUAL(3, r->id);
    TEST_ASSERT_EQUAL(0, r->match_char);
    TEST_ASSERT_EQUAL(2, r->num_sub_rules);
    TEST_ASSERT_EQUAL(2, r->sub_rules[0]->size);
    TEST_ASSERT_EQUAL(4, r->sub_rules[0]->data[0]);
    TEST_ASSERT_EQUAL(5, r->sub_rules[0]->data[1]);
    TEST_ASSERT_EQUAL(2, r->sub_rules[1]->size);
    TEST_ASSERT_EQUAL(5, r->sub_rules[1]->data[0]);
    TEST_ASSERT_EQUAL(4, r->sub_rules[1]->data[1]);

    r = p->rules[4];
    TEST_ASSERT_EQUAL(4, r->id);
    TEST_ASSERT_EQUAL('a', r->match_char);
    TEST_ASSERT_EQUAL(0, r->num_sub_rules);

    r = p->rules[5];
    TEST_ASSERT_EQUAL(5, r->id);
    TEST_ASSERT_EQUAL('b', r->match_char);
    TEST_ASSERT_EQUAL(0, r->num_sub_rules);
    //

    TEST_ASSERT_EQUAL_STRING("ababbb", p->input->data[0]);


    //problem_free(p);
}

void test_compile_regex()
{
    problem_t* p = parse_input("day19_test.txt");
    automaton_t* a = compile_regex(p, p->rules[0], NULL);
    TEST_ASSERT_EQUAL(a->id, 0);
    TEST_ASSERT_EQUAL(1, a->num_sub_nodes);
    TEST_ASSERT_EQUAL(4, a->sub_nodes[0]->id);
    TEST_ASSERT_EQUAL(1, a->sub_nodes[0]->next_node->id);
    TEST_ASSERT_EQUAL(5, a->sub_nodes[0]->next_node->next_node->id);

    p = parse_input("day19_test2.txt");
    update_problem_part_two(p);
    a = compile_regex(p, p->rules[0], NULL);
    TEST_ASSERT_EQUAL(a->id, 0);
    TEST_ASSERT_EQUAL(11, a->sub_nodes[0]->next_node->id);
    automaton_t* first_node11 = a->sub_nodes[0]->next_node;
    TEST_ASSERT_EQUAL(NULL, first_node11->next_node);
    automaton_t* second_node11 = first_node11->sub_nodes[1]->next_node;
    TEST_ASSERT_EQUAL(11, second_node11->id);
    TEST_ASSERT_EQUAL(31, second_node11->next_node->id);
    TEST_ASSERT_EQUAL(second_node11, second_node11->sub_nodes[1]->next_node);
}

void test_check_string()
{
    problem_t* p = parse_input("day19_test.txt");
    automaton_t* a = compile_regex(p, p->rules[0], NULL);
    TEST_ASSERT_TRUE(check_string(a, p->input->data[0]));
    TEST_ASSERT_FALSE(check_string(a, p->input->data[1]));
    TEST_ASSERT_TRUE(check_string(a, p->input->data[2]));
    TEST_ASSERT_FALSE(check_string(a, p->input->data[3]));
    TEST_ASSERT_FALSE(check_string(a, p->input->data[4]));
}

void test_check_string_part_two()
{
    problem_t* p = parse_input("day19_test2.txt");
    update_problem_part_two(p);
    automaton_t* a = compile_regex(p, p->rules[0], NULL);
    //TEST_ASSERT_TRUE(check_string(a, "bbabbbbaabaabba"));
    //TEST_ASSERT_TRUE(check_string(a, "babbbbaabbbbbabbbbbbaabaaabaaa"));
    //TEST_ASSERT_TRUE(check_string(a, "aaabbbbbbaaaabaababaabababbabaaabbababababaaa"));
    printf("==============================================================\n");
    TEST_ASSERT_TRUE(check_string(a, "bbbbbbbaaaabbbbaaabbabaaa"));
    //TEST_ASSERT_TRUE(check_string(a, "bbbababbbbaaaaaaaabbababaaababaabab"));
    //TEST_ASSERT_TRUE(check_string(a, "ababaaaaaabaaab"));
    //TEST_ASSERT_TRUE(check_string(a, "ababaaaaabbbaba"));
    //TEST_ASSERT_TRUE(check_string(a, "baabbaaaabbaaaababbaababb"));
    //TEST_ASSERT_TRUE(check_string(a, "abbbbabbbbaaaababbbbbbaaaababb"));
    //TEST_ASSERT_TRUE(check_string(a, "aaaaabbaabaaaaababaa"));
    //TEST_ASSERT_TRUE(check_string(a, "aaaabbaabbaaaaaaabbbabbbaaabbaabaaa"));
    //TEST_ASSERT_TRUE(check_string(a, "aabbbbbaabbbaaaaaabbbbbababaaaaabbaaabba"));
}

void test_count_valid_strings_part_one()
{
    problem_t* p = parse_input("day19_test.txt");
    TEST_ASSERT_EQUAL(2, count_valid_strings(p));

    p = parse_input("day19_test2.txt");
    TEST_ASSERT_EQUAL(3, count_valid_strings(p));
}

void test_count_valid_strings_part_two()
{

    problem_t* p = parse_input("day19_test2.txt");
    update_problem_part_two(p);
    TEST_ASSERT_EQUAL(12, count_valid_strings(p));
}

void test_strarr()
{
    strarr_t* arr = strarr_init();
    strarr_free(arr);
}


int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_strarr);
   // RUN_TEST(test_skip_whitespace);
  //  RUN_TEST(test_parse_input);
  //  RUN_TEST(test_compile_regex);
    RUN_TEST(test_check_string);
    RUN_TEST(test_check_string_part_two);
  //  RUN_TEST(test_count_valid_strings_part_one);
  //  RUN_TEST(test_count_valid_strings_part_two);
    UNITY_END();

    problem_t* p = parse_input("day19.txt");
    update_problem_part_two(p);
    size_t result_part_one = count_valid_strings(p);
    printf("Result Part 2: %lu\n", result_part_one);


    return 0;
}

