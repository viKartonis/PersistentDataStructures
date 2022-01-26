#pragma once
#include "Utils.h"

#include <memory>
#include <array>
#include <stack>
#include <stdexcept>
#include <iterator>
#include <utility>

namespace pds {
    template<typename T>
    class PersistentVector;

    template<typename T>
    class vector_const_iterator {
        std::size_t m_id;
        const PersistentVector<T>* m_pvector;
    public:
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = const T*;
        using reference = const T&;

        vector_const_iterator() = delete;
        vector_const_iterator(std::size_t id, const PersistentVector<T>* pvector) : m_id(id), m_pvector(pvector) {}
        vector_const_iterator(const vector_const_iterator& other) = default;
        vector_const_iterator(vector_const_iterator&& other) = default;

        vector_const_iterator& operator=(const vector_const_iterator& other) = default;
        vector_const_iterator& operator=(vector_const_iterator&& other) = default;

        ~vector_const_iterator() = default;

        inline vector_const_iterator& operator+=(const difference_type shift);
        inline vector_const_iterator& operator-=(const difference_type shift);
        inline const T& operator*() const;
        inline const T* operator->() const;
        inline const T& operator[](const difference_type shift) const;

        inline vector_const_iterator& operator++();
        inline vector_const_iterator& operator--();
        inline vector_const_iterator operator++(int);
        inline vector_const_iterator operator--(int);

        inline difference_type operator-(const vector_const_iterator& other) const;
        inline vector_const_iterator operator+(const difference_type shift) const;
        inline vector_const_iterator operator-(const difference_type shift) const;

        inline bool operator==(const vector_const_iterator& other) const;
        inline bool operator!=(const vector_const_iterator& other) const;
        inline bool operator>(const vector_const_iterator& other) const;
        inline bool operator<(const vector_const_iterator& other) const;
        inline bool operator>=(const vector_const_iterator& other) const;
        inline bool operator<=(const vector_const_iterator& other) const;

        std::size_t getId() const;
    };

    template<typename T>
    inline vector_const_iterator<T> operator+(const typename vector_const_iterator<T>::difference_type lhs, const vector_const_iterator<T>& rhs);
    template<typename T>
    inline vector_const_iterator<T> operator-(const typename vector_const_iterator<T>::difference_type lhs, const vector_const_iterator<T>& rhs);

    constexpr std::uint32_t m_primeTreeNodeSize = 5;

	template<typename T>
	class PersistentVector {
        template<std::uint32_t degreeOfTwo>
        class PrimeTreeNode;

        template<std::uint32_t degreeOfTwo>
        class PrimeTreeRoot;

        class VectorVersionTreeNode;

        class VectorVersionTree;

        class PrimeVectorTree;

	public:
        using const_iterator = vector_const_iterator<T>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;


        PersistentVector() :
            m_versionTreeNode(std::make_shared<VectorVersionTreeNode>(std::make_shared<PrimeTreeRoot<m_primeTreeNodeSize>>())) {}
        PersistentVector(const PersistentVector& other) = default;
        PersistentVector(PersistentVector&& other) noexcept = default;

        PersistentVector(std::size_t count);
        PersistentVector(std::size_t count, const T& value);

        template<typename InputIt, typename std::enable_if<is_iterator<InputIt>, bool>::type = false>
        PersistentVector(InputIt first, InputIt last);

        PersistentVector(std::initializer_list<T> init) : PersistentVector(init.begin(), init.end()) {}

        ~PersistentVector() = default;

        // TODO: we should probably delete them
        PersistentVector& operator=(const PersistentVector& other) = default;
        PersistentVector& operator=(PersistentVector&& other) = default;

        const_iterator cbegin() const;
        const_iterator cend() const;
        const_reverse_iterator crbegin() const;
        const_reverse_iterator crend() const;

        const T& operator[](std::size_t pos) const;

        const T& at(std::size_t pos) const;

        PersistentVector set(std::size_t pos, const T& value) const;

		bool operator==(const PersistentVector& other) const;
		bool operator!=(const PersistentVector& other) const;

		void swap(PersistentVector& other);

        PersistentVector resize(std::size_t size) const;
        PersistentVector resize(std::size_t size, const T& value) const;


        template<typename InputIt, typename std::enable_if<is_iterator<InputIt>, bool>::type = false>
        PersistentVector reset(InputIt first, InputIt last) const;

		std::size_t size() const;
		bool empty() const;

        bool canUndo() const;
        bool canRedo() const;

        PersistentVector undo() const;
        PersistentVector redo() const;

        PersistentVector clear() const;

        const T& front() const;
        const T& back() const;

        PersistentVector push_back(const T& value) const;
        PersistentVector push_back(T&& value) const;
        PersistentVector pop_back() const;

        template<typename... Args>
        PersistentVector emplace_back(Args&&... args) const;

    private:
        PersistentVector(std::shared_ptr<VectorVersionTreeNode> versionTreeNode)
            : m_versionTreeNode(versionTreeNode) {}

        void push_back_inplace(const T& value);

        using NodeCreationStatus = bool;
        static constexpr NodeCreationStatus NODE_DUPLICATE = true;
        static constexpr NodeCreationStatus NEW_NODE = false;


        /*
        *
        *   PrimeTreeNode - узел первичного дерева, которое эмулирует вектор;
        *       может быть как обычным узлом, у которого дети - узлы дерева,
        *       а также листом, у которого дети - элементы вектора
        * 
        */
        template<std::uint32_t degreeOfTwo>
        class PrimeTreeNode {
        public:
            using NodeType = bool;
            static constexpr NodeType NODE = true;
            static constexpr NodeType LEAF = false;

