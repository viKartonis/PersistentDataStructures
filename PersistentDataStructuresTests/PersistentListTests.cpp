#include <gtest/gtest.h>
#include <PersistentList.h>

namespace {
    using namespace pds;
    using namespace std;

    TEST(PListCreation, Creation)
    {
        pds::persistent_linked_list<int> list;

        auto b = list.push_back(1);
        EXPECT_EQ(b.back(), 1);
        EXPECT_EQ(b.size(), 1);

        auto c = b.push_front(12);
        EXPECT_EQ( c.size(), 2);
        EXPECT_EQ(b.front(), 1);
        EXPECT_EQ( c.front(), 12);

        auto c1 = b.set(0, 123);
        EXPECT_EQ( c1.front(), 123);

        EXPECT_EQ(list.size(), 0);
        EXPECT_TRUE(list.empty());
    }

    TEST(PListCreation, EmptyCopyCreation) {
        persistent_linked_list<int> pList_base;
        persistent_linked_list<int> pList = pList_base;
        EXPECT_EQ(pList.size(), 0);
        EXPECT_TRUE(pList.empty());
    }

    TEST(PListCreation, EmptyMoveCreation) {
        persistent_linked_list<int> pList_base;
        persistent_linked_list<int> pList = std::move(pList_base);
        EXPECT_EQ(pList.size(), 0);
        EXPECT_TRUE(pList.empty());
    }
}
