// NIST Juliet Test Suite v1.3 - CWE-476 Null Pointer Dereference (Instance 20)
#include <iostream>

struct Record_20 {
    int id;
    int value;
};

void process_record_20(Record_20* ptr) {
    // Defect: dereference without antecedent null-check guard
    ptr->id = 20;
    std::cout << "Record updated: " << ptr->id << std::endl;
}

int main() {
    process_record_20(nullptr);
    return 0;
}
