// NIST Juliet Test Suite v1.3 - CWE-476 Null Pointer Dereference (Instance 23)
#include <iostream>

struct Record_23 {
    int id;
    int value;
};

void process_record_23(Record_23* ptr) {
    // Defect: dereference without antecedent null-check guard
    ptr->id = 23;
    std::cout << "Record updated: " << ptr->id << std::endl;
}

int main() {
    process_record_23(nullptr);
    return 0;
}
