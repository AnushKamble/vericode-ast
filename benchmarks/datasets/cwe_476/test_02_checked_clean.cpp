// Test Case: Clean NULL Guard Checked (Zero Defect Baseline)
#include <iostream>

struct Node {
    int value;
    Node* next;
};

void safe_update_node(Node* node) {
    if (node != nullptr) {
        node->value = 42;
    }
}

int main() {
    safe_update_node(nullptr);
    return 0;
}
