// Test Case: CWE-476 NULL Pointer Dereference without Guard
#include <iostream>

struct Node {
    int value;
    Node* next;
};

void update_node(Node* node) {
    // Defect: Dereferencing pointer without checking if it is null
    node->value = 42;
}

int main() {
    update_node(nullptr);
    return 0;
}
