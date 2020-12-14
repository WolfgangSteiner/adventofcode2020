#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "unity.h"
#include <assert.h>
#include "hash_map.h"
#include "util.h"

typedef struct
{
    int count;
    char colorName[32];
} Contains;

typedef struct
{
    char colorName[64];
    int id;
    int numContains;
    Contains contains[4];
} Rule;


void rule_init(Rule* rule, const char* adjective, const char* color)
{
    sprintf(rule->colorName, "%s %s", adjective, color);
    rule->numContains = 0;
}


void rule_add_contains(Rule* rule, int count, const char* adjective, const char* color)
{
    Contains* contains = &rule->contains[rule->numContains];
    contains->count = count;
    sprintf(contains->colorName, "%s %s", adjective, color);
    rule->numContains++;
} 




Rule parse_rule(const char* line)
{
    char adjective[32];
    char color[32];
    char skip[32];
    char number[32];
    const char* pos = copy_word(adjective, line);
    pos = copy_word(color, pos);

    Rule rule;
    rule_init(&rule, adjective, color);

    // skip "bags"
    pos = copy_word(skip, pos);
    assert(!strcmp(skip, "bags"));
    
    pos = copy_word(skip, pos);
    assert(!strcmp(skip, "contain"));

    while (pos)
    {
        pos = copy_word(number, pos);
        if (!strcmp(number, "no")) break;

        pos = copy_word(adjective, pos);
        pos = copy_word(color, pos);
        pos = copy_word(skip, pos);
        assert(!strncmp(skip, "bag", 3));

        rule_add_contains(&rule, atoi(number), adjective, color);
    }

    return rule;
}


void assign_color_ids(Rule* rule, HashMap* colorRuleHashMap, char** idToColor)
{
    assert(hash_map_find(colorRuleHashMap, rule->colorName) == 0);
    const int newId = colorRuleHashMap->size;
    idToColor[newId] = malloc(strlen(rule->colorName) + 1);
    strcpy(idToColor[newId], rule->colorName);
    hash_map_insert(colorRuleHashMap, rule->colorName, rule);
    rule->id = newId;
}


bool check_rule(Rule* rule, HashMap* colorRuleHashMap, int targetId)
{
    HashMap* visitedHashMap = hash_map_init(1024);
    const Rule* activeRules[1024];
    int numActiveRules = 0;
    activeRules[numActiveRules++] = rule;
    //printf("================= Checking %d %s ================= \n", rule->id, rule->colorName);

    while (numActiveRules)
    {
        const Rule* currentRule = activeRules[--numActiveRules];

        if (!hash_map_has_key(visitedHashMap, currentRule->colorName))
        {
            hash_map_insert(visitedHashMap, currentRule->colorName, (void*)1);
            if (currentRule->id == targetId)
            {
                return true;
            }

            for (int i = 0; i < currentRule->numContains; ++i)
            {
                const char* containedColorName = currentRule->contains[i].colorName;
                const Rule* containedRule = hash_map_find(colorRuleHashMap, containedColorName)->value;
                activeRules[numActiveRules++] = containedRule;
                //printf("Pushed %s\n", containedRule->colorName);
            }
        }
    } 

    hash_map_free(visitedHashMap);

    return false;
}

long long count_bags(const Rule* rule, HashMap* colorRuleHashMap)
{
    long long total = 0;

    for (int i = 0; i < rule->numContains; ++i)
    {
        const Contains* contained = &rule->contains[i];
        const Rule* containedRule = hash_map_find(colorRuleHashMap, contained->colorName)->value;
        total += contained->count + contained->count * count_bags(containedRule, colorRuleHashMap);
    }

    return total;
}


void setUp() {}
void tearDown() {}

