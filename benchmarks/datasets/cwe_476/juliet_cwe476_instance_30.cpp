// NIST Juliet Test Suite v1.3 - CWE-476 Null Pointer Dereference (Instance 30)
#include <iostream>

struct Record_30 {
    int id;
    int value;
};

void process_record_30(Record_30* ptr) {
    // Defect: dereference without antecedent null-check guard
    ptr->id = 30;
    std::cout << "Record updated: " << ptr->id << std::endl;
}

int main() {
    process_record_30(nullptr);
    return 0;
}
