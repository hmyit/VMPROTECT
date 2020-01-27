#include "../include/main.hpp"
#include "../include/vmcpu.hpp"

int main()
{
    std::cout << "***************" << std::endl;
    std::cout << "VMPROTECT v0.01" << std::endl;
    std::cout << "***************" << std::endl;

    VMCPU *vm = new VMCPU();
    
    std::string password;
    do {
        std::cout << "PASSWORD: ";
        std::cin >> password;
    } while(password.size() < 2 || password.size() > 26);

    BYTE *usrInput = new BYTE(password.size() + 1);
    memset(usrInput, 0, sizeof(usrInput)/sizeof(usrInput[0]));
    for(unsigned int i =0; i < password.size(); i++) {
        usrInput[i] = (BYTE) password[i];
    }
    usrInput[password.size()] = (BYTE) 0;

    BYTE *mc = NULL;
    int mcsize = loadProtectedCode(&mc);
    if(!vm->loadCode(mc, mcsize, usrInput, password.size()))
    {
        delete[] usrInput;
        delete[] mc;
        return -1;
    }
    delete[] usrInput;
    delete[] mc;
    vm->run();
    delete vm;

    return 0;
}