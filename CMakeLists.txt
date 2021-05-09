cmake_minimum_required(VERSION 3.15)
project(avl)

set(CMAKE_CXX_STANDARD 17)

add_executable(avl main.cpp pch.h common.h storage.h avl_node.h common_node.h rb_node.h color_keeper.h trees.h whatever.h common_tree.h)