        public:
            PrimeTreeNode() = delete;
            PrimeTreeNode(std::shared_ptr<T> insertingElement);
            PrimeTreeNode(std::shared_ptr<PrimeTreeNode<degreeOfTwo>> child);
            PrimeTreeNode(std::shared_ptr<PrimeTreeNode<degreeOfTwo>> oldChild, 
                          std::shared_ptr<PrimeTreeNode<degreeOfTwo>> newChild);
            PrimeTreeNode(const PrimeTreeNode& other);
            PrimeTreeNode(PrimeTreeNode&& other) = default;

            PrimeTreeNode& operator=(const PrimeTreeNode& other) = delete;
            PrimeTreeNode& operator=(PrimeTreeNode&& other) = delete;

            ~PrimeTreeNode() = default;

            T& get(std::size_t pos, std::uint32_t level);

            NodeCreationStatus emplace_back(std::shared_ptr<T>&& value, std::shared_ptr<PrimeTreeNode>& primeTreeNode) const;

            std::shared_ptr<PrimeTreeNode> pop_back() const;

            std::shared_ptr<PrimeTreeNode> reduce_size(std::size_t pos, std::uint32_t level) const;

            std::shared_ptr<PrimeTreeNode> set(std::size_t pos, std::uint32_t level, std::shared_ptr<T>&& value);

            std::shared_ptr<PrimeTreeNode> getFirstChild() const;

            std::shared_ptr<PrimeTreeNode> getFirstNodeWithSomeChildren() const;
            
            // Set primeTreeNode only if the result is a new node (not node duplicate)
            NodeCreationStatus emplace_back_inplace(std::shared_ptr<T>&& value, std::shared_ptr<PrimeTreeNode>& primeTreeNode);

            std::size_t size() const;

            NodeType type() const;

        private:
            static constexpr std::size_t ARRAY_SIZE = Utils::binPow(degreeOfTwo);

            NodeType m_type;
            std::unique_ptr<std::array<std::shared_ptr<PrimeTreeNode>, ARRAY_SIZE>> m_children;
            std::unique_ptr<std::array<std::shared_ptr<T>, ARRAY_SIZE>> m_values;
            std::size_t m_contentAmount;
        };


        /*
        *
        *   PrimeTreeRoot - корень первичного дерева, которое эмулирует вектор;
        *       хранит указатель на узел дерева (который может быть листом),
        *       а также размер вектора; один корень соответствует одной версии вектора.
        *
        */
        template<std::uint32_t degreeOfTwo>
        class PrimeTreeRoot {
        public:
            PrimeTreeRoot() : m_child(nullptr), m_size(0), m_depth(0) {}
            PrimeTreeRoot(std::shared_ptr<PrimeTreeNode<degreeOfTwo>> child, std::size_t size);
            PrimeTreeRoot(const PrimeTreeRoot& other) = delete;
            PrimeTreeRoot(PrimeTreeRoot&& other) = delete;

            PrimeTreeRoot& operator=(const PrimeTreeRoot& other) = delete;
            PrimeTreeRoot& operator=(PrimeTreeRoot&& other) = delete;

            ~PrimeTreeRoot() = default;

            const T& operator[](std::size_t pos) const;

            std::shared_ptr<PrimeTreeRoot> emplace_back(std::shared_ptr<T>&& value) const;
            void emplace_back_inplace(std::shared_ptr<T>&& value);

            std::shared_ptr<PrimeTreeRoot> pop_back() const;

            // Size have to be different with the current size
            std::shared_ptr<PrimeTreeRoot> resize(std::size_t size) const;
            std::shared_ptr<PrimeTreeRoot> resize(std::size_t size, const T& value) const;
            
            std::shared_ptr<PrimeTreeRoot> set(std::size_t pos, std::shared_ptr<T>&& value);

            std::size_t size() const;

        private:
            void setSize(std::size_t size);

        private:
            std::shared_ptr<PrimeTreeNode<degreeOfTwo>> m_child;
            std::size_t m_size;
            std::uint32_t m_depth;
        };


        /*
        *
        *   VectorVersionTreeNode - узел версионного дерева;
        *       хранит указатель на корень первичного дерева, соответствующего текущей версии, а также номер самой версии;
        *       хранит своего родителя для возможности откатиться к предыдущей версии и для сборки мусора;
        *       хранит ребенка, к которому можно сделать redo;
        *       TODO: хранит оригинальну версию, к которой был сделан redo (если есть ребенок, иначе nullptr)
        *
        */

        class VectorVersionTreeNode {
        public:
            VectorVersionTreeNode() = delete;
            VectorVersionTreeNode(const VectorVersionTreeNode& other) = default;
            VectorVersionTreeNode(VectorVersionTreeNode&& other) = default;
            VectorVersionTreeNode(std::shared_ptr<PrimeTreeRoot<m_primeTreeNodeSize>> root, std::shared_ptr<VectorVersionTreeNode> parent) :
                m_root(std::move(root)),
                m_parent(parent),
                m_redoChild(nullptr),
                m_myOrig(nullptr) {}
            VectorVersionTreeNode(std::shared_ptr<VectorVersionTreeNode> other, std::shared_ptr<VectorVersionTreeNode> redoChild) :
                m_root(other->m_root),
                m_parent(other->m_parent),
                m_redoChild(redoChild),
                m_myOrig(other) {}
            VectorVersionTreeNode(std::shared_ptr<PrimeTreeRoot<m_primeTreeNodeSize>> root)
                : VectorVersionTreeNode(std::move(root), nullptr) {}

