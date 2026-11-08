// NIST Juliet Test Suite v1.3 - CWE-476 Null Pointer Dereference (Instance 01)
#include <iostream>

struct Record_01 {
    int id;
    int value;
};

void process_record_01(Record_01* ptr) {
    // Defect: dereference without antecedent null-check guard
    ptr->id = 1;
    std::cout << "Record updated: " << ptr->id << std::endl;
}

int main() {
    process_record_01(nullptr);
    return 0;
}
