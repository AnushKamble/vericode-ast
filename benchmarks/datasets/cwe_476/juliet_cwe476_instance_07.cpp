// NIST Juliet Test Suite v1.3 - CWE-476 Null Pointer Dereference (Instance 07)
#include <iostream>

struct Record_07 {
    int id;
    int value;
};

void process_record_07(Record_07* ptr) {
    // Defect: dereference without antecedent null-check guard
    ptr->id = 7;
    std::cout << "Record updated: " << ptr->id << std::endl;
}

int main() {
    process_record_07(nullptr);
    return 0;
}
