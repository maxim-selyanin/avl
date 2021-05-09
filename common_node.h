#pragma once

#include "common.h"

//forward declarations
template <typename T, typename key_t = int, typename = requires_int<key_t> >
class common_node;

template <typename T, typename key_t = int, typename = requires_int<key_t> >
class rb_node;

template <typename T, typename key_t, typename Node>
extern constexpr bool any_node = std::is_base_of_v<common_node<T, key_t>, Node>;

//common node itself
template <typename T, typename key_t, typename dummy_k>
class common_node {
    struct {
        key_t key;
        T value;
    } data;

protected:
    common_node *left = nullptr;
    common_node *right = nullptr;
    common_node *parent = nullptr;

    template <typename T>
    using requires_specific_node = std::enable_if_t <std::is_base_of_v<common_node<T, key_t>, T> >;

    void adopt_left(common_node *other);
    void adopt_right(common_node *other);
    void adopt(common_node *other);//requires valid pointer

    [[nodiscard]] std::size_t distance_from_root() const;

    common_node *cmn_brother();
    common_node *cmn_granny();
    common_node *cmn_uncle();

    [[nodiscard]] bool is_left_son() const;
    [[nodiscard]] bool is_right_son() const;

public:
    common_node(key_t k, T val);

    common_node *find_from(key_t key);

    static void LNR_traverse(const common_node *root, std::function<void (key_t, T)> callback);
    static void level_traverse(const common_node *root);
    static void deletion(common_node *root);
    template <typename Node, typename = requires_specific_node <Node>>
    static void advance_up(Node *&p);

    common_node *right_rtn();
    common_node *left_rtn();

    common_node *lr_rtn();//left, right
    common_node *rl_rtn();//right, left

    T val() const { return data.value;}
    key_t key() const { return data.key;}

    template <typename Node, typename = requires_specific_node<Node>>//обязательно свободная функция
    static Node *delete_node(Node *to_be_deleted);//возвращает ноду, появившуюся на месте удалённой, или nullptr

    template <typename Node, typename = requires_specific_node<Node>>//обязательно шаблонная
    static Node *insert_from(Node *&root, key_t key, T value);//возвращает указатель на новую ноду

    virtual ~common_node();
};

template<typename T, typename key_t, typename Dummy>
common_node<T, key_t, Dummy>::common_node(key_t k, T val)
: data{k, val}
{}

template<typename T, typename key_t, typename Dummy>
template <typename Node, typename>
void common_node<T, key_t, Dummy>::advance_up(Node *&p) {
    if (!p) {
        return;
    }

    common_node *ptr = p;
    while (ptr->parent) {
        ptr = ptr->parent;
    }

    p = dynamic_cast<Node*>(ptr);
}

template<typename T, typename key_t, typename Dummy>
void common_node<T, key_t, Dummy>::adopt_left(common_node *other) {
    left = other;
    if (other) {
        other->parent = this;
    }
}

template<typename T, typename key_t, typename Dummy>
void common_node<T, key_t, Dummy>::adopt_right(common_node *other) {
    right = other;
    if (other) {
        other->parent = this;
    }
}

template<typename T, typename key_t, typename Dummy>
common_node<T, key_t, Dummy> *common_node<T, key_t, Dummy>::find_from(key_t key) {
    common_node *current = this;

    do {
        if (current->key() > key) {//идём налево
            current = current->left;
        } else if (current->key() < key) {//идём направо
            current = current->right;
        } else {
            return current;
        }
    } while (current);

    return nullptr;
}

template<typename T, typename key_t, typename Dummy>
common_node<T, key_t, Dummy> *common_node<T, key_t, Dummy>::right_rtn() {
    if (!parent) {
        return this;
    }

    common_node *granny = parent->parent;
    parent->adopt_left(right);
    adopt_right(parent);

    if (granny) {
        granny->adopt(this);
    } else {
        parent = nullptr;
    }
    return this;
}

template<typename T, typename key_t, typename Dummy>
common_node<T, key_t, Dummy> *common_node<T, key_t, Dummy>::left_rtn() {
    if (!parent) {
        return this;
    }

    common_node *granny = parent->parent;
    parent->adopt_right(left);
    adopt_left(parent);

    if (granny) {
        granny->adopt(this);
    } else {
        parent = nullptr;
    }
    return this;
}

template<typename T, typename key_t, typename Dummy>//left, right
common_node<T, key_t, Dummy> *common_node<T, key_t, Dummy>::lr_rtn() {
    left_rtn();
    right_rtn();
    return this;
}

template<typename T, typename key_t, typename Dummy>//right, left
common_node<T, key_t, Dummy> *common_node<T, key_t, Dummy>::rl_rtn() {
    right_rtn();
    left_rtn();
    return this;
}

