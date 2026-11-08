// NIST Juliet Test Suite v1.3 - CWE-476 Null Pointer Dereference (Instance 10)
#include <iostream>

struct Record_10 {
    int id;
    int value;
};

void process_record_10(Record_10* ptr) {
    // Defect: dereference without antecedent null-check guard
    ptr->id = 10;
    std::cout << "Record updated: " << ptr->id << std::endl;
}

int main() {
    process_record_10(nullptr);
    return 0;
}
