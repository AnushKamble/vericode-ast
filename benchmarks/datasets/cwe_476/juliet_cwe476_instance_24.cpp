// NIST Juliet Test Suite v1.3 - CWE-476 Null Pointer Dereference (Instance 24)
#include <iostream>

struct Record_24 {
    int id;
    int value;
};

void process_record_24(Record_24* ptr) {
    // Defect: dereference without antecedent null-check guard
    ptr->id = 24;
    std::cout << "Record updated: " << ptr->id << std::endl;
}

int main() {
    process_record_24(nullptr);
    return 0;
}
