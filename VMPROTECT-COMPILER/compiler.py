import sys

from opcodes import *

dFoundedReferenceLabels = {} # {label: [str position]}
dFoundedLabels = {} # {label: code position}

def changeReferenceToLabelToAddr(outFile):
    global dFoundedReferenceLabels
    global dFoundedLabels
    for k in dFoundedReferenceLabels.keys():
        for codeIndex in dFoundedReferenceLabels[k]:
            try:
                tmp1 = outFile[:codeIndex]
                tmp2 = outFile[codeIndex + 12:]
                val = dFoundedLabels[k]
                hexVal = '{0:02X}'.format(val)

                if len(hexVal) == 2:
                    tmp1 += "0x" + hexVal + ", 0x00, "
                    outFile = tmp1 + tmp2
                elif len(hexVal) == 3:
                    tmp1 += "0x" + hexVal[1:] + ", 0x0" + hexVal[:1] + ", "
                    outFile = tmp1 + tmp2
                elif len(hexVal) == 4:
                    t1 = '{0:02X}'.format(val)[:2]
                    t2 = '{0:02X}'.format(val)[2:]
                    tmp1 += "0x" + t2 + ", 0x" + t1 + ", "
                    outFile = tmp1 + tmp2
                else:
                    raise ChangeReferenceError(codeIndex, outFile[-4:tmp1])
            except ChangeReferenceError as e:
                print(e)
                exit()
    return outFile

def checkElemInDictionary(keyToCheck, dToCheck, codeLine, codeScrap):
    try:
        if keyToCheck not in dToCheck:
            raise NoElemInDictionaryError(codeLine, codeScrap)
    except NoElemInDictionaryError as e:
        print(e)
        exit()

