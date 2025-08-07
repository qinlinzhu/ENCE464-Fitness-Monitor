#include <stdio.h>

#include "fff.h"
#include "unity.h"

DEFINE_FFF_GLOBALS;

/* Static variables */
uint32_t dummy_var = 3;

/* Mocks */
FAKE_VOID_FUNC(mocked_void_function);
FAKE_VOID_FUNC(mocked_arg_function, uint32_t*, uint32_t);
FAKE_VALUE_FUNC(uint32_t, mocked_value_function);

/* Unity setup/teardown*/
void setUp(void)
{
    RESET_FAKE(mocked_void_function);
    RESET_FAKE(mocked_arg_function);
    RESET_FAKE(mocked_value_function);
    FFF_RESET_HISTORY();
}

void tearDown(void)
{
}

/* Functions under test */
void foo(void)
{
    mocked_void_function();
    mocked_void_function();
}

void bar(void)
{
    mocked_arg_function(&dummy_var, dummy_var);
}

uint32_t baz(void)
{
    return mocked_value_function();
}

/* test cases */
void test_foo_calls_void_function_twice(void)
{
    foo();
    TEST_ASSERT_EQUAL(2, mocked_void_function_fake.call_count);
}

void test_bar_passes_dummy_var_as_pinter_and_value_args(void)
{
    bar();
    TEST_ASSERT_EQUAL_PTR(&dummy_var, mocked_arg_function_fake.arg0_val);
    TEST_ASSERT_EQUAL(dummy_var, mocked_arg_function_fake.arg1_val);
}

void test_baz_returns_the_return_value_of_mocked_value_function(void)
{
    mocked_value_function_fake.return_val = 1213;

    uint32_t result = baz();
    
    TEST_ASSERT_EQUAL(1213, result);
}


/*
a. What does the fake object’s member call_count mean?
    tracks the number of times mocked function has been called. 
    In test_foo_calls_void_function_twice fuction its verifies 
    mocked_void_function_fake is called twice when foo() is executed.

b. What do the fake object’s members arg0_val, arg1_val, etc mean? 
    represents values of arguements passed into mock function

c. Have a look at the mock definitions at the top. How does FFF know the prototype of 
   a mocked function i.e. its name, what it returns and its expected arguments.
    
d. How do we tell FFF to pretend like a mocked function returns a certain value?

e. Why is it important to reset the fake objects in the setUp function?

*/