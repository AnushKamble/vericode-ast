// NIST Juliet Test Suite v1.3 - CWE-476 Null Pointer Dereference (Instance 18)
#include <iostream>

struct Record_18 {
    int id;
    int value;
};

void process_record_18(Record_18* ptr) {
    // Defect: dereference without antecedent null-check guard
    ptr->id = 18;
    std::cout << "Record updated: " << ptr->id << std::endl;
}

int main() {
    process_record_18(nullptr);
    return 0;
}
