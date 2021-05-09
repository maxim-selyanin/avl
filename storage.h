#pragma once

#include "common.h"

template<std::size_t MaxHeight = default_storage_max_size, typename = std::enable_if_t<MaxHeight <= ulong_h_max> >
class storage {
public:
    using Storage = height_storage_t <MaxHeight>;

private:
    Storage s = 0;//изначально высота равна 1, баланс равен 0
    static constexpr std::size_t offset = bits_in<Storage> - 3;

    static constexpr Storage balance_cleaner = 7;                         //0...0111
    static constexpr Storage height_cleaner = (balance_cleaner << offset);//1110...0
    static constexpr Storage height_getter = ~height_cleaner;             //0001...1

public:
    storage();

    [[nodiscard]] bool set_balance(balance_t bal);
    bool set_height(Storage h);

    [[nodiscard]] balance_t balance() const;
    [[nodiscard]] Storage height() const;

    ~storage() = default;
};

template<std::size_t MaxHeight, typename T>
balance_t storage<MaxHeight, T>::balance() const {
    balance_t b = (s >> offset) & balance_cleaner;
    return b - 2;
}

template<std::size_t MaxHeight, typename T>
typename storage<MaxHeight, T>::Storage storage<MaxHeight, T>::height() const {
    return s & height_getter;
}

template<std::size_t MaxHeight, typename T>
bool storage<MaxHeight, T>::set_balance(balance_t bal) {
    balance_t old = balance();

    bal += 2;//прибавили смещение, bal неотрицателен
    Storage tmp = bal;//3 бита баланса

    s &= height_getter;//очищаем биты баланса в s (3 верхних бита)
    s |= (tmp << offset);//ставим биты баланса на своё место

    return old == balance();
}

template<std::size_t MaxHeight, typename T>
bool storage<MaxHeight, T>::set_height(Storage h) {
    Storage old = height();

    s &= height_cleaner;//очищаем биты высоты
    s |= h;//пользователем гарантируется, что верхние 3 бита h равны 0

    return old == height();
}

template<std::size_t MaxHeight, typename T>
storage<MaxHeight, T>::storage() {
    bool fuck_nodiscard = set_balance(0);
    (void)fuck_nodiscard;
    set_height(1);
}
