#include "../include/Utils.h"

namespace pds {
	namespace Utils {
		std::size_t getId(std::size_t pos, std::uint32_t level, std::uint32_t degreeOfTwo) {
			return pos >> (level * degreeOfTwo);
		}

		std::size_t getMask(std::uint32_t level, std::uint32_t degreeOfTwo) {
			return ((1ull << static_cast<std::size_t>(degreeOfTwo)) << ((static_cast<std::size_t>(level) - 1ull) * static_cast<std::size_t>(degreeOfTwo))) - 1ull;
		}
	}
}
