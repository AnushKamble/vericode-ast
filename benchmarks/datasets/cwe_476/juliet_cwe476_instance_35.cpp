// NIST Juliet Test Suite v1.3 - CWE-476 Null Pointer Dereference (Instance 35)
#include <iostream>

struct Record_35 {
    int id;
    int value;
};

void process_record_35(Record_35* ptr) {
    // Defect: dereference without antecedent null-check guard
    ptr->id = 35;
    std::cout << "Record updated: " << ptr->id << std::endl;
}

int main() {
    process_record_35(nullptr);
    return 0;
}
