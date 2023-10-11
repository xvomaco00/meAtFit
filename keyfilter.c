/**
 * @file keyfilter.c
 *
 * @brief Implementation of virtual keyboard filtering in car dashboard navigation.
 *
 * @author Ondřej Vomáčka (xvomaco00)
 *
 * @date 10/10/2023
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define LINE_BUFFER_LENGTH 128
#define MAX_LINE_LENGTH 100
#define ASCII_LENGTH 128

bool matches(const char* userInput, const char* storedAddress);
int parse_address(int lineIndex, const char* storedAddress);
int parse_args(int argc, char *argv[]);
void to_upper(char *strToUpper);
void trim(char *strToTrim);
void sort(char *strToSort);

/**
 * @brief Trims the whitespace at the beginning and end of string.
 *
 * @note Usage not necessary, but can prevent some parsing problems.
 *
 * @param strToTrim The string to be trimmed.
 */
void trim(char *strToTrim) {

    if (strToTrim == NULL) {

        return;
    }

    unsigned len = strlen(strToTrim);

    if (len == 0) {

        return;
    }

    unsigned actualStart = 0;
    unsigned actualEnd = len - 1;

    while (actualStart <= actualEnd && isspace(strToTrim[actualStart])) {

        actualStart++;
    }

    if (actualStart == actualEnd) {

        strToTrim[0] = '\0';
        return;
    }

    while (actualEnd >= actualStart && isspace(strToTrim[actualEnd])) {

        actualEnd--;
    }

    int offset = 0;
    while (actualStart + offset <= actualEnd) {

        strToTrim[offset] = strToTrim[actualStart + offset];
        offset++;
    }

    strToTrim[offset] = '\0';
}

/**
 * @brief Converts string characters to uppercase.
 *
 * @param strToUpper The string to be converted.
 */
void to_upper(char *strToUpper) {

    unsigned len = strlen(strToUpper);

    for (unsigned charIndex = 0; charIndex < len; ++charIndex) {

        strToUpper[charIndex] = (char)toupper(strToUpper[charIndex]);
    }
}

/**
 * @brief Check whether the arguments are valid.
 *
 * @param argc The number of arguments.
 * @param argv The array of arguments.
 *
 * @return 0, if the arguments are valid.
 * @return 1, if no arguments are provided.
 * @return 2, if too many arguments are provided.
 * @return 3, if the address exceeds 100 characters.
 */
int parse_args(int argc, char **argv) {

    if (argc == 0) {

        fprintf(stderr, "ERROR: unexpected arguments provided.\n");

        return 1;
    }

    if (argc > 2) {

        fprintf(stderr, "ERROR: too many arguments provided.\n");
        fprintf(stderr, "Usage: %s [input] <text-file\n", argv[0]);

        return 2;
    }

    if (argc == 2 && strlen(argv[1]) > MAX_LINE_LENGTH) {

        fprintf(stderr, "ERROR: invalid arguments provided.\n");
        fprintf(stderr, "The input can not exceed 100 characters.\n");

        return 3;
    }

    return 0;
}

/**
 * @brief Check whether the stored address is valid.
 *
 * @param lineIndex The index of the line where the address is stored in the file.
 *                  Used purely for error printing.
 * @param storedAddress The stored address parsed from the file.
 *
 * @return 0, if the address is valid.
 * @return 4, if the address is empty.
 * @return 5, if the address exceeds 100 characters.
 */
int parse_address(int lineIndex, const char* storedAddress) {

    unsigned lineLength = strlen(storedAddress);

    if (lineLength == 0) {

        fprintf(stderr, "ERROR: invalid data found on line number %d.\n", lineIndex);
        fprintf(stderr, "The line does not contain any characters.\n");

        return 4;
    }

    if (lineLength > MAX_LINE_LENGTH) {

        fprintf(stderr, "ERROR: invalid data found on line number %d.\n", lineIndex);
        fprintf(stderr, "A line can contain a maximum of 100 characters.\n");

        return 5;
    }

    return 0;
}

/**
 * @brief Checks whether the first characters of userInput match the first
 *        characters of storedAddress.
 *
 * @param userInput The user input parsed from arguments.
 * @param storedAddress The stored address parsed from file.
 *
 * @return A bool.
 */
