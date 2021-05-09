#pragma once

#include "pch.h"

template <typename T>
extern constexpr bool not_bool = !std::is_same_v<T, bool>;

template <typename T>
extern constexpr bool any_int = std::is_integral_v<T> && not_bool<T>;

template <typename T>
using requires_int = std::enable_if_t <any_int<T>>;

template <typename T>
extern constexpr bool any_unsigned_int = any_int<T> && std::is_unsigned_v<T>;

template <typename T>
using requires_unsigned_int = std::enable_if_t <any_unsigned_int<T>>;

using balance_t = signed char;

template <typename T>
extern constexpr std::size_t bits_in = sizeof(T)*8;

template <std::size_t N, std::size_t Low, std::size_t High>
extern constexpr bool fits_in = N >= Low && N <= High;

template <std::size_t N, std::size_t Low, std::size_t High>
using enable_if_fits_in = std::enable_if_t <fits_in<N, Low, High>>;

extern constexpr std::size_t uchar_h_min = 0;
extern constexpr std::size_t uchar_h_max = 31;

extern constexpr std::size_t ushort_h_min = uchar_h_max + 1;
extern constexpr std::size_t ushort_h_max = 8191;

extern constexpr std::size_t ulong_h_min = ushort_h_max + 1;
extern constexpr std::size_t ulong_h_max = 536870911;

template <std::size_t, typename = void>
struct height_t;
template <std::size_t Max>
struct height_t<Max, enable_if_fits_in<Max, uchar_h_min, uchar_h_max>> {
    using type = unsigned char;
};
template <std::size_t Max>
struct height_t<Max, enable_if_fits_in<Max, ushort_h_min, ushort_h_max>> {
    using type = unsigned short;
};
template <std::size_t Max>
struct height_t<Max, enable_if_fits_in<Max, ulong_h_min, ulong_h_max>> {
    using type = unsigned long;
};

template <std::size_t MaxHeight, typename = std::enable_if_t<MaxHeight <= ulong_h_max> >
using height_storage_t = typename height_t<MaxHeight>::type;

extern constexpr std::size_t default_storage_max_size = ushort_h_max;

//#define NO_DEBUG_OUTPUT
struct debug {
    template <typename T>
    debug &operator<<(const T &txt) {
        if constexpr (do_nothing) {
            return *this;
        }
        std::stringstream s;
        s << txt;
        std::cout << s.str() << ' ';
        return *this;
    }

    debug &operator<<(bool b) {
        if constexpr (do_nothing) {
            return *this;
        }
        std::cout << std::boolalpha << b << ' ';
        return *this;
    }

    ~debug() {
        if constexpr (!do_nothing) {
            std::cout << '\n';
        }
    }

private:
    static constexpr bool do_nothing =
#ifndef NO_DEBUG_OUTPUT
        false
#else
        true
#endif
        ;
};