            VectorVersionTreeNode& operator=(const VectorVersionTreeNode& other) = delete;
            VectorVersionTreeNode& operator=(VectorVersionTreeNode&& other) = delete;

            ~VectorVersionTreeNode();

            PrimeTreeRoot<m_primeTreeNodeSize>& getRoot() { return *m_root; }

            std::shared_ptr<VectorVersionTreeNode> getParent() const {
                return m_parent;
            }

            std::shared_ptr<VectorVersionTreeNode> getRedoChild() const {
                return m_redoChild;
            }

            std::shared_ptr<VectorVersionTreeNode> getOrig() const {
                return m_myOrig;
            }

        private:
            std::shared_ptr<PrimeTreeRoot<m_primeTreeNodeSize>> m_root;
            std::shared_ptr<VectorVersionTreeNode> m_parent;
            std::shared_ptr<VectorVersionTreeNode> m_redoChild;
            std::shared_ptr<VectorVersionTreeNode> m_myOrig;
        };

        std::shared_ptr<VectorVersionTreeNode> m_versionTreeNode;
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
    
    template<typename T>
    inline vector_const_iterator<T>& vector_const_iterator<T>::operator+=(const difference_type shift) {
        m_id += shift;
        return *this;
    }

    template<typename T>
    inline vector_const_iterator<T>& vector_const_iterator<T>::operator-=(const difference_type shift) {
        m_id -= shift;
        return *this;
    }

    template<typename T>
    inline const T& vector_const_iterator<T>::operator*() const {
        return (*m_pvector)[m_id];
    }

    template<typename T>
    inline const T* vector_const_iterator<T>::operator->() const {
        return &(*m_pvector)[m_id];
    }

    template<typename T>
    inline const T& vector_const_iterator<T>::operator[](const difference_type shift) const {
        return (*m_pvector)[m_id + shift];
    }

    template<typename T>
    inline vector_const_iterator<T>& vector_const_iterator<T>::operator++() {
        ++m_id;
        return *this;
    }

    template<typename T>
    inline vector_const_iterator<T>& vector_const_iterator<T>::operator--() {
        --m_id;
        return *this;
    }

    template<typename T>
    inline vector_const_iterator<T> vector_const_iterator<T>::operator++(int) {
        auto copy = *this;
        ++m_id;
        return copy;
    }

    template<typename T>
    inline vector_const_iterator<T> vector_const_iterator<T>::operator--(int) {
        auto copy = *this;
        --m_id;
        return copy;
    }

    template<typename T>
    inline typename vector_const_iterator<T>::difference_type vector_const_iterator<T>::operator-(const vector_const_iterator& other) const {
        return static_cast<vector_const_iterator<T>::difference_type>(m_id - other.m_id);
    }

    template<typename T>
    inline vector_const_iterator<T> vector_const_iterator<T>::operator+(const difference_type shift) const {
        auto copy = *this;
        copy += shift;
        return copy;
    }

    template<typename T>
    inline vector_const_iterator<T> vector_const_iterator<T>::operator-(const difference_type shift) const {
        auto copy = *this;
        copy -= shift;
        return copy;
    }

    template<typename T>
    inline bool vector_const_iterator<T>::operator==(const vector_const_iterator<T>& other) const {
        return m_id == other.m_id;
    }

    template<typename T>
    inline bool vector_const_iterator<T>::operator!=(const vector_const_iterator<T>& other) const {
        return !(*this == other);
    }

    template<typename T>
    inline bool vector_const_iterator<T>::operator<(const vector_const_iterator<T>& other) const {
        return m_id < other.m_id;
    }

    template<typename T>
    inline bool vector_const_iterator<T>::operator>(const vector_const_iterator<T>& other) const {
        return other < *this;
    }

    template<typename T>
    inline bool vector_const_iterator<T>::operator>=(const vector_const_iterator<T>& other) const {
        return !(*this < other);
    }

    template<typename T>
    inline bool vector_const_iterator<T>::operator<=(const vector_const_iterator<T>& other) const {
        return !(other < *this);
    }

    template<typename T>
    inline std::size_t vector_const_iterator<T>::getId() const {
        return m_id;
    }

    template<typename T>
    inline vector_const_iterator<T> operator+(const typename vector_const_iterator<T>::difference_type lhs, const vector_const_iterator<T>& rhs) {
        return rhs + lhs;
    }

    template<typename T>
    inline vector_const_iterator<T> operator-(const typename vector_const_iterator<T>::difference_type lhs, const vector_const_iterator<T>& rhs) {
        return rhs - lhs;
    }


    /*
    * 
    *   Persistent vector
    * 
    */
    template<typename T>
    PersistentVector<T>::PersistentVector(std::size_t count) : PersistentVector<T>::PersistentVector() {
        for (size_t i = 0; i < count; ++i) {
            push_back_inplace(T());
        }
    }

    template<typename T>
    PersistentVector<T>::PersistentVector(std::size_t count, const T& value) : PersistentVector<T>::PersistentVector() {
        for (size_t i = 0; i < count; ++i) {
            push_back_inplace(value);
        }
    }

    template<typename T>
    template<typename InputIt, typename std::enable_if<is_iterator<InputIt>, bool>::type>
    PersistentVector<T>::PersistentVector(InputIt first, InputIt last) : PersistentVector<T>::PersistentVector() {
        for (; first != last; ++first) {
            push_back_inplace(*first);
        }
    }

    template<typename T>
    typename PersistentVector<T>::const_iterator PersistentVector<T>::cbegin() const {
        return const_iterator(0, this);
    }

