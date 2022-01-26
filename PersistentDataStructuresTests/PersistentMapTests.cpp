#include <gtest/gtest.h>
#include <PersistentMap.h>
#include <thread>
#include <chrono>


namespace {
	using namespace pds;
	using namespace std;

	/*
	*	Creation
	*/

	TEST(PMapCreation, Creation) {
		PersistentMap<size_t, size_t> pmap;
		EXPECT_EQ(pmap.size(), 0);
		EXPECT_TRUE(pmap.empty());
	}

	TEST(PMapCreation, EmptyCopyCreation) {
		PersistentMap<size_t, size_t> pmap_base;
		PersistentMap<size_t, size_t> pmap = pmap_base;
		EXPECT_EQ(pmap.size(), 0);
		EXPECT_TRUE(pmap.empty());
	}

	TEST(PMapCreation, EmptyMoveCreation) {
		PersistentMap<size_t, size_t> pmap_base;
		PersistentMap<size_t, size_t> pmap = std::move(pmap_base);
		EXPECT_EQ(pmap.size(), 0);
		EXPECT_TRUE(pmap.empty());
	}


	/*
	*	Setting
	*/

	TEST(PMapSetting, OnlyOne) {
		PersistentMap<size_t, size_t> pmap;
		EXPECT_TRUE(pmap.empty());
		EXPECT_EQ(pmap.size(), 0);
		pmap = pmap.set(0, 0);
		EXPECT_FALSE(pmap.empty());
		EXPECT_EQ(pmap.size(), 1);
	}

	TEST(PMapSetting, Some) {
		PersistentMap<size_t, size_t> pmap;
		constexpr size_t size = 1 << 5;
		for (size_t i = 0; i < size; ++i) {
			pmap = pmap.set(i, i);
			EXPECT_EQ(pmap.size(), i + 1);
		}
	}

	TEST(PMapSetting, OneKey) {
		PersistentMap<size_t, size_t> pmap;
		constexpr size_t size = 1 << 5;
		for (size_t i = 0; i < size; ++i) {
			pmap = pmap.set(0, i);
			EXPECT_EQ(pmap.size(), 1);
		}
	}

	TEST(PMapSetting, SomeWithCollision) {
		constexpr size_t size = 1 << 5;
		PersistentMap<size_t, size_t> pmap(8*size);
		for (size_t i = 0; i < size; ++i) {
			pmap = pmap.set(i + 8*size, i);
			EXPECT_EQ(pmap.size(), i + 1);
		}
	}

	TEST(PMapSetting, KeyIsUnique) {
		constexpr size_t size = 1 << 5;
		PersistentMap<size_t, size_t> pmap;
		for (size_t i = 0; i < size; ++i) {
			pmap = pmap.set(0, i);
			EXPECT_EQ(pmap.size(), 1);
		}
	}

	TEST(PMapSetting, Huge) {
		constexpr size_t size = 1 << 13;
		PersistentMap<size_t, size_t> pmap;
		for (size_t i = 0; i < size; ++i) {
			pmap = pmap.set(i, i);
			EXPECT_EQ(pmap.size(), i + 1);
		}
		for (size_t i = 0; i < size >> 3; ++i) {
			pmap = pmap.set(i, 0);
			EXPECT_EQ(pmap.size(), size);
		}
	}


	/*
	*	Getting
	*/

	TEST(PMapGetting, OnlyOneElement) {
		PersistentMap<size_t, size_t> pmap;
		pmap = pmap.set(5, 13);
		EXPECT_EQ(pmap[5], 13);
	}

	TEST(PMapGetting, Some) {
		PersistentMap<size_t, size_t> pmap;
		constexpr size_t size = (1 << 5);
		for (size_t i = 0; i < size; ++i) {
			pmap = pmap.set(i, i);
			for (size_t j = 0; j <= i; ++j) {
				EXPECT_EQ(pmap[j], j);
			}
		}
		for (size_t i = 0; i < size; ++i) {
			EXPECT_EQ(pmap[i], i);
		}
	}