template<typename T, typename key_t, typename Dummy>
void common_node<T, key_t, Dummy>::adopt(common_node *other) {
    if (!other) {
        return;//последующая работа будет некорректна
    }

    if (other->key() < key()) {
        adopt_left(other);
    } else if (other->key() > key()) {
        adopt_right(other);
    }
    //в случае равенства ключей дальнейшее поведение некорректно
}

template<typename T, typename key_t, typename Dummy>
common_node<T, key_t, Dummy> *common_node<T, key_t, Dummy>::cmn_brother() {
    if (!parent) {
        return nullptr;
    }
    return (key() < parent->key()) ? parent->right : parent->left;
}

template<typename T, typename key_t, typename Dummy>
common_node<T, key_t, Dummy> *common_node<T, key_t, Dummy>::cmn_granny() {
    if (!parent) {
        return nullptr;
    }
    return parent->parent;
}

template<typename T, typename key_t, typename Dummy>
common_node<T, key_t, Dummy> *common_node<T, key_t, Dummy>::cmn_uncle() {//брат бати
    if (!parent) {
        return nullptr;
    }
    return parent->cmn_brother();
}

template<typename T, typename key_t, typename Dummy>
void common_node<T, key_t, Dummy>::LNR_traverse(const common_node<T, key_t, Dummy> *root, std::function<void (key_t, T)> callback) {
    bool leftmost_reached = false;
    while (root) {
        if (!leftmost_reached) {
            while (root->left) {
                root = root->left;
            }
            leftmost_reached = true;
        }

        callback(root->key(), root->val());

        if (root->right) {
            root = root->right;
            leftmost_reached = false;
        } else {
            while (root->parent && root == root->parent->right) {
                root = root->parent;//поднимаемся, пока находимся в правом поддереве
            }
            root = root->parent;//прийдя в левое поддерево, поднимаемся на родителя;
            //либо придя к корню, получаем nullptr в root
        }
    }
}

template<typename T, typename key_t, typename Dummy>
void common_node<T, key_t, Dummy>::deletion(common_node<T, key_t, Dummy> *root) {
    bool reach_leftmost = true;
    common_node *to_delete = nullptr;

    while (root) {
        delete to_delete;

        if (reach_leftmost) {
            while (root->left) {
                root = root->left;
            }
            reach_leftmost = false;
        }

        to_delete = root;

        if (root->parent && root->is_left_son()) {//мы находимся в левом поддереве
            if (root->parent->right) {//правый брат существует
                root = root->parent->right;//переходим в корень правого поддерева
                reach_leftmost = true;
                
            } else {//правого брата нет
                do {
                    to_delete = root;
                    root = root->parent;
                    delete to_delete;
                } while (root->parent && root->is_left_son() && !root->right);

                to_delete = nullptr;//удаление больше не нужно

                if (root->right) {
                    root = root->right;
                    reach_leftmost = true;
                }
            }

        } else {//мы находимся в правом поддереве либо в корне дерева
            if (root->right) {//если правое поддерево текущей ноды существует
                root = root->right;
                reach_leftmost = true;
                to_delete = nullptr;//пока ничего не удаляем

            } else {//правого поддерева текущей ноды нет
                while (root->parent && root->is_right_son()) {//пока находимся в правом поддереве
                    to_delete = root;
                    root = root->parent;
                    delete to_delete;
                }

                //также удаляем текущую ноду, переходим к её родителю
                to_delete = root;
                root = root->parent;
                delete to_delete;
                //в случае, если текущая нода была корнем, получим nullptr в root и цикл завершится

                to_delete = nullptr;//удаление больше не нужно
            }
        }
    }

    delete to_delete;
}

template<typename T, typename key_t, typename Dummy>
bool common_node<T, key_t, Dummy>::is_left_son() const {
    if (!parent) {
        return false;
    }
    return this == parent->left;
}

template<typename T, typename key_t, typename Dummy>
bool common_node<T, key_t, Dummy>::is_right_son() const {
    if (!parent) {
        return false;
    }
    return this == parent->right;
}

template<typename T, typename key_t, typename Dummy>
template <typename Node, typename>
Node *common_node<T, key_t, Dummy>::insert_from(Node *&root, key_t key, T value) {//возвращает указатель на новую ноду
    if (!root) {
        root = new Node(key, value);
        root->insertion_balance();
        return root;
    }

    common_node *current = root;
    common_node *parent;

    do {
        parent = current;

        if (key < current->key()) {//движемся налево
            current = current->left;
        } else if (key > current->key()) {//движемся направо
            current = current->right;
        } else {
            current->data.value = value;//здесь балансировка не требуется
            return dynamic_cast<Node*>(current);
        }
    } while (current);

    void (common_node::*adopting_function)(common_node*) = (key < parent->key()) ? &common_node::adopt_left : &common_node::adopt_right;
    Node *node = new Node(key, value);
    (parent->*adopting_function)(node);
    node->insertion_balance();
    return node;
}

