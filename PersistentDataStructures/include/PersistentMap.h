#pragma once
#include "PersistentVector.h"
#include "Utils.h"

#include <memory>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <iterator>
#include <utility>

namespace pds {
    template<typename Key, typename T, typename Hash = std::hash<Key>>
    class PersistentMap;

    template<typename Key, typename T, typename Hash>
    class PersistentMap {
        static constexpr std::size_t DEFAULT_INITIAL_SIZE = 256;
    public:
        class map_const_iterator;
        using const_iterator = map_const_iterator;

        class map_const_iterator {
            const typename PersistentVector<PersistentVector<std::pair<Key, T>>>::const_iterator m_outer_end;
            typename PersistentVector<PersistentVector<std::pair<Key, T>>>::const_iterator m_outer;
            typename PersistentVector<std::pair<Key, T>>::const_iterator m_inner;
        public:
            using iterator_category = std::random_access_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = const T*;
            using reference = const T&;

            map_const_iterator() = delete;
            map_const_iterator(typename PersistentVector<PersistentVector<std::pair<Key, T>>>::const_iterator m_outer_end,
                                typename PersistentVector<PersistentVector<std::pair<Key, T>>>::const_iterator m_outer,
                                typename PersistentVector<std::pair<Key, T>>::const_iterator m_inner)
                                : m_outer_end(m_outer_end), m_outer(m_outer), m_inner(m_inner) {}
            map_const_iterator(const map_const_iterator& other) = default;
            map_const_iterator(map_const_iterator&& other) = default;

            map_const_iterator& operator=(const map_const_iterator& other) = default;
            map_const_iterator& operator=(map_const_iterator&& other) = default;

            ~map_const_iterator() = default;

            inline const std::pair<Key, T>& operator*() const;
            inline const std::pair<Key, T>* operator->() const;

            inline map_const_iterator& operator++();
            inline map_const_iterator operator++(int);

            inline bool operator==(const map_const_iterator& other) const;
            inline bool operator!=(const map_const_iterator& other) const;
        };

        PersistentMap(std::size_t initial_size, const Hash& hash) :
            m_hash(hash),
            m_size(0),
            m_vector(std::make_shared<PersistentVector<PersistentVector<std::pair<Key, T>>>>(initial_size)) {}

        PersistentMap() : PersistentMap(DEFAULT_INITIAL_SIZE, Hash()) {}
        PersistentMap(const PersistentMap& other) = default;
        PersistentMap(PersistentMap&& other) noexcept = default;


        PersistentMap(std::size_t initial_size) : PersistentMap(initial_size, Hash()) {}

        template<typename InputIt, typename std::enable_if<is_iterator<InputIt>, bool>::type = false>
        PersistentMap(InputIt first, InputIt last, std::size_t initial_size, const Hash& hash);

        template<typename InputIt, typename std::enable_if<is_iterator<InputIt>, bool>::type = false>
        PersistentMap(InputIt first, InputIt last, std::size_t initial_size) : PersistentMap(first, last, initial_size, Hash()) {}

        PersistentMap(std::initializer_list<std::pair<const Key, T>> init, std::size_t initial_size, Hash& hash) : PersistentMap(init.begin(), init.end(), initial_size, hash) {}
        PersistentMap(std::initializer_list<std::pair<const Key, T>> init, std::size_t initial_size) : PersistentMap(init.begin(), init.end(), initial_size) {}
        PersistentMap(std::initializer_list<std::pair<const Key, T>> init) : PersistentMap(init.begin(), init.end(), DEFAULT_INITIAL_SIZE) {}

        ~PersistentMap() = default;

        PersistentMap& operator=(const PersistentMap& other) = default;
        PersistentMap& operator=(PersistentMap&& other) = default;

        const_iterator cbegin() const;
        const_iterator cend() const;

        const T& operator[](const Key& key) const;

        const T& at(const Key& key) const;

        PersistentMap set(const Key& key, const T& value) const;

        bool operator==(const PersistentMap& other) const;
        bool operator!=(const PersistentMap& other) const;

        void swap(PersistentMap& other);


        std::size_t size() const;
        bool empty() const;

