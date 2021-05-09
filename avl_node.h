#pragma once

#include "storage.h"
#include "common_node.h"

//key_t, Max_Height
template <typename T, typename key_t = int, std::size_t Max_Height = default_storage_max_size, typename Dummy = requires_int<key_t> >
class avl_node : public common_node<T, key_t, Dummy> {
    storage<Max_Height> info{};

    [[nodiscard]] bool balance_broken() const { return std::abs(info.balance()) > 1;}

    bool update();
    avl_node *find_broken_above();
    void update_above();

public:
    avl_node(key_t key, T value) : common_node(key, value){}

    static void deletion_balance(bool, avl_node *parent, avl_node*) {
        if (parent) {
            parent->balance();
        }
    }

    void balance();
    void insertion_balance() {
        this->balance();
    }

    virtual ~avl_node() = default;
};

template<typename T, typename key_t, std::size_t Max_Height, typename Dummy>
bool avl_node<T, key_t, Max_Height, Dummy>::update() {
    using storage_t = height_storage_t <Max_Height>;
    storage_t hl = (left) ? dynamic_cast<avl_node*>(left)->info.height() : 0;
    storage_t hr = (right) ? dynamic_cast<avl_node*>(right)->info.height() : 0;
    if (hl > hr) {
        info.set_height(hl + 1);
        balance_t tmp = hl - hr;
        return !info.set_balance(-tmp);
    } else {
        info.set_height(hr + 1);
        return !info.set_balance(hr - hl);
    }
}

template<typename T, typename key_t, std::size_t Max_Height, typename Dummy>
void avl_node<T, key_t, Max_Height, Dummy>::update_above() {
    avl_node *cur = this;

    while (cur && cur->update()) {//апдейтим, пока не дойдём до корня или до узла, в котором не меняется баланс
        cur = dynamic_cast<avl_node*>(cur->parent);
    }
}

template<typename T, typename key_t, std::size_t Max_Height, typename Dummy>
avl_node<T, key_t, Max_Height, Dummy> *avl_node<T, key_t, Max_Height, Dummy>::find_broken_above() {
    avl_node *cur = this;

    do {
        if (cur->update() || cur == this) {//если после апдейта баланс изменился
            if (cur->balance_broken()) {//если баланс нарушен
                return cur;
            }
            cur = dynamic_cast<avl_node*>(cur->parent);
        } else {
            return nullptr;//если на каком то моменте баланс перестаёт меняться, то сломанной ноды дальше нет
        }
    } while (cur);

    return nullptr;
}

template<typename T, typename key_t, std::size_t Max_Height, typename Dummy>
void avl_node<T, key_t, Max_Height, Dummy>::balance() {
    avl_node *broken = find_broken_above();
    if (!broken) {
        return;
    }

    if (broken->info.balance() > 0) {//balance = 2; правое поддерево больше
        if (dynamic_cast<avl_node*>(broken->right)->info.balance() >= 0) {//правое-правое поддерево больше или равно
            broken->right->left_rtn();//broken->right обязательно валиден
            broken->update_above();
        } else {//правое-левое (центральное) поддерево больше
            dynamic_cast<avl_node*>(broken->right)->left->rl_rtn();//broken->right->left обязательно валиден
            dynamic_cast<avl_node*>(broken->cmn_brother())->update();//здесь у broken точно есть брат
            broken->update_above();
        }
    } else {//balance = -2; левое поддерево больше
        if (dynamic_cast<avl_node*>(broken->left)->info.balance() <= 0) {
            broken->left->right_rtn();
            broken->update_above();
        } else {
            dynamic_cast<avl_node*>(broken->left)->right->lr_rtn();
            dynamic_cast<avl_node*>(broken->cmn_brother())->update();
            broken->update_above();
        }
    }

    avl_node *root = this;
    while (root->parent) {
        root = dynamic_cast<avl_node*>(root->parent);
    }
}