    template<typename T>
    typename PersistentVector<T>::const_iterator PersistentVector<T>::cend() const {
        return const_iterator(size(), this);
    }

    template<typename T>
    typename PersistentVector<T>::const_reverse_iterator PersistentVector<T>::crbegin() const {
        return const_reverse_iterator(cend());
    }

    template<typename T>
    typename PersistentVector<T>::const_reverse_iterator PersistentVector<T>::crend() const {
        return const_reverse_iterator(cbegin());
    }

    template<typename T>
    inline const T& PersistentVector<T>::operator[](std::size_t pos) const {
        return m_versionTreeNode->getRoot()[pos];
    }

    template<typename T>
    inline const T& PersistentVector<T>::at(std::size_t pos) const {
        if (pos >= size()) {
            throw std::out_of_range("Index is greater than vector size");
        }
        return (*this)[pos];
    }

    template<typename T>
    inline PersistentVector<T> PersistentVector<T>::set(std::size_t pos, const T& value) const {
        auto newRoot = m_versionTreeNode->getRoot().set(pos, std::move(std::make_shared<T>(value)));
        auto newVectorVersionTreeNode = nullptr == m_versionTreeNode->getRedoChild() ? m_versionTreeNode : m_versionTreeNode->getOrig();
        auto newVersionTreeNode = std::make_shared<VectorVersionTreeNode>(newRoot, newVectorVersionTreeNode);
        return PersistentVector<T>(newVersionTreeNode);
    }

    template<typename T>
    bool PersistentVector<T>::operator==(const PersistentVector<T>& other) const {
        bool out = false;
        if (m_versionTreeNode == other.m_versionTreeNode)
        {
            out = true;
        }
        else if (size() == other.size()) {
            out = true;
            for (auto it = cbegin(), it1 = other.cbegin(); it != cend() && out == true; ++it, ++it1) {
                if (*it != *it1) {
                    out = false;
                }
            }
        }
        return out;
    }

    template<typename T>
    bool PersistentVector<T>::operator!=(const PersistentVector<T>& other) const {
        return !(*this == other);
    }

    template<typename T>
    void PersistentVector<T>::swap(PersistentVector<T>& other) {
        if (this != &other) {
            std::swap(m_versionTreeNode, other.m_versionTreeNode);
        }
    }

    template<typename T>
    inline PersistentVector<T> PersistentVector<T>::resize(std::size_t size) const {
        if (size == this->size()) {
            return PersistentVector<T>(*this);
        }
        auto newRoot = m_versionTreeNode->getRoot().resize(size);
        auto newVectorVersionTreeNode = nullptr == m_versionTreeNode->getRedoChild() ? m_versionTreeNode : m_versionTreeNode->getOrig();
        auto newVersionTreeNode = std::make_shared<VectorVersionTreeNode>(newRoot, newVectorVersionTreeNode);
        return PersistentVector<T>(newVersionTreeNode);
    }

    template<typename T>
    inline PersistentVector<T> PersistentVector<T>::resize(std::size_t size, const T& value) const {
        if (size == this->size()) {
            return PersistentVector<T>(*this);
        }
        auto newRoot = m_versionTreeNode->getRoot().resize(size, value);
        auto newVectorVersionTreeNode = nullptr == m_versionTreeNode->getRedoChild() ? m_versionTreeNode : m_versionTreeNode->getOrig();
        auto newVersionTreeNode = std::make_shared<VectorVersionTreeNode>(newRoot, newVectorVersionTreeNode);
        return PersistentVector<T>(newVersionTreeNode);
    }

    template<typename T>
    inline std::size_t PersistentVector<T>::size() const {
        return m_versionTreeNode->getRoot().size();
    }

    template<typename T>
    inline bool PersistentVector<T>::empty() const {
        return 0 == size();
    }

    template<typename T>
    inline bool PersistentVector<T>::canUndo() const {
        return nullptr != m_versionTreeNode->getParent();
    }

    template<typename T>
    inline bool PersistentVector<T>::canRedo() const {
        return nullptr != m_versionTreeNode->getRedoChild();
    }

    template<typename T>
    inline PersistentVector<T> PersistentVector<T>::undo() const {
        auto newVersion = std::make_shared<VectorVersionTreeNode>(m_versionTreeNode->getParent(), m_versionTreeNode);
        return PersistentVector(newVersion);
    }

    template<typename T>
    inline PersistentVector<T> PersistentVector<T>::redo() const {
        auto redoChild = m_versionTreeNode->getRedoChild();
        return PersistentVector(redoChild);
    }

    template<typename T>
    inline const T& PersistentVector<T>::front() const {
        return (*this)[0];
    }

    template<typename T>
    inline const T& PersistentVector<T>::back() const {
        return (*this)[size() - 1];
    }

    template<typename T>
    inline PersistentVector<T> PersistentVector<T>::push_back(const T& value) const {
        return push_back(std::move(T(value)));
    }

    template<typename T>
    inline PersistentVector<T> PersistentVector<T>::push_back(T&& value) const {
        auto newRoot = m_versionTreeNode->getRoot().emplace_back(std::move(std::make_shared<T>(value)));
        auto newVectorVersionTreeNode = nullptr == m_versionTreeNode->getRedoChild() ? m_versionTreeNode : m_versionTreeNode->getOrig();
        auto newVersionTreeNode = std::make_shared<VectorVersionTreeNode>(newRoot, newVectorVersionTreeNode);
        return PersistentVector<T>(newVersionTreeNode);
    }

