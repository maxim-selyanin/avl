#pragma once

#include "common_node.h"
#include "color_keeper.h"
#include "whatever.h"

template <typename T, typename key_t, typename>
class rb_node : public common_node<T, key_t> {
    color_keeper color{};

    void del_balance(rb_node *son);

    friend void swap_colors(rb_node *one, rb_node *other) { one->color.swap(other->color);}
    void repaint() { color.repaint();}

    rb_node *father() { return dynamic_cast<rb_node*>(parent);}
    rb_node *granny() { return dynamic_cast<rb_node*>(cmn_granny());}
    rb_node *uncle() { return dynamic_cast<rb_node*>(cmn_uncle());}

public:
    rb_node(key_t key, T value) : common_node(key, value){}

    void insertion_balance();
    static void deletion_balance(bool deleted_is_black, rb_node *del_father, rb_node *del_son);

    friend bool black(const rb_node *node) { return !node || node->color.black();}
    friend bool red(const rb_node *node) { return node && node->color.red();}

    virtual ~rb_node() = default;
};

template<typename T, typename key_t, typename Dummy>
void rb_node<T, key_t, Dummy>::insertion_balance() {
    if (!parent) {
        color.set_black();
        return;
    }

    if (black(father())) {
        return;
    }

    rb_node *un = uncle();
    if (black(un)) {//дядя чёрный
        if (father()->key() < father()->father()->key() && key() < father()->key()) {//gran, dad и this образуют линию
            father()->repaint();
            granny()->repaint();
            parent->right_rtn();
        } else if (father()->key() > father()->father()->key() && key() > father()->key()) {
            father()->repaint();
            granny()->repaint();
            parent->left_rtn();
        } else if (father()->key() < father()->father()->key() && key() > father()->key()) {//gran, dad и this образуют угол
            this->repaint();
            granny()->repaint();
            this->lr_rtn();//left, right
        } else if (father()->key() > father()->father()->key() && key() < father()->key()) {
            this->repaint();
            granny()->repaint();
            this->rl_rtn();//left, right
        } else {
            throw_logic_error("impossible situation occurred in rb insertion balance");
        }

    } else {//дядя красный
        un->repaint();
        father()->repaint();
        granny()->repaint();//дед существует, тк красно-красное нарушение
        granny()->insertion_balance();
    }
}

template<typename T, typename key_t, typename Dummy>
void rb_node<T, key_t, Dummy>::del_balance(rb_node *son) {
    /* функция вызывается на предке удалённого узла; отсюда очевидно, что удалённый узел не был корневым;
     * при входе в функцию подразумевается, что удалённый узел - чёрный;
     * при входе в функцию подразумевается, что сын удалённого узла (son) - чёрный, возможно, фиктивный;
     * важно понимать, что son может быть nullptr
     */

    rb_node *brother = dynamic_cast<rb_node*>((dynamic_cast<rb_node*>(this->left) == son) ? this->right : this->left);

    rb_node *external_grandson;
    rb_node *internal_grandson;

    common_node* (common_node::*internal_rotation)();
    common_node* (common_node::*external_rotation)();

    if (son == dynamic_cast<rb_node*>(this->left)) {//сын удалённого находится слева от отца
        internal_grandson = dynamic_cast<rb_node*>(brother->left);
        external_grandson = dynamic_cast<rb_node*>(brother->right);

        internal_rotation = &common_node::left_rtn;
        external_rotation = &common_node::right_rtn;
    } else {//сын удалённого находится справа от отца
        internal_grandson = dynamic_cast<rb_node*>(brother->right);
        external_grandson = dynamic_cast<rb_node*>(brother->left);

        internal_rotation = &common_node::right_rtn;
        external_rotation = &common_node::left_rtn;
    }

    //algorithm itself
    if (red(this) && black(brother) && black(internal_grandson) && black(external_grandson)) {
        //father is red, others are black
        this->repaint();
        brother->repaint();
    } else if (black(brother) && red(external_grandson)) {
        //brother is black, cr is red, others are any
        external_grandson->repaint();
        swap_colors(this, brother);
        (brother->*internal_rotation)();
    } else if (black(brother) && black(external_grandson) && red(internal_grandson)) {
        //brother and his right son are black, brother's left son is red, father is any
        brother->repaint();
        internal_grandson->repaint();
        (internal_grandson->*external_rotation)();
        this->del_balance(son);
    } else if (red(brother)) {
        //brother is red, others have to be black, so no need in additional check
        this->repaint();
        brother->repaint();
        (brother->*internal_rotation)();
        this->del_balance(son);
    } else if (black(this) && black(brother) && black(son) && black(internal_grandson) && black(external_grandson)) {
        //they all are black
        brother->repaint();
        if (this->parent) {//если father не является корнем
            dynamic_cast<rb_node*>(this->parent)->del_balance(this);//повторяем балансировку для father в роли son
        }
    } else {
        //that can't be reached in correct tree
        throw_logic_error("the unreachable point had been reached in del_balance function in red-black tree");
    }
}

template<typename T, typename key_t, typename Dummy>
void rb_node<T, key_t, Dummy>::deletion_balance(bool deleted_is_black, rb_node<T, key_t, Dummy> *del_father, rb_node<T, key_t, Dummy> *del_son) {
    if (!deleted_is_black) {
        return;
    }//с этого момента удалённый узел считается чёрным

    if (!black(del_son)) {//если сын существует и красный
        del_son->repaint();
    } else {//сын чёрный - возможно, фиктивный
        if (del_father) {//если удалённый узел не корневой
            del_father->del_balance(del_son);//удалённый узел не корневой, чёрный, его сын чёрный, возможно фиктивный
        }
        //если удалённый узел корневой и его сын чёрный, то сын фиктивный, мы получаем пустое дерево и ничего не делаем
    }
}