        bool canUndo() const;
        bool canRedo() const;

        PersistentMap undo() const;
        PersistentMap redo() const;

        PersistentMap clear() const;

        std::size_t count(const Key& key) const;
        const_iterator find(const Key& key) const;
        bool contains(const Key& key) const;

        // if element does not exist throws out_of_range
        PersistentMap erase(const Key& key) const;

    private:
        PersistentMap(const Hash& hash, std::size_t size, std::shared_ptr<PersistentVector<PersistentVector<std::pair<Key, T>>>> vector) :
            m_hash(hash),
            m_size(size),
            m_vector(vector) {}

        // returns true if that was unique Key and false if existed key was updated
        static bool insertToSequenceAsHash(std::vector<std::vector<std::pair<Key, T>>>& sequence, const Key& key, const T& value, std::size_t hash);

        template<typename InputIt, typename std::enable_if<is_iterator<InputIt>, bool>::type = false>
        static std::vector<std::vector<std::pair<Key, T>>> getReallocatedVector(InputIt begin, InputIt end, std::size_t size, const Hash& hashFunc);

        static std::vector<PersistentVector<std::pair<Key, T>>> getReallocatedVectorOfPersistentVectors(std::vector<std::vector<std::pair<Key, T>>> resetVector, std::size_t size);

        std::vector<PersistentVector<std::pair<Key, T>>> getReallocatedVectorOfPersistentVectors(const Key& key, T value) const;

        Hash m_hash;
        std::size_t m_size;
        std::shared_ptr<PersistentVector<PersistentVector<std::pair<Key, T>>>> m_vector;
    };

    /*
    *
    *   Implementation
    *
    */


    /*
    *
    *   Iterators
    *
    */

    template<typename Key, typename T, typename Hash>
    inline const std::pair<Key, T>& PersistentMap<Key, T, Hash>::map_const_iterator::operator*() const {
        return *m_inner;
    }

    template<typename Key, typename T, typename Hash>
    inline const std::pair<Key, T>* PersistentMap<Key, T, Hash>::map_const_iterator::operator->() const {
        return &(**this);
    }

    template<typename Key, typename T, typename Hash>
    inline typename PersistentMap<Key, T, Hash>::map_const_iterator& PersistentMap<Key, T, Hash>::map_const_iterator::operator++() {
        ++m_inner;
        if (m_inner == m_outer->cend()) {
            m_outer = std::find_if(++m_outer, m_outer_end, [](const PersistentVector<std::pair<Key, T>>& pvector) { return !pvector.empty(); });
            if (m_outer != m_outer_end) {
                m_inner = m_outer->cbegin();
            }
        }
        return *this;
    }

    template<typename Key, typename T, typename Hash>
    inline typename PersistentMap<Key, T, Hash>::map_const_iterator PersistentMap<Key, T, Hash>::map_const_iterator::operator++(int) {
        auto copy = *this;
        operator++();
        return copy;
    }

    template<typename Key, typename T, typename Hash>
    inline bool PersistentMap<Key, T, Hash>::map_const_iterator::operator==(const map_const_iterator& other) const {
        return m_outer == other.m_outer;
    }

    template<typename Key, typename T, typename Hash>
    inline bool PersistentMap<Key, T, Hash>::map_const_iterator::operator!=(const map_const_iterator& other) const {
        return !(*this == other);
    }


    /*
    *
    *   Persistent map
    *
    */

    template<typename Key, typename T, typename Hash>
    template<typename InputIt, typename std::enable_if<is_iterator<InputIt>, bool>::type>
    PersistentMap<Key, T, Hash>::PersistentMap(InputIt first, InputIt last, std::size_t initial_size, const Hash& hashFunc) : PersistentMap(initial_size, hashFunc) {
        std::vector<std::vector<std::pair<Key, T>>> resetVector(initial_size);
        m_size = 0;
        for (auto it = first; it != last; ++it) {
            auto hash = hashFunc(it->first) % (initial_size);
            if (insertToSequenceAsHash(resetVector, it->first, it->second, hash)) {
                ++m_size;
            }
        }
        auto vectorOfPersistentVectors = getReallocatedVectorOfPersistentVectors(resetVector, initial_size);
        m_vector = std::make_shared<PersistentVector<PersistentVector<std::pair<Key, T>>>>(vectorOfPersistentVectors.cbegin(), vectorOfPersistentVectors.cend());
    }