    template<typename T>
    inline PersistentVector<T> PersistentVector<T>::pop_back() const {
        auto newRoot = m_versionTreeNode->getRoot().pop_back();
        auto newVectorVersionTreeNode = nullptr == m_versionTreeNode->getRedoChild() ? m_versionTreeNode : m_versionTreeNode->getOrig();
        auto newVersionTreeNode = std::make_shared<VectorVersionTreeNode>(newRoot, newVectorVersionTreeNode);
        return PersistentVector<T>(newVersionTreeNode);
    }

    template<typename T>
    template<typename InputIt, typename std::enable_if<is_iterator<InputIt>, bool>::type>
    inline PersistentVector<T> PersistentVector<T>::reset(InputIt first, InputIt last) const {
        auto newRoot = std::make_shared<PrimeTreeRoot<m_primeTreeNodeSize>>();
        for (; first != last; ++first) {
            newRoot->emplace_back_inplace(std::move(std::make_shared<T>(T(*first))));
        }
        auto newVectorVersionTreeNode = nullptr == m_versionTreeNode->getRedoChild() ? m_versionTreeNode : m_versionTreeNode->getOrig();
        auto newVersionTreeNode = std::make_shared<VectorVersionTreeNode>(newRoot, newVectorVersionTreeNode);
        return PersistentVector<T>(newVersionTreeNode);
    }

    template<typename T>
    template<typename ...Args>
    inline PersistentVector<T> PersistentVector<T>::emplace_back(Args && ...args) const {
        auto newRoot = m_versionTreeNode->getRoot().emplace_back(std::move(std::make_shared<T, Args...>(args...)));
        auto newVectorVersionTreeNode = nullptr == m_versionTreeNode->getRedoChild() ? m_versionTreeNode : m_versionTreeNode->getOrig();
        auto newVersionTreeNode = std::make_shared<VectorVersionTreeNode>(newRoot, newVectorVersionTreeNode);
        return PersistentVector<T>(newVersionTreeNode);
    }

    template<typename T>
    inline PersistentVector<T> PersistentVector<T>::clear() const {
        return resize(0);
    }

    template<typename T>
    inline void PersistentVector<T>::push_back_inplace(const T& value) {
        m_versionTreeNode->getRoot().emplace_back_inplace(std::move(std::make_shared<T>(T(value))));
    }


    /*
    * 
    *   PrimeTreeRoot
    * 
    */

    template<typename T>
    template<std::uint32_t degreeOfTwo>
    PersistentVector<T>::PrimeTreeRoot<degreeOfTwo>::PrimeTreeRoot(std::shared_ptr<PrimeTreeNode<degreeOfTwo>> child, 
                                                                   std::size_t size)
        : m_child(child),
        m_size(size)
    {
        setSize(size);
    }

    template<typename T>
    template<std::uint32_t degreeOfTwo>
    inline const T& PersistentVector<T>::PrimeTreeRoot<degreeOfTwo>::operator[](std::size_t pos) const {
        return m_child->get(pos, m_depth - 1);
    }

    template<typename T>
    template<std::uint32_t degreeOfTwo>
    inline std::size_t PersistentVector<T>::PrimeTreeRoot<degreeOfTwo>::size() const {
        return m_size;
    }

    template<typename T>
    template<std::uint32_t degreeOfTwo>
    inline void PersistentVector<T>::PrimeTreeRoot<degreeOfTwo>::setSize(std::size_t size) {
        m_size = size;
        m_depth = 0;
        if (size) {
            --size;
        }
        while (size) {
            ++m_depth;
            size >>= degreeOfTwo;
        }
    }

    template<typename T>
    template<std::uint32_t degreeOfTwo>
    typename std::shared_ptr<typename PersistentVector<T>::template PrimeTreeRoot<degreeOfTwo>>
        PersistentVector<T>::PrimeTreeRoot<degreeOfTwo>::emplace_back(std::shared_ptr<T>&& value) const
    {
        std::shared_ptr<PrimeTreeNode<degreeOfTwo>> child;
        std::shared_ptr<PrimeTreeNode<degreeOfTwo>> childOfNewRoot;
        if (nullptr == m_child.get()) {
            childOfNewRoot = std::make_shared<PrimeTreeNode<degreeOfTwo>>(std::move(value));
        }
        else {
            auto childCreationStatus = m_child->emplace_back(std::move(value), child);
            if (childCreationStatus == NEW_NODE) {
                childOfNewRoot = std::make_shared<PrimeTreeNode<degreeOfTwo>>(m_child, child);
            }
            // otherwise childCreationStatus == NODE_DUPLICATE
            else {
                childOfNewRoot = child;
            }
        }
        return std::make_shared<PrimeTreeRoot<degreeOfTwo>>(childOfNewRoot, m_size + 1);
    }

    template<typename T>
    template<std::uint32_t degreeOfTwo>
    typename std::shared_ptr<typename PersistentVector<T>::template PrimeTreeRoot<degreeOfTwo>>
        PersistentVector<T>::PrimeTreeRoot<degreeOfTwo>::pop_back() const
    {
        std::shared_ptr<PrimeTreeNode<degreeOfTwo>> child = m_child->pop_back();
        std::shared_ptr<PrimeTreeNode<degreeOfTwo>> childOfNewRoot;
        if (nullptr != child && child->type() == PrimeTreeNode<degreeOfTwo>::NODE && child->size() == 1) {
            childOfNewRoot = child->getFirstChild();
        }
        else {
            childOfNewRoot = child;
        }
        return std::make_shared<PrimeTreeRoot<degreeOfTwo>>(childOfNewRoot, m_size - 1);
    }

