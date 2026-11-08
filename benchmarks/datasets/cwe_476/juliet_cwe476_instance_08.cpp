// NIST Juliet Test Suite v1.3 - CWE-476 Null Pointer Dereference (Instance 08)
#include <iostream>

struct Record_08 {
    int id;
    int value;
};

void process_record_08(Record_08* ptr) {
    // Defect: dereference without antecedent null-check guard
    ptr->id = 8;
    std::cout << "Record updated: " << ptr->id << std::endl;
}

int main() {
    process_record_08(nullptr);
    return 0;
}
