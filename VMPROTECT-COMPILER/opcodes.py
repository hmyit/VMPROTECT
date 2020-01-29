class ChangeReferenceError(Exception):
    def __init__(self, codeLine, codeScrap):
        self.codeLine = codeLine
        self.codeScrap = codeScrap

    def __str__(self):
        e = "[ERROR] Unknow instruction at index: " + str(self.codeLine) + " ( " + self.codeScrap + " ... )" + "! \nCompilation failed!"
        return e

class CompileError(Exception):
    def __init__(self, codeLine, codeScrap):
        self.codeLine = codeLine
        self.codeScrap = codeScrap

    def __str__(self):
        e = "[ERROR] Unknow instruction at line: " + str(self.codeLine) + " ( " + self.codeScrap + " ... )" + "! \nCompilation failed!"
        return e

class ArgsError(Exception):
    def __init__(self):
        pass
    def __str__(self):
        e = "[ERROR] Too few arguments!"
        return e

class NoElemInDictionaryError(Exception):
    def __init__(self, codeLine, codeScrap):
        self.codeLine = codeLine
        self.codeScrap = codeScrap

    def __str__(self):
        e = "[ERROR] Unknow instruction at line: " + str(self.codeLine) + " ( " + self.codeScrap + " ... )" + "! \nCompilation failed!"
        return e

dOPCODES = {
    # NO PARAM
    "NOP":"0x00",
    "NOPV":"0x01",
    "EC":"0xEC",
    "POC":"0x51",
    "POCN":"0x52",
    
    # 1 PARAM
    "PUSH":"0x61",
    "POP":"0x62",

    # 2 PARAM
    "MOV":"0x02",
    "JMP":"0x11",
    "JZ":"0x12",
    "JNZ":"0x13",
    "JAE":"0x14",
    "JBE":"0x15",
    "JB":"0x16",
    "JA":"0x17",

    # 3 PARAM
    "MOVB":"0x05",
    "MOVMRB":"0x09",
    "MOVMRW":"0x0A",
    "ADRR":"0x22",
    "ADRRL":"0x23",
    "SUBRR":"0x25",
    "SUBRRL":"0x26",
    "XOR":"0x27",
    "XORL":"0x28",
    "CMP":"0x31",
    "CMPL":"0x32",

    # 4 PARAM
    "MOVMB":"0x03",
    "MOVMW":"0x04",
    "MOVW":"0x06",
    "MOVBM":"0x07",
    "MOVWM":"0x08",
    "ADVR":"0x21",
    "SUBVR":"0x24"
}

dREGS = {
    "R0":"0x00",
    "R1":"0x01",
    "R2":"0x02",
    "R3":"0x03",
    "R4":"0x04",
    "R5":"0x05"
}