	TEST(PMapGetting, SomeWithCollision) {
		PersistentMap<size_t, size_t> pmap(512);
		constexpr size_t size = (1 << 5);
		for (size_t i = 0; i < size; ++i) {
			pmap = pmap.set(i, i);
			pmap = pmap.set(i + 512, i);
			for (size_t j = 0; j <= i; ++j) {
				EXPECT_EQ(pmap[j], j);
				EXPECT_EQ(pmap[j + 512], j);
			}
		}
		for (size_t i = 0; i < size; ++i) {
			EXPECT_EQ(pmap[i], i);
			EXPECT_EQ(pmap[i + 512], i);
		}
	}

	TEST(PMapGetting, SomeWithResetting) {
		PersistentMap<size_t, size_t> pmap(512);
		constexpr size_t size = (1 << 5);
		for (size_t i = 0; i < size; ++i) {
			pmap = pmap.set(i, 0);
			for (size_t j = 0; j <= i; ++j) {
				EXPECT_EQ(pmap[j], 0);
			}
		}
		for (size_t i = 0; i < size; ++i) {
			pmap = pmap.set(i, i);
			for (size_t j = 0; j <= i; ++j) {
				EXPECT_EQ(pmap[j], j);
			}
		}
		for (size_t i = 0; i < size; ++i) {
			EXPECT_EQ(pmap[i], i);
		}
	}

	TEST(PMapGetting, Huge) {
		PersistentMap<size_t, size_t> pmap;
		constexpr size_t size = (1 << 13);
		for (size_t i = 0; i < size; ++i) {
			pmap = pmap.set(i, 0);
			pmap = pmap.set(i, i);
		}
		for (size_t i = 0; i < size; ++i) {
			EXPECT_EQ(pmap[i], i);
		}
	}


	/*
	*	Extended creation
	*/

	TEST(PMamIteratorCreation, NoOne) {
		vector<pair<size_t, size_t>> v;
		PersistentMap<size_t, size_t> pmap(v.cbegin(), v.cend(), 16);
		EXPECT_EQ(pmap.size(), 0);
		EXPECT_TRUE(pmap.empty());
	}

	TEST(PMamIteratorCreation, OnlyOne) {
		vector<pair<size_t, size_t>> v = { { 3, 12 } };
		PersistentMap<size_t, size_t> pmap(v.cbegin(), v.cend(), 16);
		EXPECT_EQ(pmap.size(), 1);
		EXPECT_EQ(pmap[3], 12);
	}

	TEST(PMamIteratorCreation, Some) {
		vector<pair<size_t, size_t>> v = { { 3, 12 }, { 8, 22 }, { 400, 65 }, { 35453, 0 }, { 555, 555 } };
		PersistentMap<size_t, size_t> pmap(v.cbegin(), v.cend(), 16);
		EXPECT_EQ(pmap.size(), 5);
		EXPECT_EQ(pmap[3], 12);
		EXPECT_EQ(pmap[8], 22);
		EXPECT_EQ(pmap[400], 65);
		EXPECT_EQ(pmap[35453], 0);
		EXPECT_EQ(pmap[555], 555);
	}

	TEST(PMamIteratorCreation, Huge) {
		constexpr size_t size = (1 << 15);
		vector<pair<size_t, size_t>> v;
		for (size_t i = 0; i < size; ++i) {
			v.emplace_back(pair<size_t, size_t>(i, i + 1));
		}
		PersistentMap<size_t, size_t> pmap(v.cbegin(), v.cend(), 2*size);
		EXPECT_EQ(pmap.size(), size);
		for (size_t i = 0; i < size; ++i) {
			EXPECT_EQ(pmap[i], i + 1);
		}
	}

	TEST(PMamInitListCreation, NoOne) {
		PersistentMap<size_t, size_t> pmap({  }, 16);
		EXPECT_EQ(pmap.size(), 0);
		EXPECT_TRUE(pmap.empty());
	}

