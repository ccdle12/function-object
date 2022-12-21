#include <memory>
#include <iostream>
#include <cassert>

namespace example {

// function class definition.
template <typename>
class function;

// R is the return value of a callabe type and Args is the variadic parameters.
template <typename R, typename... Args>
class function<R(Args...)>
{
    public:
        function() = default;

        template <typename T>
        function(T t) : m_callable{std::make_unique<Callable<T>>(t)} {};

        // Move assignment overload, copy or move the function and wrap it as a Callable
        // class.
        // T is the callable function as a generic, allowing different types to
        // be passed.
        template <typename T>
        function& operator=(T&& t) noexcept
        {
            printf("function&& operator=\n");
            m_callable = std::make_unique<Callable<T>>(t);
            return *this;
        }

        // Function call assignment overload, dereference the unique ptr containing
        // the Callable and execute the function with provided args.
        R operator()(Args... args) const
        {
            printf("function& operator=\n");
            assert(m_callable);
            return m_callable->Invoke(args...);
        }

        // For Debugging purposes.
        ~function() noexcept
        {
            printf("~function\n");
        }

    private:
        // Interface for all callables that can have delayed execution.
        class ICallable
        {
            public:
                virtual R Invoke(Args...) = 0;
        };

        // Concrete type of the Callable that can have a different T when wrapped
        // in the copy assignment overload.
        template <typename T>
        class Callable : public ICallable
        {
            private:
                // The stored function T.
                T m_t;

            public:
                Callable(const T& t) : m_t(t) {};

                // Executes the stored function T.
                R Invoke(Args... args) override
                {
                    return m_t(args...);
                }

                // For Debugging purposes.
                ~Callable() noexcept
                {
                    printf("~Callable\n");
                }
        };

        // Pointer to the callable, using dynamic dispatch allows the different
        // references to concrete implementations containing different T's.
        std::unique_ptr<ICallable> m_callable;

};
}

int add(int x, int y) 
{
    printf("DEBUG: add(%d, %d)\n", x, y);
    return x + y;
};

void incrementer(int& x) 
{
    ++x;
}

int main() 
{
    example::function<int(int, int)> f(add);
    f(1, 2);

    example::function<void()> f2;
    f2 = []() { std::cout << "DEBUG: Lambda" << std::endl; };
    f2();

    example::function<void()> f3;
    f3 = []() { std::cout << "DEBUG: Lambda 2" << std::endl; };
    f3();

    int x{5}, y{10};
    example::function<int()> f4;
    f4 = [x, y]() { return add(x, y); };
    f4();

    int z{1};
    example::function<void()> f5;
    f5 = [&z]() { incrementer(z); };
    f5();
    printf("DEBUG: result after incrementer: %d\n", z);

    example::function<void()> f6;
    f6 = std::move([&z]() { incrementer(z); });
    f6();
    printf("DEBUG: result after incrementer: %d\n", z);

    example::function<void()> f7;
    f7();
}
