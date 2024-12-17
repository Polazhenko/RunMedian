#include <iostream>
#include "include/runmedian.hpp"
#include <gtest/gtest.h>

#define RANDOM50() (uint16_t)(rand() % 51)
#define RANDOM3000() (uint16_t)(rand() % 3001)

#define GTEST_COUT std::cerr << "[          ] [ ERROR ]"

void HandleError() { std::cerr << "HandleError was called"; }

TEST(RunMedianTests, Size) {
    common::Runmedian<uint16_t, 5> q{};
    q.RegisterCallbacks(HandleError);

    ASSERT_EQ(0U, q.Size());
    q.Clear();
    ASSERT_EQ(0U, q.Size());

    q.Add(RANDOM50());
    ASSERT_EQ(1U, q.Size());

    q.Add(RANDOM50());
    q.Add(RANDOM50());
    q.Add(RANDOM50());
    q.Add(RANDOM50());
    q.Add(RANDOM50());
    ASSERT_TRUE(q._check_integrity());
    ASSERT_EQ(5U, q.Size());
    std::cerr << "Running median: " << q.Value() << std::endl;

    q.Add(RANDOM50());
    ASSERT_TRUE(q._check_integrity());
    ASSERT_EQ(5U, q.Size());
    std::cerr << "Running median: " << q.Value() << std::endl;

    q.Add(RANDOM50());
    ASSERT_TRUE(q._check_integrity());
    std::cerr << "Running median: " << q.Value() << std::endl;

    q.Clear();
    ASSERT_EQ(0U, q.Size());
    ASSERT_EQ(0U, q.Value());
}

TEST(RunMedianTests, Median4TestGrowing) {
    common::Runmedian<uint16_t, 5> q{};
    q.RegisterCallbacks(HandleError);

    q.Add(1);
    ASSERT_EQ(1, q.Value());

    q.Add(2);
    ASSERT_EQ(1, q.Value());

    q.Add(3);
    ASSERT_EQ(2, q.Value());

    q.Add(4);
    ASSERT_EQ(2, q.Value());

    q.Add(5);
    ASSERT_EQ(3, q.Value());
    ASSERT_TRUE(q._check_integrity());

    q.Add(6);
    ASSERT_EQ(4, q.Value());
    ASSERT_TRUE(q._check_integrity());

    q.Add(7);
    ASSERT_EQ(5, q.Value());
    ASSERT_TRUE(q._check_integrity());

    q.Clear();
    ASSERT_EQ(0U, q.Size());
    ASSERT_EQ(0U, q.Value());
}

TEST(RunMedianTests, Median4TestDecreasing) {
    common::Runmedian<uint16_t, 5> q{};
    q.RegisterCallbacks(HandleError);

    q.Add(7);
    ASSERT_EQ(7, q.Value());

    q.Add(6);
    ASSERT_EQ(6, q.Value());

    q.Add(5);
    ASSERT_EQ(6, q.Value());

    q.Add(4);
    ASSERT_EQ(5, q.Value());

    q.Add(3);
    ASSERT_EQ(5, q.Value());
    ASSERT_TRUE(q._check_integrity());

    q.Add(2);
    ASSERT_EQ(4, q.Value());
    ASSERT_TRUE(q._check_integrity());

    q.Add(1);
    ASSERT_EQ(3, q.Value());
    ASSERT_TRUE(q._check_integrity());

    q.Clear();
    ASSERT_EQ(0U, q.Size());
    ASSERT_EQ(0U, q.Value());
}

