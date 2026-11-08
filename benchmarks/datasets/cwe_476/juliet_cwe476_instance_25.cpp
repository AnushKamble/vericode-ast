// NIST Juliet Test Suite v1.3 - CWE-476 Null Pointer Dereference (Instance 25)
#include <iostream>

struct Record_25 {
    int id;
    int value;
};

void process_record_25(Record_25* ptr) {
    // Defect: dereference without antecedent null-check guard
    ptr->id = 25;
    std::cout << "Record updated: " << ptr->id << std::endl;
}

int main() {
    process_record_25(nullptr);
    return 0;
}
