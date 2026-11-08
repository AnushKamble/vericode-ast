// NIST Juliet Test Suite v1.3 - CWE-476 Null Pointer Dereference (Instance 17)
#include <iostream>

struct Record_17 {
    int id;
    int value;
};

void process_record_17(Record_17* ptr) {
    // Defect: dereference without antecedent null-check guard
    ptr->id = 17;
    std::cout << "Record updated: " << ptr->id << std::endl;
}

int main() {
    process_record_17(nullptr);
    return 0;
}
