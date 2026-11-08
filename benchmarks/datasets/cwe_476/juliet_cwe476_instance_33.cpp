// NIST Juliet Test Suite v1.3 - CWE-476 Null Pointer Dereference (Instance 33)
#include <iostream>

struct Record_33 {
    int id;
    int value;
};

void process_record_33(Record_33* ptr) {
    // Defect: dereference without antecedent null-check guard
    ptr->id = 33;
    std::cout << "Record updated: " << ptr->id << std::endl;
}

int main() {
    process_record_33(nullptr);
    return 0;
}