    template<typename T>
    template<std::uint32_t degreeOfTwo>
    void PersistentVector<T>::PrimeTreeRoot<degreeOfTwo>::emplace_back_inplace(std::shared_ptr<T>&& value)
    {
        if (nullptr == m_child.get()) {
            m_child = std::make_shared<PrimeTreeNode<degreeOfTwo>>(std::move(value));
        }
        else {
            std::shared_ptr<PrimeTreeNode<degreeOfTwo>> child;
            auto childCreationStatus = m_child->emplace_back_inplace(std::move(value), child);
            if (childCreationStatus == NEW_NODE) {
                m_child = std::make_shared<PrimeTreeNode<degreeOfTwo>>(m_child, child);
            }
        }
        setSize(size() + 1);
    }

    template<typename T>
    template<std::uint32_t degreeOfTwo>
    typename std::shared_ptr<typename PersistentVector<T>::template PrimeTreeRoot<degreeOfTwo>>
        PersistentVector<T>::PrimeTreeRoot<degreeOfTwo>::set(std::size_t pos, std::shared_ptr<T>&& value)
    {
        return std::make_shared<PrimeTreeRoot<degreeOfTwo>>(m_child->set(pos, m_depth - 1, std::move(value)), m_size);
    }
    
    template<typename T>
    template<std::uint32_t degreeOfTwo>
    typename std::shared_ptr<typename PersistentVector<T>::template PrimeTreeRoot<degreeOfTwo>>
        PersistentVector<T>::PrimeTreeRoot<degreeOfTwo>::resize(std::size_t size) const
    {
        std::shared_ptr<PrimeTreeRoot<degreeOfTwo>> out;
        if (size < m_size) {
            auto child = m_child->reduce_size(size, m_depth - 1);
            if (child != nullptr && child->type() == PrimeTreeNode<degreeOfTwo>::NODE && child->size() == 1) {
                child = child->getFirstNodeWithSomeChildren();
            }
            out = std::make_shared<PrimeTreeRoot<degreeOfTwo>>(std::move(child), size);
            
        }
        else {
            out = std::make_shared<PrimeTreeRoot<degreeOfTwo>>(m_child, m_size);
            for (auto i = m_size; i < size; ++i) {
                out->emplace_back_inplace(std::make_shared<T>(std::move(T())));
            }
        }
        return out;
    }

    template<typename T>
    template<std::uint32_t degreeOfTwo>
    typename std::shared_ptr<typename PersistentVector<T>::template PrimeTreeRoot<degreeOfTwo>>
        PersistentVector<T>::PrimeTreeRoot<degreeOfTwo>::resize(std::size_t size, const T& value) const
    {
        std::shared_ptr<PrimeTreeRoot<degreeOfTwo>> out;
        if (size < m_size) {
            auto child = m_child->reduce_size(size, m_depth - 1);
            if (child != nullptr && child->type() == PrimeTreeNode<degreeOfTwo>::NODE && child->size() == 1) {
                child = child->getFirstNodeWithSomeChildren();
            }
            out = std::make_shared<PrimeTreeRoot<degreeOfTwo>>(std::move(child), size);
        }
        else {
            out = std::make_shared<PrimeTreeRoot<degreeOfTwo>>(m_child, m_size);
            for (auto i = m_size; i < size; ++i) {
                out->emplace_back_inplace(std::make_shared<T>(std::move(T(value))));
            }
        }
        return out;
    }


    /*
    * 
    *   PrimeTreeNode
    * 
    */

    template<typename T>
    template<std::uint32_t degreeOfTwo>
    inline T& PersistentVector<T>::PrimeTreeNode<degreeOfTwo>::get(std::size_t pos, std::uint32_t level) {
        if (m_type == NODE) {
            auto id = Utils::getId(pos, level, degreeOfTwo);
            auto mask = Utils::getMask(level, degreeOfTwo);
            return (*m_children)[id]->get(pos & mask, level - 1);
        }
        // otherwise m_type == LEAF
        else {
            return *((*m_values)[pos]);
        }
    }

    template<typename T>
    template<std::uint32_t degreeOfTwo>
    inline typename PersistentVector<T>::NodeCreationStatus PersistentVector<T>::PrimeTreeNode<degreeOfTwo>::emplace_back(
            std::shared_ptr<T>&& value, 
            std::shared_ptr<PrimeTreeNode>& primeTreeNode) const
    {
        PersistentVector<T>::NodeCreationStatus out;
        if (m_type == LEAF) {
            if (m_contentAmount < Utils::binPow(degreeOfTwo)) {
                auto this_copy = *this;
                primeTreeNode = std::make_shared<PrimeTreeNode>(std::move(this_copy));
                (*(primeTreeNode->m_values))[primeTreeNode->m_contentAmount] = std::move(value);
                ++primeTreeNode->m_contentAmount;
                out = NODE_DUPLICATE;
            }
            else {
                primeTreeNode = std::make_shared<PrimeTreeNode>(std::move(value));
                out = NEW_NODE;
            }
        }
        else {
            std::shared_ptr<PrimeTreeNode> child;
            auto childCreationStatus = (*m_children)[m_contentAmount - 1]->emplace_back(std::move(value), child);
            if (childCreationStatus == NODE_DUPLICATE) {
                primeTreeNode = std::make_shared<PrimeTreeNode>(*this);
                (*primeTreeNode->m_children)[primeTreeNode->m_contentAmount - 1] = std::move(child);
                out = NODE_DUPLICATE;
            }
            else {
                if (m_contentAmount < Utils::binPow(degreeOfTwo)) {
                    primeTreeNode = std::make_shared<PrimeTreeNode>(*this);
                    (*(primeTreeNode->m_children))[primeTreeNode->m_contentAmount] = std::move(child);
                    ++primeTreeNode->m_contentAmount;
                    out = NODE_DUPLICATE;
                }
                else {
                    primeTreeNode = std::make_shared<PrimeTreeNode>(std::move(child));
                    out = NEW_NODE;
                }
            }
        }
        return out;
    }

