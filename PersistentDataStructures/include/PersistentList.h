#pragma once
#include <memory>
#include <vector>
#include <iterator>

namespace pds
{
	template <typename T>
	class root_node;

	template <typename T>
	class list_fat_node;

	template <typename T>
	class node
	{
		int m_version;
		T m_value;
		std::shared_ptr<list_fat_node<T>> m_prev;
		std::shared_ptr<list_fat_node<T>> m_next;

	public:
		int get_version() const
		{
			return m_version;
		}

		T& get_value()
		{
			return m_value;
		}

		std::shared_ptr<list_fat_node<T>> get_prev()
		{
			return m_prev;
		}

		std::shared_ptr<list_fat_node<T>> get_next()
		{
			return m_next;
		}

		void set_prev(std::shared_ptr<list_fat_node<T>> prev)
		{
			m_prev = prev;
		}

		void set_next(std::shared_ptr<list_fat_node<T>> next)
		{
			m_next = next;
		}

		node(int version, T value, std::shared_ptr<list_fat_node<T>> prev,
             std::shared_ptr<list_fat_node<T>> next) : m_version(version), m_value(value),
		                                                    m_prev(prev), m_next(next)
		{
		}
	};

	template <typename T>
	class root_node
	{
		int m_version;
		int m_size;
		std::shared_ptr<root_node<T>> m_parent;
		std::shared_ptr<root_node<T>> m_child;
		std::shared_ptr<list_fat_node<T>> m_front;
		std::shared_ptr<list_fat_node<T>> m_back;

	public:
		int get_version() const
		{
			return m_version;
		}

		int size() const
		{
			return m_size;
		}

		std::shared_ptr<root_node<T>> get_parent()
		{
			return m_parent;
		}

		std::shared_ptr<list_fat_node<T>> front()
		{
			return m_front;
		}

		std::shared_ptr<list_fat_node<T>> back()
		{
			return m_back;
		}


		void set_front(std::shared_ptr<list_fat_node<T>> front)
		{
			m_front = front;
		}

		void set_back(std::shared_ptr<list_fat_node<T>> back)
		{
			m_back = back;
		}

		std::shared_ptr<root_node<T>> get_child()
		{
			return m_child;
		}

		void set_child(std::shared_ptr<root_node<T>> child)
		{
			m_child = child;
		}

		root_node(int version, int size, std::shared_ptr<list_fat_node<T>> front,
                  std::shared_ptr<list_fat_node<T>> back,
                  std::shared_ptr<root_node<T>> parent)
		{
			m_version = version;
			m_size = size;
			m_front = front;
			m_back = back;
			m_parent = parent;
		}
	};

	template <typename T>
	class list_fat_node
	{
		const int m_maxSize = 2;

	public:
		std::vector<std::shared_ptr<node<T>>> m_nodes;

		std::vector<std::shared_ptr<node<T>>>& get_nodes()
		{
			return m_nodes;
		}

		list_fat_node() : m_nodes(std::vector<std::shared_ptr<node<T>>>())
		{
		}

		list_fat_node(std::shared_ptr<node<T>> node) : m_nodes(std::vector<std::shared_ptr<node<T>>>{node})
		{
		}

		void add_node(std::shared_ptr<node<T>> n)
		{
			m_nodes.push_back(n);
		}

		std::shared_ptr<node<T>> find_node(std::shared_ptr<root_node<T>> version_node)
		{
			auto it = version_node;
			while (it != nullptr)
			{
				for (int i = 0; i < m_nodes.size(); i++)
				{
					if (m_nodes[i]->get_version() == it->get_version())
					{
						return m_nodes[i];
					}
				}

				it = it->get_parent();
			}
			throw std::exception();
		}

		bool is_full()
		{
			return m_nodes.size() == m_maxSize;
		}

		bool is_empty()
		{
			return m_nodes.size() == 0;
		}

