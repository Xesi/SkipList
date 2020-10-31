// Copyright 2020 by Ulanov Georgiy under Free Public License 1.0.0

/**
\file skiplist.h
\brief skiplist.h - ���������� � ����������� ��������� ������
"������� � ����������".

SkipList - ������������� ��������� ������, ���������� �� ���������� ������������
��������������� ������� ������� � ��������������, ���������
� �������� ������� (������� O(log n) ������� ����� ��� �����������
��������)
![�����](image1.png)
*/
#pragma once
#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <random>
#include <cstddef>
#include <vector>

/**
\brief ������������� ��������� ������,
������� �������� ������������� ����� ���������� ��������.

\param [in] T - ���, �������� � ���������
\param [in] Skip_Factor - ��������� ����� 1/Skip_Factor - �����������
��������� ������ ������ � ���� � ������

skiplist - ������������� ��������� ������,
������� �������� ������������� ����� ���������� �������� ���� <b>T</b>.
�������� ������, �������� � ������� ����� ��������������� ���������.
� ������ ������ � ���������� ����� ���������� ���������������� ��������
������ ��������������� �������, ������������ � ��������� �����
� ������������ ��������������, ����� ����� �� ������ ��� ������ ����������
����� ����� ������.

![�����](image1.png)

������ ���� � ��� ������� ������������� ������� ������. ������ �����
������� ���� ������������ ����� ����������� ������ ��������� ��� �������
����, ��� ������� � i-� ���� ���������� � i+1-� ���� � 
������������ <b>1/Skip_Factor</b>.

��� �������� �������� <b>Skip_Factor</b> ����� 2.
��� �������� ������, ������� � �������� ��������� ����� ����� � ������
������� ������ �� ����� ������ � ������� ������ �� ����� �������� �� ����� 
������ ������� ����� �������� Skip_Factor. ����� ��������� �������
�� ����� � 
\f$log_p n * p\f$, ��� p - ��������� �������� Skip_Factor.
����������� O(log(n)), ��� ��� p �������� ����������� ���������.
*/
template <typename T, size_t Skip_Factor = 2>
class skiplist {
    //!< ������������ ���������� ��������� �������
    const size_t Max_Level = 32;
    //!< ����������, ����������� ���� � ������.
    struct node;
  public:
    /**
    \brief ��������� ��������������� �������������
    �� ���������� skiplist.

    skiplist<T, Skip_Factor>::Iterator ��������� ��������� ForwardIterator.
    */
    class Iterator {
      public:
        friend class skiplist<T, Skip_Factor>;
        //! \brief ������������� �����������.
        //!
        //! ������� ��������, ����������� �� ����� ������.
        Iterator() = default;
        //! \brief ���������� ���������;
        //!�� ������� ������, �� ������� ���������. 
        ~Iterator() = default;
        //! \brief ���������� �����������.
        Iterator(const Iterator&) = default;
        //! \brief �������� ������������.
        Iterator& operator=(const Iterator&) = default;
        //! \brief �������� ���������� � ���� bool.
        //!
        //! ���������� true, ���� �������� ��������� �� ����� nullptr.
        operator bool();
        //! \brief �������� ����������� ����������.
        //! \retval Iterator& - �������� ���������, ������� ���������.
        Iterator& operator++();
        //! \brief �������� ������������ ����������.
        //! \retval Iterator - �������� ���������, ������� ���������.
        Iterator operator++(int);
        //! \brief �������� �������������.
        //! \retval T& - ������ �� ������, �� ������� �������� ��������.
        T& operator*();
        //! \brief �������� ���������. ���������� �������� �� ���������.
        //! \retval bool - ��������� �� ��� ��������� �� ���� ������.
        bool operator== (const Iterator&) const;
        //! \brief �������� ���������. ���������� �������� �� �����������.
        //! \retval bool - ��������� �� ��� ��������� �� ������ �������.
        bool operator!= (const Iterator&) const;
      private:
        //!< �����������, ����������� ���� ������ � �������� ���������.
        explicit Iterator(node* ref); 
        //!< ��������� �� ���� � ������, ������� ������ ������ ���� T.
        node* current_node { nullptr };
    };

