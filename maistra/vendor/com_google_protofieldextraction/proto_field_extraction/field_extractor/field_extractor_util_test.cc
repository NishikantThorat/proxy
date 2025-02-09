// Copyright 2023 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "proto_field_extraction/field_extractor/field_extractor_util.h"

#include <memory>
#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "proto_field_extraction/test_utils/utils.h"

namespace google::protobuf::field_extraction::testing {
namespace {

using ::google::protobuf::Field;
using ::google::protobuf::Type;
using ::testing::AllOf;
using ::testing::Pointee;
using ::testing::Property;

constexpr char kFieldExtractorTestMessageTypeUrl[] =
    "type.googleapis.com/"
    "google.protobuf.field_extraction.testing.FieldExtractorTestMessage";
constexpr char kSingularFieldsTypeName[] =
    "google.protobuf.field_extraction.testing.SingularFieldTestMessage";
constexpr char kSingularFieldsTypeUrl[] =
    "type.googleapis.com/"
    "google.protobuf.field_extraction.testing.SingularFieldTestMessage";
constexpr char kRepeatedFieldTypeName[] =
    "google.protobuf.field_extraction.testing.RepeatedField";

class FieldExtractorUtilTest : public ::testing::Test {
 protected:
  void SetUp() override {
    auto status = TypeHelper::Create(GetTestDataFilePath(
        "test_utils/testdata/field_extractor_test_proto_descriptor.pb"));
    ASSERT_TRUE(status.ok());

    type_helper_ = std::move(status.value());

    test_message_type_ = FindType(kFieldExtractorTestMessageTypeUrl);
    ASSERT_NE(test_message_type_, nullptr);
  }

  // Tries to find the Type for `type_url`.
  const Type* FindType(const std::string& type_url) {
    return type_helper_->ResolveTypeUrl(type_url);
  }

  std::unique_ptr<TypeHelper> type_helper_ = nullptr;
  const Type* test_message_type_;
};

TEST_F(FieldExtractorUtilTest, FindField) {
  EXPECT_THAT(
      FindField(*test_message_type_, "singular_field"),
      Pointee(AllOf(Property(&Field::name, "singular_field"),
                    Property(&Field::type_url, kSingularFieldsTypeUrl))));
  EXPECT_THAT(
      FindField(*test_message_type_, "map_singular_field"),
      Pointee(AllOf(Property(&Field::name, "map_singular_field"),
                    Property(&Field::cardinality, Field::CARDINALITY_REPEATED),
                    Property(&Field::kind, Field::TYPE_MESSAGE))));
}

TEST_F(FieldExtractorUtilTest, IsMapMessageType) {
  EXPECT_FALSE(IsMapMessageType(test_message_type_));
  EXPECT_TRUE(IsMapMessageType(FindType(
      FindField(*test_message_type_, "map_singular_field")->type_url())));
  EXPECT_TRUE(IsMapMessageType(FindType(
      FindField(*test_message_type_, "repeated_map_field")->type_url())));
  EXPECT_TRUE(IsMapMessageType(
      FindType(FindField(*test_message_type_, "map_any_fields")->type_url())));
}

TEST_F(FieldExtractorUtilTest, IsAnyMessageType) {
  EXPECT_FALSE(IsAnyMessageType(test_message_type_));
  EXPECT_TRUE(IsAnyMessageType(FindType(
      FindField(*test_message_type_, "singular_any_field")->type_url())));
  EXPECT_TRUE(IsAnyMessageType(FindType(
      FindField(*test_message_type_, "repeated_any_fields")->type_url())));
}
}  // namespace
}  // namespace google::protobuf::field_extraction::testing
