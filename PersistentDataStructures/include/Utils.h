#pragma once
#include <cstddef>
#include <iterator>

namespace pds {
	using version_t = std::uint64_t;

	template<typename ...Types>
	using void_t = void;

	template <typename Iter>
	using Iter_cat = typename std::iterator_traits<Iter>::iterator_category;

	template <typename T, typename = void>
	constexpr bool is_iterator = false;

	template <typename T>
	constexpr bool is_iterator<T, void_t<Iter_cat<T>>> = true;

	namespace Utils {
		constexpr std::size_t binPow(std::uint32_t deg) {
			return static_cast<std::size_t>(1) << deg;
		}

		std::size_t getId(std::size_t pos, std::uint32_t level, std::uint32_t degreeOfTwo);

		std::size_t getMask(std::uint32_t level, std::uint32_t degreeOfTwo);
	}
}
