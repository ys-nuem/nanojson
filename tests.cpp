﻿#include <iostream>
#include <cassert>
#include <nanojson.hpp>

void test_primitive()
{
    nanojson::value v;

    // null value
    v = nanojson::make_value();
    assert(v.is<nanojson::null>());
    assert(!(nanojson::get<double>(v)));

    // boolean
    {
        v = nanojson::make_value(false);
        assert(v.is<bool>());

        auto ret = nanojson::get<bool>(v);
        assert((bool)ret);
        assert(*ret == false);
    }

    // integer
    {
        v = nanojson::make_value(5);
        assert(v.is<double>());

        auto ret = nanojson::get<int>(v);
        assert((bool)ret);
        assert(*ret == 5);
    }

    // floating point
    {
        v = nanojson::make_value(3.14); // double
        assert(v.is<double>());

        auto ret = nanojson::get<double>(v);
        assert((bool)ret);
        assert(*ret == 3.14);
    }

    {
        v = nanojson::make_value(3.14f); // float
        assert(v.is<double>());

        auto ret = nanojson::get<float>(v);
        assert((bool)ret);
        assert(*ret == 3.14f);
    }

    // string
    {
        v = nanojson::make_value("hogehoge");
        assert(v.is<std::string>());

        auto ret = nanojson::get<std::string>(v);
        assert((bool)ret);
        assert(*ret == "hogehoge");
    }

    {
        v = nanojson::make_value((char const*)"hogehoge");
        assert(v.is<std::string>());

        auto ret = nanojson::get<std::string>(v);
        assert((bool)ret);
        assert(*ret == "hogehoge");
    }

    {
        v = nanojson::make_value(std::string("fuga"));
        assert(v.is<std::string>());

        auto ret = nanojson::get<std::string>(v);
        assert((bool)ret);
        assert(*ret == "fuga");
    }
}

void test_array()
{
    nanojson::value v = nanojson::make_value({ 1, 2, 3 });
    assert(v.is<nanojson::array>());

    //std::cout << v.serialize(true) << std::endl;

    auto ret = nanojson::get<std::vector<int>>(v);
    assert((bool)ret);
    assert(ret->size() == 3);
    assert(ret->at(0) == 1);
    assert(ret->at(1) == 2);
    assert(ret->at(2) == 3);
}

void test_map()
{
    nanojson::value v = nanojson::make_value(std::map<std::string, int>{
        { "aa", 1 },
        { "bb", 3 },
        { "cc", 2 },
    });
    assert(v.is<nanojson::object>());

    auto ret = nanojson::get<std::map<std::string, int>>(v);
    assert((bool)ret);
    assert(ret->size() == 3);
    assert(ret->at("aa") == 1);
    assert(ret->at("bb") == 3);
    assert(ret->at("cc") == 2);
}

void test_get()
{
    nanojson::value v;
    std::unique_ptr<double> x = nanojson::get<double>(v);
    assert(!x);

    v = nanojson::value(1.0);
    std::unique_ptr<double> a = nanojson::get<double>(v);
    assert(a);
    assert(*a == 1.0);
}

void test_assign()
{
    nanojson::array arr = nanojson::make_value({ 1, 2, 3 }).get<nanojson::array>();

    int a, b, c;
    nanojson::assign(arr, a, b, c);
    assert(a == 1);
    assert(b == 2);
    assert(c == 3);
}

void test_user_defined()
{
    struct TestClass {
        int a, b;
        std::string c;

        NANOJSON_ADAPT(a, b, c);
    };
    static_assert(nanojson::detail::is_user_defined<TestClass>::value, "");

    nanojson::value v = nanojson::make_value(TestClass{ 1, 4, "hogehoge" });
    assert(v.is<nanojson::object>());

    auto ret = nanojson::get<TestClass>(v);
    assert((bool)ret);
    assert(ret->a == 1);
    assert(ret->b == 4);
    assert(ret->c == "hogehoge");
}

struct TestClass {
    std::string s;

public:
    friend std::ostream& operator<<(std::ostream& os, TestClass const& s) {
        return os << s.s;
    }

    friend std::istream& operator>>(std::istream& is, TestClass& s) {
        std::string str;
        is >> str;
        s.s = std::move(str);
        return is;
    }
};

void test_general_type()
{
    nanojson::value v = nanojson::make_value(TestClass{"hogehoge"});
    assert(v.is<std::string>());

    auto ret = nanojson::get<TestClass>(v);
    assert((bool)ret);
    assert(ret->s == "hogehoge");
}

void test_parse()
{
    std::string json = "[\"a\", \"b\", \"c\"]";

    std::string err;
    auto ret = nanojson::parse<std::vector<std::string>>(json, err);
    assert((bool)ret);
    assert(err.empty());

    assert(ret->size() == 3);
    assert(ret->at(0) == "a");
    assert(ret->at(1) == "b");
    assert(ret->at(2) == "c");
}

int main()
{
    test_primitive();
    test_array();
    test_map();
    test_get();
    test_assign();
    test_user_defined();
    test_general_type();
    test_parse();

    std::string serialized = nanojson::serialize({1, 2, 3});
    assert(serialized == "[1,2,3]");
}