		static std::shared_ptr<list_fat_node<T>> update_next(std::shared_ptr<list_fat_node<T>> fat_node,
		                                                     std::shared_ptr<list_fat_node<T>> next_node,
		                                                     std::shared_ptr<root_node<T>> version_node)
		{
			auto found_node = fat_node->find_node(version_node);
			auto new_node = std::make_shared<node<T>>(version_node->get_version(), found_node->get_value(),
                                                      found_node->get_prev(), next_node);

			if (fat_node->is_full())
			{
				auto fat = std::make_shared<list_fat_node<T>>();
				fat->add_node(new_node);
				if (found_node->get_prev() != nullptr)
				{
					new_node->set_prev(list_fat_node<T>::update_next(found_node->get_prev(), fat, version_node));
				}
				else
				{
					version_node->set_front(fat);
				}

				return fat;
			}

			fat_node->add_node(new_node);
			return fat_node;
		}

		static std::shared_ptr<list_fat_node<T>> update_prev(std::shared_ptr<list_fat_node<T>> fat_node,
		                                                     std::shared_ptr<list_fat_node<T>> prev_node,
		                                                     std::shared_ptr<root_node<T>> version_node)
		{
			auto found_node = fat_node->find_node(version_node);
			auto new_node = std::make_shared<node<T>>(version_node->get_version(), found_node->get_value(),
                                                      prev_node,
                                                      found_node->get_next());

			if (fat_node->is_full())
			{
				auto fat = std::make_shared<list_fat_node<T>>();
				fat->add_node(new_node);
				if (found_node->get_next() != nullptr)
				{
					new_node->set_next(update_prev(found_node->get_next(), fat, version_node));
				}
				else
				{
					version_node->set_back(fat);
				}
				return fat;
			}

			fat_node->add_node(new_node);
			return fat_node;
		}
	};

	template <typename T>
	class list_const_iterator
	{
		std::shared_ptr<list_fat_node<T>> m_node;
		std::shared_ptr<root_node<T>> m_root;

	public:
		using pointer = const T*;
		using reference = const T&;

		list_const_iterator() = delete;

		list_const_iterator(std::shared_ptr<list_fat_node<T>> node,
		                    std::shared_ptr<root_node<T>> root)
		{
			m_node = node;
			m_root = root;
		}

		list_const_iterator(const list_const_iterator& other) = default;
		list_const_iterator(list_const_iterator&& other) noexcept = default;

		list_const_iterator& operator=(const list_const_iterator& other) = default;
		list_const_iterator& operator=(list_const_iterator&& other) noexcept = default;

		~list_const_iterator() = default;

		inline const T& operator*() const;
		inline const T* operator->() const;

		inline list_const_iterator& operator++();
		inline list_const_iterator& operator--();
		inline const list_const_iterator operator++(int);
		inline const list_const_iterator operator--(int);

		inline bool operator==(const list_const_iterator& other) const;
		inline bool operator!=(const list_const_iterator& other) const;
	};

	template <typename T>
	inline const T& list_const_iterator<T>::operator*() const
	{
		return m_node->find_node(m_root)->get_value();
	}

	template <typename T>
	inline const T* list_const_iterator<T>::operator->() const
	{
		return &(m_node->find_node(m_root)->get_value());
	}

	template <typename T>
	inline list_const_iterator<T>& list_const_iterator<T>::operator++()
	{
		m_node = m_node->find_node(m_root)->get_next();
		return *this;
	}

	template <typename T>
	inline list_const_iterator<T>& list_const_iterator<T>::operator--()
	{
		m_node = m_node->find_node(m_root)->get_prev();
		return *this;
	}

	template <typename T>
	inline const list_const_iterator<T> list_const_iterator<T>::operator++(int)
	{
		auto copy = *this;
		m_node = m_node->find_node(m_root)->get_next();
		return copy;
	}

	template <typename T>
	inline const list_const_iterator<T> list_const_iterator<T>::operator--(int)
	{
		auto copy = *this;
		m_node = m_node->find_node(m_root)->get_prev();
		return copy;
	}

