
    #include <cstdint>
    #include <string>
    #include <string_view>
    #include <vector>
    #include <iostream>
    #include <variant>
    #include <optional>
    #include <cstddef>

    using int8_t   = int8_t;
    using int16_t  = int16_t;
    using int32_t  = int32_t;
    using int64_t  = int64_t;
    using uint8_t  = uint8_t;
    using uint16_t = uint16_t;
    using uint32_t = uint32_t;
    using uint64_t = uint64_t;
    using usize_t  = size_t;
    using isize_t  = ptrdiff_t;

    // Result<T, E>
    template<typename T, typename E>
    struct Result {
        std::variant<T, E> data;
        static Result Ok(T v)  { return Result{std::variant<T,E>(std::in_place_index<0>, v)}; }
        static Result Err(E e) { return Result{std::variant<T,E>(std::in_place_index<1>, e)}; }
        bool is_ok()  const { return data.index() == 0; }
        bool is_err() const { return data.index() == 1; }
        T&   unwrap()       { return std::get<0>(data); }
        E&   unwrap_err()   { return std::get<1>(data); }
    };

    // println / print / eprintln / eprint
    template<typename... Args>
    void println(Args&&... args) { (std::cout << ... << args); std::cout << "\n"; }
    template<typename... Args>
    void print(Args&&... args)   { (std::cout << ... << args); }
    template<typename... Args>
    void eprintln(Args&&... args){ (std::cerr << ... << args); std::cerr << "\n"; }
    template<typename... Args>
    void eprint(Args&&... args)  { (std::cerr << ... << args); }

    struct Point;
struct People;

struct Point {
    int32_t x;
    int32_t y;
    static Point flux_new(int32_t x, int32_t y) {
        return Point{.x=x, .y=y};
    }

    Point max(Point other) {
        if ((this->foo() > other.foo())) {
            return (*this);
        } else {
            return other;
        }
    }

    int32_t foo() {
        return (this->x + this->y);
    }

};

struct People {
    std::string_view name;
    int32_t age;

    static People flux_new(std::string_view name, int32_t age) {
        return People{.name=name, .age=age};
    }

    void print_name() {
        println(this->name);
    }

};

int32_t main() {
    auto animal = People::flux_new(std::string("John"), 16);
    int16_t x = 1345;
    println(x);
    auto pt = Point::flux_new(3, 6);
    println(pt.x);
    return 0;
}

