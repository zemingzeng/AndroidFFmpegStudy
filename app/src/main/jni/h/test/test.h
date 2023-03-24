//
// Created by mingzz on 2023/3/3.
//

#ifndef FFMPEG_TEST_CPP
#define FFMPEG_TEST_CPP




//############################test#######################################
#include <type_traits>
#include <iostream>
#include <functional>
#include <vector>
#include "array"
#include "base.h"
#include <utility>
#include <compare>
//#include <format>

using namespace std;
namespace Space {}

constexpr int getNumber(int a) {
    return a;
}

template<class T, typename H>
int play() {
    int a;
    switch (int a = 10; a) {
        case 2:
            [[fallthrough]];
        case 0:
        default:
            break;
    }
    switch (int a = 10;a) {

        case 1:
        default:
            break;

    }
    int i{11};

    std::vector<int> intArrays{1, 2, 3};
    intArrays[0];
    typedef T G;
    struct K {
    };
    typedef K GG;

    using INT = int;
    INT number = 10;



    namespace KK = Space;

    struct number {
        int a, b, c;
    };
    struct number number1{1, 2, 3};

//    auto[x, y, z]{number1};

    array<int, 3> arrays{1, 2, 3};
    auto[x, y, z]{arrays};

    for (array<int, 3> arr{1, 2, 4}; int i : arr) {
        cout << i << endl;
    }

    string myString{"Hello, World"};
    myString.size();
    string str = "sdf";
    str.size();
    string ing[2]{"Hello, World"};

    vector<int> afs{1, 2, 3};

    int dd = 9;
    float sf = dd;

    int yy[getNumber(3)] = {0};
    int &ll = dd;
    int oo = 0;
    ll = oo;

    int *gh = new int(23);
    int &ghg = *gh;
    delete gh;
    ghg = 100;

    int &fg = ghg;
    const int &tt{3};

    return yy[0];

}

#include <string>
#include <initializer_list>

class Apple {

public:
    int &priceRef;
    int price{0};
    double count{0.0};

    ~Apple() {
        LOGI("~Apple  ~Apple ~Apple");
    }

    Apple() : priceRef(price) {
        LOGI("Apple() Apple() Apple()");
    }

    Apple(const Apple &apple) : priceRef(price) {
        LOGI("Apple(const Apple &apple)");
    }

    Apple(int price, double count) : priceRef(price) {
        this->price = price;
        this->count = count;
        LOGI("Apple(int price, double count)");
    }

    Apple(initializer_list<int> list1, initializer_list<double> list2) : priceRef(price) {
        LOGI("Apple(initializer_list<int>)");
    }
};

long ptr = 0;

//int getApple(int price) {
Apple getApple(int price) {
    Apple apple;
    apple.price = price;
    LOGI("address: %p", &apple);
    if (ptr == 0)
        ptr = (long) (&apple);
//    return ptr;
    return apple;
}

void addOne(const int &number) {
    LOGI("addOne ref");
//    number++;
}

void addOne(int number) {
    LOGI("addOne value");
//    number++;
}

#include <stdexcept>


#include <stdlib.h>

#include <cstring>

void getSignedIntBinary(const signed int &number, char binary[33]) {
    binary[0] = number > 0 ? '0' : '1';
    int size = 32;
    int number_ = abs(number);
    for (int i = 1; i < size; ++i) {
        int bit = 1 << (size - i - 1);
        bit &= number_;
        binary[i] = bit == 0 ? '0' : '1';
    }
    binary[size] = '\0';
}

typedef int (*(*pArray)[10])(int *);

typedef int (*pArray10[10])(int *);


int (*h)(int *);

int (*(*ff)(int, int))(int);

int (*fff(int, int))[10];


int (*kl)(int, int);

int (*(*kll))(int, int);

int func1(int, int) {
    LOGI("xxxxxxxxxxxxxddddddddddddddddddddddddddddddvvvvvvvvvvvvvv-----------vvvvvvvvvvvvvvvddd");
    return 0;
}

int func(int *) {
    LOGI("ddddddddddddddddddddddddddddddvvvvvvvvvvvvvvvvvvvvvvvvvvvvvddd");
    return 0;
}