	template <typename T>
	inline bool list_const_iterator<T>::operator==(const list_const_iterator<T>& other) const
	{
		return m_node == other.m_node && m_root->get_version() == other.m_root->get_version();
	}

	template <typename T>
	inline bool list_const_iterator<T>::operator!=(const list_const_iterator<T>& other) const
	{
		return !(*this == other);
	}

	template <typename T>
	class persistent_linked_list
	{
		///Persistent list
		std::shared_ptr<int> m_versionPtr;
		std::shared_ptr<root_node<T>> m_root;

		persistent_linked_list(std::shared_ptr<int> version, std::shared_ptr<root_node<T>> root)
		{
			m_versionPtr = std::move(version);
			m_root = root;
		}

		std::shared_ptr<list_fat_node<T>> get_node_by_index(int index)
		{
			auto it = m_root->front();
			for (auto i = 0; i < index; i++)
			{
				it = it->find_node(m_root)->get_next();
			}

			return it;
		}

		persistent_linked_list<T> remove_by_node(std::shared_ptr<list_fat_node<T>> fat_node)
		{
			auto del_node = fat_node->find_node(m_root);
			if (del_node->get_prev() == nullptr && del_node->get_next() == nullptr)
			{
				auto newV = std::make_shared<root_node<T>>(++(*m_versionPtr), m_root->size() - 1,
                                                           std::make_shared<list_fat_node<T>>(),
                                                           std::make_shared<list_fat_node<T>>(), m_root);
				return persistent_linked_list<T>(m_versionPtr, newV);
			}

			auto new_version = std::make_shared<root_node<T>>(++(*m_versionPtr), m_root->size() - 1, m_root->front(),
                                                              m_root->back(),
                                                              m_root);
			if (del_node->get_next() == nullptr)
			{
				auto new_left = list_fat_node<T>::update_next(del_node->get_prev(), nullptr, new_version);
				if (fat_node == m_root->back())
				{
					new_version->set_back(new_left);
				}

				return persistent_linked_list<T>(m_versionPtr, new_version);
			}

			if (del_node->get_prev() == nullptr)
			{
				auto new_right = list_fat_node<T>::update_prev(del_node->get_next(), nullptr, new_version);
				if (fat_node == m_root->front())
				{
					new_version->set_front(new_right);
				}

				return persistent_linked_list<T>(m_versionPtr, new_version);
			}

			if (!del_node->get_next()->is_full())
			{
				auto new_left = list_fat_node<T>::update_next(del_node->get_prev(), del_node->get_next(),
				                                              new_version);
				auto new_right = list_fat_node<T>::update_prev(del_node->get_next(), del_node->get_prev(),
				                                               new_version);
				return persistent_linked_list<T>(m_versionPtr, new_version);
			}

			auto fake_left = std::make_shared<list_fat_node<T>>();
			auto new_right_node = list_fat_node<T>::update_prev(del_node->get_next(), fake_left, new_version);
			auto new_left_node = list_fat_node<T>::update_next(del_node->get_prev(), new_right_node, new_version);
			new_right_node->get_nodes()[0]->set_prev(new_left_node);

			return persistent_linked_list<T>(m_versionPtr, new_version);
		}

		persistent_linked_list<T> init_root(T value)
		{
			auto v = ++(*m_versionPtr);
			auto new_node = std::make_shared<node<T>>(v, value, std::shared_ptr<list_fat_node<T>>(nullptr),
                                                      std::shared_ptr<list_fat_node<T>>(nullptr));
			auto new_fat_node = std::make_shared<list_fat_node<T>>(new_node);

			auto new_version = std::make_shared<root_node<T>>(v, m_root->size() + 1, new_fat_node, new_fat_node,
                                                              m_root);

			return persistent_linked_list<T>(m_versionPtr, new_version);
		}

