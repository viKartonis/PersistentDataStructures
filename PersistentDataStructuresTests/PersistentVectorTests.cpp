#include <gtest/gtest.h>
#include <PersistentVector.h>
#include <thread>
#include <chrono>


namespace {
	using namespace pds;
	using namespace std;

	/*
	*	Creation
	*/

	TEST(PVectorCreation, Creation) {
		PersistentVector<int> pvector;
		EXPECT_EQ(pvector.size(), 0);
		EXPECT_TRUE(pvector.empty());
	}

	TEST(PVectorCreation, EmptyCopyCreation) {
		PersistentVector<int> pvector_base;
		PersistentVector<int> pvector = pvector_base;
		EXPECT_EQ(pvector.size(), 0);
		EXPECT_TRUE(pvector.empty());
	}

	TEST(PVectorCreation, EmptyMoveCreation) {
		PersistentVector<int> pvector_base;
		PersistentVector<int> pvector = std::move(pvector_base);
		EXPECT_EQ(pvector.size(), 0);
		EXPECT_TRUE(pvector.empty());
	}


	/*
	*	Inserting
	*/

	TEST(PVectorInserting, push_back_onlyOneLeaf) {
		PersistentVector<size_t> pvector;
		constexpr size_t size = (1 << 5);
		for (size_t i = 0; i < size; ++i) {
			pvector = pvector.push_back(i);
			EXPECT_EQ(pvector.size(), i + 1);
		}
	}

	TEST(PVectorInserting, push_back_twoLeafs) {
		PersistentVector<size_t> pvector;
		constexpr size_t size = 2*(1 << 5);
		for (size_t i = 0; i < size; ++i) {
			pvector = pvector.push_back(i);
			EXPECT_EQ(pvector.size(), i + 1);
		}
	}

	TEST(PVectorInserting, push_back_twoLevels) {
		PersistentVector<size_t> pvector;
		constexpr size_t size = ((1 << 5) << 5) + 1;
		for (size_t i = 0; i < size; ++i) {
			pvector = pvector.emplace_back(i);
			EXPECT_EQ(pvector.size(), i + 1);
		}
	}

	TEST(PVectorInserting, emplace_back_onlyOneLeaf) {
		PersistentVector<size_t> pvector;
		constexpr size_t size = (1 << 5);
		for (size_t i = 0; i < size; ++i) {
			pvector = pvector.emplace_back(i);
			EXPECT_EQ(pvector.size(), i + 1);
		}
	}

	TEST(PVectorInserting, emplace_back_twoLeafs) {
		PersistentVector<size_t> pvector;
		constexpr size_t size = 2 * (1 << 5);
		for (size_t i = 0; i < size; ++i) {
			pvector = pvector.emplace_back(i);
			EXPECT_EQ(pvector.size(), i + 1);
		}
	}

	TEST(PVectorInserting, emplace_back_twoLevels) {
		PersistentVector<size_t> pvector;
		constexpr size_t size = ((1 << 5) << 5) + 1;
		for (size_t i = 0; i < size; ++i) {
			pvector = pvector.emplace_back(i);
			EXPECT_EQ(pvector.size(), i + 1);
		}
	}


	/*
	*	Getting
	*/

	TEST(PVectorGetting, onlyOneElement) {
		PersistentVector<size_t> pvector;
		pvector = pvector.push_back(0);
		EXPECT_EQ(pvector[0], 0);
	}

	TEST(PVectorGetting, onlyOneLeaf) {
		PersistentVector<size_t> pvector;
		constexpr size_t size = (1 << 5);
		for (size_t i = 0; i < size; ++i) {
			pvector = pvector.push_back(i);
			for (size_t j = 0; j <= i; ++j) {
				EXPECT_EQ(pvector[j], j);
			}
		}
		for (size_t i = 0; i < size; ++i) {
			EXPECT_EQ(pvector[i], i);
		}
	}

	TEST(PVectorGetting, TwoLeafs) {
		PersistentVector<size_t> pvector;
		constexpr size_t size = (1 << 5) + 1;
		for (size_t i = 0; i < size; ++i) {
			pvector = pvector.push_back(i);
			for (size_t j = 0; j <= i; ++j) {
				EXPECT_EQ(pvector[j], j);
			}
		}
		for (size_t i = 0; i < size; ++i) {
			EXPECT_EQ(pvector[i], i);
		}
	}

	TEST(PVectorGetting, TwoLelves) {
		PersistentVector<size_t> pvector;
		constexpr size_t size = (1 << 10) + 1;
		for (size_t i = 0; i < size; ++i) {
			pvector = pvector.push_back(i);
			for (size_t j = 0; j <= i; ++j) {
				EXPECT_EQ(pvector[j], j);
			}
		}
		for (size_t i = 0; i < size; ++i) {
			EXPECT_EQ(pvector[i], i);
		}
	}

	TEST(PVectorGetting, Huge) {
		PersistentVector<size_t> pvector;
		constexpr size_t size = (1 << 12) + 1;
		for (size_t i = 0; i < size; ++i) {
			pvector = pvector.push_back(i);
			for (size_t j = 0; j <= i; ++j) {
				EXPECT_EQ(pvector[j], j);
			}
		}
		for (size_t i = 0; i < size; ++i) {
			EXPECT_EQ(pvector[i], i);
		}
	}


	/*
	*	Extended creation
	*/

	TEST(PVectorCertainValueCreation, NoOne) {
		PersistentVector<size_t> pvector(0, 0);
		EXPECT_EQ(pvector.size(), 0);
		EXPECT_TRUE(pvector.empty());
	}

	TEST(PVectorCertainValueCreation, OnlyOne) {
		PersistentVector<size_t> pvector(1, 12345);
		EXPECT_EQ(pvector.size(), 1);
		EXPECT_EQ(pvector[0], 12345);
	}

	TEST(PVectorCertainValueCreation, Huge) {
		constexpr size_t size = (1 << 15) + 1;
		PersistentVector<size_t> pvector(size, 12345);
		EXPECT_EQ(pvector.size(), size);
		for (size_t i = 0; i < size; ++i) {
			EXPECT_EQ(pvector[i], 12345);
		}
	}

