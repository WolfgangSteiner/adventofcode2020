#include <stdio.h>
#include "unity.h"
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "util.h"
#include "hash_map.h"

typedef struct
{
    int start;
    int end;
} range_t;


typedef struct
{
    char name[32];
    range_t valid_ranges[2];
    size_t possible_fields[64];
    size_t num_possible_fields;
} rule_t;


typedef struct
{
    int num_fields;
    int fields[64];
} ticket_t;


typedef struct
{
    HashMap* rules;
    size_t num_tickets;
    ticket_t* tickets[512];
} problem_t;


problem_t* problem_init()
{
    problem_t* p = calloc(1, sizeof(problem_t));
    p->rules = hash_map_init(64);
    return p;
}


void problem_free(problem_t* problem)
{
    hash_map_free(problem->rules, free);
    for (int i = 0; i < problem->num_tickets; ++i)
    {
        free(problem->tickets[i]);
    }
    free(problem);
}


void problem_add_ticket(problem_t* problem, ticket_t* ticket)
{
    problem->tickets[problem->num_tickets++] = ticket;
    MImplies(problem->num_tickets >= 2, ticket->num_fields == problem->tickets[0]->num_fields);
}


void problem_add_rule(problem_t* problem, rule_t* rule)
{
    hash_map_insert(problem->rules, rule->name, rule);
}


ticket_t* parse_ticket(const char* line)
{
    ticket_t* ticket = calloc(1, sizeof(ticket_t));
    ticket->num_fields = 0;

    const char* pos = line;
    while(pos != 0)
    {
        const int value = atoi(pos);
        ticket->fields[ticket->num_fields++] = value;
        pos = strstr(pos, ",");
        if (pos == NULL) break;
        pos++;
    }

    return ticket;
}

rule_t* parse_rule(const char* line)
{
    rule_t* rule = calloc(1, sizeof(rule_t));
    int result = sscanf(line, "%[^:]: %d-%d or %d-%d", 
            rule->name,
            &rule->valid_ranges[0].start,
            &rule->valid_ranges[0].end,
            &rule->valid_ranges[1].start,
            &rule->valid_ranges[1].end);
    assert(result == 5);

    return rule;
}


problem_t* read_input(const char* file_name)
{
    problem_t* problem = problem_init();

    FILE* fp = fopen(file_name, "r");
    char line[256];
    while(!feof(fp))
    {
        if (fgets(line, 256, fp))
        {
            chomp(line);
            if (strstr(line, "ticket"))
            {
                continue;
            }
            else if (line[0] == '\0')
            {
                continue;
            }
            else if (strstr(line, ":"))
            {
                rule_t* rule = parse_rule(line);
                problem_add_rule(problem, rule);
            }
            else
            {
                ticket_t* ticket = parse_ticket(line);
                problem_add_ticket(problem, ticket);
            }
        }
    }

    return problem;
}


bool is_value_valid_for_rule(rule_t* rule, int value)
{
    return (value >= rule->valid_ranges[0].start && value <= rule->valid_ranges[0].end)
        || (value >= rule->valid_ranges[1].start && value <= rule->valid_ranges[1].end);
}


bool is_ticket_value_valid(const problem_t* problem, int value)
{
    HashMapIterator* iter = hash_map_iterator_begin(problem->rules);

    bool result = false;

    while (!hash_map_iterator_is_end(iter))
    {
        rule_t* rule = hash_map_iterator_get_value(iter);
        
        if (is_value_valid_for_rule(rule, value))
        {
            result = true;
            break;
        }

        hash_map_iterator_next(iter);
    }

    free(iter);
    return result;
}



int check_invalid_ticket(const problem_t* problem, const ticket_t* ticket)
{
    int result = 0;

    for (int i = 0; i < ticket->num_fields; ++i)
    {
        int value = ticket->fields[i];
        if (!is_ticket_value_valid(problem, value))
        {
            result += value;
        }
    }

    return result;
}


int find_invalid_tickets(problem_t* problem)
{
    int result = 0;

    for (int i = 1; i < problem->num_tickets; ++i)
    {
        result += check_invalid_ticket(problem, problem->tickets[i]);
    }

    return result;
}

bool should_delete_ticket(const void* element, const void* user_data)
{
    const ticket_t* ticket = element;
    const problem_t* problem = user_data;
    bool result = check_invalid_ticket(problem, ticket);
    return result;
}