    template<typename T>
    template<std::uint32_t degreeOfTwo>
    inline typename std::shared_ptr<typename PersistentVector<T>::template PrimeTreeNode<degreeOfTwo>> 
        PersistentVector<T>::PrimeTreeNode<degreeOfTwo>::pop_back() const
    {
        std::shared_ptr<PrimeTreeNode> out;
        if (m_type == LEAF) {
            if (m_contentAmount > 1) {
                out = std::make_shared<PrimeTreeNode>(*this);
                --out->m_contentAmount;
                (*(out->m_values))[out->m_contentAmount].reset();
            }
            else {
                out = nullptr;
            }
        }
        else {
            std::shared_ptr<PrimeTreeNode> child = (*m_children)[m_contentAmount - 1]->pop_back();
            if (nullptr != child) {
                out = std::make_shared<PrimeTreeNode>(*this);
                (*out->m_children)[out->m_contentAmount - 1] = std::move(child);
            }
            else {
                if (m_contentAmount > 1) {
                    out = std::make_shared<PrimeTreeNode>(*this);
                    --out->m_contentAmount;
                    (*(out->m_children))[out->m_contentAmount].reset();
                }
                else {
                    out = nullptr;
                }
            }
        }
        return out;
    }

    template<typename T>
    template<std::uint32_t degreeOfTwo>
    inline typename std::shared_ptr<typename PersistentVector<T>::template PrimeTreeNode<degreeOfTwo>> 
        PersistentVector<T>::PrimeTreeNode<degreeOfTwo>::reduce_size(std::size_t pos, std::uint32_t level) const
    {
        std::shared_ptr<PrimeTreeNode<degreeOfTwo>> out;
        if (m_type == LEAF) {
            if (!pos) {
                out = nullptr;
            }
            else {
                out = std::make_shared<PrimeTreeNode>(m_values->front());
                for (std::size_t i = 1; i < pos; ++i) {
                    (*(out->m_values))[i] = (*m_values)[i];
                }
                out->m_contentAmount = pos;
            }
        }
        else {
            auto id = Utils::getId(pos, level, degreeOfTwo);
            auto mask = Utils::getMask(level, degreeOfTwo);
            auto child = (*m_children)[id]->reduce_size(pos & mask, level - 1);
            if (nullptr != child) {
                out = std::make_shared<PrimeTreeNode>(m_children->front());
                for (std::size_t i = 1; i < id; ++i) {
                    (*(out->m_children))[i] = (*m_children)[i];
                }
                (*(out->m_children))[id] = child;
                out->m_contentAmount = id + 1;
            }
            else {
                if (id > 0) {
                    out = std::make_shared<PrimeTreeNode>(m_children->front());
                    for (std::size_t i = 1; i < id; ++i) {
                        (*(out->m_children))[i] = (*m_children)[i];
                    }
                    out->m_contentAmount = id;
                }
                else {
                    out = nullptr;
                }
            }
        }
        return out;
    }


    template<typename T>
    template<std::uint32_t degreeOfTwo>
    inline typename std::shared_ptr<typename PersistentVector<T>::template PrimeTreeNode<degreeOfTwo>> PersistentVector<T>::PrimeTreeNode<degreeOfTwo>::set(
        std::size_t pos,
        std::uint32_t level,
        std::shared_ptr<T>&& value)
    {
        auto id = Utils::getId(pos, level, degreeOfTwo);
        auto mask = Utils::getMask(level, degreeOfTwo);
        std::shared_ptr<PrimeTreeNode<degreeOfTwo>> out;
        if (m_type == LEAF) {
            auto this_copy = *this;
            out = std::make_shared<PrimeTreeNode>(std::move(this_copy));
            (*(out->m_values))[pos] = std::move(value);
        }
        else {
            out = std::make_shared<PrimeTreeNode>(*this);
            (*out->m_children)[id] = std::move((*m_children)[id]->set(pos & mask, level - 1, std::move(value)));
        }
        return out;
    }

    template<typename T>
    template<std::uint32_t degreeOfTwo>
    inline typename std::shared_ptr<typename PersistentVector<T>::template PrimeTreeNode<degreeOfTwo>> PersistentVector<T>::PrimeTreeNode<degreeOfTwo>::getFirstChild() const {
        return (*m_children)[0];
    }

    template<typename T>
    template<std::uint32_t degreeOfTwo>
    inline typename std::shared_ptr<typename PersistentVector<T>::template PrimeTreeNode<degreeOfTwo>> PersistentVector<T>::PrimeTreeNode<degreeOfTwo>::getFirstNodeWithSomeChildren() const {
        std::shared_ptr<PrimeTreeNode<degreeOfTwo>> out;
        if ((*m_children)[0]->type() == LEAF) {
            out = m_children->front();
        }
        else if ((*m_children)[0]->size() > 1) {
            out = (*m_children)[0];
        }
        else {
            out = (*m_children)[0]->getFirstNodeWithSomeChildren();
        }
        return out;
    }
    
