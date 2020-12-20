#include <stdio.h>
#include "unity.h"
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "util.h"
#include "bitmap.h"

char* test1 = 
" # #"
"# # "
"#  #"
" ## ";

char* test2 = 
"  # #"
" # # "
"# #  "
"#   #"
" ### ";

char* test3 = 
"#   "
"    "
"    "
"    ";

char* test3_flip_diag = 
"    "
"    "
"    "
"   #";

char* test1_flip_x =    "# # " " # #" "#  #" " ## ";
char* test1_flip_y =    " ## " "#  #" "# # " " # #"; 
char* test1_flip_diag =
" # #"
"# # "
"#  #"
" ## ";
char* test1_rotate =    "# # " " # #" "#  #" " ## ";
char* test2_flip_x =    "# #  " " # # " "  # #" "#   #" " ### ";
char* test2_flip_y =    " ### " "#   #" "# #  " " # # " "  # #";
char* test2_flip_diag = " #  #" "#  # " "# # #" "#  # " " ##  ";
char* test2_rotate =    "#  # " " #  #" "# # #" " #  #" "  ## ";

char* test1_blit = 
" # #"" # #"
"# # ""# # "
"#  #""#  #"
" ## "" ## ";

char* test4 = 
"##  "
"#   "
"    "
"    ";

char* test4_rotate90 = 
"    "
"    "
"#   "
"##  ";

char* test4_rotate180 = 
"    "
"    "
"   #"
"  ##";

char* test4_rotate270 = 
"  ##"
"   #"
"    "
"    ";

char* test5 = 
"##   "
"#    "
"     "
"     "
"     ";

char* test5_rotate90 = 
"     "
"     "
"     "
"#    "
"##   ";

char* test5_rotate180 = 
"     "
"     "
"     "
"    #"
"   ##";

char* test5_rotate270 = 
"    ##"
"     #"
"     "
"     "
"     ";

char* test6 = 
"    "
" #  "
"    "
"    ";

char* test6_rotate90 = 
"    "
"    "
" #  "
"    ";

char* test6_rotate180 = 
"    "
"    "
"  # "
"    ";

char* test6_rotate270 = 
"    "
"  # "
"    "
"    ";

void setUp()
{
}


void tearDown()
{
}


void test_parse_print()
{
    bitmap_t* b = bitmap_parse(test1);
    bitmap_print(b);
    bitmap_free(b);
}

void test_bitmap_flip_x()
{
    bitmap_t* b1 = bitmap_parse(test1);
    bitmap_flip_x(b1);
    TEST_ASSERT(bitmap_equal_str(b1, test1_flip_x));
    bitmap_free(b1);

    bitmap_t* b2 = bitmap_parse(test2);
    bitmap_flip_x(b2);
    TEST_ASSERT(bitmap_equal_str(b2, test2_flip_x));
    bitmap_free(b2);
}

void test_bitmap_flip_y()
{
    bitmap_t* b1= bitmap_parse(test1);
    bitmap_flip_y(b1);
    TEST_ASSERT(bitmap_equal_str(b1, test1_flip_y));
    bitmap_free(b1);

    bitmap_t* b2= bitmap_parse(test2);
    bitmap_flip_y(b2);
    TEST_ASSERT(bitmap_equal_str(b2, test2_flip_y));
    bitmap_free(b2);
}

void test_bitmap_flip_diag()
{
    bitmap_t* b1 = bitmap_parse(test1);
    bitmap_flip_diag(b1);
    TEST_ASSERT(bitmap_equal_str(b1, test1_flip_diag));
    bitmap_free(b1);

    bitmap_t* b2 = bitmap_parse(test2);
    bitmap_flip_diag(b2);
    TEST_ASSERT(bitmap_equal_str(b2, test2_flip_diag));
    bitmap_free(b2);

    bitmap_t* b3 = bitmap_parse(test3);
    bitmap_flip_diag(b3);
    TEST_ASSERT(bitmap_equal_str(b3, test3_flip_diag));
    bitmap_free(b3);
}

void test_bitmap_rotate()
{
    bitmap_t* b1 = bitmap_parse(test1);
    bitmap_rotate(b1);
    TEST_ASSERT(bitmap_equal_str(b1, test1_rotate));
    bitmap_free(b1);

    bitmap_t* b2 = bitmap_parse(test2);
    bitmap_rotate(b2);
    TEST_ASSERT(bitmap_equal_str(b2, test2_rotate));
    bitmap_free(b2);

    bitmap_t* b4 = bitmap_parse(test4);
    bitmap_rotate(b4);
    TEST_ASSERT(bitmap_equal_str(b4, test4_rotate90));
    bitmap_rotate(b4);
    TEST_ASSERT(bitmap_equal_str(b4, test4_rotate180));
    bitmap_rotate(b4);
    TEST_ASSERT(bitmap_equal_str(b4, test4_rotate270));
    bitmap_rotate(b4);
    TEST_ASSERT(bitmap_equal_str(b4, test4));
    bitmap_free(b4);
}


void test_bitmap_blit()
{
    bitmap_t* b1 = bitmap_parse(test1);
    bitmap_t* b2 = bitmap_init(8, 4);
    bitmap_blit(b2, (rect_t){0,0,4,4}, b1, (rect_t){0,0,4,4});
    bitmap_blit(b2, (rect_t){4,0,8,4}, b1, (rect_t){0,0,4,4});
    TEST_ASSERT(bitmap_equal_str(b2, test1_blit));
    bitmap_free(b1);
}

int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_parse_print);
    RUN_TEST(test_bitmap_flip_x);
    RUN_TEST(test_bitmap_flip_y);
    RUN_TEST(test_bitmap_flip_diag);
    RUN_TEST(test_bitmap_rotate);
    RUN_TEST(test_bitmap_blit);
    UNITY_END();

    return 0;
}

