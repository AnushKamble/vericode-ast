// NIST Juliet Test Suite v1.3 - CWE-476 Null Pointer Dereference (Instance 28)
#include <iostream>

struct Record_28 {
    int id;
    int value;
};

void process_record_28(Record_28* ptr) {
    // Defect: dereference without antecedent null-check guard
    ptr->id = 28;
    std::cout << "Record updated: " << ptr->id << std::endl;
}

int main() {
    process_record_28(nullptr);
    return 0;
}
