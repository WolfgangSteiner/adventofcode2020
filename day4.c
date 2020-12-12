#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>

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
    int idx = 0;
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

    for (int i = 1; i < strlen(val); ++i)
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

    for (int i = 0; i < strlen(val); ++i)
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


void test_check_byr()
{
    assert(check_byr("byr:1974") == true);
    assert(check_byr("byr:20000") == false);
    assert(check_byr("byr:x2000") == false);
    assert(check_byr("byr:x200") == false);
    assert(check_byr("byr:2000asdf") == false);
    assert(check_byr("iyr:2000") == false);
    assert(check_byr("iyr:2020 byr:1968"));
    printf("test_byr OK\n");
}


void test_parse_int()
{
    int result;
    assert(parse_int("2", &result) == 1 && result == 2);
    assert(parse_int("20", &result) == 2 && result == 20);
    assert(parse_int("200", &result) == 3 && result == 200);
    assert(parse_int("2000", &result) == 4 && result == 2000);
    assert(parse_int("x2000", &result) == -1);
    assert(parse_int("2000x", &result) == -1);
    printf("test_parse_int OK\n");
}


void test_find_end_of_word()
{
    assert(find_end_of_word("") == 0);
    assert(find_end_of_word("a") == 1);
    assert(find_end_of_word("a a") == 1);
    assert(find_end_of_word("a ") == 1);
    assert(find_end_of_word("a\n") == 1);
}


void assert_str_equal(char* a, char* b)
{
    assert(!strcmp(a, b));
    free(a);
}


void test_get_value()
{
    assert_str_equal(get_value("byr:2000"), "2000");
    assert_str_equal(get_value("byr:2000 "), "2000");
    assert_str_equal(get_value("byr:2000asdf"), "2000asdf");
    assert_str_equal(get_value("byr:2000asdf iyr:2000"), "2000asdf");
}

void test_check_hgt()
{
    int size = 0;
    assert(get_size("120cm", "cm", &size) && size == 120);
    assert(get_size("120in", "in", &size) && size == 120);
    assert(!get_size("120cm", "cmm", &size));
    assert(!get_size("120cm", "in", &size));
    assert(check_hgt("hgt:170cm"));
    assert(!check_hgt("hgt:149cm"));
    assert(!check_hgt("hgt:194cm"));
    assert(check_hgt("hgt:60in"));
    assert(!check_hgt("hgt:58in"));
    assert(!check_hgt("hgt:77in"));
    printf("test_check_hgt OK\n");
}

void test_check_hcl()
{
    assert(check_hcl("hcl:#012abc"));
    assert(check_hcl("hcl:#345def"));
    assert(check_hcl("hcl:#6789ab"));
    assert(!check_hcl("hcl:#012abce"));
    assert(!check_hcl("hcl:#012abk"));
    printf("test_check_hcl OK\n");
}


void test_check_ecl()
{
    assert(check_ecl("ecl:amb"));
    assert(check_ecl("ecl:blu"));
    assert(check_ecl("ecl:brn"));
    assert(check_ecl("ecl:gry"));
    assert(check_ecl("ecl:grn"));
    assert(check_ecl("ecl:hzl"));
    assert(check_ecl("ecl:oth"));
    assert(!check_ecl("ecl:amba"));
    assert(!check_ecl("ecl:bluz"));
    assert(!check_ecl("ecl:bri"));
    assert(!check_ecl("acl:blu"));
    printf("test_check_ecl OK\n");
}

void test_check_pid()
{
    assert(check_pid("pid:012345678"));
    assert(!check_pid("pid:1234"));
    assert(!check_pid("pid:0123456789"));
    assert(!check_pid("pid:012a45678"));
    assert(!check_pid("pid:a12345678"));
    assert(!check_pid("pid:012345678a"));
    printf("test_check_pid OK\n");
}


void test_check_passport()
{
    char* p1 = "iyr:2020 byr:1968\necl:gry\neyr:2030 hcl:#1976b0\ncid:127 pid:701862616\nhgt:161cm\n";

    char *p2 = "eyr:2027 iyr:2019 ecl:#a0eeca hcl:#c0946f pid:724783488 byr:1943 cid:282 hgt:124\n";
    assert(check_iyr(p1));
    assert(check_byr(p1));
    assert(check_eyr(p1));
    assert(check_passport_valid(p1));
    assert(!check_passport_valid(p2));
}


int main(int argc, char** argv)
{
    test_find_end_of_word();
    test_get_value();
    test_parse_int();
    test_check_byr();
    test_check_hgt();
    test_check_hcl();
    test_check_ecl();
    test_check_pid();
    test_check_passport();

    FILE* fp = fopen("day4.txt", "r");

    char line[512];
    char record[2048];
    record[0] = '\0';

    int numValid = 0;
    int readResult;

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
