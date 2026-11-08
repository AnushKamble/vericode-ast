// NIST Juliet Test Suite v1.3 - CWE-476 Null Pointer Dereference (Instance 05)
#include <iostream>

struct Record_05 {
    int id;
    int value;
};

void process_record_05(Record_05* ptr) {
    // Defect: dereference without antecedent null-check guard
    ptr->id = 5;
    std::cout << "Record updated: " << ptr->id << std::endl;
}

int main() {
    process_record_05(nullptr);
    return 0;
}