void test_copy_word()
{
    char line[] = "mirrored orange bags contain 2 dim green bags, 2 striped red bags, 4 drab plum bags, 1 pale teal bag.";
    char word[32];
    const char* pos = line;

    pos = copy_word(word, pos); TEST_ASSERT_EQUAL_STRING("mirrored", word);
    pos = copy_word(word, pos); TEST_ASSERT_EQUAL_STRING("orange", word);
    pos = copy_word(word, pos); TEST_ASSERT_EQUAL_STRING("bags", word);
    pos = copy_word(word, pos); TEST_ASSERT_EQUAL_STRING("contain", word);
    pos = copy_word(word, pos); TEST_ASSERT_EQUAL_STRING("2", word);
    pos = copy_word(word, pos); TEST_ASSERT_EQUAL_STRING("dim", word);
    pos = copy_word(word, pos); TEST_ASSERT_EQUAL_STRING("green", word);
    pos = copy_word(word, pos); TEST_ASSERT_EQUAL_STRING("bags", word);
    pos = copy_word(word, pos); TEST_ASSERT_EQUAL_STRING("2", word);
    pos = copy_word(word, pos); TEST_ASSERT_EQUAL_STRING("striped", word);
    pos = copy_word(word, pos); TEST_ASSERT_EQUAL_STRING("red", word);
    pos = copy_word(word, pos); TEST_ASSERT_EQUAL_STRING("bags", word);
    pos = copy_word(word, pos); TEST_ASSERT_EQUAL_STRING("4", word);
    pos = copy_word(word, pos); TEST_ASSERT_EQUAL_STRING("drab", word);
    pos = copy_word(word, pos); TEST_ASSERT_EQUAL_STRING("plum", word);
    pos = copy_word(word, pos); TEST_ASSERT_EQUAL_STRING("bags", word);
    pos = copy_word(word, pos); TEST_ASSERT_EQUAL_STRING("1", word);
    pos = copy_word(word, pos); TEST_ASSERT_EQUAL_STRING("pale", word);
    pos = copy_word(word, pos); TEST_ASSERT_EQUAL_STRING("teal", word);
    pos = copy_word(word, pos); TEST_ASSERT_EQUAL_STRING("bag", word);
}

void test_parse_rule()
{
    const char line[] = "mirrored orange bags contain 2 dim green bags, 2 striped red bags, 4 drab plum bags, 1 pale teal bag.";
    const Rule rule = parse_rule(line);
    TEST_ASSERT_EQUAL_STRING("mirrored orange", rule.colorName);
    TEST_ASSERT_EQUAL(4, rule.numContains);
    TEST_ASSERT_EQUAL(2, rule.contains[0].count);
    TEST_ASSERT_EQUAL_STRING("dim green", rule.contains[0].colorName);
    TEST_ASSERT_EQUAL(2, rule.contains[1].count);
    TEST_ASSERT_EQUAL_STRING("striped red", rule.contains[1].colorName);
    TEST_ASSERT_EQUAL(4, rule.contains[2].count);
    TEST_ASSERT_EQUAL_STRING("drab plum", rule.contains[2].colorName);
    TEST_ASSERT_EQUAL(1, rule.contains[3].count);
    TEST_ASSERT_EQUAL_STRING("pale teal", rule.contains[3].colorName);
}


