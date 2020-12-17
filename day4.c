#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include "util.h"
#include "unity.h"


bool check_passport_fields_present(char* record)
{
    return strstr(record, "byr:")
        && strstr(record, "iyr:")
        && strstr(record, "eyr:")
        && strstr(record, "hgt:")
        && strstr(record, "hcl:")
        && strstr(record, "ecl:")
        && strstr(record, "pid:");
}


int find_end_of_word(char* word)
{
    char* end = word;
    char c = *end;
    while(c != '\0' && !isblank(c) && c != ',' && c != '\n')
    {
        end++;
        c = *end;
    }

    return end - word;
}


char* get_value(char* field)
{
    char* valueStart = strchr(field, ':');

    if (valueStart == 0)
    {
        return 0;
    }
    valueStart++;
    int endIdx = find_end_of_word(valueStart);
    char* result = malloc(endIdx + 1);
    strncpy(result, valueStart, endIdx); 
    result[endIdx] = '\0';
    return result;
}

int parse_int(char* str, int* result)
{
    int count = 0;
    *result = -1;

    while (true)
    {
        const char c = str[count];
        if (isdigit(c))
        {
            count++;
        }
        else if (isblank(c) || c == '\0')
        {
            *result = atoi(str);
            return count;
        }
        else
        {
            return -1;
        }
    }
}


bool check_unit(const char* str, const char* unit)
{
    if (strncmp(str, unit, strlen(unit))) return false;
    
    char c = str[strlen(unit)];

    return isspace(c) || c == '\0';
}


bool get_size(const char* str, const char* unit, int* result)
{
    int count = 0;

    while (true)
    {
        char c = str[count];
        if (isdigit(c))
        {
            count++;
        }
        else if (c == unit[0])
        {
            if (!check_unit(&str[count], unit)) return false;
            *result = atoi(str);
            return true;
        }
        else
        {
            return false;
        }
    }
}


bool get_year(const char* record, char* key, int* result)
{
    char token[512];
    sprintf(token, "%s:", key);
    char* pos = strstr(record, token);
    if (pos == 0) return false;

    char* valueStr = get_value(pos);
    int numDigits = parse_int(valueStr, result);
    free(valueStr);
    return numDigits == 4;
}


bool check_byr(const char* record)
{
    int year;
    return get_year(record, "byr", &year)
        && year >= 1920
        && year <= 2002;
}

bool check_iyr(const char* record)
{
    int year;
    return get_year(record, "iyr", &year)
        && year >= 2010
        && year <= 2020;
}

bool check_eyr(const char* record)
{
    int year;
    return get_year(record, "eyr", &year)
        && year >= 2020
        && year <= 2030;
}

bool check_hgt(const char* record)
{
    char* start = strstr(record, "hgt:");
    
    if (start == 0)
    {
        return false;
    }
    else
    {
        int height;
        bool inchesValid = get_size(start + 4, "in", &height);
        if (inchesValid)
        {
            return height >= 59 && height <= 76;
        }
       
        bool cmValid = get_size(start + 4, "cm", &height);
        if (cmValid)
        {
            return height >= 150 && height <= 193;
        }
 
        return false;
    }    
}

bool check_hcl(const char* record)
{
    bool result = false;
   
    char* pos = strstr(record, "hcl:");
    if (!pos) return false;

    char* val = get_value(pos);
    if (strlen(val) != 7 || val[0] != '#') goto end; 

    for (size_t i = 1; i < strlen(val); ++i)
    {
        if (!isxdigit(val[i])) goto end;
    }

    result = true;

end:
    free(val);
    return result;
}

bool check_ecl(const char* record)
{
    bool result = false;
    char* pos = strstr(record, "ecl:");
    if (!pos) return false;

    char* val = get_value(pos);
    
    if (strlen(val) != 3) goto end;

    result = strcmp(val, "amb")
        && strcmp(val, "blu")
        && strcmp(val, "brn")     
        && strcmp(val, "gry")     
        && strcmp(val, "grn")     
        && strcmp(val, "hzl")     
        && strcmp(val, "oth");     

    result = !result;

end:
    free(val);
    return result;
}


