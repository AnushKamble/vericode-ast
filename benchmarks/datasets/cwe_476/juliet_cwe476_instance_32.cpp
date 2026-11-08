// NIST Juliet Test Suite v1.3 - CWE-476 Null Pointer Dereference (Instance 32)
#include <iostream>

struct Record_32 {
    int id;
    int value;
};

void process_record_32(Record_32* ptr) {
    // Defect: dereference without antecedent null-check guard
    ptr->id = 32;
    std::cout << "Record updated: " << ptr->id << std::endl;
}

int main() {
    process_record_32(nullptr);
    return 0;
}
