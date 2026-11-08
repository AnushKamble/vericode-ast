// NIST Juliet Test Suite v1.3 - CWE-476 Null Pointer Dereference (Instance 22)
#include <iostream>

struct Record_22 {
    int id;
    int value;
};

void process_record_22(Record_22* ptr) {
    // Defect: dereference without antecedent null-check guard
    ptr->id = 22;
    std::cout << "Record updated: " << ptr->id << std::endl;
}

int main() {
    process_record_22(nullptr);
    return 0;
}