  public:
    //! \brief ������������� �����������.
    //!
    //! ������� ������ ������.
    skiplist();
    //! \brief ���������� ����������, ����������� ������.
    ~skiplist();
    //! \brief ���������� �����������.
    //!
    //! ��������� ����� ������ � ���������� � ���������� �������
    //! ���������. ���������� �������� 
    //! ��������� �������� � ����� � ����������������� ��������� ����������.
    skiplist(const skiplist&);
    //! \brief �������� ������������.
    //!
    //! ������������ �������� �����������.
    skiplist& operator= (const skiplist&);
    
    //! \brief �����������, ����������� � �������� ���������� ��� ���������.
    //! \param [in] first ��������, ����������� �� ������ �������
    //! �������������� ������ ���������
    //! \param [in] last ��������, ����������� �� ������� ����� ���������� �
    //! ������������� ������ ���������
    //!
    //! ��������� ��������� ������ � ���������� �� ������� ����������
    //! c ������������� ������� ���������.
    template<typename InputIt>
    skiplist(InputIt first, InputIt last) {
        head_ = new node();
        std::vector<node*> refs = { head_ };
        for (InputIt it = first; it != last; ++it) {
            T* value = new T(*it);
            node* insertingNode = new node(value);
            refs[0]->next = insertingNode;
            refs[0] = insertingNode;
            size_t cur_lvl = 0;
            while (++cur_lvl < Max_Level && coin_flip()) {
                node* newNode = new node(value, nullptr, insertingNode);
                if (cur_lvl >= count_lvls) {
                    head_ = new node(nullptr, newNode, head_);
                    refs.push_back(head_);
                    count_lvls++;
                }
                refs[cur_lvl]->next = newNode;
                refs[cur_lvl] = newNode;
                insertingNode = newNode;
            }
            ++size_;
        }
    }
    //! \brief ���������� �������� �� ������ �������
    //!
    //! ��� ������ ���������� ���������� �������� �� ����� (��. end()).
    //! \param (���)
    //! \retval Iterator �� ������ �������
    //!
    //! <b>���������</b><br> O(logN), ��� N - ���������� ��������� � ����������.
    Iterator begin() const;
    //! \brief ���������� �������� �� �������, ��������� �� ���������
    //! \param (���)
    //! \retval Iterator �� �������, ��������� �� ���������
    //!
    //! <b>���������</b><br> �����������.
    Iterator end() const;
    //! \brief ���������� �������� �� ������ ������� �� ������,
    //! ��� �������� ��������
    //! \param value - �������� �������� ��� ��������� ���������.
    //! \retval Iterator  �� ������ �������, ������� ��
    //! ������, ��� value. ���� ����� ������� �� ������, ������������
    //! �������� �� ����� ������ (��. end()).
    //!
    //! <b>���������</b><br> O(logN), ��� N - ���������� ��������� � ����������.
    Iterator lower_bound(const T&) const;
    //! \brief ���������� �������� �� ������ ������� ������,
    //! ��� ������������ ��������
    //! \param value - �������� �������� ��� ��������� ���������.
    //! \retval Iterator �� ������ �������, �������
    //! ������, ��� value. ���� ����� ������� �� ������, ������������
    //! �������� �� ����� ������ (��. end()).
    //!
    //! <b>���������</b><br> O(logN), ��� N - ���������� ��������� � ����������.
    Iterator upper_bound(const T&) const;

