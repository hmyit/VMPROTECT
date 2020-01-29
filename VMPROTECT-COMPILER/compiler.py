import sys

dFoundedReferenceLabels = {} # {label: [str position]}
dFoundedLabels = {} # {label: code position}

def changeLabelToAddr(outFile):
    for k in dFoundedReferenceLabels.keys():
        for l in dFoundedReferenceLabels[k]:
            tmp1 = outFile[:l]
            tmp2 = outFile[l + 12:]
            val = dFoundedLabels[k]

            sVal = '{0:02X}'.format(val)

            if len(sVal) == 2:
                tmp1 += "0x" + sVal + ", 0x00, "
                outFile = tmp1 + tmp2
            elif len(sVal) == 3:
                tmp1 += "0x" + sVal[1:] + ", 0x0" + sVal[:1] + ", "
                outFile = tmp1 + tmp2
            elif len(sVal) == 4:
                t1 = '{0:02X}'.format(val)[:2]
                t2 = '{0:02X}'.format(val)[2:]
                tmp1 += "0x" + t2 + ", 0x" + t1 + ", "
                outFile = tmp1 + tmp2
            else:
                errorC(l)
    return outFile

def errorC(codeLine):
    print("[ERROR] Unknow instruction at line: " + str(codeLine) + "!")
    print("[ERROR] Compilation failed!")
    exit()

