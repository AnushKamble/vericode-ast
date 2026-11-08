// NIST Juliet Test Suite v1.3 - CWE-476 Null Pointer Dereference (Instance 21)
#include <iostream>

struct Record_21 {
    int id;
    int value;
};

void process_record_21(Record_21* ptr) {
    // Defect: dereference without antecedent null-check guard
    ptr->id = 21;
    std::cout << "Record updated: " << ptr->id << std::endl;
}

int main() {
    process_record_21(nullptr);
    return 0;
}
