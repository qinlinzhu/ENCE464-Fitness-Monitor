#include <stdio.h>

#include "unity.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_1_equals_1(void)
{
    TEST_ASSERT_EQUAL(1, 2);
}

static int is_even(int num)
{
    return num % 2 == 0;
}

void test_positive_even_is_even(void)
{
    TEST_ASSERT(is_even(2));
}

void test_negative_even_is_even(void)
{
    TEST_ASSERT(is_even(-2));
}

void test_positive_odd_is_not_even(void)
{
    TEST_ASSERT_FALSE(is_even(7));
}

void test_negative_odd_is_not_even(void)
{
    TEST_ASSERT_FALSE(is_even(-7));
}

void test_zero_is_even(void)
{
    TEST_ASSERT(is_even(0));
}