	TEST(PVectorIteratorCreation, OnlyOne) {
		PersistentMap<size_t, size_t> pmap({ { 3, 12 }, { 8, 22 }, { 400, 65 }, { 35453, 0 }, { 555, 555 } }, 16);
		EXPECT_EQ(pmap.size(), 5);
		EXPECT_EQ(pmap[3], 12);
		EXPECT_EQ(pmap[8], 22);
		EXPECT_EQ(pmap[400], 65);
		EXPECT_EQ(pmap[35453], 0);
		EXPECT_EQ(pmap[555], 555);
	}
	namespace {
		struct MyHash {
			std::size_t operator()(size_t key) const {
				return key;
			}
		};

		TEST(PVectorOwnHash, IteratorCreation) {
			vector<pair<size_t, size_t>> v = { { 3, 12 }, { 8, 22 }, { 400, 65 }, { 35453, 0 }, { 555, 555 } };
			PersistentMap<size_t, size_t, MyHash> pmap(v.cbegin(), v.cend(), 16, MyHash());
			EXPECT_EQ(pmap.size(), 5);
			EXPECT_EQ(pmap[3], 12);
			EXPECT_EQ(pmap[8], 22);
			EXPECT_EQ(pmap[400], 65);
			EXPECT_EQ(pmap[35453], 0);
			EXPECT_EQ(pmap[555], 555);
		}

		TEST(PVectorOwnHash, InitListCreation) {
			vector<pair<size_t, size_t>> v = { { 3, 12 }, { 8, 22 }, { 400, 65 }, { 35453, 0 }, { 555, 555 } };
			PersistentMap<size_t, size_t, MyHash> pmap(v.cbegin(), v.cend(), 16, MyHash());
			EXPECT_EQ(pmap.size(), 5);
			EXPECT_EQ(pmap[3], 12);
			EXPECT_EQ(pmap[8], 22);
			EXPECT_EQ(pmap[400], 65);
			EXPECT_EQ(pmap[35453], 0);
			EXPECT_EQ(pmap[555], 555);
		}
	}


	/*
	*	Erasing
	*/

	TEST(PVectorErasing, Existed) {
		vector<pair<size_t, size_t>> v = { { 0, 0 }, { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 4 } };
		PersistentMap<size_t, size_t, MyHash> pmap(v.cbegin(), v.cend(), 16, MyHash());
		size_t size = 5;
		for (size_t j = 0; j < 5; ++j, --size) {
			EXPECT_EQ(pmap.size(), size);
			for (size_t i = 0; i < size; ++i) {
				EXPECT_EQ(pmap[i], i);
			}
			pmap = pmap.erase(4 - j);
		}
		EXPECT_TRUE(pmap.empty());
	}

	TEST(PVectorErasing, NotExisted) {
		vector<pair<size_t, size_t>> v = { { 0, 0 }, { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 4 } };
		PersistentMap<size_t, size_t, MyHash> pmap(v.cbegin(), v.cend(), 16, MyHash());
		for (size_t j = 5; j < 10; ++j) {
			EXPECT_THROW(pmap.erase(j), out_of_range);
			EXPECT_EQ(pmap.size(), 5);
		}
	}


	/*
	*	Iterators
	*/


	TEST(PMapIterator, Empty) {
		PersistentMap<size_t, size_t, MyHash> pmap;
		EXPECT_TRUE(pmap.cbegin() == pmap.cend());
	}

	TEST(PMapIterator, OnlyOne) {
		PersistentMap<size_t, size_t, MyHash> pmap = { { 12345, 12345 } };

		EXPECT_TRUE(pmap.cbegin() != pmap.cend());
		auto it = pmap.cbegin();
		EXPECT_EQ(it->second, 12345);
		EXPECT_TRUE(++it == pmap.cend());
	}

	TEST(PMapIterator, Some) {
		constexpr size_t size = 10;
		PersistentMap<size_t, size_t, MyHash> pmap;
		for (size_t i = 0; i < size; ++i) {
			pmap = pmap.set(12345 + i, 54321);
		}
		EXPECT_TRUE(pmap.cbegin() != pmap.cend());
		for (auto it = pmap.cbegin(); it != pmap.cend(); ++it) {
			EXPECT_EQ(it->second, 54321);
		}
	}

