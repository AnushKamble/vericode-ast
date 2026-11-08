// NIST Juliet Test Suite v1.3 - CWE-476 Null Pointer Dereference (Instance 26)
#include <iostream>

struct Record_26 {
    int id;
    int value;
};

void process_record_26(Record_26* ptr) {
    // Defect: dereference without antecedent null-check guard
    ptr->id = 26;
    std::cout << "Record updated: " << ptr->id << std::endl;
}

int main() {
    process_record_26(nullptr);
    return 0;
}
