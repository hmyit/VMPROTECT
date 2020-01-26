import sys

dOPCODES = {
    "NOP":"0x00",
    "NOPV":"0x01",
    "EC":"0xEC"
}

dREGS = {
    "R0":"0x00",
    "R1":"0x01",
    "R2":"0x02",
    "R3":"0x03"
}

def main():
    if len(sys.argv) != 2:
        print("[ERROR] Too few arguments!")
    fileNameToCompile = sys.argv[1]
    indexNumber = 0
    outFile = ""
    outFileInstIndexNumber = 0
    with open(fileNameToCompile)  as fd:
        line = fd.readline()
        indexNumber += 1
        while line:
            # Check if label
            if line.find(":") != -1:
                line = fd.readline()
                indexNumber += 1
                text = line.split(",")[0]
                for i in range(text.find("\"") + 1, len(text) - 1):
                    outFile += str(hex(ord(text[i])))
                    outFile += ", "
                    outFileInstIndexNumber += 6
                outFile += "0x00, "
                outFileInstIndexNumber += 6
            # Check if it is reference to label
            elif line.find("@") != -1:
                pass
            else:
                if len(line.split()) == 1:
                    outFile += dOPCODES[line.split()[0]]
                    outFile += ", "
                    outFileInstIndexNumber += 6
                elif len(line.split()) == 2:
                    pass
                elif len(line.split()) == 0:
                    pass
                else:
                    print("[ERROR] Unknow instruction at " + str(indexNumber) + "!")
                    print("[ERROR] Compilation failed!")
                    exit()
            line = fd.readline()
            indexNumber += 1
    outFD = open((sys.argv[1].split("."))[0] + ".vex", "w")
    outFD.write(outFile)
    outFD.close()

if __name__ == "__main__":
    main()