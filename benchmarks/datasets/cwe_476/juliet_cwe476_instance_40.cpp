// NIST Juliet Test Suite v1.3 - CWE-476 Null Pointer Dereference (Instance 40)
#include <iostream>

struct Record_40 {
    int id;
    int value;
};

void process_record_40(Record_40* ptr) {
    // Defect: dereference without antecedent null-check guard
    ptr->id = 40;
    std::cout << "Record updated: " << ptr->id << std::endl;
}

int main() {
    process_record_40(nullptr);
    return 0;
}
