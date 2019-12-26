/*
 Copyright 2010-2019 Kristian Duske

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
 rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit
 persons to whom the Software is furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
 Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <gtest/gtest.h>

#include "kdl/glob_index.h"
#include "kdl/vector_utils.h"

#include <iterator>

namespace kdl {
    using test_index = glob_index<std::string>;

#define ASSERT_QUERY(exp_, index, pattern) {\
    std::vector<std::string> exp(exp_);\
    std::vector<std::string> act;\
    index.query(pattern, std::back_inserter(act));\
    vec_sort(exp);\
    vec_sort(act);\
    ASSERT_EQ(exp, act);\
}

    TEST(glob_index_test, insert) {
        test_index index;
        index.insert("key", "value");
        index.insert("key2", "value");
        index.insert("key22", "value2");
        index.insert("k1", "value3");
        index.insert("test", "value4");

        ASSERT_QUERY(std::vector<std::string>({}), index, "whoops")

        ASSERT_QUERY(std::vector<std::string>({}), index, "key222")
        ASSERT_QUERY(std::vector<std::string>({}), index, "key22?")
        ASSERT_QUERY({ "value2" }, index, "key22*")
        ASSERT_QUERY(std::vector<std::string>({ "value", "value2" }), index, "key%%*")
        ASSERT_QUERY(std::vector<std::string>({ "value", "value", "value2" }), index, "key%*")
        ASSERT_QUERY(std::vector<std::string>({ "value", "value", "value2" }), index, "key*")

        ASSERT_QUERY(std::vector<std::string>({ "value", "value", "value2", "value3" }), index, "k*")
        ASSERT_QUERY(std::vector<std::string>({ "value", "value2" }), index, "k*2")

        ASSERT_QUERY(std::vector<std::string>({ "value4" }), index, "test")
        ASSERT_QUERY(std::vector<std::string>({ "value4" }), index, "test*")
        ASSERT_QUERY(std::vector<std::string>({}), index, "test?")
        ASSERT_QUERY(std::vector<std::string>({}), index, "test%")
        ASSERT_QUERY(std::vector<std::string>({ "value4" }), index, "test%*")

        index.insert("k", "value4");

        ASSERT_QUERY(std::vector<std::string>({ "value4" }), index, "k")
        ASSERT_QUERY(std::vector<std::string>({ "value3" }), index, "k%")
        ASSERT_QUERY(std::vector<std::string>({ "value", "value", "value2", "value3", "value4" }), index, "k*")

        ASSERT_QUERY(std::vector<std::string>({ "value", "value", "value2", "value3", "value4", "value4" }), index, "*")
    }

    TEST(glob_index_test, remove) {
        test_index index;
        index.insert("andrew", "value");
        index.insert("andreas", "value");
        index.insert("andrar", "value2");
        index.insert("andrary", "value3");
        index.insert("andy", "value4");

        ASSERT_QUERY(std::vector<std::string>({ "value", "value", "value2", "value3", "value4" }), index, "*")

        ASSERT_FALSE(index.remove("andrary", "value2"));

        ASSERT_TRUE(index.remove("andrary", "value3"));
        ASSERT_QUERY(std::vector<std::string>({}), index, "andrary*")

        ASSERT_QUERY(std::vector<std::string>({ "value2" }), index, "andrar*")
        ASSERT_TRUE(index.remove("andrar", "value2"));
        ASSERT_QUERY(std::vector<std::string>({}), index, "andrar*")

        ASSERT_QUERY(std::vector<std::string>({ "value4" }), index, "andy")
        ASSERT_TRUE(index.remove("andy", "value4"));
        ASSERT_QUERY(std::vector<std::string>({}), index, "andy")

        ASSERT_QUERY(std::vector<std::string>({ "value", "value" }), index, "andre*")
        ASSERT_QUERY(std::vector<std::string>({ "value" }), index, "andreas")
        ASSERT_TRUE(index.remove("andreas", "value"));
        ASSERT_QUERY(std::vector<std::string>({ "value" }), index, "andre*")
        ASSERT_QUERY(std::vector<std::string>({}), index, "andreas")

        ASSERT_QUERY(std::vector<std::string>({ "value" }), index, "andrew")
        ASSERT_TRUE(index.remove("andrew", "value"));
        ASSERT_QUERY(std::vector<std::string>({}), index, "andrew")

        ASSERT_QUERY(std::vector<std::string>({}), index, "*")
    }

    TEST(glob_index_test, query_exact) {
        test_index index;
        index.insert("key", "value");
        index.insert("key2", "value");
        index.insert("key22", "value2");
        index.insert("k1", "value3");

        ASSERT_QUERY(std::vector<std::string>({}), index, "whoops")
        ASSERT_QUERY(std::vector<std::string>({}), index, "key222")
        ASSERT_QUERY(std::vector<std::string>({ "value" }), index, "key")
        ASSERT_QUERY(std::vector<std::string>({}), index, "k")
        ASSERT_QUERY(std::vector<std::string>({ "value3" }), index, "k1")

        index.insert("key", "value4");
        ASSERT_QUERY(std::vector<std::string>({ "value", "value4" }), index, "key")

        ASSERT_QUERY(std::vector<std::string>({}), index, "")
    }

    TEST(glob_index_test, query_wildcards) {
        test_index index;
        index.insert("key", "value");
        index.insert("key2", "value");
        index.insert("key22", "value2");
        index.insert("k1", "value3");
        index.insert("test", "value4");

        ASSERT_QUERY(std::vector<std::string>({}), index, "whoops")
        ASSERT_QUERY(std::vector<std::string>({ "value", "value", "value2" }), index, "k??%*")
        ASSERT_QUERY(std::vector<std::string>({ "value" }), index, "?ey")
        ASSERT_QUERY(std::vector<std::string>({ "value", "value", "value2" }), index, "?ey*")
        ASSERT_QUERY(std::vector<std::string>({ "value", "value", "value2", "value3", "value4" }), index, "?*")
        ASSERT_QUERY(std::vector<std::string>({ "value", "value", "value2", "value3", "value4" }), index, "*??")
        ASSERT_QUERY(std::vector<std::string>({ "value", "value", "value2", "value4" }), index, "*???")
        ASSERT_QUERY(std::vector<std::string>({ "value", "value2" }), index, "k*2")
        ASSERT_QUERY(std::vector<std::string>({ "value", "value", "value2", "value3" }), index, "k*")
        ASSERT_QUERY(std::vector<std::string>({ "value4" }), index, "t??t")
        ASSERT_QUERY(std::vector<std::string>({ "value4" }), index, "t??*")
        ASSERT_QUERY(std::vector<std::string>({ "value4" }), index, "t*")
        ASSERT_QUERY(std::vector<std::string>({ "value4" }), index, "*st")
        ASSERT_QUERY(std::vector<std::string>({ "value4" }), index, "t*t")
        ASSERT_QUERY(std::vector<std::string>({ "value4" }), index, "t??t")

        index.insert("this2345that", "value5");
        ASSERT_QUERY(std::vector<std::string>({ "value5" }), index, "t*%%%%that")
        ASSERT_QUERY(std::vector<std::string>({ "value5" }), index, "t*%*that")
        ASSERT_QUERY(std::vector<std::string>({ "value4", "value5" }), index, "t*%**t")
        ASSERT_QUERY(std::vector<std::string>({ "value4", "value5" }), index, "t*%**")
        ASSERT_QUERY(std::vector<std::string>({ "value4", "value5" }), index, "t*")
        ASSERT_QUERY(std::vector<std::string>({ "value4", "value5" }), index, "t**")
        ASSERT_QUERY(std::vector<std::string>({ "value4", "value5" }), index, "t?*")
        ASSERT_QUERY(std::vector<std::string>({ "value4", "value5" }), index, "t??*")
        ASSERT_QUERY(std::vector<std::string>({ "value4", "value5" }), index, "t???*")
        ASSERT_QUERY(std::vector<std::string>({ "value5" }), index, "t????*")
        ASSERT_QUERY(std::vector<std::string>({}), index, "t*%*")
    }

    TEST(glob_index_test, query_digit_suffix) {
        test_index index;
        index.insert("key", "value");
        index.insert("key2", "value");
        index.insert("key22", "value2");
        index.insert("key22bs", "value4");
        index.insert("k1", "value3");

        ASSERT_QUERY(std::vector<std::string>({}), index, "whoops")
        ASSERT_QUERY(std::vector<std::string>({ "value", "value", "value2" }), index, "key%*")
        ASSERT_QUERY(std::vector<std::string>({ "value", "value2" }), index, "key%%*")
        ASSERT_QUERY(std::vector<std::string>({ "value", "value2" }), index, "key2%*")
        ASSERT_QUERY(std::vector<std::string>({ "value3" }), index, "k%*")

        index.remove("k1", "value3");
        ASSERT_QUERY(std::vector<std::string>({}), index, "k%*")
    }

    TEST(glob_index_test, split_merge_with_numbers) {
        test_index index;
        index.insert("3.67", "value3");
        index.insert("3.6", "value2");
        index.insert("3.5", "value1");

        /*
         The insertion of the given values in the given order results in a tree with the following structure. Note that
         splitting nodes can introduce new numbered values at the new child nodes because a non-number string can have
         a numbered prefix or suffix. This must be considered when nodes are split.

          3.67 no numbered values

          3.6  no numbered values
           |
           7   no numbered values (error)

           3.  no numbered values
          / \
         5   6 no numbered values (error)
             |
             7 no numbered values (error)


         */

        // This statement will attempt to remove "value2" from node '6'. Because '6' is a number, it will consider "value"
        // as numbered at node '6', but the value was not added to the numbered values when the node was split. Thus, an
        // exception will be thrown.
        ASSERT_TRUE(index.remove("3.6", "value2"));
    }
}