    //! \brief	��������� ���������� ��������� � ����������
    //! \param (���)
    //! \retval bool <b>true</b>, ���� ��������� ����, <b>false</b> �����
    //!
    //! <b>���������</b><br> �����������.
    bool empty() const;
    //! \brief ���������� ���������� ��������� � ����������.
    //! \param (���)
    //! \retval std::ptrdiff_t - ���������� ��������� � ����������.
    //!
    //! <b>���������</b><br> �����������.
    std::ptrdiff_t size() const;
    //! \brief ������� ���������
    //!
    //! ������ ����������������� ��� ������, ��������� ��� ���������
    //! ����������� �� �������� ��������.
    //! <br><br><b>���������</b><br> �������� �� ������� ����������.
    void clear();
    //! \brief ������� ������� � �������� ������
    //! \param value - �������� �������� �������� ��� ������.
    //! \retval Iterator �� ������� �� ��������� value. ���� ����� �������
    //! �� ������, ������������ ��������, ����������� �� ����� ������
    //! (��. end()).
    //!
    //! <b>���������</b><br> O(logN), ��� N - ���������� ���������
    //! � ����������.
    Iterator find(const T&) const;
    //! \brief ��������� �������
    //!
    //! ���� ������� � ����� ��������� ��� ����������, ������� �� ����������.
    //! \param value - ����������� ��������
    //! \retval Iterator �� ����������� �������
    //!
    //! <b>���������</b><br> O(logN), ��� N - ���������� ��������� � ����������
    Iterator insert(const T&);
    //! \brief ������� �������
    //!
    //! ���� ������� � ����� ��������� ����������� � ����������,
    //! �������� �� ����������.
    //! \param value - ��������� ��������
    //! \retval Iterator �� ��������� ������� ����� ����������.
    //!
    //! <b>���������</b><br> O(logN), ��� N - ���������� ��������� � ����������
    Iterator erase(const T&);
    //! \brief ������� �������
    //!
    //! ���� �������, �� ������� ��������� ��������, ����������� � ����������,
    //! �������� �� ����������.
    //! \param it - �������� �� ������� ��� ��������
    //! \retval Iterator �� ��������� ������� ����� ����������.
    //!
    //! <b>���������</b><br> O(logN), ��� N - ���������� ��������� � ����������
    Iterator erase(Iterator);
    //! \brief ������� ���������� ��������� �� �����
    //! \param value - �������� �����
    //! \retval ���������� ��������� � ������ value
    //!
    //! <b>���������</b><br> O(logN), ��� N - ���������� ��������� � ����������
    std::ptrdiff_t count(const T&) const;

  private:
    std::random_device rd;
    //!< ���������� true � ������������ 1/Skip_Factor
    inline bool coin_flip() { return (rd() % Skip_Factor == 0); }

    struct node {
        node() = default;
        explicit node(T* ref_value, node* next_node = nullptr,
             node* down_node = nullptr)
            : value(ref_value)
            , next(next_node)
            , down(down_node) {
        }

        T* value {nullptr};
        node* next {nullptr};
        node* down {nullptr};
    };
    size_t count_lvls { 1 };//!< ���������� ������������ �������.
    node* head_ { nullptr };//!< ��������� �� ������ �������� ������.
    std::ptrdiff_t size_ { 0 };//!< ���������� ��������� � �������.
};

template <typename T, size_t p>
skiplist<T, p>::Iterator::Iterator(node* ref)
    : current_node(ref) {
}

template<typename T, size_t p>
skiplist<T, p>::Iterator::operator bool() {
    return current_node != nullptr;
}


template<typename T, size_t p>
typename skiplist<T, p>::Iterator& skiplist<T, p>::Iterator::operator++ () {
    current_node = current_node->next;
    return *this;
}

template<typename T, size_t p>
typename skiplist<T, p>::Iterator skiplist<T, p>::Iterator::operator++(int) {
    Iterator oldValue = *this;
    current_node = current_node->next;
    return oldValue;
}

template<typename T, size_t p>
T& skiplist<T, p>::Iterator::operator*() {
    return *(current_node->value);
}

template<typename T, size_t p>
bool skiplist<T, p>::Iterator::operator==
    (const skiplist<T, p>::Iterator& other) const {
    return current_node == other.current_node;
}

template<typename T, size_t p>
bool skiplist<T, p>::Iterator::operator!=
    (const skiplist<T, p>::Iterator& other) const {
    return !(*this == other);
}

template<typename T, size_t p>
skiplist<T, p>::skiplist() {
    head_ = new node();
}

template<typename T, size_t p>
skiplist<T, p>::~skiplist() {
    clear();
    node* curNode = head_;
    while (curNode != nullptr) {
        node* nextNode = curNode->down;
        delete curNode;
        curNode = nextNode;
    }
}