bool check_pid(const char* record)
{
    bool result = false;
   
    char* pos = strstr(record, "pid:");
    if (!pos) return false;

    char* val = get_value(pos);
    if (strlen(val) != 9) goto end; 

    for (size_t i = 0; i < strlen(val); ++i)
    {
        if (!isdigit(val[i])) goto end;
    }

    result = true;

end:
    free(val);
    return result;
}

bool check_passport_valid(char* record)
{
    printf("Checking %s\n", record);

    return check_byr(record)
        && check_iyr(record)
        && check_eyr(record)
        && check_hgt(record)
        && check_hcl(record)
        && check_ecl(record)
        && check_pid(record);
}


void setUp() {}
void tearDown() {}

void test_check_byr()
{
    TEST_ASSERT_TRUE(check_byr("byr:1974"));
    TEST_ASSERT_FALSE(check_byr("byr:20000") == false);
    TEST_ASSERT_FALSE(check_byr("byr:x2000") == false);
    TEST_ASSERT_FALSE(check_byr("byr:x200") == false);
    TEST_ASSERT_FALSE(check_byr("byr:2000asdf") == false);
    TEST_ASSERT_FALSE(check_byr("iyr:2000") == false);
    TEST_ASSERT_TRUE(check_byr("iyr:2020 byr:1968"));
}


void test_parse_int()
{
    int result;
    TEST_ASSERT_EQUAL(1, parse_int("2", &result));
    TEST_ASSERT_EQUAL(2, result);
        
    TEST_ASSERT_EQUAL(2, parse_int("20", &result));
    TEST_ASSERT_EQUAL(20, result);

    TEST_ASSERT_EQUAL(3, parse_int("200", &result));
    TEST_ASSERT_EQUAL(200, result);

    TEST_ASSERT_EQUAL(4, parse_int("2000", &result));
    TEST_ASSERT_EQUAL(2000, result);

    TEST_ASSERT_EQUAL(-1, parse_int("x2000", &result));
    TEST_ASSERT_EQUAL(-1, parse_int("2000x", &result));
}


void test_find_end_of_word()
{
    TEST_ASSERT_EQUAL(0, find_end_of_word(""));
    TEST_ASSERT_EQUAL(1, find_end_of_word("a"));
    TEST_ASSERT_EQUAL(1, find_end_of_word("a a"));
    TEST_ASSERT_EQUAL(1, find_end_of_word("a "));
    TEST_ASSERT_EQUAL(1, find_end_of_word("a\n"));
}



void test_get_value()
{
    TEST_ASSERT_EQUAL_STRING(get_value("byr:2000"), "2000");
    TEST_ASSERT_EQUAL_STRING(get_value("byr:2000 "), "2000");
    TEST_ASSERT_EQUAL_STRING(get_value("byr:2000asdf"), "2000asdf");
    TEST_ASSERT_EQUAL_STRING(get_value("byr:2000asdf iyr:2000"), "2000asdf");
}

void test_check_hgt()
{
    int size;
    TEST_ASSERT_TRUE(get_size("120cm", "cm", &size) && size == 120);
    TEST_ASSERT_TRUE(get_size("120in", "in", &size) && size == 120);
    TEST_ASSERT_TRUE(get_size("120cm", "cm", &size) && size == 120);
    TEST_ASSERT_TRUE(get_size("120in", "in", &size) && size == 120);
    TEST_ASSERT_FALSE(get_size("120cm", "cmm", &size));
    TEST_ASSERT_FALSE(get_size("120cm", "in", &size));
    TEST_ASSERT_TRUE(check_hgt("hgt:170cm"));
    TEST_ASSERT_FALSE(check_hgt("hgt:149cm"));
    TEST_ASSERT_FALSE(check_hgt("hgt:194cm"));
    TEST_ASSERT_TRUE(check_hgt("hgt:60in"));
    TEST_ASSERT_FALSE(check_hgt("hgt:58in"));
    TEST_ASSERT_FALSE(check_hgt("hgt:77in"));
    printf("test_check_hgt OK\n");
}