def main():
    global dFoundedReferenceLabels
    global dFoundedLabels
    global dOPCODES
    global dREGS
    try:
        if len(sys.argv) != 2:
            raise ArgsError()
        fileNameToCompile = sys.argv[1]
        outFile = ""
        outFileInstIndexNumber = 0
        outFileInstCodeOffset = 0
        countCodeLines = 0

        with open(fileNameToCompile)  as fd:
            line = fd.readline()
            countCodeLines += 1
            while line:
                # Check if comment
                if line.find("#") != -1:
                    pass
                # Check if DATA LABEL
                elif line.find("DL:") != -1:
                    label = line.split(":")[0]
                    line = fd.readline()
                    countCodeLines += 1
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
                    try:
                        if len(line.split()) == 2:
                            pass
                        elif len(line.split()) == 3:
                            outFileInstCodeOffset += 4
                            opc = line.split()[0]
                            p1 = (line.split()[1]).split(',')[0]
                            p2 = line.split()[2]
                            checkElemInDictionary(opc, dOPCODES, countCodeLines, outFile[-4:])
                            outFile += dOPCODES[opc]
                            outFile += ", "
                            outFileInstIndexNumber += 6
                            checkElemInDictionary(p1, dREGS, countCodeLines, outFile[-4:])
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
                            raise CompileError(countCodeLines, codeScrap)
                    except CompileError as e:
                        print(e)
                        exit()
                # Check if JUMP LOCATION LABEL
                elif line.find("JLL:") != -1:
                    label = line.split(":")[0]
                    dFoundedLabels[label] = outFileInstCodeOffset
                # Check if it is reference to JUMP LOCATION LABEL
                elif line.find("JLL@") != -1:
                    try:
                        if len(line.split()) == 2:
                            outFileInstCodeOffset += 3
                            opc = line.split()[0]
                            arg = line.split()[1]
                            checkElemInDictionary(opc, dOPCODES, countCodeLines, outFile[-4:])
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
                            raise CompileError(outFileInstIndexNumber, codeScrap)
                    except CompileError as e:
                        print(e)
                        exit()
                else:
                # Parse code
                    try:
                        # NO PARAM
                        if len(line.split()) == 1:
                            outFileInstCodeOffset += 1
                            checkElemInDictionary(line.split()[0], dOPCODES, countCodeLines, outFile[-4:])
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
                            checkElemInDictionary(opc, dOPCODES, countCodeLines, outFile[-4:])
                            outFile += dOPCODES[opc]
                            outFile += ", "
                            outFileInstIndexNumber += 6
                            checkElemInDictionary(param, dREGS, countCodeLines, outFile[-4:])
                            outFile += dREGS[param]
                            outFile += ", "
                            outFileInstIndexNumber += 6
                        # 2 PARAM
                        elif len(line.split()) == 3:
                            opc = line.split()[0]
                            p1 = (line.split()[1]).split(',')[0]
                            p2 = line.split()[2]
                            checkElemInDictionary(opc, dOPCODES, countCodeLines, outFile[-4:])
                            outFile += dOPCODES[opc]
                            outFile += ", "
                            outFileInstIndexNumber += 6
                            if opc == "MOV":
                                checkElemInDictionary(p1, dREGS, countCodeLines, outFile[-4:])
                                checkElemInDictionary(p2, dREGS, countCodeLines, outFile[-4:])
                                param1 = dREGS[p1][3:]
                                param2 = dREGS[p2][3:]
                                outFile += "0x" + param1 + param2
                                outFile += ", "
                                outFileInstIndexNumber += 6
                                outFileInstCodeOffset += 2
                            elif opc == "MOVMRB" or opc == "MOVMRW" or opc == "ADRR" or opc == "ADRRL" or opc == "SUBRR" or opc == "SUBRRL" or opc == "XOR" or opc == "XORL" or opc == "CMP" or opc == "CMPL":
                                checkElemInDictionary(p1, dREGS, countCodeLines, outFile[-4:])
                                checkElemInDictionary(p2, dREGS, countCodeLines, outFile[-4:])
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
                                checkElemInDictionary(p1, dREGS, countCodeLines, outFile[-4:])
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
                                checkElemInDictionary(p1, dREGS, countCodeLines,outFile[-4:])
                                param1 = dREGS[p1]
                                outFile += param1
                                outFile += ", "
                                outFileInstIndexNumber += 6
                                param2 = "0x" + p2[2:] + ", 0x" + p2[:2]
                                outFile += ", "
                                outFileInstIndexNumber += 12
                                outFileInstCodeOffset += 3
                            elif opc == "MOVB":       
                                checkElemInDictionary(p1, dREGS, countCodeLines, outFile[-4:])
                                outFile += dREGS[p1]
                                outFile += ", "
                                outFileInstIndexNumber += 6
                                outFile += "0x" + p2
                                outFile += ", "
                                outFileInstIndexNumber += 6
                                outFileInstCodeOffset += 3
                            elif opc == "MOVW":
                                checkElemInDictionary(p1, dREGS, countCodeLines, outFile[-4:])
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
                                checkElemInDictionary(p1, dREGS, countCodeLines, outFile[-4:])
                                param1 = dREGS[p1]
                                outFile += param1
                                outFile += ", "
                                outFileInstIndexNumber += 6
                                param2 = "0x" + p2[2:] + ", 0x" + p2[:2]
                                outFile += param2
                                outFile += ", "
                                outFileInstIndexNumber += 12
                                outFileInstCodeOffset += 4
                        else:
                            raise CompileError(countCodeLines, outFile[-4:])
                    except CompileError as e:
                        print(e)
                        exit()
                line = fd.readline()
                countCodeLines += 1
        if bool(dFoundedReferenceLabels):
            outFile = changeReferenceToLabelToAddr(outFile)
        outFD = open((sys.argv[1].split("."))[0] + ".vex", "w")
        outF = outFile[:-2]
        count = 0
        outFD.write(outF)
        outFD.close()

    except CompileError as e:
                print(e)
                exit()

if __name__ == "__main__":
    main()