template<typename T, size_t p>
skiplist<T, p>::skiplist(const skiplist<T, p>& other) {
    head_ = new node();
    std::vector<node*> refs = { head_ };
    for (Iterator it = other.begin(); it != other.end(); ++it) {
        T* value = new T(*it);
        node* insertingNode = new node(value);
        refs[0]->next = insertingNode;
        refs[0] = insertingNode;
        size_t cur_lvl = 0;
        while (++cur_lvl < Max_Level && coin_flip()) {
            node* newNode = new node(value, nullptr, insertingNode);
            if (cur_lvl >= count_lvls) {
                head_ = new node(nullptr, newNode, head_);
                refs.push_back(head_);
                count_lvls++;
            }
            refs[cur_lvl]->next = newNode;
            refs[cur_lvl] = newNode;
            insertingNode = newNode;
        }
        ++size_;
    }
}

template<typename T, size_t p>
skiplist<T, p>& skiplist<T, p>::operator= (const skiplist<T, p>& other) {
    if (this != &other) {
        clear();
        node* curNode = head_;
        while (curNode != nullptr) {
            node* nextNode = curNode->down;
            delete curNode;
            curNode = nextNode;
        }
        size_ = 0;
        count_lvls = 1;
        head_ = new node();
        std::vector<node*> refs = { head_ };
        for (Iterator it = other.begin(); it != other.end(); ++it) {
            T* value = new T(*it);
            node* insertingNode = new node(value);
            refs[0]->next = insertingNode;
            refs[0] = insertingNode;
            size_t cur_lvl = 0;
            while (++cur_lvl < Max_Level && coin_flip()) {
                node* newNode = new node(value, nullptr, insertingNode);
                if (cur_lvl >= count_lvls) {
                    head_ = new node(nullptr, newNode, head_);
                    refs.push_back(head_);
                    count_lvls++;
                }
                refs[cur_lvl]->next = newNode;
                refs[cur_lvl] = newNode;
                insertingNode = newNode;
            }
            ++size_;
        }
    }
    return *this;
}

template<typename T, size_t p>
typename skiplist<T, p>::Iterator skiplist<T, p>::begin() const {
    node* curNode = head_;
    while (curNode->down) curNode = curNode->down;
    return skiplist<T, p>::Iterator(curNode->next);
}

template<typename T, size_t p>
typename skiplist<T, p>::Iterator skiplist<T, p>::end() const {
    return Iterator(nullptr);
}

template<typename T, size_t p>
typename skiplist<T, p>::Iterator
           skiplist<T, p>::lower_bound(const T& value) const {
    node* curNode = head_;
    while (curNode->down != nullptr) {
        if (curNode->next == nullptr || !(*(curNode->next->value) < value)) {
            curNode = curNode->down;
        } else {
            curNode = curNode->next;
        }
    }
    while (curNode->next != nullptr && *(curNode->next->value) < value) {
        curNode = curNode->next;
    }
    return Iterator(curNode->next);
}

template<typename T, size_t p>
typename skiplist<T, p>::Iterator
        skiplist<T, p>::upper_bound(const T& value) const {
    node* curNode = head_;
    while (curNode->down != nullptr) {
        if (curNode->next == nullptr || value < *(curNode->next->value)) {
            curNode = curNode->down;
        } else {
            curNode = curNode->next;
        }
    }
    while (curNode->next != nullptr && !(value < *(curNode->next->value))) {
        curNode = curNode->next;
    }
    return Iterator(curNode->next);
}

template<typename T, size_t p>
bool skiplist<T, p>::empty() const {
    return size_ == 0;
}

template<typename T, size_t p>
std::ptrdiff_t skiplist<T, p>::size() const {
    return size_;
}

template<typename T, size_t p>
void skiplist<T, p>::clear() {
    node* curNode = head_;
    node* onDelete;
    node* nextNode;
    while (curNode->down != nullptr) {
        onDelete = curNode->next;
        while (onDelete != nullptr) {
            nextNode = onDelete->next;
            delete onDelete;
            onDelete = nextNode;
        }
        curNode->next = nullptr;
        curNode = curNode->down;
    }
    onDelete = curNode->next;
    curNode->next = nullptr;
    while (onDelete != nullptr) {
        nextNode = onDelete->next;
        delete onDelete->value;
        delete onDelete;
        onDelete = nextNode;
    }
    size_ = 0;
}

