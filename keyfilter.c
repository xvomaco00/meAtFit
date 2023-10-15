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

#define ERROR_NO_ARGUMENTS 1
#define ERROR_TOO_MANY_ARGUMENTS 2
#define ERROR_TOO_LONG_USER_INPUT 3
#define ERROR_EMPTY_ADDRESS 4
#define ERROR_TOO_LONG_ADDRESS 5
#define ERROR_NO_ADDRESSES 6

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

        return ERROR_NO_ARGUMENTS;
    }

    if (argc > 2) {

        fprintf(stderr, "ERROR: too many arguments provided.\n");
        fprintf(stderr, "Usage: %s [input] <text-file\n", argv[0]);

        return ERROR_TOO_MANY_ARGUMENTS;
    }

    if (argc == 2 && strlen(argv[1]) > MAX_LINE_LENGTH) {

        fprintf(stderr, "ERROR: invalid arguments provided.\n");
        fprintf(stderr, "The input can not exceed 100 characters.\n");

        return ERROR_TOO_LONG_USER_INPUT;
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

        return ERROR_EMPTY_ADDRESS;
    }

    if (lineLength > MAX_LINE_LENGTH) {

        fprintf(stderr, "ERROR: invalid data found on line number %d.\n", lineIndex);
        fprintf(stderr, "A line can contain a maximum of 100 characters.\n");

        return ERROR_TOO_LONG_ADDRESS;
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

    for (unsigned fromStartIndex = 0; fromStartIndex < strLen - 1; ++fromStartIndex) {

        char smallestChar = ASCII_LENGTH - 1;
        unsigned smallestIndex;

        for (unsigned comparingIndex = 0; comparingIndex < strLen - fromStartIndex; ++comparingIndex) {

            if (strToSort[fromStartIndex + comparingIndex] < smallestChar) {

                smallestChar = strToSort[fromStartIndex + comparingIndex];
                smallestIndex = fromStartIndex + comparingIndex;
            }
        }

        if (smallestIndex) {

            char originalChar = strToSort[fromStartIndex];

            strToSort[fromStartIndex] = smallestChar;
            strToSort[smallestIndex] = originalChar;
        }
    }
}

int main(int argc, char *argv[]) {

    int parseResultCode = parse_args(argc, argv);
    if (parseResultCode != 0) {

        return parseResultCode;
    }

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

    char allowedSymbols[ASCII_LENGTH] = "";
    char matchedAddress[LINE_BUFFER_LENGTH];
    char currentLine[LINE_BUFFER_LENGTH];
    int lineIndex = 0;
    int foundNum = 0;
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

        parseResultCode = parse_address(lineIndex, currentLine);
        if (parseResultCode != 0) {

            return parseResultCode;
        }

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

    if (!lineIndex) {

        fprintf(stderr, "ERROR: provided address book is empty.\n");

        return ERROR_NO_ADDRESSES;
    }

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
