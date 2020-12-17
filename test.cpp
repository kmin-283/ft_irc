#include <iostream>
#include <map>
#include <string>
#include <vector>

// struct A
// {
//     int a;
//     std::string aa;
// };

// int main(void)
// {
//     std::map<int, struct A> m;
//     std::vector<std::string> sm;

//     struct A AAAA;
//     AAAA.a = 1;
//     AAAA.aa = "hello";
//     m.insert(std::pair<int, struct A>(1, AAAA));
    
//     sm.push_back("hello");
//     sm.push_back("hello:aa:aaa:Bb:bb:DD");

//     std::cout << m[1].a << " " << m[1].aa << std::endl;
// }

int main(void)
{
    std::vector<std::string> sv;

    sv.reserve(4);

    sv.push_back("aaa");
    sv.push_back("hello");
    // sv[0] = "aaaa";
    // sv[1] = "Hello";

    std::cout << sv[0] << std::endl;
    std::cout << sv[1] << std::endl;
    std::cout << sv.size() << std::endl;
    
}