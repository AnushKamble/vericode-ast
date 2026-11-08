// NIST Juliet Test Suite v1.3 - CWE-476 Null Pointer Dereference (Instance 16)
#include <iostream>

struct Record_16 {
    int id;
    int value;
};

void process_record_16(Record_16* ptr) {
    // Defect: dereference without antecedent null-check guard
    ptr->id = 16;
    std::cout << "Record updated: " << ptr->id << std::endl;
}

int main() {
    process_record_16(nullptr);
    return 0;
}