	public:
		using const_iterator = list_const_iterator<T>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		persistent_linked_list(): m_versionPtr(std::make_shared<int>(0)),
		                          m_root(std::make_shared<root_node<T>>(
			                          ++(*m_versionPtr), 0, std::make_shared<list_fat_node<T>>(),
			                          std::make_shared<list_fat_node<T>>(),
			                          std::shared_ptr<root_node<T>>(nullptr)))
		{
		}

		template <class InputIterator,
		          class = std::enable_if_t<
			          std::is_base_of<
				          std::input_iterator_tag,
				          typename std::iterator_traits<InputIterator>::iterator_category
			          >{}
		          >>
		persistent_linked_list(InputIterator first, InputIterator last)
		{
			int v = 1;
			m_versionPtr = std::make_shared<int>(v);
			int size = 0;

			auto prev_node = std::shared_ptr<node<T>>(nullptr);
			auto prev_fat = std::shared_ptr<list_fat_node<T>>(nullptr);

			std::shared_ptr<list_fat_node<T>> front;

			for (; first != last; ++first)
			{
				size++;
				auto new_node = std::make_shared<node<T>>(v, *first, prev_fat,
                                                          std::shared_ptr<list_fat_node<T>>(nullptr));
				auto new_fat = std::make_shared<list_fat_node<T>>(new_node);

				if (size == 1)
				{
					front = new_fat;
				}
				else
				{
					prev_node->set_next(new_fat);
				}

				prev_node = new_node;
				prev_fat = new_fat;
			}

			m_root = std::make_shared<root_node<T>>(v, size, front, prev_fat,
                                                    std::shared_ptr<root_node<T>>(nullptr));
		}

		const_iterator cbegin() const
		{
			return const_iterator(m_root->front(), m_root);
		}

		const_iterator cend() const
		{
			return const_iterator(m_root->back()->find_node(m_root)->get_next(), m_root);
		}

		const_reverse_iterator crbegin() const
		{
			return const_reverse_iterator(cend());
		}

		const_reverse_iterator crend() const
		{
			return const_reverse_iterator(cbegin());
		}

		const T& front()
		{
			return m_root->front()->find_node(m_root)->get_value();
		}

		const T& back()
		{
			return m_root->back()->find_node(m_root)->get_value();
		}

		persistent_linked_list<T> pop_back()
		{
			return remove_by_node(m_root->back());
		}

		persistent_linked_list<T> pop_front()
		{
			return remove_by_node(m_root->front());
		}

		persistent_linked_list<T> insert(int index, T value)
		{
			if (index < 0 || index > m_root->size())
			{
				throw std::exception();
			}

			if (index == m_root->size())
			{
				return push_back(value);
			}

			auto v = ++(*m_versionPtr);
			auto new_node = std::make_shared<node<T>>(v, value, std::shared_ptr<list_fat_node<T>>(nullptr),
                                                      std::shared_ptr<list_fat_node<T>>(nullptr));
			auto new_fat_node = std::make_shared<list_fat_node<T>>(new_node);

			auto front = m_root->front();
			if (front->is_empty())
			{
				return persistent_linked_list<T>(m_versionPtr,
				                                 std::make_shared<root_node<T>>(
					                                 v, m_root->size() + 1, new_fat_node, new_fat_node, m_root));
			}

			auto newVersion = std::make_shared<root_node<T>>(v, m_root->size() + 1, m_root->front(), m_root->back(),
                                                             m_root);
			if (index == 0)
			{
				new_node->set_next(list_fat_node<T>::update_prev(front, new_fat_node, newVersion));
				newVersion->set_front(new_fat_node);
				return persistent_linked_list<T>(m_versionPtr, newVersion);
			}

			auto prev_inserting = get_node_by_index(index - 1);
			new_node->set_prev(list_fat_node<T>::update_next(prev_inserting, new_fat_node, newVersion));
			auto next_inserting = prev_inserting->find_node(m_root)->get_next();

			if (next_inserting == nullptr)
			{
				newVersion->set_back(new_fat_node);
			}
			else
			{
				new_node->set_next(list_fat_node<T>::update_prev(next_inserting, new_fat_node, newVersion));
			}

			return persistent_linked_list<T>(m_versionPtr, newVersion);
		}