	TEST(PMapIterator, Huge) {
		constexpr size_t size = (1 << 13) + 1;
		PersistentMap<size_t, size_t, MyHash> pmap;
		for (size_t i = 0; i < size; ++i) {
			pmap = pmap.set(12345 + i, 54321);
		}
		EXPECT_TRUE(pmap.cbegin() != pmap.cend());
		for (auto it = pmap.cbegin(); it != pmap.cend(); ++it) {
			EXPECT_EQ(it->second, 54321);
		}
	}

	TEST(PVectorIterator, Comparation) {
		constexpr size_t size = 10;
		PersistentMap<size_t, size_t, MyHash> pmap;
		for (size_t i = 0; i < size; ++i) {
			pmap = pmap.set(12345 + i, 54321 * i);
		}
		EXPECT_TRUE(pmap.cbegin() != pmap.cend());
		auto it1 = pmap.cbegin();
		auto it2 = pmap.cbegin();
		EXPECT_TRUE(pmap.cbegin() != pmap.cend());
		EXPECT_FALSE(pmap.cbegin() == pmap.cend());
		EXPECT_TRUE(it1 == it2);
		EXPECT_FALSE(it1 != it2);
		++it1;
		EXPECT_FALSE(it1 == it2);
		EXPECT_TRUE(it1 != it2);
		++it2;
		EXPECT_TRUE(it1 == it2);
		EXPECT_FALSE(it1 != it2);
		++it2;
		EXPECT_FALSE(it1 == it2);
		EXPECT_TRUE(it1 != it2);


		EXPECT_TRUE(it1 != pmap.cbegin());
		EXPECT_TRUE(it1 != pmap.cend());
		EXPECT_TRUE(it2 != pmap.cbegin());
		EXPECT_TRUE(it2 != pmap.cend());
	}


	/*
	*
	*	Comparation
	*
	*/

	TEST(PMapComparation, Empty) {
		PersistentMap<size_t, size_t, MyHash> pmap;
		PersistentMap<size_t, size_t, MyHash> pmap1;
		PersistentMap<size_t, size_t, MyHash> pmap2 = pmap;
		EXPECT_TRUE(pmap == pmap);
		EXPECT_FALSE(pmap != pmap);
		EXPECT_TRUE(pmap == pmap1);
		EXPECT_FALSE(pmap != pmap1);
		EXPECT_TRUE(pmap == pmap2);
		EXPECT_FALSE(pmap != pmap2);
	}

	TEST(PMapComparation, NonEmptyConstructed) {
		PersistentMap<size_t, size_t, MyHash> pmap = { { 0, 0 }, { 1, 2 }, { 2, 4 } };
		PersistentMap<size_t, size_t, MyHash> pmap1 = { { 0, 0 }, { 2, 4 }, { 1, 2 } };
		PersistentMap<size_t, size_t, MyHash> pmap2 = pmap;
		EXPECT_TRUE(pmap == pmap);
		EXPECT_FALSE(pmap != pmap);
		EXPECT_TRUE(pmap == pmap1);
		EXPECT_FALSE(pmap != pmap1);
		EXPECT_TRUE(pmap == pmap2);
		EXPECT_FALSE(pmap != pmap2);
	}

	TEST(PMapComparation, SameLength) {
		constexpr size_t size = (1 << 13) + 1;
		PersistentMap<size_t, size_t, MyHash> pmap;
		PersistentMap<size_t, size_t, MyHash> pmap1;
		for (size_t i = 0; i < size; ++i) {
			pmap = pmap.set(i, i + 1);
			pmap1 = pmap1.set(i, i + 1);
		}
		EXPECT_TRUE(pmap == pmap1);
		EXPECT_FALSE(pmap != pmap1);
		auto pmap2 = pmap1.set(size - 1, 54321);
		EXPECT_TRUE(pmap == pmap1);
		EXPECT_FALSE(pmap != pmap1);
		EXPECT_FALSE(pmap == pmap2);
		EXPECT_TRUE(pmap != pmap2);
		pmap2 = pmap2.set(size - 1, size);
		EXPECT_TRUE(pmap2 == pmap1);
		EXPECT_FALSE(pmap2 != pmap1);
		EXPECT_TRUE(pmap2 == pmap);
		EXPECT_FALSE(pmap2 != pmap);
		pmap2 = pmap2.set(size - 5, 54321);
		EXPECT_TRUE(pmap == pmap1);
		EXPECT_FALSE(pmap != pmap1);
		EXPECT_FALSE(pmap == pmap2);
		EXPECT_TRUE(pmap != pmap2);
		pmap2 = pmap2.set(size - 5, size - 4);
		EXPECT_TRUE(pmap2 == pmap1);
		EXPECT_FALSE(pmap2 != pmap1);
		EXPECT_TRUE(pmap2 == pmap);
		EXPECT_FALSE(pmap2 != pmap);
	}