size_t count_invalid_tickets(const problem_t* problem)
{
    size_t result = 0;
    for (int i = 0; i < problem->num_tickets; ++i)
    {
        const ticket_t* t = problem->tickets[i];
        if (check_invalid_ticket(problem, t))
        {
            result++;
        }
    }

    return result;
}

void discard_invalid_tickets(problem_t* problem)
{
    remove_array_elements(
            (void**) problem->tickets,
            &problem->num_tickets,
            should_delete_ticket,
            problem);
}


bool are_all_field_values_valid_for_rule(problem_t* problem, rule_t* rule, int field_index)
{
    for (int i = 0; i < problem->num_tickets; ++i)
    {
        const ticket_t* ticket = problem->tickets[i];
        const int value = ticket->fields[field_index];
        if (!is_value_valid_for_rule(rule, value))
        {
            return false;
        }
    }

    return true;
}

void match_rule_to_field(problem_t* problem, rule_t* rule, int field_index)
{
    if (are_all_field_values_valid_for_rule(problem, rule, field_index))
    {
        rule->possible_fields[rule->num_possible_fields++] = field_index;
    }
}

void match_rule_to_fields(problem_t* problem, rule_t* rule)
{
    const int num_fields = problem->tickets[0]->num_fields;

    for (int i = 0; i < num_fields; ++i)
    {
        match_rule_to_field(problem, rule, i);
    }
}


rule_t** copy_rules_to_array(problem_t* problem)
{
    rule_t** rules = malloc(sizeof(rule_t) * problem->rules->size);

    size_t index = 0;
    HashMapIterator* iter = hash_map_iterator_begin(problem->rules);
    while (!hash_map_iterator_is_end(iter))
    {
        rule_t* rule = hash_map_iterator_get_value(iter);
        rules[index++] = rule;
        hash_map_iterator_next(iter);
    }

    free(iter);
    return rules;
}


void match_rules_to_fields(problem_t* problem)
{
    HashMapIterator* iter = hash_map_iterator_begin(problem->rules);

    while (!hash_map_iterator_is_end(iter))
    {
        rule_t* rule = hash_map_iterator_get_value(iter);
        match_rule_to_fields(problem, rule);
        hash_map_iterator_next(iter);
    }

    free(iter);
}


bool shoud_remove_possible_field(const void* element, const void* user_data)
{
    size_t possible_field = (size_t)element;
    size_t field_index = (size_t)user_data;
    return possible_field == field_index;
}


void eliminate_field_for_rule(rule_t* rule, size_t field_index)
{
    remove_array_elements(
        (void*) rule->possible_fields,
        &rule->num_possible_fields,
        shoud_remove_possible_field,
        (void*)field_index);
}


void eliminate_field_for_rules(rule_t** rules, size_t num_rules, size_t start_index, size_t field_index)
{
    for (int i = start_index; i < num_rules; ++i)
    {
        rule_t* rule = rules[i];
        eliminate_field_for_rule(rule, field_index);
    }
}


int compare_rule_possibilities(const void* pa, const void* pb)
{
    const rule_t* const * a = pa;
    const rule_t* const * b = pb;
    size_t na = (*a)->num_possible_fields;
    size_t nb = (*b)->num_possible_fields;
    return na < nb ? -1 : na == nb ? 0 : 1;
}


void print_possible_fields_for_rule(const rule_t* rule)
{
    printf("Rule: %s Possible fields: ", rule->name);
    
    for (int i = 0; i < rule->num_possible_fields; ++i)
    {
        printf(" %ld", rule->possible_fields[i]);
    }
    printf("\n");
}

void print_possible_fields(rule_t*const* rules, size_t num_rules)
{
    for (int i = 0; i < num_rules; ++i)
    {
        print_possible_fields_for_rule(rules[i]);
    }
}


void print_possible_rules_for_field(problem_t* problem, rule_t** rules, size_t field_index)
{
    printf("All possible rules for field: %ld\n", field_index);
    for (int i = 0; i < problem->rules->size; ++i)
    {
       const rule_t* rule = rules[i];
       if (are_all_field_values_valid_for_rule(problem, (rule_t*)rule, field_index))
       {
           printf("Rule %s id possible for field %ld\n", rule->name, field_index);
       }
    }
}
         



void resolve_field_assignments(problem_t* problem)
{
    size_t num_rules = problem->rules->size;
    rule_t** rules = copy_rules_to_array(problem);

    qsort(rules, num_rules, sizeof(rule_t*), compare_rule_possibilities);

    for (int i = 0; i < num_rules - 1; ++i)
    {
        rule_t* rule = rules[i];
        
        if (rule->num_possible_fields == 1)
        {
            size_t field_to_eliminate = rule->possible_fields[0];
            eliminate_field_for_rules(rules, num_rules, i + 1, field_to_eliminate);
        }
    }

    // assert(rules[num_rules - 1]->num_possible_fields == 1);
    free(rules); 
}