    template<typename Key, typename T, typename Hash>
    inline typename PersistentMap<Key, T, Hash>::const_iterator PersistentMap<Key, T, Hash>::cbegin() const {
        auto it = std::find_if(m_vector->cbegin(), m_vector->cend(), [](const PersistentVector<std::pair<Key, T>>& pvector) { return !pvector.empty(); });
        return const_iterator(m_vector->cend(), it, it == m_vector->cend() ? m_vector->cbegin()->cbegin() : it->cbegin());
    }

    template<typename Key, typename T, typename Hash>
    inline typename PersistentMap<Key, T, Hash>::const_iterator PersistentMap<Key, T, Hash>::cend() const {
        return const_iterator(m_vector->cend(), m_vector->cend(), m_vector->cbegin()->cbegin());
    }

    template<typename Key, typename T, typename Hash>
    inline const T& PersistentMap<Key, T, Hash>::operator[](const Key& key) const {
        auto hash = m_hash(key) % m_vector->size();
        auto it = std::find_if((*m_vector)[hash].cbegin(), (*m_vector)[hash].cend(), [&key](const std::pair<Key, T>& wrapper) { return key == wrapper.first; });
        return it->second;
    }

    template<typename Key, typename T, typename Hash>
    inline const T& PersistentMap<Key, T, Hash>::at(const Key& key) const {
        auto hash = m_hash(key) % m_vector->size();
        const auto& subseq = (*m_vector)[hash];
        if (subseq.empty()) {
            throw std::out_of_range("Key not found");
        }
        auto it = std::find_if(subseq.cbegin(), subseq.cend(), [&key](const std::pair<Key, T>& wrapper) { return key == wrapper.first; });
        if (it == subseq.cend()) {
            throw std::out_of_range("Key not found");
        }
        return it->second;
    }

    template<typename Key, typename T, typename Hash>
    inline PersistentMap<Key, T, Hash> PersistentMap<Key, T, Hash>::set(const Key& key, const T& value) const {
        std::size_t newSize = m_size;
        auto hash = m_hash(key) % m_vector->size();
        std::shared_ptr<PersistentVector<PersistentVector<std::pair<Key, T>>>> outVector;
        if ((*m_vector)[hash].empty()) {
            ++newSize;
            if (newSize > m_vector->size() / 2) {
                auto reallocatedVector = getReallocatedVectorOfPersistentVectors(key, value);
                outVector = std::make_shared<PersistentVector<PersistentVector<std::pair<Key, T>>>>(
                    m_vector->reset(reallocatedVector.cbegin(), reallocatedVector.cend()));
            }
            else {
                outVector = std::make_shared<PersistentVector<PersistentVector<std::pair<Key, T>>>>(
                    m_vector->set(hash, (*m_vector)[hash].push_back(std::move(std::pair<Key, T>(key, value)))));
            }
        }
        else {
            auto collided = std::find_if((*m_vector)[hash].cbegin(), (*m_vector)[hash].cend(), [&key](const std::pair<Key, T>& wrapper) { return key == wrapper.first; });
            if (collided == (*m_vector)[hash].cend()) {
                ++newSize;
                if (newSize > m_vector->size() / 2) {
                    auto reallocatedVector = getReallocatedVectorOfPersistentVectors(key, value);
                    outVector = std::make_shared<PersistentVector<PersistentVector<std::pair<Key, T>>>>(
                        m_vector->reset(reallocatedVector.cbegin(), reallocatedVector.cend()));
                }
                else {
                    outVector = std::make_shared<PersistentVector<PersistentVector<std::pair<Key, T>>>>(
                        m_vector->set(hash, (*m_vector)[hash].push_back(std::move(std::pair<Key, T>(key, value)))));
                }
            }
            else {
                outVector = std::make_shared<PersistentVector<PersistentVector<std::pair<Key, T>>>>(
                    m_vector->set(hash, (*m_vector)[hash].set(collided.getId(), std::move(std::pair<Key, T>(key, value)))));
            }
        }
        return PersistentMap<Key, T, Hash>(m_hash, newSize, outVector);
    }