template<typename T, size_t p>
typename skiplist<T, p>::Iterator skiplist<T, p>::find(const T& value) const {
    node* curNode = head_;
    while (curNode->down != nullptr) {
        if (curNode->next == nullptr || value < *(curNode->next->value)) {
            curNode = curNode->down;
        } else {
            curNode = curNode->next;
        }
    }
    while (curNode->next != nullptr && !(value < *(curNode->next->value))) {
        curNode = curNode->next;
    }
    if (curNode->value == nullptr || *(curNode->value) != value) {
        return end();
    }
    return Iterator(curNode);
}

template<typename T, size_t p>
typename skiplist<T, p>::Iterator
        skiplist<T, p>::insert(const T& insertingValue) {
    std::vector<node*> refs(count_lvls);

    node* curNode = head_;
    size_t cur_lvl = count_lvls - 1;

    while (cur_lvl > 0) {
        if (curNode->next == nullptr ||
                insertingValue < *(curNode->next->value)) {
            refs[cur_lvl] = curNode;
            --cur_lvl;
            curNode = curNode->down;
        } else {
            curNode = curNode->next;
        }
    }
    while (curNode->next != nullptr &&
            !(insertingValue < *(curNode->next->value))) {
        curNode = curNode->next;
    }
    refs[cur_lvl] = curNode;

    if (curNode->value != nullptr && *(curNode->value) == insertingValue) {
        return Iterator(curNode);
    }
    T* data = new T(insertingValue);
    node* insertingNode = new node(data, curNode->next, nullptr);
    curNode->next = insertingNode;
    ++size_;

    node* lastInsertingNode = insertingNode;
    while (coin_flip() && ++cur_lvl < Max_Level) {
        node* nextLevelNode = new node(data);
        nextLevelNode->down = lastInsertingNode;
        lastInsertingNode = nextLevelNode;

        if (cur_lvl < count_lvls) {
            nextLevelNode->next = refs[cur_lvl]->next;
            refs[cur_lvl]->next = nextLevelNode;
        } else {
            head_ = new node(nullptr, nextLevelNode, head_);
            refs.push_back(head_);
            ++count_lvls;
        }
    }

    return Iterator(insertingNode);
}
template<typename T, size_t p>
typename skiplist<T, p>::Iterator skiplist<T, p>::erase(const T& value) {
    std::vector<node*> refs(count_lvls);
    node* curNode = head_;
    size_t cur_lvl = count_lvls - 1;
    while (curNode->down != nullptr) {
        if (curNode->next == nullptr || !(*(curNode->next->value) < value)) {
            refs[cur_lvl] = curNode;
            --cur_lvl;
            curNode = curNode->down;
        } else {
            curNode = curNode->next;
        }
    }
    while (curNode->next != nullptr && *(curNode->next->value) < value) {
        curNode = curNode->next;
    }
    if (curNode->next == nullptr || *(curNode->next->value) != value) {
        return Iterator(curNode->next);
    }
    refs[cur_lvl] = curNode;
    T* ref_on_value = curNode->next->value;
    for (node* ref : refs) {
        if (ref->next == nullptr || ref->next->value != ref_on_value) {
            break;
        }
        node* onDelete = ref->next;;
        ref->next = ref->next->next;
        delete onDelete;
    }
    delete ref_on_value;
    --size_;
    while (head_->next == nullptr && count_lvls > 1) {
        node* next_head = head_->down;
        delete head_;
        head_ = next_head;
        --count_lvls;
    }
    return Iterator((*refs.begin())->next);
}

template<typename T, size_t p>
typename skiplist<T, p>::Iterator
        skiplist<T, p>::erase(skiplist<T, p>::Iterator it) {
    return erase(*it);
}

template<typename T, size_t p>
std::ptrdiff_t skiplist<T, p>::count(const T& value) const {
    return (find(value) == end() ? 0 : 1);
}
#endif
