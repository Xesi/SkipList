// Copyright 2020 by Ulanov Georgiy under Free Public License 1.0.0

/**
\file skiplist.h
\brief skiplist.h - библиотека с реализацией структуры данных
"Очередь с пропусками".

SkipList - вероятностная структура данных, основанная на нескольких параллельных
отсортированных связных списках с эффективностью, сравнимой
с двоичным деревом (порядка O(log n) среднее время для большинства
операций)
![схема](image1.png)
*/
#pragma once
#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <random>
#include <cstddef>
#include <vector>

/**
\brief вероятностная структура данных,
которая содержит упорядоченный набор уникальных объектов.

\param [in] T - тип, хранимый в структуре
\param [in] Skip_Factor - основание дроби 1/Skip_Factor - вероятности
появления нового уровня у узла в списке

skiplist - вероятностная структура данных,
которая содержит упорядоченный набор уникальных объектов типа <b>T</b>.
Операции поиска, удаления и вставки имеют логарифмическую сложность.
В основе списка с пропусками лежит расширение отсортированного связного
списка дополнительными связями, добавленными в случайных путях
с биномиальным распределением, чтобы поиск по списку мог быстро пропускать
части этого списка.

![схема](image1.png)

Нижний слой — это обычный упорядоченный связный список. Каждый более
высокий слой представляет собой «выделенную полосу движения» для списков
ниже, где элемент в i-м слое появляется в i+1-м слое с 
вероятностью <b>1/Skip_Factor</b>.

Без указания значения <b>Skip_Factor</b> равен 2.
При операции поиска, вставки и удаления ожидаемое число шагов в каждом
связном списке от места начала в текущем списке до места перехода на более 
низкий уровень равно значению Skip_Factor. общие ожидаемые затраты
на поиск — 
\f$log_p n * p\f$, где p - шаблонный параметр Skip_Factor.
Асимптотика O(log(n)), так как p является константным значением.
*/
template <typename T, size_t Skip_Factor = 2>
class skiplist {
    //!< Максимальное количество связанных списков
    const size_t Max_Level = 32;
    //!< структрура, реализующая узел в списке.
    struct node;
  public:
    /**
    \brief позволяет однонапрявленно итерироваться
    по контейнеру skiplist.

    skiplist<T, Skip_Factor>::Iterator реализует концепцию ForwardIterator.
    */
    class Iterator {
      public:
        friend class skiplist<T, Skip_Factor>;
        //! \brief Умолчательный конструктор.
        //!
        //! Создает итератор, указывающий на конец списка.
        Iterator() = default;
        //! \brief Деструктор итератора;
        //!не удаляет объект, на который ссылается. 
        ~Iterator() = default;
        //! \brief Копирующий конструктор.
        Iterator(const Iterator&) = default;
        //! \brief Оператор присваивания.
        Iterator& operator=(const Iterator&) = default;
        //! \brief Оператор приведения к типу bool.
        //!
        //! Возвращает true, если значение итератора не равно nullptr.
        operator bool();
        //! \brief оператор префиксного инкримента.
        //! \retval Iterator& - значение итератора, идущего следующим.
        Iterator& operator++();
        //! \brief оператор постфиксного инкримента.
        //! \retval Iterator - значение итератора, идущего следующим.
        Iterator operator++(int);
        //! \brief оператор разыменования.
        //! \retval T& - объект из списка, на который указывал итератор.
        T& operator*();
        //! \brief оператор сравнения. Сравнивает значение на равенство.
        //! \retval bool - ссылаются ли два итератора на один объект.
        bool operator== (const Iterator&) const;
        //! \brief оператор сравнения. Сравнивает значение на неравенство.
        //! \retval bool - ссылаются ли два итератора на разные объекты.
        bool operator!= (const Iterator&) const;
      private:
        //!< конструктор, принимающий узел списка в качестве параметра.
        explicit Iterator(node* ref); 
        //!< указатель на узел в списке, который хранит объект типа T.
        node* current_node { nullptr };
    };

  public:
    //! \brief Умолчательный конструктор.
    //!
    //! Создает пустой список.
    skiplist();
    //! \brief Деструктор экземпляра, освобождает память.
    ~skiplist();
    //! \brief Копирующий конструктор.
    //!
    //! Создается новый список с пропусками с идентичным набором
    //! элементов. Внутреннее строение 
    //! полностью меняется в связи с рандомизированным принципом построения.
    skiplist(const skiplist&);
    //! \brief Оператор присваивания.
    //!
    //! Осуществляет глубокое копирование.
    skiplist& operator= (const skiplist&);
    