TEST(RunMedianTests, Median5TestDiff) {
    common::Runmedian<uint16_t, 5> q{};
    q.RegisterCallbacks(HandleError);

    q.Add(7);
    ASSERT_EQ(7, q.Value());

    q.Add(1);
    // std::cerr << q.Value() << std::endl;
    ASSERT_EQ(4, q.Value());

    q.Add(5);
    ASSERT_EQ(5, q.Value());

    q.Add(2);
    ASSERT_EQ(3, q.Value());

    q.Add(6);
    ASSERT_EQ(5, q.Value());
    ASSERT_TRUE(q._check_integrity());

    q.Add(3);
    ASSERT_EQ(3, q.Value());
    ASSERT_TRUE(q._check_integrity());

    q.Add(5);
    ASSERT_EQ(5, q.Value());
    ASSERT_EQ(5U, q.Size());
    ASSERT_TRUE(q._check_integrity());

    q.Add(10);
    ASSERT_EQ(5, q.Value());
    ASSERT_EQ(5U, q.Size());
    ASSERT_TRUE(q._check_integrity());

    q.Add(1);
    ASSERT_EQ(5, q.Value());
    ASSERT_EQ(5U, q.Size());
    ASSERT_TRUE(q._check_integrity());

    q.Add(4);
    ASSERT_EQ(4, q.Value());
    ASSERT_EQ(5U, q.Size());
    ASSERT_TRUE(q._check_integrity());

    q.Clear();
    ASSERT_EQ(0U, q.Size());
    ASSERT_EQ(0U, q.Value());
}

TEST(RunMedianTests, RandomTest) {
    common::Runmedian<uint16_t, 19> q{};
    q.RegisterCallbacks(HandleError);

    for (int i = 0; i < 2000; i++) {
        q.Add(RANDOM3000());
        ASSERT_TRUE(q._check_integrity());
    }
}

TEST(RunMedianTests, FullArrayTest) {
    common::Runmedian<uint16_t, 5> q{};
    q.RegisterCallbacks(HandleError);

    q.Add(7);
    ASSERT_EQ(7, q.Value());

    q.Add(1);
    // std::cerr << q.Value() << std::endl;
    ASSERT_EQ(4, q.Value());

    q.Add(5);
    ASSERT_EQ(5, q.Value());
    ASSERT_EQ(3, q.Size());

    q.Add(4);
    ASSERT_EQ(4, q.Value());

    q.Add(3);
    ASSERT_EQ(4, q.Value());
    ASSERT_TRUE(q._check_integrity());

    q.Add(2);
    ASSERT_EQ(3, q.Value());
    ASSERT_TRUE(q._check_integrity());

    q.Add(6);
    ASSERT_EQ(4, q.Value());
    ASSERT_TRUE(q._check_integrity());

    q.Add(7);
    ASSERT_EQ(4, q.Value());
    ASSERT_TRUE(q._check_integrity());

    q.Add(8);
    ASSERT_EQ(6, q.Value());
    ASSERT_TRUE(q._check_integrity());

    q.Add(2);
    ASSERT_EQ(6, q.Value());
    ASSERT_TRUE(q._check_integrity());

    q.Add(10);
    ASSERT_EQ(7, q.Value());
    ASSERT_TRUE(q._check_integrity());

    q.Add(1);
    ASSERT_EQ(7, q.Value());

    q.Clear();
    ASSERT_EQ(0U, q.Size());
    ASSERT_EQ(0U, q.Value());
}

TEST(RunMedianTests, DeleteAll) {
    common::Runmedian<uint16_t, 5> q{};
    q.RegisterCallbacks(HandleError);

    ASSERT_EQ(0U, q.Size());

    q.Add(RANDOM50());
    q.Add(RANDOM50());
    q.Add(RANDOM50());
    q.Add(RANDOM50());
    q.Add(RANDOM50());
    ASSERT_TRUE(q._check_integrity());

    ASSERT_EQ(5U, q.Size());
    std::cerr << "Running median: " << q.Value() << std::endl;

    q.Add(RANDOM50());
    ASSERT_EQ(5U, q.Size());
    ASSERT_TRUE(q._check_integrity());
    std::cerr << "Running median: " << q.Value() << std::endl;

    q.Clear();
    ASSERT_EQ(0U, q.Size());
    ASSERT_EQ(0U, q.Value());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    srand((unsigned int)time(NULL));
    return RUN_ALL_TESTS();
}

//int main()
//{
//    common::Runmedian<int, 8> rm;
//    rm.Add(1);
//    std::cout << "Hello World!\n" << rm.Value() << std::endl;
//}