		persistent_linked_list<T> set(int index, T value)
		{
			if (index < 0 || index > m_root->size())
			{
				throw std::exception();
			}

			auto setFatNode = get_node_by_index(index);
			auto foundNode = setFatNode->find_node(m_root);

			auto v = ++(*m_versionPtr);
			auto newNode
				= std::make_shared<node<T>>(v, value, foundNode->get_prev(), foundNode->get_next());
			auto newVersion = std::make_shared<root_node<T>>(v, m_root->size(), m_root->front(), m_root->back(),
                                                             m_root);

			if (!setFatNode->is_full())
			{
				setFatNode->add_node(newNode);
				return persistent_linked_list<T>(m_versionPtr, newVersion);
			}

			auto newFatNode = std::make_shared<list_fat_node<T>>(newNode);

			if (newNode->get_prev() != nullptr)
			{
				newNode->set_prev(list_fat_node<T>::update_next(foundNode->get_prev(), newFatNode, newVersion));
			}
			else
			{
				newVersion->set_front(newFatNode);
			}

			if (newNode->get_next() != nullptr)
			{
				newNode->set_next(list_fat_node<T>::update_prev(foundNode->get_next(), newFatNode, newVersion));
			}
			else
			{
				newVersion->set_back(newFatNode);
			}

			return persistent_linked_list<T>(m_versionPtr, newVersion);
		}

		persistent_linked_list<T> push_back(T value)
		{
			if (m_root->size() == 0)
			{
				return init_root(value);
			}

			auto v = ++(*m_versionPtr);
			auto newNode = std::make_shared<node<T>>(v, value, std::shared_ptr<list_fat_node<T>>(nullptr),
                                                     std::shared_ptr<list_fat_node<T>>(nullptr));
			auto newFatNode = std::make_shared<list_fat_node<T>>(newNode);

			auto newVersion = std::make_shared<root_node<T>>(v, m_root->size() + 1, m_root->front(), newFatNode,
                                                             m_root);
			newNode->set_prev(list_fat_node<T>::update_next(m_root->back(), newFatNode, newVersion));

			return persistent_linked_list<T>(m_versionPtr, newVersion);
		}

		int size() const
		{
			return m_root->size();
		}

		bool empty() const
		{
			return m_root->size() == 0;
		}

		persistent_linked_list<T> push_front(T value)
		{
			if (m_root->size() == 0)
			{
				return init_root(value);
			}

			auto v = ++(*m_versionPtr);
			auto newNode = std::make_shared<node<T>>(v, value, std::shared_ptr<list_fat_node<T>>(nullptr),
                                                     std::shared_ptr<list_fat_node<T>>(nullptr));
			auto newFatNode = std::make_shared<list_fat_node<T>>(newNode);

			auto newVersion = std::make_shared<root_node<T>>(v, m_root->size() + 1, newFatNode, m_root->back(),
                                                             m_root);
			newNode->set_next(list_fat_node<T>::update_prev(m_root->front(), newFatNode, newVersion));

			return persistent_linked_list<T>(m_versionPtr, newVersion);
		}

		persistent_linked_list<T> undo()
		{
			if (m_root->get_parent() == nullptr)
			{
				return *this;
			}
			auto p = m_root->get_parent();
			auto new_root = std::make_shared<root_node<T>>(p->get_version(), p->size(), p->front(), p->back(),
                                                           p->get_parent());
			new_root->set_child(m_root);

			return persistent_linked_list<T>(m_versionPtr, new_root);
		}

		persistent_linked_list<T> redo()
		{
			if (m_root->get_child() == nullptr)
			{
				return *this;
			}

			auto p = m_root->get_child();
			auto new_root = std::make_shared<root_node<T>>(p->get_version(), p->size(), p->front(), p->back(),
                                                           m_root);
			new_root->set_child(p->get_child());

			return persistent_linked_list<T>(m_versionPtr, new_root);
		}
	};
}