	TEST(PMapComparation, DifferentLength) {
		constexpr size_t size = (1 << 13) + 1;
		PersistentMap<size_t, size_t, MyHash> pmap;
		PersistentMap<size_t, size_t, MyHash> pmap1;
		for (size_t i = 0; i < size - 1; ++i) {
			pmap = pmap.set(i, i + 1);
			pmap1 = pmap1.set(i, i + 1);
		}
		pmap = pmap.set(size - 1, size);
		EXPECT_FALSE(pmap == pmap1);
		EXPECT_TRUE(pmap != pmap1);
		auto pmap2 = pmap1.set(size - 2, 54321);
		EXPECT_FALSE(pmap == pmap1);
		EXPECT_TRUE(pmap != pmap1);
		EXPECT_FALSE(pmap1 == pmap2);
		EXPECT_TRUE(pmap1 != pmap2);
		pmap2 = pmap2.set(size - 2, size - 1);
		EXPECT_FALSE(pmap == pmap1);
		EXPECT_TRUE(pmap != pmap1);
		EXPECT_TRUE(pmap1 == pmap2);
		EXPECT_FALSE(pmap1 != pmap2);
	}


	/*
	*
	*	Swap
	*
	*/

	TEST(PMapSwap, SwapItself) {
		PersistentMap<size_t, size_t, MyHash> pmap = { { 0, 0 }, { 1, 2 }, { 2, 4 } };
		pmap.swap(pmap);
		EXPECT_TRUE(pmap == pmap);
		std::swap(pmap, pmap);
		EXPECT_TRUE(pmap == pmap);
	}

	TEST(PMapSwap, SwapDifferent) {
		PersistentMap<size_t, size_t, MyHash> pmap1 = { { 0, 0 }, { 1, 2 }, { 2, 4 } };
		PersistentMap<size_t, size_t, MyHash> pmap2 = { { 0, 4 }, { 1, 2 }, { 2, 0 } };
		PersistentMap<size_t, size_t, MyHash> pmap1_copy = pmap1;
		PersistentMap<size_t, size_t, MyHash> pmap2_copy = pmap2;
		EXPECT_TRUE(pmap1 != pmap2);
		EXPECT_TRUE(pmap1 == pmap1_copy);
		EXPECT_TRUE(pmap2 == pmap2_copy);
		pmap1.swap(pmap2);
		EXPECT_TRUE(pmap1 != pmap2);
		EXPECT_TRUE(pmap2 == pmap1_copy);
		EXPECT_TRUE(pmap1 == pmap2_copy);
		std::swap(pmap1, pmap2);
		EXPECT_TRUE(pmap1 != pmap2);
		EXPECT_TRUE(pmap1 == pmap1_copy);
		EXPECT_TRUE(pmap2 == pmap2_copy);
	}


	/*
	*	Clear
	*/

	TEST(PMapClear, Empty) {
		PersistentMap<size_t, size_t, MyHash> pmap;
		EXPECT_TRUE(pmap.empty());
		auto pmap1 = pmap.clear();
		EXPECT_TRUE(pmap.empty());
		EXPECT_TRUE(pmap1.empty());
	}

	TEST(PMapClear, One) {
		PersistentMap<size_t, size_t, MyHash> pmap;
		pmap = pmap.set(123, 123);
		auto pmap1 = pmap.clear();
		EXPECT_EQ(pmap.size(), 1);
		EXPECT_TRUE(pmap1.empty());
	}

