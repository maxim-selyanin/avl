#pragma once

#include "common_node.h"
#include "whatever.h"

//слева ключи меньше, справа больше
template <typename T, typename key_t, typename Node,
        typename = std::enable_if_t<any_node<T, key_t, Node>>, typename Dummy_k = requires_int <key_t>>
class common_tree {
    Node *root = nullptr;

public:
    common_tree() = default;

    [[nodiscard]] bool correct() const;

    [[nodiscard]] T find(key_t key) const;

    void insert(key_t key, T value);

    std::vector<std::pair<key_t, T>> ordered_elements() const;

    T remove(key_t key);

    void level_order() const { common_node<T, key_t, Dummy_k>::level_traverse(root);}

    [[nodiscard]] bool empty() const { return !root;}

    ~common_tree() {common_node<T, key_t, Dummy_k>::deletion(root);}
};

template<typename T, typename key_t, typename Node, typename Dummy_n, typename Dummy_k>
bool common_tree<T, key_t, Node, Dummy_n, Dummy_k>::correct() const {
    bool is_correct = true;
    common_node<T, key_t, Dummy_k>::LNR_traverse(root, tree_examiner<T, key_t>{is_correct});
    return is_correct;
}

template<typename T, typename key_t, typename Node, typename Dummy_n, typename Dummy_k>
T common_tree<T, key_t, Node, Dummy_n, Dummy_k>::find(key_t key) const {
    if (!root) {
        throw_logic_error("searching in empty tree");
    }

    const common_node<T, key_t> *result = root->find_from(key);
    if (!result) {
        throw_logic_error(std::stringstream() << "searching for the key " << key << " that does not exist");
    }
    return result->val();
}

template<typename T, typename key_t, typename Node, typename Dummy_n, typename Dummy_k>
void common_tree<T, key_t, Node, Dummy_n, Dummy_k>::insert(key_t key, T value) {
    common_node<T, key_t, Dummy_k>::insert_from<Node, Dummy_n>(root, key, value);
    common_node<T, key_t, Dummy_k>::advance_up<Node, Dummy_n>(root);
}

template<typename T, typename key_t, typename Node, typename Dummy_n, typename Dummy_k>
T common_tree<T, key_t, Node, Dummy_n, Dummy_k>::remove(key_t key) {
    if (!root) {
        throw_logic_error("removing from empty tree");
    }

    Node *to_be_removed = dynamic_cast<Node*>(root->find_from(key));
    if (!to_be_removed) {
        throw_logic_error("trying to delete nonexistent key");
    }
    T del_value = to_be_removed->val();
    Node *son_of_deleted_node = common_node<T, key_t, Dummy_k>::delete_node<Node, Dummy_n>(to_be_removed);
    if (to_be_removed == root) {
        root = son_of_deleted_node;
    }
    common_node<T, key_t, Dummy_k>::advance_up<Node, Dummy_n>(root);
    return del_value;
}

template<typename T, typename key_t, typename Node, typename dummy_n, typename Dummy_k>
std::vector<std::pair<key_t, T>> common_tree<T, key_t, Node, dummy_n, Dummy_k>::ordered_elements() const {
    std::vector<std::pair<key_t, T>> v;
    common_node<T, key_t, Dummy_k>::LNR_traverse(root, [&v](key_t key, T value) {
        v.push_back(std::make_pair(key, value));
    });
    return v;
}
