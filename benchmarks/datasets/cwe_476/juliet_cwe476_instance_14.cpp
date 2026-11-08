// NIST Juliet Test Suite v1.3 - CWE-476 Null Pointer Dereference (Instance 14)
#include <iostream>

struct Record_14 {
    int id;
    int value;
};

void process_record_14(Record_14* ptr) {
    // Defect: dereference without antecedent null-check guard
    ptr->id = 14;
    std::cout << "Record updated: " << ptr->id << std::endl;
}

int main() {
    process_record_14(nullptr);
    return 0;
}
