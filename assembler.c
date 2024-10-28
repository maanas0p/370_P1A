/**
 * Project 1
 * Assembler code fragment for LC-2K
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// Every LC2K file will contain less than 1000 lines of assembly.
#define MAXLINELENGTH 1000

int readAndParse(FILE *, char *, char *, char *, char *, char *);
static void checkForBlankLinesInCode(FILE *inFilePtr);
static inline int isNumber(char *);
static inline void printHexToFile(FILE *, int);

int main(int argc, char **argv)
{
    char *inFileString, *outFileString;
    FILE *inFilePtr, *outFilePtr;
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
        arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];
    int labelCount = 0;

    if (argc != 3)
    {
        printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
               argv[0]);
        exit(1);
    }

    inFileString = argv[1];
    outFileString = argv[2];

    inFilePtr = fopen(inFileString, "r");
    if (inFilePtr == NULL)
    {
        printf("error in opening %s\n", inFileString);
        exit(1);
    }

    // Check for blank lines in the middle of the code.
    checkForBlankLinesInCode(inFilePtr);

    // firstPass(intFileptr, labelist, &totalLabels);

    outFilePtr = fopen(outFileString, "w");
    if (outFilePtr == NULL)
    {
        printf("error in opening %s\n", outFileString);
        exit(1);
    }

    struct Label
    {
        char label[MAXLINELENGTH];
        int address;
    };

    struct Relocation
    {
        int addy;
        char instruction[10];
        char name[10];
    };

    int address = 0;
    struct Label labels[MAXLINELENGTH];
    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2) != 0)
    {
        if (strlen(label) > 0)
        {
            for (int i = 0; i < labelCount; ++i)
            {
                if (strcmp(labels[i].label, label) == 0)
                {
                    exit(1);
                }
            }
            strcpy(labels[labelCount].label, label);
            labels[labelCount].address = address;
            ++labelCount;
        }
        ++address;
    }

    /* this is how to rewind the file ptr so that you start reading from the
        beginning of the file */
    rewind(inFilePtr);
    int count = 0;
    while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2) != 0)
    {
        int regA = atoi(arg0);
        int regB = atoi(arg1);
        int machineCode = 0;
        if (strcmp(opcode, "add") == 0)
        {
            if (!isNumber(arg0) || !isNumber(arg1))
            {
                exit(1);
            }
            if (regA < 0 || regA > 7 || regB < 0 || regB > 7)
            {
                exit(1);
            }
            int destReg = atoi(arg2);
            if (destReg < 0 || destReg > 7)
            {
                exit(1);
            }
            machineCode = (0b000 << 22) | (regA << 19) | (regB << 16) | (destReg << 0);
            printHexToFile(outFilePtr, machineCode);
        }
        else if (strcmp(opcode, "nor") == 0)
        {
            if (!isNumber(arg0) || !isNumber(arg1))
            {
                exit(1);
            }
            if (regA < 0 || regA > 7 || regB < 0 || regB > 7)
            {
                exit(1);
            }
            int destReg = atoi(arg2);
            if (destReg < 0 || destReg > 7)
            {
                exit(1);
            }
            machineCode = (0b001 << 22) | (regA << 19) | (regB << 16) | (destReg << 0);
            printHexToFile(outFilePtr, machineCode);
        }
        else if (strcmp(opcode, "lw") == 0)
        {
            if (!isNumber(arg0) || !isNumber(arg1))
            {
                exit(1);
            }
            if (regA < 0 || regA > 7 || regB < 0 || regB > 7)
            {
                exit(1);
            }
            // int destReg = 0;
            int offset = 0;
            if (isNumber(arg2))
            {
                offset = atoi(arg2);
                if (offset < -32768 || offset > 32767)
                {
                    // offset out of range
                    exit(1);
                }
                machineCode = (0b010 << 22) | (regA << 19) | (regB << 16) | (offset & 0xFFFF);
            }
            else
            {
                bool found = false;
                for (int i = 0; i < labelCount; ++i)
                {
                    if (strcmp(labels[i].label, arg2) == 0)
                    {
                        offset = labels[i].address;
                        if (offset < -32768 || offset > 32767)
                        {
                            // offset out of range
                            exit(1);
                        }
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    // undefined label
                    exit(1);
                }
                // shud it be shifted 22 or 24?
                machineCode = (0b010 << 22) | (regA << 19) | (regB << 16) | (offset & 0xFFFF);
            }
            printHexToFile(outFilePtr, machineCode);
        }
        else if (strcmp(opcode, "sw") == 0)
        {
            if (!isNumber(arg0) || !isNumber(arg1))
            {
                exit(1);
            }
            if (regA < 0 || regA > 7 || regB < 0 || regB > 7)
            {
                exit(1);
            }
            int offset = 0;
            if (isNumber(arg2))
            {
                offset = atoi(arg2);
                if (offset < -32768 || offset > 32767)
                {
                    // offset out of range
                    exit(1);
                }
                machineCode = (0b011 << 22) | (regA << 19) | (regB << 16) | (offset & 0xFFFF);
            }
            else
            {
                bool found = false;
                for (int i = 0; i < labelCount; ++i)
                {
                    if (strcmp(labels[i].label, arg2) == 0)
                    {
                        offset = labels[i].address;
                        if (offset < -32768 || offset > 32767)
                        {
                            // offset out of range
                            exit(1);
                        }
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    // undefined label
                    exit(1);
                }
                // shud it be shifted 22 or 24?
                machineCode = (0b011 << 22) | (regA << 19) | (regB << 16) | (offset & 0xFFFF);
            }
            printHexToFile(outFilePtr, machineCode);
        }
        else if (strcmp(opcode, "beq") == 0)
        {
            if (!isNumber(arg0) || !isNumber(arg1))
            {
                exit(1);
            }
            if (regA < 0 || regA > 7 || regB < 0 || regB > 7)
            {
                exit(1);
            }
            int offset = 0;
            if (isNumber(arg2))
            {
                offset = atoi(arg2);
                if (offset < -32768 || offset > 32767)
                {
                    // offset out of range
                    exit(1);
                }
                machineCode = (0b100 << 22) | (regA << 19) | (regB << 16) | (offset & 0xFFFF);
            }
            else
            {
                bool found = false;
                for (int i = 0; i < labelCount; ++i)
                {
                    if (strcmp(labels[i].label, arg2) == 0)
                    {
                        // printf("%d\n", count);
                        // printf("%s\n", arg0);
                        // printf("%s\n", arg1);
                        offset = labels[i].address - count - 1;
                        if (offset < -32768 || offset > 32767)
                        {
                            // offset out of range
                            exit(1);
                        }
                        // printf("%d\n", offset);
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    exit(1);
                }
                // shud it be shifted 22 or 24?
                machineCode = (0b100 << 22) | (regA << 19) | (regB << 16) | (offset & 0xFFFF);
            }
            printHexToFile(outFilePtr, machineCode);
        }
        else if (strcmp(opcode, "jalr") == 0)
        {
            if (!isNumber(arg0) || !isNumber(arg1))
            {
                exit(1);
            }
            if (regA < 0 || regA > 7 || regB < 0 || regB > 7)
            {
                exit(1);
            }
            // int destReg = atoi(arg2);
            machineCode = (0b101 << 22) | (regA << 19) | (regB << 16);
            printHexToFile(outFilePtr, machineCode);
        }
        else if (strcmp(opcode, "halt") == 0)
        {
            machineCode = (0b110 << 22);
            printHexToFile(outFilePtr, machineCode);
        }
        else if (strcmp(opcode, "noop") == 0)
        {
            machineCode = (0b111 << 22);
            printHexToFile(outFilePtr, machineCode);
        }
        else if (strcmp(opcode, ".fill") == 0)
        {
            if (isNumber(arg0))
            {
                // printf("%s\n", arg0);
                machineCode = atoi(arg0);
            }
            else
            {
                bool found = false;
                for (int i = 0; i < labelCount; ++i)
                {
                    if (strcmp(labels[i].label, arg0) == 0)
                    {
                        machineCode = labels[i].address;
                        // printf("%d\n", machineCode);
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    // undefined label
                    exit(1);
                }
            }
            printHexToFile(outFilePtr, machineCode);
        }
        else
        {
            // unrecognized oppcode
            exit(1);
        }
        ++count;
    }

    // // Relocation table code here:
    // rewind(inFilePtr);
    // int relocationCount = 0;
    // while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2) != 0)
    // {
    //     if ((strcmp(opcode, "lw") == 0) || (strcmp(opcode, "sw") == 0))
    //     {
    //     }
    // }

    /* after doing a readAndParse, you may want to do the following to test the
        opcode */
    if (!strcmp(opcode, "add"))
    {
        /* do whatever you need to do for opcode "add" */
    }

    /* here is an example of using isNumber. "5" is a number, so this will
       return true */
    // if (isNumber("5"))
    // {
    //     printf("It's a number\n");
    // }

    /* here is an example of using printHexToFile. This will print a
       machine code word / number in the proper hex format to the output file */
    // printHexToFile(outFilePtr, 123);

    exit(0);
    return (0);
}
// Returns non-zero if the line contains only whitespace.
static int lineIsBlank(char *line)
{
    char whitespace[4] = {'\t', '\n', '\r', ' '};
    int nonempty_line = 0;
    for (int line_idx = 0; line_idx < strlen(line); ++line_idx)
    {
        int line_char_is_whitespace = 0;
        for (int whitespace_idx = 0; whitespace_idx < 4; ++whitespace_idx)
        {
            if (line[line_idx] == whitespace[whitespace_idx])
            {
                line_char_is_whitespace = 1;
                break;
            }
        }
        if (!line_char_is_whitespace)
        {
            nonempty_line = 1;
            break;
        }
    }
    return !nonempty_line;
}

// Exits 2 if file contains an empty line anywhere other than at the end of the file.
// Note calling this function rewinds inFilePtr.
static void checkForBlankLinesInCode(FILE *inFilePtr)
{
    char line[MAXLINELENGTH];
    int blank_line_encountered = 0;
    int address_of_blank_line = 0;
    rewind(inFilePtr);

    for (int address = 0; fgets(line, MAXLINELENGTH, inFilePtr) != NULL; ++address)
    {
        // Check for line too long
        if (strlen(line) >= MAXLINELENGTH - 1)
        {
            printf("error: line too long\n");
            exit(1);
        }

        // Check for blank line.
        if (lineIsBlank(line))
        {
            if (!blank_line_encountered)
            {
                blank_line_encountered = 1;
                address_of_blank_line = address;
            }
        }
        else
        {
            if (blank_line_encountered)
            {
                printf("Invalid Assembly: Empty line at address %d\n", address_of_blank_line);
                exit(2);
            }
        }
    }
    rewind(inFilePtr);
}

/*
 * NOTE: The code defined below is not to be modifed as it is implimented correctly.
 */

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
                 char *arg1, char *arg2)
{
    char line[MAXLINELENGTH];
    char *ptr = line;

    /* delete prior values */
    label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

    /* read the line from the assembly-language file */
    if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL)
    {
        /* reached end of file */
        return (0);
    }

    /* check for line too long */
    if (strlen(line) == MAXLINELENGTH - 1)
    {
        printf("error: line too long\n");
        exit(1);
    }

    // Ignore blank lines at the end of the file.
    if (lineIsBlank(line))
    {
        return 0;
    }

    /* is there a label? */
    ptr = line;
    if (sscanf(ptr, "%[^\t\n ]", label))
    {
        /* successfully read label; advance pointer over the label */
        ptr += strlen(label);
    }

    /*
     * Parse the rest of the line.  Would be nice to have real regular
     * expressions, but scanf will suffice.
     */
    sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]",
           opcode, arg0, arg1, arg2);

    return (1);
}

static inline int
isNumber(char *string)
{
    int num;
    char c;
    return ((sscanf(string, "%d%c", &num, &c)) == 1);
}

// Prints a machine code word in the proper hex format to the file
static inline void
printHexToFile(FILE *outFilePtr, int word)
{
    fprintf(outFilePtr, "0x%08X\n", word);
}