void deduce_field_assignments(problem_t* problem)
{
    discard_invalid_tickets(problem);
    match_rules_to_fields(problem);
    resolve_field_assignments(problem);
}


size_t compute_result_part_two(problem_t* problem)
{
    size_t result = 1;
    char* prefix = "departure";
    ticket_t* my_ticket = problem->tickets[0];
    HashMapIterator* iter = hash_map_iterator_begin(problem->rules);

    while (!hash_map_iterator_is_end(iter))
    {
        rule_t* rule = hash_map_iterator_get_value(iter);
        if (!strncmp(rule->name, prefix, strlen(prefix)))
        {
            size_t field_index = rule->possible_fields[0];
            int field_value = my_ticket->fields[field_index];
            result *= field_value;
        }

        hash_map_iterator_next(iter);
    }
    free(iter);

    return result;
}


void setUp()
{
}


void tearDown()
{
}


void test_read_input()
{
    problem_t* problem = read_input("day16_test.txt");
    TEST_ASSERT_EQUAL(5, problem->num_tickets);
    TEST_ASSERT_EQUAL(3, problem->rules->size);
  
    HashMapIterator* iter = hash_map_find(problem->rules, "class");
    rule_t* rule = hash_map_iterator_get_value(iter);
    TEST_ASSERT_EQUAL(1, rule->valid_ranges[0].start);
    TEST_ASSERT_EQUAL(3, rule->valid_ranges[0].end);
    TEST_ASSERT_EQUAL(5, rule->valid_ranges[1].start);
    TEST_ASSERT_EQUAL(7, rule->valid_ranges[1].end);
    free(iter);

    TEST_ASSERT_EQUAL(3,  problem->tickets[4]->num_fields);
    TEST_ASSERT_EQUAL(38, problem->tickets[4]->fields[0]);
    TEST_ASSERT_EQUAL(6,  problem->tickets[4]->fields[1]);
    TEST_ASSERT_EQUAL(12, problem->tickets[4]->fields[2]);

    problem_free(problem);
}


void test_find_invalid_tickets()
{
    problem_t* problem = read_input("day16_test.txt");
    TEST_ASSERT_EQUAL(71, find_invalid_tickets(problem));

    free(problem);
}


void test_discard_invalid_tickets()
{
    problem_t* problem = read_input("day16_test.txt");
    discard_invalid_tickets(problem);
    TEST_ASSERT_EQUAL(2, problem->num_tickets);
    TEST_ASSERT_EQUAL(7, problem->tickets[1]->fields[0]);
    TEST_ASSERT_EQUAL(3,  problem->tickets[1]->fields[1]);
    TEST_ASSERT_EQUAL(47, problem->tickets[1]->fields[2]);
    TEST_ASSERT_EQUAL(0, find_invalid_tickets(problem));
    free(problem);
}


void test_deduce_field_assignments()
{
    problem_t* problem = read_input("day16_test2.txt");
    deduce_field_assignments(problem);
    HashMapIterator* iter = hash_map_find(problem->rules, "class");
    rule_t* rule = hash_map_iterator_get_value(iter);
    TEST_ASSERT_EQUAL(1, rule->possible_fields[0]);
    free(iter);

    iter = hash_map_find(problem->rules, "row");
    rule = hash_map_iterator_get_value(iter);
    TEST_ASSERT_EQUAL(0, rule->possible_fields[0]);
    free(iter);

    iter = hash_map_find(problem->rules, "seat");
    rule = hash_map_iterator_get_value(iter);
    TEST_ASSERT_EQUAL(2, rule->possible_fields[0]);
    free(iter);

    free(problem);
}

int main(int argc, char** argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_read_input);
    RUN_TEST(test_find_invalid_tickets);
    RUN_TEST(test_discard_invalid_tickets);
    RUN_TEST(test_deduce_field_assignments);
    UNITY_END();


    problem_t* problem = read_input("day16.txt");
    printf("Result Part 1: %d\n", find_invalid_tickets(problem));

    deduce_field_assignments(problem);
    size_t result_part_two = compute_result_part_two(problem);
    printf("Result Part 2: %ld\n", result_part_two);
    
    free(problem);

    return 0;
}

