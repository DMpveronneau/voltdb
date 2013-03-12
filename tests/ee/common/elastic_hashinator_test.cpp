/* This file is part of VoltDB.
 * Copyright (C) 2008-2013 VoltDB Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */
#include "harness.h"
#include "common/serializeio.h"
#include "common/ElasticHashinator.h"

#include <cfloat>
#include <limits>

using namespace std;
using namespace voltdb;

class ElasticHashinatorTest : public Test {
};

TEST_F(ElasticHashinatorTest, TestMinMaxToken)
{
    boost::scoped_ptr<char> config(new char[4 + (12 * 3)]);
    ReferenceSerializeOutput output(config.get(), 4 + (12 * 3));

    output.writeInt(3);
    output.writeLong(std::numeric_limits<int64_t>::min());
    output.writeInt(0);
    output.writeLong(0);
    output.writeInt(1);
    output.writeLong(std::numeric_limits<int64_t>::max());
    output.writeInt(2);

    boost::scoped_ptr<ElasticHashinator> hashinator(ElasticHashinator::newInstance(config.get()));
    EXPECT_EQ( 0, hashinator->partitionForToken(std::numeric_limits<int64_t>::min()));
    EXPECT_EQ( 0, hashinator->partitionForToken(std::numeric_limits<int64_t>::min() + 1));

    EXPECT_EQ( 1, hashinator->partitionForToken(0));
    EXPECT_EQ( 1, hashinator->partitionForToken(1));

    EXPECT_EQ( 2, hashinator->partitionForToken(std::numeric_limits<int64_t>::max()));
    EXPECT_EQ( 1, hashinator->partitionForToken(std::numeric_limits<int64_t>::max() - 1));

    output.initializeWithPosition(config.get(), 4 + (12 * 3), 0);

    output.writeInt(3);
    output.writeLong(std::numeric_limits<int64_t>::min() + 1);
    output.writeInt(0);
    output.writeLong(0);
    output.writeInt(1);
    output.writeLong(std::numeric_limits<int64_t>::max() - 1);
    output.writeInt(2);

    hashinator.reset(ElasticHashinator::newInstance(config.get()));

    EXPECT_EQ( 2, hashinator->partitionForToken(std::numeric_limits<int64_t>::min()));
    EXPECT_EQ( 0, hashinator->partitionForToken(std::numeric_limits<int64_t>::min() + 1));

    EXPECT_EQ( 1, hashinator->partitionForToken(0));
    EXPECT_EQ( 1, hashinator->partitionForToken(1));

    EXPECT_EQ( 2, hashinator->partitionForToken(std::numeric_limits<int64_t>::max()));
    EXPECT_EQ( 2, hashinator->partitionForToken(std::numeric_limits<int64_t>::max() - 1));
}

int main() {
    return TestSuite::globalInstance()->runAll();
}