template<typename T, typename key_t, typename Dummy>
template <typename Node, typename node_dummy>//обязательно свободная функция
Node *common_node<T, key_t, Dummy>::delete_node(Node *to_be_deleted) {//returns new node on the place of deleted, or nullptr
    if (!to_be_deleted) {
        return nullptr;
    }

    Node *parent = dynamic_cast<Node*>(to_be_deleted->parent);

    bool deleted_is_black = true;
    if constexpr (std::is_same_v<Node, rb_node<T, key_t, Dummy>>) {
        deleted_is_black = black(to_be_deleted);
    }

    class automatic_balance {
        Node *parent;
        Node *son = nullptr;
        bool deleted_is_black;
        bool balancing_cancelled = false;

    public:
        explicit automatic_balance(Node *p, bool black) : parent(p), deleted_is_black(black){}
        void cancel_balance() {balancing_cancelled = true;}
        void set_son(Node *s) {son = s;}
        ~automatic_balance() {
            if (!balancing_cancelled) {
                Node::deletion_balance(deleted_is_black, parent, son);
            }
        }
    } auto_balance(parent, deleted_is_black);//automatic balance

    class automatic_deleter {
        Node *ptr;
        bool deletion_cancelled = false;

    public:
        explicit automatic_deleter(Node *p) : ptr(p) {}
        void cancel_deletion() {deletion_cancelled = true;}
        ~automatic_deleter() {
            if (!deletion_cancelled) {
                delete ptr;
            }
        }
    } deleter(to_be_deleted);//automatic deleter

    if (to_be_deleted->left && to_be_deleted->right) {//2 сына
        Node *current = dynamic_cast<Node*>(to_be_deleted->right);//current изначально валиден, так как оба сына существуют

        while (current->left) {
            current = dynamic_cast<Node*>(current->left);
        }

        to_be_deleted->data = std::move(current->data);//переносим содержимое current в текущую ноду
        deleter.cancel_deletion();//текущая нода уже не удаляется
        auto_balance.cancel_balance();//баланс с родителя текущей ноды уже не вызывается
        return common_node<T, key_t, Dummy>::delete_node<Node, node_dummy>(current);//удаляем current

    } else if (to_be_deleted->left || to_be_deleted->right) {//1 сын
        Node *son = dynamic_cast<Node*>((to_be_deleted->left) ? to_be_deleted->left : to_be_deleted->right);
        auto_balance.set_son(son);

        if (!parent) {
            son->parent = nullptr;
            return son;
        }//поскольку parent == nullptr, балансировка здесь вызываться не будет

        void (common_node::*adopting_function)(common_node*) =
                (to_be_deleted->key() > parent->key()) ? &common_node::adopt_right : &common_node::adopt_left;

        (parent->*adopting_function)(son);//присоединяем существующего сына к нужной стороне отца
        return son;

    } else {//нет сыновей
        auto_balance.set_son(nullptr);
        if (!parent) {
            return nullptr;
        }

        if (to_be_deleted->key() > parent->key()) {//находимся справа
            parent->right = nullptr;
        } else {//находимся слева
            parent->left = nullptr;
        }

        return nullptr;
    }
}

template<typename T, typename key_t, typename dummy_k>
common_node<T, key_t, dummy_k>::~common_node() {
    if (parent) {
        if (parent->right == this) {
            parent->right = nullptr;
        }
        if (parent->left == this) {
            parent->left = nullptr;
        }
    }

    if (left && left->parent == this) {
        left->parent = nullptr;
    }

    if (right && right->parent == this) {
        right->parent = nullptr;
    }
}

template<typename T, typename key_t, typename dummy_k>
void common_node<T, key_t, dummy_k>::level_traverse(const common_node *root) {
    if (!root) {
        return;
    }

    std::queue<const common_node*> q;
    q.push(root);
    const common_node *current;

    std::size_t prev_dist = 0;

    do {
        current = q.front();
        q.pop();

        if (current->distance_from_root() > prev_dist) {
            std::cout << '\n';
        }
        prev_dist = current->distance_from_root();

        std::cout << current->key() << ' ';

        if (current->left) {
            q.push(current->left);
        }
        if (current->right) {
            q.push(current->right);
        }
    } while (!q.empty());
    std::cout << '\n';
}

template<typename T, typename key_t, typename dummy_k>
std::size_t common_node<T, key_t, dummy_k>::distance_from_root() const {
    const common_node *p = this;
    std::size_t distance = 0;
    while (p->parent) {
        ++distance;
        p = p->parent;
    }
    return distance;
}
