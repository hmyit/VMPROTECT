import sys

dFoundedReferenceLabels = {} # {label: str position}
dFoundedLabels = {} # {label: code position}

def changeLabelToAddr(outFile):
    for k in dFoundedReferenceLabels.keys():
        tmp1 = outFile[:dFoundedReferenceLabels[k]]
        tmp2 = outFile[dFoundedReferenceLabels[k] + 12:]
        val = dFoundedLabels[k]

        if val < 10:
            tmp1 += "0x0" + hex(val)[2:] + ", 0x00, "
            outFile = tmp1 + tmp2
        elif val < 16:
            tmp1 += "0x0" + hex(val)[2:] + ", 0x00, "
            outFile = tmp1 + tmp2
        elif val < 100:
            tmp1 += hex(val) + ", 0x00, "
            outFile = tmp1 + tmp2
        elif val < 1000:
            t1 = hex(val)[2:3]
            t2 = hex(val)[3:5]
            tmp1 += "0x" + t2 + ", 0x0" + t1 + ", "
            outFile = tmp1 + tmp2
        else:
            t1 = hex(val)[2:4]
            t2 = hex(val)[4:6]
            tmp1 += "0x" + t2 + ", 0x" + t1 + ", "
            outFile = tmp1 + tmp2
    return outFile

def errorC():
    print("[ERROR] Unknow instruction!")
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
                    outFile += str(hex(ord(text[i])))
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
                    dFoundedReferenceLabels[p2[:-1]] = outFileInstIndexNumber
                    outFile += "0xFF, 0xFF"
                    outFile += ", "
                    outFileInstIndexNumber += 12
                else:
                    errorC()
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
                    dFoundedReferenceLabels[arg[:-1]] = outFileInstIndexNumber
                    outFile += "0xFF, 0xFF"
                    outFile += ", "
                    outFileInstIndexNumber += 12
                else:
                    errorC()
            else:
            # Parse code
                # 1 byte instruction
                if len(line.split()) == 1:
                    outFileInstCodeOffset += 1
                    outFile += dOPCODES[line.split()[0]]
                    outFile += ", "
                    outFileInstIndexNumber += 6
                # white space
                elif len(line.split()) == 0:
                    pass
                # 2 byte instruction
                elif len(line.split()) == 2:
                    # TODO
                    outFileInstCodeOffset += 2
                    pass
                # 3 byte instruction
                elif len(line.split()) == 3:
                    outFileInstCodeOffset += 3
                    opc = line.split()[0]
                    p1 = (line.split()[1]).split(',')[0]
                    p2 = line.split()[2]
                    outFile += dOPCODES[opc]
                    outFile += ", "
                    outFileInstIndexNumber += 6
                    if opc == "MOVB":       
                        outFile += dREGS[p1]
                        outFile += ", "
                        outFileInstIndexNumber += 6
                        outFile += "0x" + p2
                        outFile += ", "
                        outFileInstIndexNumber += 6
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
                # 4 byte instruction
                elif len(line.split()) == 4:
                    # TODO
                    outFileInstCodeOffset += 4
                    pass
                else:
                    errorC()
            line = fd.readline()
    if bool(dFoundedReferenceLabels):
        outFile = changeLabelToAddr(outFile)
    outFD = open((sys.argv[1].split("."))[0] + ".vex", "w")
    outFD.write(outFile[:-2])
    outFD.close()

dOPCODES = {
    # NO PARAM
    "NOP":"0x00",
    "NOPV":"0x01",
    "EC":"0xEC",
    "POC":"0x51",
    "POCN":"0x52",
    
    # 2 PARAM
    "MOV":"0x02",

    # 3 PARAM
    "MOVB":"0x05",
    "MOVMRB":"0x09",
    "MOVMRW":"0x0A",

    # 4 PARAM
    "MOVMB":"0x03",
    "MOVMW":"0x04",
    "MOVW":"0x06",
    "MOVBM":"0x07",
    "MOVWM":"0x08",
    
    # 1 PARAM
    "JMP":"0x11",
    "JZ":"0x12",
    "JNZ":"0x13",
    "JAE":"0x14",
    "JBE":"0x15",
    "JB":"0x16",
    "JA":"0x17",
    "PUSH":"0x61",
    "POP":"0x62"
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