#include "../include/main.hpp"
#include "../include/vmcpu.hpp"

int main()
{
    std::cout << "***************" << std::endl;
    std::cout << "VMPROTECT v0.02" << std::endl;
    std::cout << "***************" << std::endl;

    VMCPU *vm = new VMCPU();
    
    std::string password;
    do {
        std::cout << "PASSWORD: ";
        std::cin >> password;
    } while(password.size() < 2 || password.size() > 26);

    BYTE *usrInput = new BYTE(password.size() + 1);
    try {
        memset(usrInput, 0, sizeof(usrInput)/sizeof(usrInput[0]));
        for(unsigned int i =0; i < password.size(); i++) {
            usrInput[i] = (BYTE) password[i];
        }
        usrInput[password.size()] = (BYTE) 0;
    } catch (...) {
        delete[] usrInput;
        delete vm;
        return -1;
    }

    BYTE *mc = NULL;
    int mcsize = loadProtectedCode(&mc);
    if(!vm->loadCode(mc, mcsize, usrInput, password.size()))
    {
        delete[] usrInput;
        delete[] mc;
        delete vm;
        return -1;
    }

    delete[] usrInput;
    delete[] mc;
    
    try{
        vm->run();
    } catch(...){
        delete vm;
        return -1;
    }

    delete vm;
    return 0;
}