bool matches(const char* userInput, const char* storedAddress) {

    unsigned inputLen = strlen(userInput);

    if (inputLen > strlen(storedAddress)) {

        return false;
    }

    for (unsigned charIndex = 0; charIndex < inputLen; ++charIndex) {
        if (storedAddress[charIndex] != userInput[charIndex]) {

            return false;
        }
    }

    return true;
}

/**
 * @brief Sorts the characters of string in alphabetical order.
 *
 * @param strToSort The string to be sorted.
 */
void sort(char *strToSort) {

    unsigned strLen = strlen(strToSort);

    for (unsigned i = 0; i < strLen - 1; ++i) {

        char smallestChar = ASCII_LENGTH - 1;
        unsigned smallestIndex;

        for (unsigned j = 0; j < strLen - i; ++j) {

            if (strToSort[i + j] < smallestChar) {

                smallestChar = strToSort[i + j];
                smallestIndex = i + j;
            }
        }

        if (smallestIndex) {

            char originalChar = strToSort[i];

            strToSort[i] = smallestChar;
            strToSort[smallestIndex] = originalChar;
        }
    }
}

//*******************************
// The main logic of the program.
//*******************************

int main(int argc, char *argv[]) {

    //****************************************
    // Parse and check the provided arguments.
    //****************************************

    /** Parse result code. */
    int parseResultCode = parse_args(argc, argv);
    if (parseResultCode != 0) {

        return parseResultCode;
    }

    /** The user input. If no input is provided, userInput = "". */
    char *userInput;

    if (argc == 2) {

        userInput = argv[1];
    } else {

        userInput = "";
    }

    //******************************************************
    // Convert the input to uppercase for later comparisons.
    //******************************************************

    to_upper(userInput);

    /** Array for storing allowed symbols. */
    char allowedSymbols[ASCII_LENGTH] = "";

    /** String for the found / matched address. */
    char matchedAddress[LINE_BUFFER_LENGTH];

    /** Buffer for storing current line of file. */
    char currentLine[LINE_BUFFER_LENGTH];

    /** Integer representing current line number of file. */
    int lineIndex = 0;

    /** Integer representing number of found / matched addresses. */
    int foundNum = 0;

    /** Integer representing length of found / matched addresses. */
    unsigned foundLen = 0;

    //***********************************************************
    // Enter loop iterating over the lines of the file until EOF.
    //***********************************************************

    while (fgets(currentLine, LINE_BUFFER_LENGTH, stdin) != NULL) {

        lineIndex++;

        //******************************************************************
        // Trim and convert current line to uppercase for later comparisons.
        //******************************************************************

        trim(currentLine);
        to_upper(currentLine);

        //**********************************
        // Parse and check the current line.
        //**********************************

        parseResultCode = parse_address(lineIndex, currentLine);
        if (parseResultCode != 0) {

            return parseResultCode;
        }

        //****************************************************
        // Check, if user input is substring of current line.
        // If not, skip current iteration and check next line.
        //****************************************************

        if (!matches(userInput, currentLine)) {

            continue;
        }

        //*************************************************************
        // If no matched address stored, store it. We could remove this
        // condition, but then it would unnecessarily store each match.
        //*************************************************************

        if (foundNum == 0) {

            strcpy(matchedAddress, currentLine);
        }

        unsigned currentLineLen = strlen(currentLine);

        if (currentLineLen != foundLen) {

            foundNum++;
        }

        foundLen = currentLineLen;
        unsigned userInputLen = strlen(userInput);

        //*****************************************************
        // If the user input length equals current line length,
        // there is no need to store the continuing character.
        // The matched / found address is safely stored, and
        // we can simply continue to the next loop iteration.
        //*****************************************************

        if (userInputLen == currentLineLen) {

            continue;
        }

        char charToAdd = (char)toupper(currentLine[userInputLen]);

        //*************************************************************
        // Finally, if the continuing character is not already stored,
        // we append it to the end of allowedSymbols[] using strncat().
        //*************************************************************

        if (strchr(allowedSymbols, charToAdd) == NULL) {

            strncat(allowedSymbols, &charToAdd, 1);
        }
    }

    //*****************
    // ↑ End of loop. ↑
    //*****************


    //*************
    //
    //*************

    if (foundNum == 1) {

        printf("Found: %s\n", matchedAddress);
    } else if (foundNum) {

        sort(allowedSymbols);
        printf("Enable: %s\n", allowedSymbols);
    } else {

        printf("Not found\n");
    }

    return 0;
}
