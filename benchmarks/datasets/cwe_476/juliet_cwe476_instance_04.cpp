// NIST Juliet Test Suite v1.3 - CWE-476 Null Pointer Dereference (Instance 04)
#include <iostream>

struct Record_04 {
    int id;
    int value;
};

void process_record_04(Record_04* ptr) {
    // Defect: dereference without antecedent null-check guard
    ptr->id = 4;
    std::cout << "Record updated: " << ptr->id << std::endl;
}

int main() {
    process_record_04(nullptr);
    return 0;
}
