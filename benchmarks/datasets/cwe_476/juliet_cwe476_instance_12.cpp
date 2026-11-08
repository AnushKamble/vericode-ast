// NIST Juliet Test Suite v1.3 - CWE-476 Null Pointer Dereference (Instance 12)
#include <iostream>

struct Record_12 {
    int id;
    int value;
};

void process_record_12(Record_12* ptr) {
    // Defect: dereference without antecedent null-check guard
    ptr->id = 12;
    std::cout << "Record updated: " << ptr->id << std::endl;
}

int main() {
    process_record_12(nullptr);
    return 0;
}
