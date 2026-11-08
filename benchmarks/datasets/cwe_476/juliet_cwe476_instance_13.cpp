// NIST Juliet Test Suite v1.3 - CWE-476 Null Pointer Dereference (Instance 13)
#include <iostream>

struct Record_13 {
    int id;
    int value;
};

void process_record_13(Record_13* ptr) {
    // Defect: dereference without antecedent null-check guard
    ptr->id = 13;
    std::cout << "Record updated: " << ptr->id << std::endl;
}

int main() {
    process_record_13(nullptr);
    return 0;
}