def main():
    if len(sys.argv) != 2:
        print("[ERROR] Too few arguments!")
    fileNameToCompile = sys.argv[1]
    outFile = ""
    outFileInstIndexNumber = 0
    outFileInstCodeOffset = 0
    with open(fileNameToCompile)  as fd:
        line = fd.readline()
        while line:
            # Check if comment
            if line.find("#") != -1:
                pass
            # Check if DATA LABEL
            elif line.find("DL:") != -1:
                label = line.split(":")[0]
                line = fd.readline()
                text = line.split(",")[0]
                dFoundedLabels[label] = outFileInstCodeOffset
                for i in range(text.find("\"") + 1, len(text) - 1):
                    outFile += hex(ord(text[i]))
                    outFile += ", "
                    outFileInstIndexNumber += 6
                    outFileInstCodeOffset += 1
                outFile += "0x00, "
                outFileInstIndexNumber += 6
            # Check if it is reference to DATA LABEL
            elif line.find("DL@") != -1:
                if len(line.split()) == 2:
                    pass
                elif len(line.split()) == 3:
                    outFileInstCodeOffset += 4
                    opc = line.split()[0]
                    p1 = (line.split()[1]).split(',')[0]
                    p2 = line.split()[2]
                    outFile += dOPCODES[opc]
                    outFile += ", "
                    outFileInstIndexNumber += 6
                    outFile += dREGS[p1]
                    outFile += ", "
                    outFileInstIndexNumber += 6
                    if p2[:-1] in dFoundedReferenceLabels.keys():
                        dFoundedReferenceLabels[p2[:-1]].append(outFileInstIndexNumber)
                    else:
                        dFoundedReferenceLabels[p2[:-1]] = [outFileInstIndexNumber]
                    outFile += "0xFF, 0xFF"
                    outFile += ", "
                    outFileInstIndexNumber += 12
                else:
                    errorC(outFileInstIndexNumber)
            # Check if JUMP LOCATION LABEL
            elif line.find("JLL:") != -1:
                label = line.split(":")[0]
                dFoundedLabels[label] = outFileInstCodeOffset
            # Check if it is reference to JUMP LOCATION LABEL
            elif line.find("JLL@") != -1:
                if len(line.split()) == 2:
                    outFileInstCodeOffset += 3
                    opc = line.split()[0]
                    arg = line.split()[1]
                    outFile += dOPCODES[opc]
                    outFile += ", "
                    outFileInstIndexNumber += 6
                    if arg[:-1] in dFoundedReferenceLabels.keys():
                        dFoundedReferenceLabels[arg[:-1]].append(outFileInstIndexNumber)
                    else:
                        dFoundedReferenceLabels[arg[:-1]] = [outFileInstIndexNumber]
                    outFile += "0xFF, 0xFF"
                    outFile += ", "
                    outFileInstIndexNumber += 12
                else:
                    errorC(outFileInstIndexNumber)
            else:
            # Parse code
                # NO PARAM
                if len(line.split()) == 1:
                    outFileInstCodeOffset += 1
                    outFile += dOPCODES[line.split()[0]]
                    outFile += ", "
                    outFileInstIndexNumber += 6
                # white space
                elif len(line.split()) == 0:
                    pass
                # 1 PARAM
                elif len(line.split()) == 2:
                    outFileInstCodeOffset += 2
                    opc = line.split()[0]
                    param = line.split()[1]
                    outFile += dOPCODES[opc]
                    outFile += ", "
                    outFileInstIndexNumber += 6
                    outFile += dREGS[param]
                    outFile += ", "
                    outFileInstIndexNumber += 6
                # 2 PARAM
                elif len(line.split()) == 3:
                    opc = line.split()[0]
                    p1 = (line.split()[1]).split(',')[0]
                    p2 = line.split()[2]
                    outFile += dOPCODES[opc]
                    outFile += ", "
                    outFileInstIndexNumber += 6
                    if opc == "MOV":
                        param1 = dREGS[p1][3:]
                        param2 = dREGS[p2][3:]
                        outFile += "0x" + param1 + param2
                        outFile += ", "
                        outFileInstIndexNumber += 6
                        outFileInstCodeOffset += 2
                    elif opc == "MOVMRB" or opc == "MOVMRW" or opc == "ADRR" or opc == "ADRRL" or opc == "SUBRR" or opc == "SUBRRL" or opc == "XOR" or opc == "XORL" or opc == "CMP" or opc == "CMPL":
                        param1 = dREGS[p1]
                        param2 = dREGS[p2]
                        outFile += param1
                        outFile += ", "
                        outFileInstIndexNumber += 6
                        outFile += param2
                        outFile += ", "
                        outFileInstIndexNumber += 6
                        outFileInstCodeOffset += 3
                    elif opc == "MOVMB" or opc == "MOVMW":
                        param1 = dREGS[p1]
                        outFile += param1
                        outFile += ", "
                        outFileInstIndexNumber += 6
                        param2 = "0x" + p2[2:] + ", 0x" + p2[:2]
                        outFile += param2
                        outFile += ", "
                        outFileInstIndexNumber += 10
                        outFileInstCodeOffset += 4
                    elif opc == "MOVBM" or opc == "MOVWM":
                        param1 = dREGS[p1]
                        outFile += param1
                        outFile += ", "
                        outFileInstIndexNumber += 6
                        param2 = "0x" + p2[2:] + ", 0x" + p2[:2]
                        outFile += ", "
                        outFileInstIndexNumber += 12
                        outFileInstCodeOffset += 3
                    elif opc == "MOVB":       
                        outFile += dREGS[p1]
                        outFile += ", "
                        outFileInstIndexNumber += 6
                        outFile += "0x" + p2
                        outFile += ", "
                        outFileInstIndexNumber += 6
                        outFileInstCodeOffset += 3
                    elif opc == "MOVW":
                        outFile += dREGS[p1]
                        outFile += ", "
                        outFileInstIndexNumber += 6
                        outFile += "0x" + p2[2:]
                        outFile += ", "
                        outFileInstIndexNumber += 6
                        outFile += "0x" + p2[:-2]
                        outFile += ", "
                        outFileInstIndexNumber += 6
                        outFileInstCodeOffset += 4
                    elif opc == "JMP" or opc == "JZ" or opc == "JNZ" or opc == "JAE" or opc == "JBE" or opc == "JB" or opc == "JA":
                        outFile += "0x" + p2
                        outFile += ", "
                        outFileInstIndexNumber += 6
                        outFile += "0x" + p1
                        outFile += ", "
                        outFileInstIndexNumber += 6
                        outFileInstCodeOffset += 3
                    elif opc == "ADVR" or opc == "SUBVR":
                        param1 = dREGS[p1]
                        outFile += param1
                        outFile += ", "
                        outFileInstIndexNumber += 6
                        param2 = "0x" + p2[2:] + ", 0x" + p2[:2]
                        outFile += param2
                        outFile += ", "
                        outFileInstIndexNumber += 12
                        outFileInstCodeOffset += 4
                # 3 PARAM
                elif len(line.split()) == 4:
                    # TODO
                    pass
                else:
                    errorC(outFileInstIndexNumber)
            line = fd.readline()
    if bool(dFoundedReferenceLabels):
        outFile = changeLabelToAddr(outFile)
    outFD = open((sys.argv[1].split("."))[0] + ".vex", "w")
    outF = outFile[:-2]
    count = 0
    outFD.write(outF)
    # for c in outF:
    #     outFD.write(c)
    #     count += 1
        # if count >= 48:
        #     if(c == " "):
        #         count = 0
        #         outFD.write("\n")
        #     else:
        #         outFD.write(c)
    outFD.close()

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

if __name__ == "__main__":
    main()