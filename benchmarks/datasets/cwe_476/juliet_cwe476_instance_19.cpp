// NIST Juliet Test Suite v1.3 - CWE-476 Null Pointer Dereference (Instance 19)
#include <iostream>

struct Record_19 {
    int id;
    int value;
};

void process_record_19(Record_19* ptr) {
    // Defect: dereference without antecedent null-check guard
    ptr->id = 19;
    std::cout << "Record updated: " << ptr->id << std::endl;
}

int main() {
    process_record_19(nullptr);
    return 0;
}