	TEST(PMapClear, Some) {
		constexpr size_t size = 6;
		PersistentMap<size_t, size_t, MyHash> pmap;
		for (size_t i = 0; i < size; ++i) {
			pmap = pmap.set(i, i);
		}
		auto pmap1 = pmap.clear();
		EXPECT_EQ(pmap.size(), size);
		EXPECT_TRUE(pmap1.empty());
	}

	TEST(PMapClear, Huge) {
		constexpr size_t size = 1 << 13;
		PersistentMap<size_t, size_t, MyHash> pmap;
		for (size_t i = 0; i < size; ++i) {
			pmap = pmap.set(i, i);
		}
		auto pmap1 = pmap.clear();
		EXPECT_EQ(pmap.size(), size);
		EXPECT_TRUE(pmap1.empty());
	}


	/*
	*	Searching
	*/

	TEST(PMapSearching, Empty) {
		PersistentMap<size_t, size_t, MyHash> pmap;
		EXPECT_EQ(pmap.find(0), pmap.cend());
	}

	TEST(PMapSearching, Exists) {
		PersistentMap<size_t, size_t, MyHash> pmap;
		pmap = pmap.set(123, 123);
		EXPECT_EQ(pmap.find(123), pmap.cbegin());
	}

	TEST(PMapSearching, DoesntExist) {
		PersistentMap<size_t, size_t, MyHash> pmap;
		pmap = pmap.set(123, 123);
		EXPECT_EQ(pmap.find(0), pmap.cend());
	}

	TEST(PMapSearching, ExistsHuge) {
		constexpr size_t size = 1 << 13;
		PersistentMap<size_t, size_t, MyHash> pmap;
		for (size_t i = 0; i < size; ++i) {
			pmap = pmap.set(i, i);
		}
		auto it = pmap.cbegin();
		++(++(++it));
		EXPECT_EQ(pmap.find(it->first), it);
	}

	TEST(PMapSearching, DoesntExistHuge) {
		constexpr size_t size = 1 << 13;
		PersistentMap<size_t, size_t, MyHash> pmap;
		for (size_t i = 0; i < size; ++i) {
			pmap = pmap.set(i, i);
		}
		EXPECT_EQ(pmap.find(size + 1), pmap.cend());
	}


	/*
	*	UNDO & REDO
	*/

	TEST(PMapUndoRedo, linear) {
		PersistentMap<size_t, size_t, MyHash> pmap;
		constexpr size_t size = (1 << 5);
		for (size_t i = 0; i < size; ++i) {
			pmap = pmap.set(i, i);
		}
		auto pmap1 = pmap;
		for (size_t i = 0; i < size; ++i) {
			EXPECT_EQ(pmap1.size(), size - i);
			EXPECT_EQ(pmap1[size - i - 1], size - i - 1);
			EXPECT_THROW(pmap1.at(size - i), out_of_range);
			pmap1 = pmap1.undo();
			EXPECT_EQ(pmap1.size(), size - i - 1);
			EXPECT_THROW(pmap1.at(size - i - 1), out_of_range);
		}
		EXPECT_TRUE(pmap1.empty());
		for (size_t i = 0; i < size; ++i) {
			EXPECT_EQ(pmap1.size(), i);
			EXPECT_THROW(pmap1.at(i), out_of_range);
			pmap1 = pmap1.redo();
			EXPECT_EQ(pmap1[i], i);
			EXPECT_EQ(pmap1.size(), i + 1);
		}
	}