    template<typename Key, typename T, typename Hash>
    inline bool PersistentMap<Key, T, Hash>::operator==(const PersistentMap& other) const {
        bool equals = true;
        if (this != &other && m_vector != other.m_vector) {
            if (m_size != other.m_size) {
                equals = false;
            }
            else {
                for (auto it = cbegin(); equals && it != cend(); ++it) {
                    try {
                        equals = it->second == other.at(it->first);
                    }
                    catch (std::out_of_range&) {
                        equals = false;
                    }
                }
            }
        }
        return equals;
    }

    template<typename Key, typename T, typename Hash>
    inline bool PersistentMap<Key, T, Hash>::operator!=(const PersistentMap& other) const {
        return !(*this == other);
    }

    template<typename Key, typename T, typename Hash>
    inline void PersistentMap<Key, T, Hash>::swap(PersistentMap& other) {
        if (this != &other) {
            std::swap(m_hash, other.m_hash);
            std::swap(m_size, other.m_size);
            std::swap(m_vector, other.m_vector);
        }
    }

    template<typename Key, typename T, typename Hash>
    inline std::size_t PersistentMap<Key, T, Hash>::size() const {
        return m_size;
    }

    template<typename Key, typename T, typename Hash>
    inline bool PersistentMap<Key, T, Hash>::empty() const {
        return 0 == size();
    }

    template<typename Key, typename T, typename Hash>
    inline bool PersistentMap<Key, T, Hash>::canUndo() const {
        return m_vector->canUndo();
    }

    template<typename Key, typename T, typename Hash>
    inline bool PersistentMap<Key, T, Hash>::canRedo() const {
        return m_vector->canRedo();
    }

    template<typename Key, typename T, typename Hash>
    inline PersistentMap<Key, T, Hash> PersistentMap<Key, T, Hash>::undo() const {
        auto outVector = std::make_shared<PersistentVector<PersistentVector<std::pair<Key, T>>>>(m_vector->undo());
        std::size_t newSize = 0;
        for (auto it = outVector->cbegin(); it != outVector->cend(); ++it) {
            newSize += it->size();
        }
        return PersistentMap<Key, T, Hash>(m_hash, newSize, outVector);
    }

    template<typename Key, typename T, typename Hash>
    inline PersistentMap<Key, T, Hash> PersistentMap<Key, T, Hash>::redo() const {
        auto outVector = std::make_shared<PersistentVector<PersistentVector<std::pair<Key, T>>>>(m_vector->redo());
        std::size_t newSize = 0;
        for (auto it = outVector->cbegin(); it != outVector->cend(); ++it) {
            newSize += it->size();
        }
        return PersistentMap<Key, T, Hash>(m_hash, newSize, outVector);
    }

    template<typename Key, typename T, typename Hash>
    inline PersistentMap<Key, T, Hash> PersistentMap<Key, T, Hash>::clear() const {
        auto outVector = std::make_shared<PersistentVector<PersistentVector<std::pair<Key, T>>>>(m_vector->clear());
        std::size_t newSize = 0;
        return PersistentMap<Key, T, Hash>(m_hash, newSize, outVector);
    }

    template<typename Key, typename T, typename Hash>
    inline std::size_t PersistentMap<Key, T, Hash>::count(const Key& key) const {
        return find(key) == cend() ? 0 : 1;
    }

    template<typename Key, typename T, typename Hash>
    inline typename PersistentMap<Key, T, Hash>::const_iterator PersistentMap<Key, T, Hash>::find(const Key& key) const {
        auto hash = m_hash(key) % m_vector->size();
        auto inner = std::find_if((*m_vector)[hash].cbegin(), (*m_vector)[hash].cend(), [&key](const std::pair<Key, T>& wrapper) { return key == wrapper.first; });
        return inner == (*m_vector)[hash].cend() ? 
            const_iterator(m_vector->cend(), m_vector->cend(), inner) :
            const_iterator(m_vector->cend(), typename PersistentVector<PersistentVector<std::pair<Key, T>>>::const_iterator(hash, m_vector.get()), inner);
    }

