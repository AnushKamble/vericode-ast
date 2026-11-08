// NIST Juliet Test Suite v1.3 - CWE-476 Null Pointer Dereference (Instance 09)
#include <iostream>

struct Record_09 {
    int id;
    int value;
};

void process_record_09(Record_09* ptr) {
    // Defect: dereference without antecedent null-check guard
    ptr->id = 9;
    std::cout << "Record updated: " << ptr->id << std::endl;
}

int main() {
    process_record_09(nullptr);
    return 0;
}
