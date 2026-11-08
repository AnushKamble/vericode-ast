// NIST Juliet Test Suite v1.3 - CWE-476 Null Pointer Dereference (Instance 11)
#include <iostream>

struct Record_11 {
    int id;
    int value;
};

void process_record_11(Record_11* ptr) {
    // Defect: dereference without antecedent null-check guard
    ptr->id = 11;
    std::cout << "Record updated: " << ptr->id << std::endl;
}

int main() {
    process_record_11(nullptr);
    return 0;
}
