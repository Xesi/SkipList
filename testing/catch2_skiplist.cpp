#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <skiplist.h>

#include <vector>

TEST_CASE("skiplist ctor()", "[skiplist]") {
    skiplist<int> sk;
    CHECK(0u == sk.size());
    CHECK(sk.empty());
}
TEST_CASE("skiplist ctor(two iters)", "[skiplist]") {
    std::vector<int> v = { 1, 2, 3, 4 };
    skiplist<int> sk(v.begin(), v.end());
    CHECK(v.size() == sk.size());
    CHECK(!sk.empty());
    skiplist<int>::Iterator it = sk.begin();
    for (size_t i = 0; i < v.size(); ++i) {
        CHECK(v[i] == *it);
        ++it;
    }
    skiplist<int> sk2(v.begin(), v.begin());
    CHECK(sk2.size() == 0);
    CHECK(sk2.empty());

    skiplist<int> sk3(v.end(), v.end());
    CHECK(sk3.size() == 0);
    CHECK(sk3.empty());
}
TEST_CASE("skiplist ctor(other skiplist)", "[skiplist]") {
    std::vector<int> v = { 1, 2, 3, 4, 5 };
    skiplist<int> sk1(v.begin(), v.end());
    skiplist<int> sk2(sk1);
    CHECK(sk1.size() == sk2.size());
    CHECK(!sk2.empty());
    skiplist<int>::Iterator it1 = sk1.begin();
    skiplist<int>::Iterator it2 = sk2.begin();
    while (it1 != sk1.end()) {
        CHECK(*it1 == *it2);
        ++it1, ++it2;
    }
}
TEST_CASE("skiplist operator=", "[skiplist]") {
    std::vector<int> v1 = { 1, 2, 3 };
    std::vector<int> v2 = { 4, 5, 6, 7 };
    skiplist<int> sk1(v1.begin(), v1.end());
    skiplist<int> sk2(v2.begin(), v2.end());
    sk1 = sk2;
    CHECK(sk1.size() == v2.size());
    skiplist<int>::Iterator it1 = sk1.begin();
    skiplist<int>::Iterator it2 = sk2.begin();
    while (it1 != sk1.end()) {
        CHECK(*it1 == *it2);
        ++it1, ++it2;
    }
    *sk1.begin() = 0;
    CHECK(*sk1.begin() != *sk2.begin());
    sk1.clear();
    CHECK(sk1.empty());
    CHECK(!sk2.empty());

    sk1 = sk2;
    
    skiplist<int> sk3;
    sk2 = sk3;
    CHECK(sk2.size() == 0);
    CHECK(sk2.empty());
    
    sk2 = sk1;
    CHECK(sk2.size() == 4);
    CHECK(!sk2.empty());
}
TEST_CASE("skiplist begin() and end()", "[skiplist]") {
    skiplist<int> sk;
    CHECK(sk.begin() == sk.end());
    skiplist<int>::Iterator lastBegin = sk.begin();
    skiplist<int>::Iterator lastEnd = sk.end();
    std::vector<int> v = { 1, 2, 3 };
    sk = skiplist<int>(v.begin(), v.end());
    CHECK(sk.begin() != sk.end());
    CHECK(sk.begin() != lastBegin);
    CHECK(sk.end() == lastEnd);
    CHECK(*sk.begin() == 1);
}
TEST_CASE("skiplist lower_bound(T)", "[skiplist]") {
    std::vector<int> v = { 2, 4, 6 };
    skiplist<int> sk(v.begin(), v.end());
    CHECK(*sk.lower_bound(1) == 2);
    CHECK(*sk.lower_bound(2) == 2);
    CHECK(*sk.lower_bound(3) == 4);
    CHECK(*sk.lower_bound(4) == 4);
    CHECK(*sk.lower_bound(5) == 6);
    CHECK(*sk.lower_bound(6) == 6);
    CHECK(sk.lower_bound(7) == sk.end());
}
TEST_CASE("skiplist upper_bound(T)", "[skiplist]") {
    std::vector<int> v = { 2, 4, 6 };
    skiplist<int> sk(v.begin(), v.end());
    CHECK(*sk.upper_bound(1) == 2);
    CHECK(*sk.upper_bound(2) == 4);
    CHECK(*sk.upper_bound(3) == 4);
    CHECK(*sk.upper_bound(4) == 6);
    CHECK(*sk.upper_bound(5) == 6);
    CHECK(sk.upper_bound(6) == sk.end());
    CHECK(sk.lower_bound(7) == sk.end());
}
TEST_CASE("skiplist empty()", "[skiplist]") {
    skiplist<int> sk;
    CHECK(sk.empty());
    std::vector<int> v = { 1, 2, 3 };
    sk = skiplist<int>(v.begin(), v.end());
    CHECK(!sk.empty());
    sk.clear();
    CHECK(sk.empty());
    sk = skiplist<int>();
    CHECK(sk.empty());
}
TEST_CASE("skiplist size()", "[skiplist]") {
    skiplist<int> sk;
    CHECK(sk.size() == 0);
    std::vector<int> v = { 0, 10, 100 };
    sk = skiplist<int>(v.begin(), v.end());
    CHECK(sk.size() == 3);
    sk.clear();
    CHECK(sk.size() == 0);
}
TEST_CASE("skiplist clear()", "[skiplist]") {
    std::vector<double> v = { 1.1, 2.2, 3.3};
    skiplist<double> sk(v.begin(), v.end());
    CHECK(sk.begin() != sk.end());
    CHECK(sk.size() == 3);
    CHECK(!sk.empty());
    sk.clear();
    CHECK(sk.empty());
    CHECK(sk.size() == 0);
    CHECK(sk.begin() == sk.end());
}
TEST_CASE("skiplist find(T)", "[skiplist]") {
    std::vector<int> v = { 2, 4, 6, 8, 10 };
    skiplist<int> sk(v.begin(), v.end());
    for (int i = 2; i <= 10; i += 2) {
        skiplist<int>::Iterator it = sk.find(i);
        CHECK(it != sk.end());
        CHECK(*it == i);
    }
    for (int i = 1; i <= 11; i += 2) {
        skiplist<int>::Iterator it = sk.find(i);
        CHECK(it == sk.end());
    }
}
TEST_CASE("skiplist insert(T)", "[skiplist]") {
    skiplist<int> sk;
    for (int i = 1; i < 10; ++i) {
        sk.insert(i);
        CHECK(sk.size() == i);
        skiplist<int>::Iterator it = sk.find(i);
        CHECK(it != sk.end());
        CHECK(*it == i);
    }
    std::ptrdiff_t last_size = sk.size();
    for (int i = 0; i <= 10; ++i) {
        sk.insert(i);
    }
    CHECK(sk.size() == last_size + 2);
    for (int i = 20; i > 10; --i) {
        sk.insert(i);
    }
    skiplist<int>::Iterator it = sk.begin();
    for (int i = 0; i <= 20; ++i) {
        CHECK(*it == i);
        ++it;
    }
    CHECK(sk.size() == 21);
}
TEST_CASE("skiplist erase(T)", "[skiplist]") {
    std::vector<int> v = { 0, 1, 2, 3, 4, 5 };
    skiplist<int> sk(v.begin(), v.end());
    CHECK(sk.size() == 6);
    sk.erase(0);
    CHECK(sk.size() == 5);
    CHECK(sk.find(0) == sk.end());
    sk.erase(0);
    CHECK(sk.size() == 5);
    skiplist<int> sk2(sk);
    for (int i = 1; i < 6; ++i) {
        sk2.erase(i);
    }
    CHECK(sk2.size() == 0);
    for (int i = 5; i > 0; --i) {
        sk.erase(i);
    }
    CHECK(sk.empty());
}
TEST_CASE("skiplist erase(iter)", "[skiplist]") {
    std::vector<int> v = { 1, 2, 3, 4, 5 };
    skiplist<int> sk(v.begin(), v.end());
    skiplist<int> sk2(sk);
    while (sk2.begin() != sk2.end()) {
        sk2.erase(sk2.begin());
    }
    CHECK(sk2.empty());
    std::vector<skiplist<int>::Iterator> vec_it;
    skiplist<int>::Iterator it = sk.begin();
    while (it != sk.end()) {
        vec_it.push_back(it);
        ++it;
    }
    for (int i = vec_it.size() - 1; i >= 0; --i) {
        sk.erase(vec_it[i]);
    }
    CHECK(sk.empty());
}
TEST_CASE("skiplist count(T)", "[skiplist]") {
    std::vector<int> v = { 2, 4, 6, 8, 10 };
    skiplist<int> sk(v.begin(), v.end());
    for (int i = 1; i < 12; i += 2) {
        CHECK(sk.count(i) == 0);
    }
    for (int i = 2; i < 12; i += 2) {
        CHECK(sk.count(i) == 1);
    }
}
