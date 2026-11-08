// NIST Juliet Test Suite v1.3 - CWE-476 Null Pointer Dereference (Instance 03)
#include <iostream>

struct Record_03 {
    int id;
    int value;
};

void process_record_03(Record_03* ptr) {
    // Defect: dereference without antecedent null-check guard
    ptr->id = 3;
    std::cout << "Record updated: " << ptr->id << std::endl;
}

int main() {
    process_record_03(nullptr);
    return 0;
}