	TEST(PVectorIteratorCreation, NoOne) {
		std::vector<size_t> srcvector;
		PersistentVector<size_t> pvector(srcvector.begin(), srcvector.end());
		EXPECT_EQ(pvector.size(), 0);
		EXPECT_TRUE(pvector.empty());
	}

	TEST(PVectorIteratorCreation, OnlyOne) {
		std::vector<size_t> srcvector = { 12345 };
		PersistentVector<size_t> pvector(srcvector.begin(), srcvector.end());
		EXPECT_EQ(pvector.size(), 1);
		EXPECT_EQ(pvector[0], 12345);
	}

	TEST(PVectorIteratorCreation, Some) {
		std::vector<size_t> srcvector = { 1, 2, 3, 4, 5 };
		PersistentVector<size_t> pvector(srcvector.begin(), srcvector.end());
		EXPECT_EQ(pvector.size(), srcvector.size());
		for (size_t i = 0; i < srcvector.size(); ++i) {
			EXPECT_EQ(pvector[i], srcvector[i]);
		}
	}

	TEST(PVectorIteratorCreation, Huge) {
		constexpr size_t size = (1 << 15) + 1;
		std::vector<size_t> srcvector(size);

		for (size_t i = 0; i < srcvector.size(); ++i) {
			srcvector[i] = i + 1;
		}
		PersistentVector<size_t> pvector(srcvector.begin(), srcvector.end());
		EXPECT_EQ(pvector.size(), srcvector.size());
		for (size_t i = 0; i < srcvector.size(); ++i) {
			EXPECT_EQ(pvector[i], srcvector[i]);
		}
	}

	TEST(PVectorInitListCreation, NoOne) {
		PersistentVector<size_t> pvector = {};
		EXPECT_EQ(pvector.size(), 0);
		EXPECT_TRUE(pvector.empty());
	}

	TEST(PVectorInitListCreation, OnlyOne) {
		PersistentVector<size_t> pvector = { 12345 };
		EXPECT_EQ(pvector.size(), 1);
		EXPECT_EQ(pvector[0], 12345);
	}

	TEST(PVectorInitListCreation, Some) {
		PersistentVector<size_t> pvector = { 1, 2, 3, 4, 5 };
		EXPECT_EQ(pvector.size(), 5);
		for (size_t i = 0; i < 5; ++i) {
			EXPECT_EQ(pvector[i], i + 1);
		}
	}


	/*
	*	Setting
	*/

	TEST(PVectorSetting, OnlyOne) {
		PersistentVector<size_t> srcpvector = { 12345 };
		auto pvector = srcpvector.set(0, 0);
		EXPECT_EQ(srcpvector.size(), 1);
		EXPECT_EQ(pvector.size(), 1);
		EXPECT_EQ(srcpvector[0], 12345);
		EXPECT_EQ(pvector[0], 0);
	}

	TEST(PVectorSetting, Some) {
		PersistentVector<size_t> pvector = { 1, 2, 3, 4, 5 };
		for (size_t i = 0; i < pvector.size(); ++i) {
			pvector = pvector.set(i, i);
		}
		EXPECT_EQ(pvector.size(), 5);
		for (size_t i = 0; i < pvector.size(); ++i) {
			EXPECT_EQ(pvector[i], i);
		}
	}

	TEST(PVectorSetting, Huge) {
		constexpr size_t size = (1 << 15) + 1;
		PersistentVector<size_t> pvector(size, 0);
		for (size_t i = 0; i < pvector.size(); ++i) {
			pvector = pvector.set(i, i + 1);
		}
		EXPECT_EQ(pvector.size(), size);
		for (size_t i = 0; i < pvector.size(); ++i) {
			EXPECT_EQ(pvector[i], i + 1);
		}
	}


	/*
	*	Iterators
	*/


	TEST(PVectorIterator, Empty) {
		PersistentVector<size_t> pvector;
		EXPECT_TRUE(pvector.cbegin() == pvector.cend());
		EXPECT_TRUE(pvector.crbegin() == pvector.crend());
	}

	
	TEST(PVectorIterator, OnlyOne) {
		PersistentVector<size_t> pvector = { 12345 };
		// const iterator
		{
			EXPECT_TRUE(pvector.cbegin() != pvector.cend());
			auto it = pvector.cbegin();
			EXPECT_EQ(*it, 12345);
			EXPECT_TRUE(++it == pvector.cend());
		}
		// const reversed iterator
		{
			EXPECT_TRUE(pvector.crbegin() != pvector.crend());
			auto it = pvector.crbegin();
			EXPECT_EQ(*it, 12345);
			EXPECT_TRUE(++it == pvector.crend());
		}
	}
	
	TEST(PVectorIterator, Some) {
		PersistentVector<size_t> pvector(10, 12345);
		// const iterator
		{
			EXPECT_TRUE(pvector.cbegin() != pvector.cend());
			for (auto it = pvector.cbegin(); it != pvector.cend(); ++it) {
				EXPECT_EQ(*it, 12345);
			}
		}
		// const reversed iterator
		{
			EXPECT_TRUE(pvector.crbegin() != pvector.crend());
			for (auto it = pvector.crbegin(); it != pvector.crend(); ++it) {
				EXPECT_EQ(*it, 12345);
			}
		}
	}

	TEST(PVectorIterator, Huge) {
		PersistentVector<size_t> pvector(10, 12345);
		// const iterator
		{
			EXPECT_TRUE(pvector.cbegin() != pvector.cend());
			for (auto it = pvector.cbegin(); it != pvector.cend(); ++it) {
				EXPECT_EQ(*it, 12345);
			}
		}
		// const reversed iterator
		{
			EXPECT_TRUE(pvector.crbegin() != pvector.crend());
			for (auto it = pvector.crbegin(); it != pvector.crend(); ++it) {
				EXPECT_EQ(*it, 12345);
			}
		}
	}
	