void test_check_hcl()
{
    TEST_ASSERT_TRUE(check_hcl("hcl:#012abc"));
    TEST_ASSERT_TRUE(check_hcl("hcl:#345def"));
    TEST_ASSERT_TRUE(check_hcl("hcl:#6789ab"));
    TEST_ASSERT_FALSE(check_hcl("hcl:#012abce"));
    TEST_ASSERT_FALSE(check_hcl("hcl:#012abk"));
}


void test_check_ecl()
{
    TEST_ASSERT_TRUE(check_ecl("ecl:amb"));
    TEST_ASSERT_TRUE(check_ecl("ecl:blu"));
    TEST_ASSERT_TRUE(check_ecl("ecl:brn"));
    TEST_ASSERT_TRUE(check_ecl("ecl:gry"));
    TEST_ASSERT_TRUE(check_ecl("ecl:grn"));
    TEST_ASSERT_TRUE(check_ecl("ecl:hzl"));
    TEST_ASSERT_TRUE(check_ecl("ecl:oth"));
    TEST_ASSERT_FALSE(check_ecl("ecl:amba"));
    TEST_ASSERT_FALSE(check_ecl("ecl:bluz"));
    TEST_ASSERT_FALSE(check_ecl("ecl:bri"));
    TEST_ASSERT_FALSE(check_ecl("acl:blu"));
}

void test_check_pid()
{
    TEST_ASSERT_TRUE(check_pid("pid:012345678"));
    TEST_ASSERT_FALSE(check_pid("pid:1234"));
    TEST_ASSERT_FALSE(check_pid("pid:0123456789"));
    TEST_ASSERT_FALSE(check_pid("pid:012a45678"));
    TEST_ASSERT_FALSE(check_pid("pid:a12345678"));
    TEST_ASSERT_FALSE(check_pid("pid:012345678a"));
}


void test_check_passport()
{
    char* p1 = "iyr:2020 byr:1968\necl:gry\neyr:2030 hcl:#1976b0\ncid:127 pid:701862616\nhgt:161cm\n";
    char *p2 = "eyr:2027 iyr:2019 ecl:#a0eeca hcl:#c0946f pid:724783488 byr:1943 cid:282 hgt:124\n";
    TEST_ASSERT_TRUE(check_iyr(p1));
    TEST_ASSERT_TRUE(check_byr(p1));
    TEST_ASSERT_TRUE(check_eyr(p1));
    TEST_ASSERT_TRUE(check_passport_valid(p1));
    TEST_ASSERT_FALSE(check_passport_valid(p2));
}


int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_find_end_of_word);
    RUN_TEST(test_get_value);
    RUN_TEST(test_parse_int);
    RUN_TEST(test_check_byr);
    RUN_TEST(test_check_hgt);
    RUN_TEST(test_check_hcl);
    RUN_TEST(test_check_ecl);
    RUN_TEST(test_check_pid);
    RUN_TEST(test_check_passport);
    UNITY_END();

    FILE* fp = fopen("day4.txt", "r");

    char line[512];
    char record[2048];
    record[0] = '\0';

    int numValid = 0;

    while (true)
    {
        fgets(line, 512, fp);

        if (feof(fp) || !strcmp(line, "\n"))
        {
            numValid += check_passport_valid(record);
            if (check_passport_valid(record))
            {
                printf("Found valid passport!\n");
            }

            record[0] = '\0';
        }
        else
        {
            strcat(record, line);
        }

        if (feof(fp)) break;
    }


    printf("Num. valid passports: %d\n", numValid);

    return 0;
}