    template<typename Key, typename T, typename Hash>
    inline bool PersistentMap<Key, T, Hash>::contains(const Key& key) const {
        return count(key) > 0;
    }

    template<typename Key, typename T, typename Hash>
    inline PersistentMap<Key, T, Hash> PersistentMap<Key, T, Hash>::erase(const Key& key) const {
        auto hash = m_hash(key) % m_vector->size();
        std::vector<std::pair<Key, T>> v((*m_vector)[hash].cbegin(), (*m_vector)[hash].cend());
        auto target = std::find_if(v.begin(), v.end(), [&key](const std::pair<Key, T>& wrapper) { return key == wrapper.first; });
        if (target == v.end()) {
            throw std::out_of_range("Key not found");
        }
        std::size_t newSize = m_size - 1;
        v.erase(target);
        PersistentVector<std::pair<Key, T>> newSeq(v.begin(), v.end());
        auto outVector = std::make_shared<PersistentVector<PersistentVector<std::pair<Key, T>>>>(m_vector->set(hash, newSeq));
        return PersistentMap<Key, T, Hash>(m_hash, newSize, outVector);
    }

    template<typename Key, typename T, typename Hash>
    inline bool PersistentMap<Key, T, Hash>::insertToSequenceAsHash(std::vector<std::vector<std::pair<Key, T>>>& sequence, const Key& key, const T& value, std::size_t hash) {
        bool found = false;
        if (sequence[hash].empty()) {
            sequence[hash].emplace_back(key, value);
        }
        else {
            for (auto& wrapper : sequence[hash]) {
                if (wrapper.first == key) {
                    wrapper.second = value;
                    found = true;
                    break;
                }
            }
            if (!found) {
                sequence[hash].emplace_back(key, value);
            }
        }
        return !found;
    }

    template<typename Key, typename T, typename Hash>
    template<typename InputIt, typename std::enable_if<is_iterator<InputIt>, bool>::type>
    inline std::vector<std::vector<std::pair<Key, T>>> PersistentMap<Key, T, Hash>::getReallocatedVector(InputIt begin, InputIt end, std::size_t size, const Hash& hashFunc) {
        std::vector<std::vector<std::pair<Key, T>>> resetVector(size);
        for (auto it = begin; it != end; ++it) {
            for (auto it_in = it->cbegin(); it_in != it->cend(); ++it_in) {
                auto hash = hashFunc(it_in->first) % (size);
                insertToSequenceAsHash(resetVector, it_in->first, it_in->second, hash);
            }
        }
        return resetVector;
    }

    template<typename Key, typename T, typename Hash>
    inline std::vector<PersistentVector<std::pair<Key, T>>> 
        PersistentMap<Key, T, Hash>::getReallocatedVectorOfPersistentVectors(std::vector<std::vector<std::pair<Key, T>>> resetVector, std::size_t size)
    {
        std::vector<PersistentVector<std::pair<Key, T>>> out(size);
        for (std::size_t i = 0; i < resetVector.size(); ++i) {
            out[i] = PersistentVector<std::pair<Key, T>>(resetVector[i].cbegin(), resetVector[i].cend());
        }
        return out;
    }

    template<typename Key, typename T, typename Hash>
    inline std::vector<PersistentVector<std::pair<Key, T>>> PersistentMap<Key, T, Hash>::getReallocatedVectorOfPersistentVectors(const Key& key, T value) const {
        auto resetVector = getReallocatedVector(m_vector->cbegin(), m_vector->cend(), m_vector->size() * 2, m_hash);
        auto hash = m_hash(key) % (m_vector->size() * 2);
        if (!resetVector[hash].empty()) {
            bool found = false;
            for (auto& pair : resetVector[hash]) {
                if (pair.first == key) {
                    pair.second = std::move(value);
                    found = true;
                    break;
                }
            }
            if (!found) {
                resetVector[hash].emplace_back(key, std::move(value));
            }
        }
        else {
            resetVector[hash].emplace_back(key, std::move(value));
        }
        return getReallocatedVectorOfPersistentVectors(resetVector, m_vector->size() * 2);
    }
}
