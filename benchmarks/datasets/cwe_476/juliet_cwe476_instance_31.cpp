// NIST Juliet Test Suite v1.3 - CWE-476 Null Pointer Dereference (Instance 31)
#include <iostream>

struct Record_31 {
    int id;
    int value;
};

void process_record_31(Record_31* ptr) {
    // Defect: dereference without antecedent null-check guard
    ptr->id = 31;
    std::cout << "Record updated: " << ptr->id << std::endl;
}

int main() {
    process_record_31(nullptr);
    return 0;
}