	TEST(PVectorIterator, ChangeAssign) {
#define PVECTOR_ITERATOR_CHANGE_ASSIGN(pvector, begin, end)	\
		auto it = pvector.begin();							\
		EXPECT_EQ(*it, 1);									\
		it += 0;											\
		EXPECT_EQ(*it, 1);									\
		it += 1;											\
		EXPECT_EQ(*it, 2);									\
		it += 2;											\
		EXPECT_EQ(*it, 4);									\
		it += 3;											\
		EXPECT_EQ(*it, 7);									\
		it += 4;											\
		EXPECT_TRUE(it == pvector.end());					\
		it -= 1;											\
		EXPECT_EQ(*it, 10);									\
		it -= 0;											\
		EXPECT_EQ(*it, 10);									\
		it -= 2;											\
		EXPECT_EQ(*it, 8);									\
		it -= 3;											\
		EXPECT_EQ(*it, 5);									\
		it -= 4;											\
		EXPECT_EQ(*it, 1);									\
		EXPECT_TRUE(it == pvector.begin());

		// const iterator
		{
			PersistentVector<size_t> pvector = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
			PVECTOR_ITERATOR_CHANGE_ASSIGN(pvector, cbegin, cend)
		}
		// const reversed iterator
		{
			PersistentVector<size_t> pvector = { 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
			PVECTOR_ITERATOR_CHANGE_ASSIGN(pvector, crbegin, crend)
		}
	}

	TEST(PVectorIterator, IncrementAndDecrement) {
#define PVECTOR_ITERATOR_INCREMENT_AND_DECREMENT(pvector, begin, end)	\
		auto it = pvector.begin();										\
		EXPECT_EQ(*it, 1);												\
		++it;															\
		EXPECT_EQ(*it, 2);												\
		it++;															\
		EXPECT_EQ(*it, 3);												\
		EXPECT_EQ(*(++it), 4);											\
		EXPECT_EQ(*(it++), 4);											\
		EXPECT_EQ(*it, 5);												\
		EXPECT_EQ(*(it++), 5);											\
		EXPECT_TRUE(it == pvector.end());								\
		--it;															\
		EXPECT_EQ(*it, 5);												\
		it--;															\
		EXPECT_EQ(*it, 4);												\
		EXPECT_EQ(*(--it), 3);											\
		EXPECT_EQ(*(it--), 3);											\
		EXPECT_EQ(*it, 2);												\
		EXPECT_EQ(*(it--), 2);											\
		EXPECT_TRUE(it == pvector.begin());


		// const iterator
		{
			PersistentVector<size_t> pvector = { 1, 2, 3, 4, 5 };
			PVECTOR_ITERATOR_INCREMENT_AND_DECREMENT(pvector, cbegin, cend)
		}
		// const reversed iterator
		{
			PersistentVector<size_t> pvector = { 5, 4, 3, 2, 1 };
			PVECTOR_ITERATOR_INCREMENT_AND_DECREMENT(pvector, crbegin, crend)
		}
	}


	TEST(PVectorIterator, GetById) {
#define PVECTOR_ITERATOR_GET_BY_ID(pvector, begin)	\
		auto it = pvector.begin();					\
		EXPECT_EQ(it[0], 1);						\
		EXPECT_EQ(it[9], 10);						\
		it += 4;									\
		EXPECT_EQ(it[-4], 1);						\
		EXPECT_EQ(it[0], 5);						\
		EXPECT_EQ(it[5], 10);						\
		it += 5;									\
		EXPECT_EQ(it[-9], 1);						\
		EXPECT_EQ(it[0], 10);


		// const iterator
		{
			PersistentVector<size_t> pvector = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
			PVECTOR_ITERATOR_GET_BY_ID(pvector, cbegin)
		}
		// const reversed iterator
		{
			PersistentVector<size_t> pvector = { 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
			PVECTOR_ITERATOR_GET_BY_ID(pvector, crbegin)
		}
	}

	TEST(PVectorIterator, BinaryPlusAndMinus) {
#define PVECTOR_ITERATOR_BINARY_PLUS_AND_MINUS(pvector, begin, end)	\
		auto it = pvector.begin();									\
		EXPECT_EQ(*(it + 0), 1);									\
		EXPECT_EQ(*(it + 1), 2);									\
		EXPECT_EQ(*(it + 2), 3);									\
		EXPECT_EQ(*(it + 5), 6);									\
		EXPECT_EQ(*(it + 9), 10);									\
		EXPECT_TRUE(it + 10 == pvector.end());						\
		it = it + 9;												\
		EXPECT_EQ(*(it - 0), 10);									\
		EXPECT_EQ(*(it - 1), 9);									\
		EXPECT_EQ(*(it - 2), 8);									\
		EXPECT_EQ(*(it - 5), 5);									\
		EXPECT_EQ(*(it - 9), 1);									\
		EXPECT_TRUE(it - 9 == pvector.begin());



		// const iterator
		{
			PersistentVector<size_t> pvector = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
			PVECTOR_ITERATOR_BINARY_PLUS_AND_MINUS(pvector, cbegin, cend)
		}
		// const reversed iterator
		{
			PersistentVector<size_t> pvector = { 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
			PVECTOR_ITERATOR_BINARY_PLUS_AND_MINUS(pvector, crbegin, crend)
		}
	}

	TEST(PVectorIterator, Difference) {
#define PVECTOR_ITERATOR_DIFFERENCE(pvector, begin, end)	\
		const auto begin = pvector.begin();					\
		const auto end = pvector.end();						\
		auto it = pvector.begin();							\
		EXPECT_EQ(it - begin, 0);							\
		EXPECT_EQ(begin - it, 0);							\
		it += 1;											\
		EXPECT_EQ(it - begin, 1);							\
		EXPECT_EQ(begin - it, -1);							\
		it += 2;											\
		EXPECT_EQ(it - begin, 3);							\
		EXPECT_EQ(begin - it, -3);							\
		it += 5;											\
		EXPECT_EQ(it - begin, 8);							\
		EXPECT_EQ(begin - it, -8);							\
		it += 2;											\
		EXPECT_EQ(it - begin, 10);							\
		EXPECT_EQ(begin - it, -10);							\
		EXPECT_EQ(it - end, 0);								\
		EXPECT_EQ(end - it, 0);								\
		it -= 1;											\
		EXPECT_EQ(it - end, -1);							\
		EXPECT_EQ(end - it, 1);								\
		it -= 2;											\
		EXPECT_EQ(it - end, -3);							\
		EXPECT_EQ(end - it, 3);								\
		it -= 5;											\
		EXPECT_EQ(it - end, -8);							\
		EXPECT_EQ(end - it, 8);								\
		it -= 2;											\
		EXPECT_EQ(it - end, -10);							\
		EXPECT_EQ(end - it, 10);							\
		EXPECT_EQ(it - begin, 0);							\
		EXPECT_EQ(begin - it, 0);


		// const iterator
		{
			PersistentVector<size_t> pvector = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
			PVECTOR_ITERATOR_DIFFERENCE(pvector, cbegin, cend)
		}
		// const reversed iterator
		{
			PersistentVector<size_t> pvector = { 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
			PVECTOR_ITERATOR_DIFFERENCE(pvector, crbegin, crend)
		}
	}

	TEST(PVectorIterator, Comparation) {
#define PVECTOR_ITERATOR_COMPARATION(pvector, begin, end)	\
			const auto begin = pvector.begin();				\
			const auto end = pvector.end();					\
			auto it = pvector.begin();						\
			EXPECT_TRUE(it == begin);						\
			EXPECT_TRUE(it + 5 == end);						\
			EXPECT_TRUE(it >= begin);						\
			EXPECT_TRUE(it + 5 >= end);						\
			EXPECT_TRUE(it <= begin);						\
			EXPECT_TRUE(it + 5 <= end);						\
			it += 2;										\
			EXPECT_FALSE(it == begin);						\
			EXPECT_FALSE(it == end);						\
			EXPECT_TRUE(it != begin);						\
			EXPECT_TRUE(it != end);							\
			EXPECT_TRUE(begin < it);						\
			EXPECT_TRUE(it < end);;							\
			EXPECT_TRUE(it > begin);						\
			EXPECT_TRUE(end > it);


		PersistentVector<size_t> pvector = { 1, 2, 3, 4, 5 };
		// const iterator
		{
			PVECTOR_ITERATOR_COMPARATION(pvector, cbegin, cend)
		}
		// const reversed iterator
		{
			PVECTOR_ITERATOR_COMPARATION(pvector, crbegin, crend)
		}
	}


	/*
	* 
	*	Comparation
	* 
	*/

	TEST(PVectorComparation, Empty) {
		PersistentVector<size_t> pvector;
		PersistentVector<size_t> pvector1;
		PersistentVector<size_t> pvector2 = pvector;
		EXPECT_TRUE(pvector == pvector);
		EXPECT_FALSE(pvector != pvector);
		EXPECT_TRUE(pvector == pvector1);
		EXPECT_FALSE(pvector != pvector1);
		EXPECT_TRUE(pvector == pvector2);
		EXPECT_FALSE(pvector != pvector2);
	}

	TEST(PVectorComparation, NonEmptyConstructed) {
		PersistentVector<size_t> pvector = { 1, 2, 3 };
		PersistentVector<size_t> pvector1 = { 1, 2, 3 };
		PersistentVector<size_t> pvector2 = pvector;
		EXPECT_TRUE(pvector == pvector);
		EXPECT_FALSE(pvector != pvector);
		EXPECT_TRUE(pvector == pvector1);
		EXPECT_FALSE(pvector != pvector1);
		EXPECT_TRUE(pvector == pvector2);
		EXPECT_FALSE(pvector != pvector2);
	}

	TEST(PVectorComparation, SameLength) {
		constexpr size_t size = (1 << 15) + 1;
		PersistentVector<size_t> pvector(size, 12345);
		PersistentVector<size_t> pvector1(size, 12345);
		EXPECT_TRUE(pvector == pvector1);
		EXPECT_FALSE(pvector != pvector1);
		auto pvector2 = pvector1.set(size - 1, 54321);
		EXPECT_TRUE(pvector == pvector1);
		EXPECT_FALSE(pvector != pvector1);
		EXPECT_FALSE(pvector == pvector2);
		EXPECT_TRUE(pvector != pvector2);
		pvector2 = pvector2.set(size - 1, 12345);
		EXPECT_TRUE(pvector2 == pvector);
		EXPECT_FALSE(pvector2 != pvector);
		EXPECT_TRUE(pvector2 == pvector1);
		EXPECT_FALSE(pvector2 != pvector1);
		pvector2 = pvector2.set(size - 5, 54321);
		EXPECT_FALSE(pvector == pvector2);
		EXPECT_TRUE(pvector != pvector2);
		EXPECT_FALSE(pvector1 == pvector2);
		EXPECT_TRUE(pvector1 != pvector2);
	}

	TEST(PVectorComparation, DifferentLength) {
		constexpr size_t size = (1 << 15) + 1;
		PersistentVector<size_t> pvector(size, 12345);
		PersistentVector<size_t> pvector1(size - 1, 12345);
		EXPECT_FALSE(pvector == pvector1);
		EXPECT_TRUE(pvector != pvector1);
		auto pvector2 = pvector1.set(size - 2, 54321);
		EXPECT_FALSE(pvector == pvector1);
		EXPECT_TRUE(pvector != pvector1);
		EXPECT_FALSE(pvector1 == pvector2);
		EXPECT_TRUE(pvector1 != pvector2);
		pvector2 = pvector2.set(size - 2, 12345);
		EXPECT_FALSE(pvector == pvector1);
		EXPECT_TRUE(pvector != pvector1);
		EXPECT_TRUE(pvector1 == pvector2);
		EXPECT_FALSE(pvector1 != pvector2);
	}


	/*
	*
	*	Swap
	*
	*/

	TEST(PVectorSwap, SwapItself) {
		constexpr size_t size = 1 << 12;
		PersistentVector<size_t> pvector(size, 12345);
		pvector.swap(pvector);
		EXPECT_TRUE(pvector == pvector);
		std::swap(pvector, pvector);
		EXPECT_TRUE(pvector == pvector);
	}

	TEST(PVectorSwap, SwapDifferent) {
		constexpr size_t size = 1 << 12;
		PersistentVector<size_t> pvector1(size, 12345);
		PersistentVector<size_t> pvector2(size, 54321);
		PersistentVector<size_t> pvector1_copy = pvector1;
		PersistentVector<size_t> pvector2_copy = pvector2;
		EXPECT_TRUE(pvector1 != pvector2);
		EXPECT_TRUE(pvector1 == pvector1_copy);
		EXPECT_TRUE(pvector2 == pvector2_copy);
		pvector1.swap(pvector2);
		EXPECT_TRUE(pvector1 != pvector2);
		EXPECT_TRUE(pvector2 == pvector1_copy);
		EXPECT_TRUE(pvector1 == pvector2_copy);
		std::swap(pvector1, pvector2);
		EXPECT_TRUE(pvector1 != pvector2);
		EXPECT_TRUE(pvector1 == pvector1_copy);
		EXPECT_TRUE(pvector2 == pvector2_copy);
	}



	/*
	*	Popping
	*/

	TEST(PVectorPopping, onlyOneElement) {
		PersistentVector<size_t> pvector;
		pvector = pvector.push_back(0);
		auto pvector1 = pvector.pop_back();
		EXPECT_EQ(pvector[0], 0);
		EXPECT_TRUE(pvector1.empty());
	}

	TEST(PVectorPopping, SomeElementsInARow) {
		PersistentVector<size_t> pvector;
		constexpr size_t size = 6;
		for (size_t i = 0; i < size; ++i) {
			pvector = pvector.push_back(i);
			EXPECT_EQ(pvector[0], i);
			EXPECT_EQ(pvector.size(), 1);
			pvector = pvector.pop_back();
			EXPECT_TRUE(pvector.empty());
		}
	}

	TEST(PVectorPopping, onlyOneLeaf) {
		constexpr size_t size = (1 << 5);
		std::vector<size_t> vector;
		for (size_t i = 0; i < size; ++i) {
			vector.push_back(i);
		}
		PersistentVector<size_t> pvector(vector.begin(), vector.end());
		for (size_t i = 0; i < size; ++i) {
			EXPECT_EQ(pvector.size(), size - i);
			EXPECT_EQ(pvector[size - i - 1], size - i - 1);
			pvector = pvector.pop_back();
			EXPECT_EQ(pvector.size(), size - i - 1);
		}
		EXPECT_TRUE(pvector.empty());
	}

	TEST(PVectorPopping, TwoLeafs) {
		constexpr size_t size = (1 << 5) + 1;
		std::vector<size_t> vector;
		for (size_t i = 0; i < size; ++i) {
			vector.push_back(i);
		}
		PersistentVector<size_t> pvector(vector.begin(), vector.end());
		for (size_t i = 0; i < size; ++i) {
			EXPECT_EQ(pvector.size(), size - i);
			EXPECT_EQ(pvector[size - i - 1], size - i - 1);
			pvector = pvector.pop_back();
			EXPECT_EQ(pvector.size(), size - i - 1);
		}
		EXPECT_TRUE(pvector.empty());
	}

	TEST(PVectorPopping, TwoLelves) {
		constexpr size_t size = (1 << 10) + 1;
		std::vector<size_t> vector;
		for (size_t i = 0; i < size; ++i) {
			vector.push_back(i);
		}
		PersistentVector<size_t> pvector(vector.begin(), vector.end());
		for (size_t i = 0; i < size; ++i) {
			EXPECT_EQ(pvector.size(), size - i);
			EXPECT_EQ(pvector[size - i - 1], size - i - 1);
			pvector = pvector.pop_back();
			EXPECT_EQ(pvector.size(), size - i - 1);
		}
		EXPECT_TRUE(pvector.empty());
	}

	TEST(PVectorPopping, Huge) {
		constexpr size_t size = (1 << 15) + 1;
		std::vector<size_t> vector;
		for (size_t i = 0; i < size; ++i) {
			vector.push_back(i);
		}
		PersistentVector<size_t> pvector(vector.begin(), vector.end());
		for (size_t i = 0; i < size; ++i) {
			EXPECT_EQ(pvector.size(), size - i);
			EXPECT_EQ(pvector[size - i - 1], size - i - 1);
			pvector = pvector.pop_back();
			EXPECT_EQ(pvector.size(), size - i - 1);
		}
		EXPECT_TRUE(pvector.empty());
	}

	TEST(PVectorPopping, BranchingVersioning) {
		PersistentVector<size_t> pvector = { 1, 2, 3, 4, 5 };
		auto pvector1 = pvector.pop_back();
		auto pvector2 = pvector.push_back(6);
		auto pvector3 = pvector2.pop_back();
		PersistentVector<size_t> pvector4 = { 1, 2, 3, 4 };
		PersistentVector<size_t> pvector5 = { 1, 2, 3, 4, 5, 6 };
		EXPECT_TRUE(pvector1 == pvector4);
		EXPECT_TRUE(pvector2 == pvector5);
		EXPECT_TRUE(pvector3 == pvector);
	}


	/*
	*	Resize
	*/

	TEST(PVectorResize, EmptyToEmpty) {
		PersistentVector<size_t> pvector;
		EXPECT_TRUE(pvector.empty());
		auto pvector1 = pvector.resize(0);
		EXPECT_TRUE(pvector.empty());
		EXPECT_TRUE(pvector1.empty());
	}

	TEST(PVectorResize, EmptyToOne) {
		PersistentVector<size_t> pvector;
		EXPECT_TRUE(pvector.empty());
		auto pvector1 = pvector.resize(1, 1);
		EXPECT_TRUE(pvector.empty());
		EXPECT_EQ(pvector1.size(), 1);
		EXPECT_EQ(pvector1[0], 1);
	}

	TEST(PVectorResize, EmptyToSome) {
		constexpr size_t size = 10;
		PersistentVector<size_t> pvector;
		EXPECT_TRUE(pvector.empty());
		auto pvector1 = pvector.resize(size, 12345);
		EXPECT_TRUE(pvector.empty());
		EXPECT_EQ(pvector1.size(), size);
		for (auto it = pvector1.cbegin(); it != pvector1.cend(); ++it) {
			EXPECT_EQ(*it, 12345);
		}
	}

	TEST(PVectorResize, EmptyToHuge) {
		constexpr size_t size = 1 << 15;
		PersistentVector<size_t> pvector;
		EXPECT_TRUE(pvector.empty());
		auto pvector1 = pvector.resize(size, 12345);
		EXPECT_TRUE(pvector.empty());
		EXPECT_EQ(pvector1.size(), size);
		for (auto it = pvector1.cbegin(); it != pvector1.cend(); ++it) {
			EXPECT_EQ(*it, 12345);
		}
	}

	TEST(PVectorResize, OneToEmpty) {
		PersistentVector<size_t> pvector = { 0 };
		auto pvector1 = pvector.resize(0);
		EXPECT_EQ(pvector.size(), 1);
		EXPECT_TRUE(pvector1.empty());
	}

	TEST(PVectorResize, OneToOne) {
		PersistentVector<size_t> pvector = { 0 };
		auto pvector1 = pvector.resize(1, 1);
		EXPECT_EQ(pvector.size(), 1);
		EXPECT_EQ(pvector1.size(), 1);
		EXPECT_EQ(pvector1[0], 0);
	}

	TEST(PVectorResize, OneToSome) {
		PersistentVector<size_t> pvector = { 0 };
		constexpr size_t size = 10;
		auto pvector1 = pvector.resize(size, 12345);
		EXPECT_EQ(pvector.size(), 1);
		EXPECT_EQ(pvector1.size(), size);
		EXPECT_EQ(pvector1.size(), size);
		EXPECT_EQ(pvector1[0], 0);
		for (auto it = pvector1.cbegin() + 1; it != pvector1.cend(); ++it) {
			EXPECT_EQ(*it, 12345);
		}
	}

	TEST(PVectorResize, OneToHuge) {
		PersistentVector<size_t> pvector = { 0 };
		constexpr size_t size = 1 << 15;
		auto pvector1 = pvector.resize(size, 12345);
		EXPECT_EQ(pvector.size(), 1);
		EXPECT_EQ(pvector1.size(), size);
		EXPECT_EQ(pvector1.size(), size);
		EXPECT_EQ(pvector1[0], 0);
		for (auto it = pvector1.cbegin() + 1; it != pvector1.cend(); ++it) {
			EXPECT_EQ(*it, 12345);
		}
	}

	TEST(PVectorResize, SomeToEmpty) {
		PersistentVector<size_t> pvector = { 0, 1, 2, 3, 4, 5 };
		auto pvector1 = pvector.resize(0);
		EXPECT_EQ(pvector.size(), 6);
		EXPECT_TRUE(pvector1.empty());
	}

	TEST(PVectorResize, SomeToOne) {
		PersistentVector<size_t> pvector = { 0, 1, 2, 3, 4, 5 };
		auto pvector1 = pvector.resize(1, 1);
		EXPECT_EQ(pvector.size(), 6);
		EXPECT_EQ(pvector1.size(), 1);
		EXPECT_EQ(pvector1[0], 0);
	}

	TEST(PVectorResize, SomeToSomeLess) {
		constexpr size_t size = 4;
		PersistentVector<size_t> pvector = { 0, 1, 2, 3, 4, 5 };
		auto pvector1 = pvector.resize(size, 1);
		EXPECT_EQ(pvector.size(), 6);
		EXPECT_EQ(pvector1.size(), size);
		for (size_t i = 0; i < size; ++i) {
			EXPECT_EQ(pvector1[i], i);
		}
	}

	TEST(PVectorResize, SomeToSomeEq) {
		constexpr size_t size = 6;
		PersistentVector<size_t> pvector = { 0, 1, 2, 3, 4, 5 };
		auto pvector1 = pvector.resize(size, 1);
		EXPECT_EQ(pvector.size(), 6);
		EXPECT_EQ(pvector1.size(), size);
		for (size_t i = 0; i < size; ++i) {
			EXPECT_EQ(pvector1[i], i);
		}
		EXPECT_EQ(pvector, pvector1);
	}

	TEST(PVectorResize, SomeToSomeBigger) {
		constexpr size_t size = 10;
		PersistentVector<size_t> pvector = { 0, 1, 2, 3, 4, 5 };
		auto pvector1 = pvector.resize(size, 12345);
		EXPECT_EQ(pvector.size(), 6);
		EXPECT_EQ(pvector1.size(), size);
		for (size_t i = 0; i < 6; ++i) {
			EXPECT_EQ(pvector1[i], i);
		}
		for (size_t i = 6; i < size; ++i) {
			EXPECT_EQ(pvector1[i], 12345);
		}
	}

	TEST(PVectorResize, SomeToHuge) {
		constexpr size_t size = 1 << 15;
		PersistentVector<size_t> pvector = { 0, 1, 2, 3, 4, 5 };
		auto pvector1 = pvector.resize(size, 12345);
		EXPECT_EQ(pvector.size(), 6);
		EXPECT_EQ(pvector1.size(), size);
		for (size_t i = 0; i < 6; ++i) {
			EXPECT_EQ(pvector1[i], i);
		}
		for (size_t i = 6; i < size; ++i) {
			EXPECT_EQ(pvector1[i], 12345);
		}
	}

	TEST(PVectorResize, HugeToEmpty) {
		constexpr size_t size = 1 << 15;
		PersistentVector<size_t> pvector(size, 0);
		auto pvector1 = pvector.resize(0);
		EXPECT_EQ(pvector.size(), size);
		EXPECT_TRUE(pvector1.empty());
	}

	TEST(PVectorResize, HugeToOne) {
		constexpr size_t size = 1 << 15;
		PersistentVector<size_t> pvector(size, 0);
		auto pvector1 = pvector.resize(1, 1);
		EXPECT_EQ(pvector.size(), size);
		EXPECT_EQ(pvector1.size(), 1);
		EXPECT_EQ(pvector1[0], 0);
	}

	TEST(PVectorResize, HugeToSome) {
		constexpr size_t size = 1 << 15;
		constexpr size_t size1 = 4;
		PersistentVector<size_t> pvector(size, 0);
		auto pvector1 = pvector.resize(size1, 1);
		EXPECT_EQ(pvector.size(), size);
		EXPECT_EQ(pvector1.size(), size1);
		for (size_t i = 0; i < size1; ++i) {
			EXPECT_EQ(pvector1[i], 0);
		}
	}

	TEST(PVectorResize, HugeToHugeLess) {
		constexpr size_t size = 1 << 15;
		constexpr size_t size1 = 1 << 12;
		PersistentVector<size_t> pvector(size, 0);
		auto pvector1 = pvector.resize(size1, 1);
		EXPECT_EQ(pvector.size(), size);
		EXPECT_EQ(pvector1.size(), size1);
		for (size_t i = 0; i < size1; ++i) {
			EXPECT_EQ(pvector1[i], 0);
		}
	}

	TEST(PVectorResize, HugeToHugeEq) {
		constexpr size_t size = 1 << 15;
		constexpr size_t size1 = 1 << 15;
		PersistentVector<size_t> pvector(size, 0);
		auto pvector1 = pvector.resize(size1, 1);
		EXPECT_EQ(pvector.size(), size);
		EXPECT_EQ(pvector1.size(), size1);
		for (size_t i = 0; i < size1; ++i) {
			EXPECT_EQ(pvector1[i], 0);
		}
		EXPECT_EQ(pvector, pvector1);
	}

	TEST(PVectorResize, HugeToHugeBigger) {
		constexpr size_t size = 1 << 15;
		constexpr size_t size1 = 1 << 16;
		PersistentVector<size_t> pvector(size, 0);
		auto pvector1 = pvector.resize(size1, 1);
		EXPECT_EQ(pvector.size(), size);
		EXPECT_EQ(pvector1.size(), size1);
		for (size_t i = 0; i < size; ++i) {
			EXPECT_EQ(pvector1[i], 0);
		}
		for (size_t i = size; i < size1; ++i) {
			EXPECT_EQ(pvector1[i], 1);
		}
	}


	/*
	*	Clear
	*/

	TEST(PVectorClear, Empty) {
		PersistentVector<size_t> pvector;
		EXPECT_TRUE(pvector.empty());
		auto pvector1 = pvector.resize(0);
		EXPECT_TRUE(pvector.empty());
		EXPECT_TRUE(pvector1.empty());
	}

	TEST(PVectorClear, One) {
		PersistentVector<size_t> pvector = { 0 };
		auto pvector1 = pvector.resize(0);
		EXPECT_EQ(pvector.size(), 1);
		EXPECT_TRUE(pvector1.empty());
	}

	TEST(PVectorClear, Some) {
		PersistentVector<size_t> pvector = { 0, 1, 2, 3, 4, 5 };
		auto pvector1 = pvector.resize(0);
		EXPECT_EQ(pvector.size(), 6);
		EXPECT_TRUE(pvector1.empty());
	}

	TEST(PVectorClear, Huge) {
		constexpr size_t size = 1 << 15;
		PersistentVector<size_t> pvector(size, 0);
		auto pvector1 = pvector.resize(0);
		EXPECT_EQ(pvector.size(), size);
		EXPECT_TRUE(pvector1.empty());
	}


	/*
	*	UNDO & REDO
	*/

	TEST(PVectorUndoRedo, linear) {
		PersistentVector<size_t> pvector;
		constexpr size_t size = (1 << 5);
		for (size_t i = 0; i < size; ++i) {
			pvector = pvector.push_back(i);
		}
		auto pvector1 = pvector;
		for (size_t i = 0; i < size; ++i) {
			EXPECT_EQ(pvector1.size(), size - i);
			EXPECT_EQ(pvector1.back(), size - i - 1);
			pvector1 = pvector1.undo();
			EXPECT_EQ(pvector1.size(), size - i - 1);
		}
		EXPECT_TRUE(pvector1.empty());
		for (size_t i = 0; i < size; ++i) {
			EXPECT_EQ(pvector1.size(), i);
			pvector1 = pvector1.redo();
			EXPECT_EQ(pvector1.back(), i);
			EXPECT_EQ(pvector1.size(), i + 1);
		}
	}

	TEST(PVectorUndoRedo, linearHuge) {
		PersistentVector<size_t> pvector;
		constexpr size_t size = (1 << 15);
		for (size_t i = 0; i < size; ++i) {
			pvector = pvector.push_back(i);
		}
		auto pvector1 = pvector;
		for (size_t i = 0; i < size; ++i) {
			EXPECT_EQ(pvector1.size(), size - i);
			EXPECT_EQ(pvector1.back(), size - i - 1);
			pvector1 = pvector1.undo();
			EXPECT_EQ(pvector1.size(), size - i - 1);
		}
		EXPECT_TRUE(pvector1.empty());
		for (size_t i = 0; i < size; ++i) {
			EXPECT_EQ(pvector1.size(), i);
			pvector1 = pvector1.redo();
			EXPECT_EQ(pvector1.back(), i);
			EXPECT_EQ(pvector1.size(), i + 1);
		}
		EXPECT_EQ(pvector, pvector1);
	}

	TEST(PVectorUndoRedo, linearHugeDestruction) {
		PersistentVector<size_t> pvector;
		constexpr size_t size = (1 << 15);
		for (size_t i = 0; i < size; ++i) {
			pvector = pvector.push_back(i);
		}
		for (size_t i = 0; i < size; ++i) {
			EXPECT_EQ(pvector.size(), size - i);
			EXPECT_EQ(pvector.back(), size - i - 1);
			pvector = pvector.undo();
			EXPECT_EQ(pvector.size(), size - i - 1);
		}
		pvector = pvector.push_back(0);
	}

	TEST(PVectorUndoRedo, LinearWithUndoRedo) {
		PersistentVector<size_t> pvector;
		constexpr size_t size = (1 << 5);
		for (size_t i = 0; i < size; ++i) {
			pvector = pvector.push_back(i);
			pvector = pvector.undo();
			EXPECT_EQ(pvector.size(), i);
			EXPECT_TRUE(pvector.empty() || pvector.back() == i - 1);
			pvector = pvector.redo();
			EXPECT_EQ(pvector.size(), i + 1);
			EXPECT_EQ(pvector.back(), i);
		}
	}

	TEST(PVectorUndoRedo, NotLinearWithUndoRedo) {
		PersistentVector<size_t> pvector;
		constexpr size_t size = (1 << 15);
		for (size_t i = 0; i < size; i += 2) {
			pvector = pvector.push_back(i);
			pvector = pvector.push_back(i * 2);
			pvector = pvector.undo();
			pvector = pvector.push_back(i + 1);
			pvector = pvector.undo();
			pvector = pvector.redo();
			EXPECT_EQ(pvector.back(), i + 1);
		}
		EXPECT_EQ(pvector.size(), size);
		for (size_t i = 0; i < size; ++i) {
			EXPECT_EQ(pvector[i], i);
		}
	}

	TEST(PVectorUndoRedo, NotLinearWithUndoRedoTwo) {
		PersistentVector<size_t> pvector;
		constexpr size_t size = (1 << 15);
		for (size_t i = 0; i < size; i += 2) {
			pvector = pvector.push_back(i);
			pvector = pvector.push_back(i * 2);
			pvector = pvector.push_back(i * 3);
			pvector = pvector.undo();
			pvector = pvector.undo();
			pvector = pvector.push_back(i + 1);
			pvector = pvector.undo();
			pvector = pvector.redo();
			EXPECT_EQ(pvector.back(), i + 1);
		}
		EXPECT_EQ(pvector.size(), size);
		for (size_t i = 0; i < size; ++i) {
			EXPECT_EQ(pvector[i], i);
		}
	}

	/*
	*	canUndo & canRedo
	*/

	TEST(PVectorCanUndoRedo, NewCreated) {
		PersistentVector<size_t> pvector;
		EXPECT_FALSE(pvector.canUndo());
		EXPECT_FALSE(pvector.canRedo());
	}

	TEST(PVectorCanUndoRedo, CanUndoCantRedo) {
		PersistentVector<size_t> pvector;
		pvector = pvector.push_back(0);
		EXPECT_TRUE(pvector.canUndo());
		EXPECT_FALSE(pvector.canRedo());
	}

	TEST(PVectorCanUndoRedo, CantUndoCanRedo) {
		PersistentVector<size_t> pvector;
		pvector = pvector.push_back(0);
		pvector = pvector.undo();
		EXPECT_FALSE(pvector.canUndo());
		EXPECT_TRUE(pvector.canRedo());
	}

	TEST(PVectorCanUndoRedo, CanUndoCanRedo) {
		PersistentVector<size_t> pvector;
		pvector = pvector.push_back(0).push_back(1);
		pvector = pvector.undo();
		EXPECT_TRUE(pvector.canUndo());
		EXPECT_TRUE(pvector.canRedo());
	}



	/*
	*	Concurrency
	*/

	namespace {
		void PushBackFunc(PersistentVector<size_t>& pvector) {
			auto id = std::hash<std::thread::id>{}(std::this_thread::get_id());
			auto myPvector = pvector.push_back(id);
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			myPvector = myPvector.push_back(id + 1);
			for (size_t i = 0; i < 5; ++i) {
				EXPECT_EQ(myPvector[i], i + 1);
			}
			for (size_t i = 0; i < 2; ++i) {
				EXPECT_EQ(myPvector[i + 5], id + i);
			}
		}

		TEST(PVectorConcurrency, PushBack) {
			constexpr size_t repeats_number = 100;
			constexpr size_t threads_number = 16;
			PersistentVector<size_t> pvector = { 1, 2, 3, 4, 5 };
			std::thread threads[threads_number];
			for (size_t repeat = 0; repeat < repeats_number; ++repeat) {
				for (size_t i = 0; i < threads_number; ++i) {
					threads[i] = std::thread(&PushBackFunc, std::ref(pvector));
				}
				for (size_t i = 0; i < threads_number; ++i) {
					threads[i].join();
				}
			}
		}
	}

	namespace {
		void UndoRedoFunc(PersistentVector<size_t>& pvector) {
			auto id = std::hash<std::thread::id>{}(std::this_thread::get_id());
			auto myPvector = pvector.undo();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			myPvector = myPvector.undo();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			myPvector = myPvector.undo();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			myPvector = myPvector.undo();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			myPvector = myPvector.undo();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			myPvector = myPvector.redo();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			myPvector = myPvector.redo();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			myPvector = myPvector.redo();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			myPvector = myPvector.push_back(id);
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			myPvector = myPvector.push_back(id + 1);
			for (size_t i = 0; i < 3; ++i) {
				EXPECT_EQ(myPvector[i], i + 1);
			}
			for (size_t i = 0; i < 2; ++i) {
				EXPECT_EQ(myPvector[i + 3], id + i);
			}
		}

		TEST(PVectorConcurrency, UndoRedo) {
			constexpr size_t repeats_number = 100;
			constexpr size_t threads_number = 16;
			PersistentVector<size_t> pvector;
			pvector = pvector.push_back(1).push_back(2).push_back(3).push_back(4).push_back(5);
			std::thread threads[threads_number];
			for (size_t repeat = 0; repeat < repeats_number; ++repeat) {
				for (size_t i = 0; i < threads_number; ++i) {
					threads[i] = std::thread(&UndoRedoFunc, std::ref(pvector));
				}
				for (size_t i = 0; i < threads_number; ++i) {
					threads[i].join();
				}
			}
		}
	}
}
