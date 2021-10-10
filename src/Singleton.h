//
// Created by PikachuHy on 2021/4/3.
//

// https://zhuanlan.zhihu.com/p/232319083

#ifndef MYNOTES_SINGLETON_H
#define MYNOTES_SINGLETON_H
template<typename T>
class Singleton
{
public:
    static T* instance()
    {
        static T t;
        return &t;
    }

    Singleton(T&&) = delete;
    Singleton(const T&) = delete;
    void operator= (const T&) = delete;

protected:
    Singleton() = default;
    virtual ~Singleton() = default;
};
#endif //MYNOTES_SINGLETON_H
