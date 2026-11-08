// NIST Juliet Test Suite v1.3 - CWE-476 Null Pointer Dereference (Instance 29)
#include <iostream>

struct Record_29 {
    int id;
    int value;
};

void process_record_29(Record_29* ptr) {
    // Defect: dereference without antecedent null-check guard
    ptr->id = 29;
    std::cout << "Record updated: " << ptr->id << std::endl;
}

int main() {
    process_record_29(nullptr);
    return 0;
}
