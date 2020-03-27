#include <unity.h>


void test_add(void)
{
    TEST_ASSERT_EQUAL(2+2,4);
}

void test_subtract(void)
{
    TEST_ASSERT_EQUAL(2-2,0);
}


int main(int argc, char ** argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_add);
    RUN_TEST(test_subtract);
    UNITY_END();

    return 0;
}