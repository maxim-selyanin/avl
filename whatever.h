#pragma once

#include "common.h"

template <typename T, typename key_t = int, typename Dummy = requires_int <key_t> >
class tree_examiner {
    key_t previous;
    bool first = true;
    bool &correct;

public:
    explicit tree_examiner(bool &c)
    : correct(c)
    {
        correct = true;
    }

    void operator()(key_t key, T) {
        if (first) {
            first = false;
            previous = key;
        } else {
            if (key <= previous) {
                correct = false;
            };
            previous = key;
        }
    }
};

[[noreturn]] inline void throw_logic_error(const char *msg) {
    std::cerr << msg << '\n';
    throw std::logic_error(msg);
}

[[noreturn]] inline void throw_logic_error(std::stringstream s) {
    std::cerr << s.str().c_str() << '\n';
    throw std::logic_error(s.str().c_str());
}