	TEST(PMapUndoRedo, linearHuge) {
		PersistentMap<size_t, size_t, MyHash> pmap;
		constexpr size_t size = (1 << 10);
		for (size_t i = 0; i < size; ++i) {
			pmap = pmap.set(i, i);
		}
		auto pmap1 = pmap;
		for (size_t i = 0; i < size; ++i) {
			EXPECT_EQ(pmap1.size(), size - i);
			EXPECT_EQ(pmap1[size - i - 1], size - i - 1);
			EXPECT_THROW(pmap1.at(size - i), out_of_range);
			pmap1 = pmap1.undo();
			EXPECT_EQ(pmap1.size(), size - i - 1);
			EXPECT_THROW(pmap1.at(size - i - 1), out_of_range);
		}
		EXPECT_TRUE(pmap1.empty());
		for (size_t i = 0; i < size; ++i) {
			EXPECT_EQ(pmap1.size(), i);
			EXPECT_THROW(pmap1.at(i), out_of_range);
			pmap1 = pmap1.redo();
			EXPECT_EQ(pmap1[i], i);
			EXPECT_EQ(pmap1.size(), i + 1);
		}
	}

	TEST(PMapUndoRedo, linearHugeDestruction) {
		PersistentMap<size_t, size_t, MyHash> pmap;
		constexpr size_t size = (1 << 10);
		for (size_t i = 0; i < size; ++i) {
			pmap = pmap.set(i, i);
		}
		for (size_t i = 0; i < size; ++i) {
			EXPECT_EQ(pmap.size(), size - i);
			EXPECT_EQ(pmap[size - i - 1], size - i - 1);
			EXPECT_THROW(pmap.at(size - i), out_of_range);
			pmap = pmap.undo();
			EXPECT_EQ(pmap.size(), size - i - 1);
			EXPECT_THROW(pmap.at(size - i - 1), out_of_range);
		}
		pmap = pmap.set(0, 0);
		EXPECT_EQ(pmap[0], 0);
		EXPECT_THROW(pmap.at(1), out_of_range);
	}

	TEST(PMapUndoRedo, LinearWithUndoRedo) {
		PersistentMap<size_t, size_t, MyHash> pmap;
		constexpr size_t size = (1 << 5);
		for (size_t i = 0; i < size; ++i) {
			pmap = pmap.set(i, i);
			pmap = pmap.undo();
			EXPECT_EQ(pmap.size(), i);
			EXPECT_TRUE(pmap.empty() || pmap[i - 1] == i - 1);
			pmap = pmap.redo();
			EXPECT_EQ(pmap.size(), i + 1);
			EXPECT_EQ(pmap[i], i);
			EXPECT_THROW(pmap.at(i + 1), out_of_range);
		}
	}

	TEST(PMapUndoRedo, NotLinearWithUndoRedo) {
		PersistentMap<size_t, size_t, MyHash> pmap;
		constexpr size_t size = (1 << 10);
		for (size_t i = 0; i < size; i += 2) {
			pmap = pmap.set(i, i);
			pmap = pmap.set(i + 1, i * 2);
			pmap = pmap.undo();
			pmap = pmap.set(i + 1, i + 1);
			pmap = pmap.undo();
			pmap = pmap.redo();
			EXPECT_EQ(pmap[i + 1], i + 1);
			EXPECT_THROW(pmap.at(i + 2), out_of_range);
		}
		EXPECT_EQ(pmap.size(), size);
		for (size_t i = 0; i < size; ++i) {
			EXPECT_EQ(pmap[i], i);
		}
		EXPECT_THROW(pmap.at(size), out_of_range);
	}

	TEST(PMapUndoRedo, NotLinearWithUndoRedoTwo) {
		PersistentMap<size_t, size_t, MyHash> pmap;
		constexpr size_t size = (1 << 10);
		for (size_t i = 0; i < size; i += 2) {
			pmap = pmap.set(i, i);
			pmap = pmap.set(i + 1, i * 2);
			pmap = pmap.set(i + 2, i * 3);
			pmap = pmap.undo();
			pmap = pmap.undo();
			pmap = pmap.set(i + 1, i + 1);
			pmap = pmap.undo();
			pmap = pmap.redo();
			EXPECT_EQ(pmap[i + 1], i + 1);
			EXPECT_THROW(pmap.at(i + 2), out_of_range);
		}
		EXPECT_EQ(pmap.size(), size);
		for (size_t i = 0; i < size; ++i) {
			EXPECT_EQ(pmap[i], i);
		}
		EXPECT_THROW(pmap.at(size), out_of_range);
	}

