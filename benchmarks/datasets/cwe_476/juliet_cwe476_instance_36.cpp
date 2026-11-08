// NIST Juliet Test Suite v1.3 - CWE-476 Null Pointer Dereference (Instance 36)
#include <iostream>

struct Record_36 {
    int id;
    int value;
};

void process_record_36(Record_36* ptr) {
    // Defect: dereference without antecedent null-check guard
    ptr->id = 36;
    std::cout << "Record updated: " << ptr->id << std::endl;
}

int main() {
    process_record_36(nullptr);
    return 0;
}