void test_hash_map()
{
    HashMap* map = hash_map_init(16);

    TEST_ASSERT_EQUAL(0, hash_map_find(map, "Test"));
    
    hash_map_insert(map, "eins",      (void*)1);
    hash_map_insert(map, "zwei",      (void*)2);
    hash_map_insert(map, "drei",      (void*)3);
    hash_map_insert(map, "vier",      (void*)4);
    hash_map_insert(map, "fuenf",     (void*)5);
    hash_map_insert(map, "sechs",     (void*)6);
    hash_map_insert(map, "sieben",    (void*)7);
    hash_map_insert(map, "acht",      (void*)8);
    hash_map_insert(map, "neun",      (void*)9);
    hash_map_insert(map, "zehn",      (void*)10);
    hash_map_insert(map, "elf",       (void*)11);
    hash_map_insert(map, "zwoelf",    (void*)12);
    hash_map_insert(map, "dreizehn",  (void*)13);
    hash_map_insert(map, "vierzehn",  (void*)14);
    hash_map_insert(map, "fuenfzehn", (void*)15);
    hash_map_insert(map, "sechzehn",  (void*)16);
    TEST_ASSERT(hash_map_find(map, "eins") != 0);
    TEST_ASSERT_EQUAL(1, (size_t)hash_map_find(map, "eins")->value);
    TEST_ASSERT_EQUAL(2, (size_t)hash_map_find(map, "zwei")->value);
    TEST_ASSERT_EQUAL(3, (size_t)hash_map_find(map, "drei")->value);
    TEST_ASSERT_EQUAL(4, (size_t)hash_map_find(map, "vier")->value);
    TEST_ASSERT_EQUAL(5, (size_t)hash_map_find(map, "fuenf")->value);
    TEST_ASSERT_EQUAL(6, (size_t)hash_map_find(map, "sechs")->value);
    TEST_ASSERT_EQUAL(7, (size_t)hash_map_find(map, "sieben")->value);
    TEST_ASSERT_EQUAL(8, (size_t)hash_map_find(map, "acht")->value);
    TEST_ASSERT_EQUAL(9, (size_t)hash_map_find(map, "neun")->value);
    TEST_ASSERT_EQUAL(10, (size_t)hash_map_find(map, "zehn")->value);
    TEST_ASSERT_EQUAL(11, (size_t)hash_map_find(map, "elf")->value);
    TEST_ASSERT_EQUAL(12, (size_t)hash_map_find(map, "zwoelf")->value);
    TEST_ASSERT_EQUAL(13, (size_t)hash_map_find(map, "dreizehn")->value);
    TEST_ASSERT_EQUAL(14, (size_t)hash_map_find(map, "vierzehn")->value);
    TEST_ASSERT_EQUAL(15, (size_t)hash_map_find(map, "fuenfzehn")->value);
    TEST_ASSERT_EQUAL(16, (size_t)hash_map_find(map, "sechzehn")->value);

    hash_map_free(map);
}



int main(int argc, char** argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_copy_word);
    RUN_TEST(test_parse_rule);
    RUN_TEST(test_hash_map);
    UNITY_END();

    FILE* fp = fopen("day7.txt", "r");
    char line[512];
    Rule rules[1024];
    int ruleCount = 0;
    HashMap* colorRuleHashMap = hash_map_init(1024);
    char* idToColor[1024];
    while (!feof(fp))
    {
        if (fgets(line, 512, fp))
        {
            line[strlen(line) - 1] = '\0';
            rules[ruleCount] = parse_rule(line);
            assign_color_ids(&rules[ruleCount], colorRuleHashMap, idToColor);
            if (!strcmp(rules[ruleCount].colorName, "shiny gold"))
            {
                printf("%s\n", line);
            }
            ruleCount++;
        }
    }

    const int numColors = colorRuleHashMap->size;

    for (int id = 0; id < numColors; ++id)
    {
        const char* name = idToColor[id];
        assert(hash_map_has_key(colorRuleHashMap, name));
        Rule* rule = hash_map_find(colorRuleHashMap, name)->value;
        assert(rule->id == id);
    }

    const int shinyGoldId = ((Rule*)hash_map_find(colorRuleHashMap, "shiny gold")->value)->id;
    printf("shiny gold: %d\n", shinyGoldId);

    int count = 0;

    for (int i = 0; i < ruleCount; ++i)
    {
        Rule* rule = &rules[i];
        if (rule->id == shinyGoldId)
        {
            continue;
        }

        if (check_rule(rule, colorRuleHashMap, shinyGoldId))
        {
            count++;
        }
    }

    printf("Result: %d\n", count);

    Rule* shinyGoldRule = hash_map_find(colorRuleHashMap, "shiny gold")->value;
    long long numBags = count_bags(shinyGoldRule, colorRuleHashMap);
    printf("Required Bags: %lld\n", numBags);


    HashMapIterator* iter = hash_map_iterator_begin(colorRuleHashMap);
    while (!hash_map_iterator_is_end(iter))
    {
        printf("%s\n", hash_map_iterator_get_key(iter));
        hash_map_iterator_next(iter);
    }

    free(iter);
    hash_map_free(colorRuleHashMap);
    return 0;
}