    template<typename T>
    template<std::uint32_t degreeOfTwo>
    inline typename PersistentVector<T>::NodeCreationStatus PersistentVector<T>::PrimeTreeNode<degreeOfTwo>::emplace_back_inplace(
        std::shared_ptr<T>&& value,
        std::shared_ptr<PrimeTreeNode>& primeTreeNode) {
        PersistentVector<T>::NodeCreationStatus out;
        if (m_type == LEAF) {
            if (m_contentAmount < Utils::binPow(degreeOfTwo)) {
                (*(m_values))[m_contentAmount] = std::move(value);
                ++m_contentAmount;
                out = NODE_DUPLICATE;
            }
            else {
                primeTreeNode = std::make_shared<PrimeTreeNode>(std::move(value));
                out = NEW_NODE;
            }
        }
        else {
            std::shared_ptr<PrimeTreeNode> child;
            auto childCreationStatus = (*m_children)[m_contentAmount - 1]->emplace_back_inplace(std::move(value), child);
            out = NODE_DUPLICATE;
            if (childCreationStatus == NEW_NODE) {
                if (m_contentAmount < Utils::binPow(degreeOfTwo)) {
                    (*m_children)[m_contentAmount] = std::move(child);
                    ++m_contentAmount;
                    out = NODE_DUPLICATE;
                }
                else {
                    primeTreeNode = std::make_shared<PrimeTreeNode>(std::move(child));
                    out = NEW_NODE;
                }
            }
        }
        return out;
    }

    template<typename T>
    template<std::uint32_t degreeOfTwo>
    std::size_t PersistentVector<T>::PrimeTreeNode<degreeOfTwo>::size() const {
        return m_contentAmount;
    }

    template<typename T>
    template<std::uint32_t degreeOfTwo>
    inline typename PersistentVector<T>::template PrimeTreeNode<degreeOfTwo>::NodeType PersistentVector<T>::PrimeTreeNode<degreeOfTwo>::type() const {
        return m_type;
    }

    template<typename T>
    template<std::uint32_t degreeOfTwo>
    inline PersistentVector<T>::PrimeTreeNode<degreeOfTwo>::PrimeTreeNode(std::shared_ptr<T> insertingElement) : m_type(LEAF) {
        m_values = std::make_unique<std::array<std::shared_ptr<T>, ARRAY_SIZE>>();
        (*m_values)[0] = insertingElement;
        m_contentAmount = 1;
    }

    template<typename T>
    template<std::uint32_t degreeOfTwo>
    inline PersistentVector<T>::PrimeTreeNode<degreeOfTwo>::PrimeTreeNode(std::shared_ptr<PrimeTreeNode<degreeOfTwo>> child) : m_type(NODE) {
        m_children = std::make_unique<std::array<std::shared_ptr<PrimeTreeNode>, ARRAY_SIZE>>();
        (*m_children)[0] = child;
        m_contentAmount = 1;
    }

    template<typename T>
    template<std::uint32_t degreeOfTwo>
    inline PersistentVector<T>::PrimeTreeNode<degreeOfTwo>::PrimeTreeNode(std::shared_ptr<PrimeTreeNode<degreeOfTwo>> oldChild, std::shared_ptr<PrimeTreeNode<degreeOfTwo>> newChild)
        : m_type(NODE)
    {
        m_children = std::make_unique<std::array<std::shared_ptr<PrimeTreeNode>, ARRAY_SIZE>>();
        (*m_children)[0] = oldChild;
        (*m_children)[1] = newChild;
        m_contentAmount = 2;
    }

    template<typename T>
    template<std::uint32_t degreeOfTwo>
    inline PersistentVector<T>::PrimeTreeNode<degreeOfTwo>::PrimeTreeNode(const PrimeTreeNode& other)
        : m_type(other.m_type)
    {
        if (m_type == NODE) {
            auto children_copy = *(other.m_children);
            m_children = std::make_unique<std::array<std::shared_ptr<PrimeTreeNode>, ARRAY_SIZE>>(std::move(children_copy));
        }
        // otherwise m_type = LEAF
        else {
            auto values_copy = *(other.m_values);
            m_values = std::make_unique<std::array<std::shared_ptr<T>, ARRAY_SIZE>>(std::move(values_copy));
        }
        m_contentAmount = other.m_contentAmount;
    }


    /*
    * 
    *   VectorVersionTreeNode
    * 
    */

    template<typename T>
    PersistentVector<T>::VectorVersionTreeNode::~VectorVersionTreeNode() {
        std::stack<std::shared_ptr<VectorVersionTreeNode>> uniqueLinkedParents;
        bool stop = false;
        if (nullptr != m_redoChild) {
            auto currentChild = m_redoChild;
            while (!stop) {
                if (currentChild.use_count() == 2) {
                    currentChild->m_myOrig.reset();
                    uniqueLinkedParents.push(currentChild);
                    currentChild = currentChild->m_redoChild;
                }
                else {
                    stop = true;
                }
            }
            if (!uniqueLinkedParents.empty()) {
                uniqueLinkedParents.pop();
            }
            while (!uniqueLinkedParents.empty()) {
                uniqueLinkedParents.top()->m_redoChild.reset();
                uniqueLinkedParents.pop();
            }
        }
        auto currentParent = m_parent;
        while (!stop) {
            if (currentParent.use_count() == 2) {
                uniqueLinkedParents.push(currentParent);
                currentParent = currentParent->m_parent;
            }
            else {
                stop = true;
            }
        }
        if (!uniqueLinkedParents.empty()) {
            uniqueLinkedParents.pop();
        }
        while (!uniqueLinkedParents.empty()) {
            uniqueLinkedParents.top()->m_parent.reset();
            uniqueLinkedParents.pop();
        }
    };

}
