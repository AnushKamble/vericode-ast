// NIST Juliet Test Suite v1.3 - CWE-476 Null Pointer Dereference (Instance 38)
#include <iostream>

struct Record_38 {
    int id;
    int value;
};

void process_record_38(Record_38* ptr) {
    // Defect: dereference without antecedent null-check guard
    ptr->id = 38;
    std::cout << "Record updated: " << ptr->id << std::endl;
}

int main() {
    process_record_38(nullptr);
    return 0;
}
