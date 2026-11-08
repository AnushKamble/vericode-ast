// NIST Juliet Test Suite v1.3 - CWE-476 Null Pointer Dereference (Instance 15)
#include <iostream>

struct Record_15 {
    int id;
    int value;
};

void process_record_15(Record_15* ptr) {
    // Defect: dereference without antecedent null-check guard
    ptr->id = 15;
    std::cout << "Record updated: " << ptr->id << std::endl;
}

int main() {
    process_record_15(nullptr);
    return 0;
}
