#include "unity.h"
#include "rocky/lexer/token.h"
#include "rocky/lexer/lexer.h" 

void setUp(void) {}
void tearDown(void) {}

Lexer lexer; 

// Tests for checking basic correctness

// Operator check
void test_operator_token(void) {
    
    lexer_init(&lexer, "+");
    Token token = lexer_next_token(&lexer);
    TEST_ASSERT_EQUAL(TOKEN_PLUS, token.type);

}

// Integer check
void test_int_number(void) {
    
    lexer_init(&lexer, "123");
    Token token = lexer_next_token(&lexer);
    TEST_ASSERT_EQUAL(TOKEN_INT, token.type);

}

// Float check
void test_float_number(void) {

    lexer_init(&lexer, "123.");
    Token token = lexer_next_token(&lexer);
    TEST_ASSERT_EQUAL(TOKEN_FLOAT, token.type);

}

// Invalid token check
void test_invalid_token(void) {

    lexer_init(&lexer, "@");
    Token token = lexer_next_token(&lexer);
    TEST_ASSERT_EQUAL(TOKEN_INVALID, token.type);

}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_operator_token);
    RUN_TEST(test_int_number);
    RUN_TEST(test_float_number);
    RUN_TEST(test_invalid_token);
    return UNITY_END();
}