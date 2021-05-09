#pragma once

#include "avl_node.h"
#include "rb_node.h"
#include "common_tree.h"

template <typename T, typename key_t = int, std::size_t MaxHeight = default_storage_max_size, typename = requires_int <key_t> >
using avl_tree = common_tree<T, key_t, avl_node<T, key_t, MaxHeight>>;

template <typename T, typename key_t = int, typename = requires_int <key_t> >
using rb_tree = common_tree<T, key_t, rb_node<T, key_t>>;