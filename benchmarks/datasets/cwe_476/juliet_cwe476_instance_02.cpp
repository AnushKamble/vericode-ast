// NIST Juliet Test Suite v1.3 - CWE-476 Null Pointer Dereference (Instance 02)
#include <iostream>

struct Record_02 {
    int id;
    int value;
};

void process_record_02(Record_02* ptr) {
    // Defect: dereference without antecedent null-check guard
    ptr->id = 2;
    std::cout << "Record updated: " << ptr->id << std::endl;
}

int main() {
    process_record_02(nullptr);
    return 0;
}
