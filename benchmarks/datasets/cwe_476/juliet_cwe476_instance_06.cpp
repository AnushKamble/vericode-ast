// NIST Juliet Test Suite v1.3 - CWE-476 Null Pointer Dereference (Instance 06)
#include <iostream>

struct Record_06 {
    int id;
    int value;
};

void process_record_06(Record_06* ptr) {
    // Defect: dereference without antecedent null-check guard
    ptr->id = 6;
    std::cout << "Record updated: " << ptr->id << std::endl;
}

int main() {
    process_record_06(nullptr);
    return 0;
}