void test_() {
    LOGI("test start------------------------------------------------------");

    pArray10 pArray10_;
    int (*g)(int *) =func;
    pArray10_[0] = g;


    int (*gg)(int, int) =func1;
    kl = &func1;
    kll = &kl;
//    kl(1, 1);
    (*kl)(1, 1);

    (*kll)(1, 1);

//    (*(void (*)()) 0)();

    int iff = 0;

    int *ffp = &iff;
    *(double *) ffp = 99;

//    *f = &g;
    //(g)(nullptr);
    int (*(*p)[10])(int *) =&pArray10_;
    (**p)(nullptr);

    int ( *pfArr[4] )(int, int);
    int ( *( *ppfArr )[4])(int, int) = &pfArr;

    int ab[10];
    *ab = 10;

    getApple(10);
    LOGI("%d ", ((Apple *) ptr)->price);
    getApple(100);
    LOGI("%d ", ((Apple *) ptr)->price);
    getApple(1000);
    LOGI("%d ", ((Apple *) ptr)->price);
    getApple(100000);
    LOGI("%d ", ((Apple *) ptr)->price);
    int number = 100;
//    addOne(100);
//    addOne(number);
//    LOGI("%d ", ((Apple *) ptr)->price);
//    Apple apple = getApple();
//    LOGI("address : %p", &apple);
//    Apple apple1 = apple;
//    LOGI("address : %p", &apple1);

    int *pi{new int(8)};
    int *&pRef = pi;

    Apple apple = Apple{};
//    Apple apple1{};

    auto pApple = &apple;
    pApple->price;

    const int jk = 10;
    const int *pInt = &jk;
    const int *pInt1 = const_cast<const int *>(pInt);

    Apple apple2{1, 5.6};

    auto a = {1, 4};
    auto b{1};
    decltype(b) y = 0; //decltype和auto不同他不会strip reference和const限定符所以不会产生copy

    signed int number1 = -4;
    LOGI("%d", number1);

    char binary[33];
    getSignedIntBinary(-888, binary);
    LOGI("getBinaryString--------------->%s", binary);
    LOGI("getBinaryString--------------->%x", (-1 << 30));
    LOGI("getBinaryString--------------->%d", (-1 << 30));


    const Apple &appleRef = apple;
//    Apple apple3{appleRef}; //copy construct
    Apple apple3(appleRef);

    const char char_ = 'r';
    const char *const_str = &(char_);
    char *str = const_cast< char *>(const_str);
    size_t strLen = strlen(str);
    str[0] = 'u';
    const int x = 10;
    const int *x_ = &x;
    int *t = const_cast<int *>(x_);
    *t = 100;

    LOGI("xxxxxxxx:%d,addr:%p", x, &x);
    LOGI("tttttttt:%d,,addr:%p", *t, t);
    LOGI("x_x_x_:%d,,addr:%p", *x_, x_);
    LOGI("str:%s", str);
    LOGI("char_:%c", char_);
    LOGI("const_str:%c", *const_str);
    LOGI("strLen:%ld", strLen);


    const char *str1 = const_cast<const char *>("hello");
    *(str + 1) = '\0';
    strcat(str, str1);

    LOGI("str after strCat :%s", str);

//
//    const char *rawStr{R"(hello，
//                          “xxxx”)"};
    const char *rawStr{R"q(Embedded )" characters)q"};
    LOGI("raw string : %s", rawStr);



    using namespace std;
    int i{11};
    strong_ordering result{i <=> 0};
    if (is_lt(result)) { LOGI("less"); }
    if (is_gt(result)) { LOGI("greater"); }
    if (is_eq(result)) { LOGI("equal"); }

    std::string aa{"12"};
    std::string bb{"34"};
//    myStr2.compare(myStr1);
//    strong_ordering result1 = {myStr1 <=> myStr2}
//    is_gt(result1);
//    auto result1{aa <=> bb};
//    if (is_lt(result1)) { cout << "less" << endl; }
//    if (is_gt(result1)) { cout << "greater" << endl; }
//    if (is_eq(result1)) { cout << "equal" << endl; }
    string::npos;
    LOGI("test end------------------------------------------------------");
}
//void test__();
#endif //FFMPEG_TEST_CP
