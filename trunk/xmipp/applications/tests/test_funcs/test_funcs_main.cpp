#include <data/xmipp_funcs.h>
#include <iostream>
#include "../../../external/gtest-1.6.0/fused-src/gtest/gtest.h"
// MORE INFO HERE: http://code.google.com/p/googletest/wiki/AdvancedGuide
// This test is named "Size", and belongs to the "MetadataTest"
// test case.
class FuncTest : public ::testing::Test
{
protected:
    //init metadatas
    virtual void SetUp()
    {
#define len 128
        //find binaries directory
        char szTmp[len];
        char pBuf[len];
        sprintf(szTmp, "/proc/%d/exe", getpid());
        int bytes = std::min(readlink(szTmp, pBuf, len), (ssize_t)len - 1);
        if(bytes >= 0)
            pBuf[bytes] = '\0';
        //remove last token
        FileName filename(pBuf);
        filename = filename.removeFilename();
        //get example images/staks
        source1 = filename + "/../applications/tests/test_funcs/singleImage.spi";
        source2 = filename + "/../applications/tests/test_funcs/singleImage.mrc";
    }

    // virtual void TearDown() {}//Destructor
    FileName source1;
    FileName source2;
};


TEST_F( FuncTest, CompareTwoFiles)
{
    ASSERT_TRUE(compareTwoFiles(source1,source1,0));
    ASSERT_FALSE(compareTwoFiles(source1,source2,0));
}


GTEST_API_ int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
