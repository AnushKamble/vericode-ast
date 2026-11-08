// NIST Juliet Test Suite v1.3 - CWE-476 Null Pointer Dereference (Instance 27)
#include <iostream>

struct Record_27 {
    int id;
    int value;
};

void process_record_27(Record_27* ptr) {
    // Defect: dereference without antecedent null-check guard
    ptr->id = 27;
    std::cout << "Record updated: " << ptr->id << std::endl;
}

int main() {
    process_record_27(nullptr);
    return 0;
}
