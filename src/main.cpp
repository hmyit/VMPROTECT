#include "../include/main.hpp"
#include "../include/vmcpu.hpp"

int main()
{
    std::cout << "***************" << std::endl;
    std::cout << "VMPROTECT v0.01" << std::endl;
    std::cout << "***************" << std::endl;

    VMCPU *vm = new VMCPU();

    BYTE *code = loadProtectedCode();
    vm->loadCode(code);
    deleteProtectedCode(code);
    vm->run();
    delete vm;

    return 0;
}