#include "pch.h"
#include <gtest/gtest.h>

#include "../SSCCVerifier/SSCCVerifier.cpp"

TEST(TestCaseName, TestName) {
  std::vector<std::string> errors;
  EXPECT_TRUE(validate_ai_payload("00", "034260311130776594", errors));
  EXPECT_FALSE(validate_ai_payload("02", "044260311130776512", errors));
  EXPECT_FALSE(validate_ai_payload("00", "034260311130776144", errors));
  EXPECT_TRUE(validate_ai_payload("00", "034260311130776570", errors));
  EXPECT_FALSE(validate_ai_payload("00", "03426031113077646", errors));
  EXPECT_FALSE(validate_ai_payload("00", "034260321130774636", errors));
}