    //! \brief конструктор, принимающий в качестве параметров два итератора.
    //! \param [in] first итератор, указывающий на первый элемент
    //! упорядоченного набора элементов
    //! \param [in] last итератор, указывающий на элемент после последнего в
    //! упорядоченном наборе элементов
    //!
    //! Позволяет создавать список с пропусками от другого контейнера
    //! c упорядоченным набором элементов.
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
    //! \brief Возвращает итератор на первый элемент
    //!
    //! При пустом контейнере возвращает итератор за конец (см. end()).
    //! \param (Нет)
    //! \retval Iterator на первый элемент
    //!
    //! <b>Сложность</b><br> O(logN), где N - количество элементов в контейнере.
    Iterator begin() const;
    //! \brief Возвращает итератор на элемент, следующий за последним
    //! \param (Нет)
    //! \retval Iterator на элемент, следующий за последним
    //!
    //! <b>Сложность</b><br> константная.
    Iterator end() const;
    //! \brief возвращает итератор на первый элемент не меньше,
    //! чем заданное значение
    //! \param value - ключевое значение для сравнения элементов.
    //! \retval Iterator  на первый элемент, который не
    //! меньше, чем value. Если такой элемент не найден, возвращается
    //! итератор за конец списка (см. end()).
    //!
    //! <b>Сложность</b><br> O(logN), где N - количество элементов в контейнере.
    Iterator lower_bound(const T&) const;
    //! \brief возвращает итератор на первый элемент больше,
    //! чем определенное значение
    //! \param value - ключевое значение для сравнения элементов.
    //! \retval Iterator на первый элемент, который
    //! больше, чем value. Если такой элемент не найден, возвращается
    //! итератор за конец списка (см. end()).
    //!
    //! <b>Сложность</b><br> O(logN), где N - количество элементов в контейнере.
    Iterator upper_bound(const T&) const;

    //! \brief	Проверяет отсутствие элементов в контейнере
    //! \param (Нет)
    //! \retval bool <b>true</b>, если контейнер пуст, <b>false</b> иначе
    //!
    //! <b>Сложность</b><br> константная.
    bool empty() const;
    //! \brief Возвращает количество элементов в контейнере.
    //! \param (Нет)
    //! \retval std::ptrdiff_t - количество элементов в контейнере.
    //!
    //! <b>Сложность</b><br> константная.
    std::ptrdiff_t size() const;
    //! \brief Очищает контейнер
    //!
    //! Делает недействительными все ссылки, указатели или итераторы
    //! указывающие на удалённые элементы.
    //! <br><br><b>Сложность</b><br> линейная от размера контейнера.
    void clear();
    //! \brief находит элемент с заданным ключом
    //! \param value - ключевое значение элемента для поиска.
    //! \retval Iterator на элемент со значением value. Если такой элемент
    //! не найден, возвращается итератор, указывающий за конец списка
    //! (см. end()).
    //!
    //! <b>Сложность</b><br> O(logN), где N - количество элементов
    //! в контейнере.
    Iterator find(const T&) const;
    //! \brief Вставляет элемент
    //!
    //! Если элемент с таким значением уже существует, вставка не происходит.
    //! \param value - вставляемое значение
    //! \retval Iterator на вставленный элемент
    //!
    //! <b>Сложность</b><br> O(logN), где N - количество элементов в контейнере
    Iterator insert(const T&);
    //! \brief Удаляет элемент
    //!
    //! Если элемент с таким значением отсутствует в контейнере,
    //! удаление не происходит.
    //! \param value - удаляемое значение
    //! \retval Iterator на следующий элемент после удаляемого.
    //!
    //! <b>Сложность</b><br> O(logN), где N - количество элементов в контейнере
    Iterator erase(const T&);
    //! \brief Удаляет элемент
    //!
    //! Если элемент, на который указывает итератор, отсутствует в контейнере,
    //! удаление не происходит.
    //! \param it - итератор на элемент для удаления
    //! \retval Iterator на следующий элемент после удаляемого.
    //!
    //! <b>Сложность</b><br> O(logN), где N - количество элементов в контейнере
    Iterator erase(Iterator);
    //! \brief находит количество элементов по ключу
    //! \param value - значение ключа
    //! \retval количество элементов с ключом value
    //!
    //! <b>Сложность</b><br> O(logN), где N - количество элементов в контейнере
    std::ptrdiff_t count(const T&) const;

  private:
    std::random_device rd;
    //!< возвращает true с вероятностью 1/Skip_Factor
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
    size_t count_lvls { 1 };//!< количество параллельных списков.
    node* head_ { nullptr };//!< указатель на начало верхнего списка.
    std::ptrdiff_t size_ { 0 };//!< количество элементов в массиве.
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
