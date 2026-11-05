#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

// Mirrors `std::_Rb_tree_node_base`, followed by a key.
struct node {
    int color;
    struct node *parent;
    struct node *left;
    struct node *right;
    int key;
};

// The out-of-line red-black tree algorithms in GNU libstdc++. Inlined
// `std::set` and `std::map` code calls these, and then reads the links
// that they write.
void _ZSt29_Rb_tree_insert_and_rebalancebPSt18_Rb_tree_node_baseS0_RS_(
    bool insert_left, struct node *x, struct node *p, struct node *header);
struct node *_ZSt28_Rb_tree_rebalance_for_erasePSt18_Rb_tree_node_baseRS_(
    struct node *z, struct node *header);
struct node *_ZSt18_Rb_tree_incrementPSt18_Rb_tree_node_base(struct node *x);

static void tree_insert(struct node *header, int key) {
    struct node *x = malloc(sizeof(struct node));
    x->key = key;
    struct node *p = header;
    struct node *cur = header->parent;
    bool left = true;
    while (cur) {
        p = cur;
        left = key < cur->key;
        cur = left ? cur->left : cur->right;
    }
    _ZSt29_Rb_tree_insert_and_rebalancebPSt18_Rb_tree_node_baseS0_RS_(left || p == header, x,
                                                                       p, header);
}

static void tree_erase(struct node *header, int key) {
    struct node *cur = header->parent;
    while (cur && cur->key != key)
        cur = key < cur->key ? cur->left : cur->right;
    if (!cur)
        abort();
    free(_ZSt28_Rb_tree_rebalance_for_erasePSt18_Rb_tree_node_baseRS_(cur, header));
}

// Rewrites every link from instrumented code, as inlined code like
// `_M_copy` does. If these nodes are later freed and reallocated at the
// same addresses, then libstdc++ will write pointers that are equal to the
// ones stored here, but that belong to new allocations.
static void tree_touch(struct node *n) {
    if (!n)
        return;
    n->left = n->left;
    n->right = n->right;
    n->parent = n->parent;
    tree_touch(n->left);
    tree_touch(n->right);
}

static int tree_sum(struct node *header) {
    int sum = 0;
    for (struct node *n = header->left; n != header;
         n = _ZSt18_Rb_tree_incrementPSt18_Rb_tree_node_base(n))
        sum += n->key;
    return sum;
}

int libstdcxx_rb_tree_churn(int rounds) {
    struct node *header = calloc(1, sizeof(struct node));
    header->left = header;
    header->right = header;
    int sum = 0;
    for (int r = 0; r < rounds; r++) {
        for (int i = 0; i < 16; i++)
            tree_insert(header, (i * 7) % 16);
        header->parent = header->parent;
        tree_touch(header->parent);
        sum += tree_sum(header);
        for (int i = 0; i < 16; i++)
            tree_erase(header, (i * 5) % 16);
    }
    free(header);
    return sum;
}
