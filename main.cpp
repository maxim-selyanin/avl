#define NO_DEBUG_OUTPUT
#include "trees.h"

int main() {
    avl_tree <std::string> tree;
    std::cout << std::boolalpha;

    int amount = 40;
    for (int i = 0; i < amount; ++i) {
        tree.insert(i, (std::stringstream() << i * 100).str());
    }

    std::cout << "is tree correct: " << tree.correct() << '\n';
    int check_num = 10;
    std::cout << "corresponding to " << check_num << ": " << tree.find(check_num) << '\n';

    std::cout << "printing ordered keys\n";
    auto v = tree.ordered_elements();
    for (auto i = v.begin(); i != v.end(); ++i) {
        std::cout << i->first << ' ';
    }
    std::cout << '\n';

    for (auto i = v.begin(); i != v.end(); ++i) {
        std::cout << i->second << ' ';
    }
    std::cout << '\n';

    std::cout << "level order\n";
    tree.level_order();

    std::cout << "removing some of elements\n";
    for (int i = amount; i > 0; --i) {
        std::cout << tree.remove(i - 1) << '\n';
    }

    std::cout << "is tree empty: " << tree.empty() << '\n';
    std::cout << "is tree correct: " << tree.correct() << '\n';

    amount = 10;
    for (int i = 0; i < amount; ++i) {
        tree.insert(i, (std::stringstream() << i + 5).str());
    }

    std::cout << "level order\n";
    tree.level_order();

    for (int i = 0; i < amount; ++i) {
        std::cout << tree.remove(i) << ' ';
    }
    std::cout << '\n';

    std::cout << "is empty: " << tree.empty() << '\n';

    return 0;
}