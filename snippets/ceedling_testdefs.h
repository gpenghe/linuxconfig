#ifndef TESTDEFS_H
#define TESTDEFS_H

#define AT(x)       TEST_ASSERT_TRUE_MESSAGE(x, "var: ["#x"]")
#define AF(x)       TEST_ASSERT_FALSE_MESSAGE(x, "var: ["#x"]")
#define AE(x, y)    TEST_ASSERT_EQUAL_MESSAGE((x), (y), "var: ["#y"]")
#define AEI(x, y)   TEST_ASSERT_EQUAL_INT_MESSAGE((x), (y), "var: ["#y"]")
#define AES(x, y)   TEST_ASSERT_EQUAL_STRING_MESSAGE((x), (y), "var: ["#y"]")
#define AEF(x, y)   TEST_ASSERT_EQUAL_FLOAT_MESSAGE(x, y, "var: ["#y"]");  // "equal" means they are very close
#define AEM(x, y)   TEST_ASSERT_NOT_EQUAL_MEMORY_MESSAGE((x), (y), "var: ["#y"]")

#define ANE(x, y)   TEST_ASSERT_NOT_EQUAL_MESSAGE((x), (y), "var: ["#y"]")

#endif