	/*
	*	canUndo & canRedo
	*/

	TEST(PMapCanUndoRedo, NewCreated) {
		PersistentMap<size_t, size_t, MyHash> pmap;
		EXPECT_FALSE(pmap.canUndo());
		EXPECT_FALSE(pmap.canRedo());
	}

	TEST(PMapCanUndoRedo, CanUndoCantRedo) {
		PersistentMap<size_t, size_t, MyHash> pmap;
		pmap = pmap.set(0, 0);
		EXPECT_TRUE(pmap.canUndo());
		EXPECT_FALSE(pmap.canRedo());
	}

	TEST(PMapCanUndoRedo, CantUndoCanRedo) {
		PersistentMap<size_t, size_t, MyHash> pmap;
		pmap = pmap.set(0, 0);
		pmap = pmap.undo();
		EXPECT_FALSE(pmap.canUndo());
		EXPECT_TRUE(pmap.canRedo());
	}

	TEST(PMapCanUndoRedo, CanUndoCanRedo) {
		PersistentMap<size_t, size_t, MyHash> pmap;
		pmap = pmap.set(0, 0).set(0, 1);
		pmap = pmap.undo();
		EXPECT_TRUE(pmap.canUndo());
		EXPECT_TRUE(pmap.canRedo());
	}



	/*
	*	Concurrency
	*/

	namespace {
		void AddNewKeyFunc(PersistentMap<size_t, size_t, MyHash>& pmap) {
			auto id = std::hash<std::thread::id>{}(std::this_thread::get_id());
			auto myPmap = pmap.set(0, id);
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			myPmap = myPmap.set(1, id + 1);
			for (size_t i = 0; i < 2; ++i) {
				EXPECT_EQ(myPmap[i], id + i);
			}
			for (size_t i = 3; i < 5; ++i) {
				EXPECT_EQ(myPmap[i], i + 1);
			}
		}

		TEST(PMapConcurrency, AddNewKey) {
			constexpr size_t repeats_number = 100;
			constexpr size_t threads_number = 16;
			PersistentMap<size_t, size_t, MyHash> pmap = { { 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 4 }, { 4, 5 } };
			std::thread threads[threads_number];
			for (size_t repeat = 0; repeat < repeats_number; ++repeat) {
				for (size_t i = 0; i < threads_number; ++i) {
					threads[i] = std::thread(&AddNewKeyFunc, std::ref(pmap));
				}
				for (size_t i = 0; i < threads_number; ++i) {
					threads[i].join();
				}
			}
		}
	}

	namespace {
		void UndoRedoFunc(PersistentMap<size_t, size_t, MyHash>& pmap) {
			auto id = std::hash<std::thread::id>{}(std::this_thread::get_id());
			auto myPmap = pmap.undo();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			myPmap = myPmap.undo();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			myPmap = myPmap.undo();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			myPmap = myPmap.undo();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			myPmap = myPmap.undo();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			myPmap = myPmap.redo();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			myPmap = myPmap.redo();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			myPmap = myPmap.redo();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			myPmap = myPmap.set(0, id);
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			myPmap = myPmap.set(1, id + 1);
			for (size_t i = 0; i < 2; ++i) {
				EXPECT_EQ(myPmap[i], id + i);
			}
			for (size_t i = 2; i < 3; ++i) {
				EXPECT_EQ(myPmap[i], i + 1);
			}
		}

		TEST(PMapConcurrency, UndoRedo) {
			constexpr size_t repeats_number = 100;
			constexpr size_t threads_number = 16;
			PersistentMap<size_t, size_t, MyHash> pmap;
			pmap = pmap.set(0, 1).set(1, 2).set(2, 3).set(3, 4).set(4, 5);
			std::thread threads[threads_number];
			for (size_t repeat = 0; repeat < repeats_number; ++repeat) {
				for (size_t i = 0; i < threads_number; ++i) {
					threads[i] = std::thread(&UndoRedoFunc, std::ref(pmap));
				}
				for (size_t i = 0; i < threads_number; ++i) {
					threads[i].join();
				}
			}
		}
	}
}
