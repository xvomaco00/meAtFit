/**
 * @file keyfilter.c
 * @brief Implementation of virtual keyboard filtering in car dashboard navigation.
 * @author Ondřej Vomáčka (xvomaco00)
 * @date 10/10/2023
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define PRINTABLE_ASCII_LENGTH 128
#define LINE_BUFFER_LENGTH 110
#define MAX_LINE_LENGTH 100

#define ERROR_NO_ARGUMENTS 1
#define ERROR_TOO_MANY_ARGUMENTS 2
#define ERROR_TOO_LONG_USER_INPUT 3
#define ERROR_EMPTY_ADDRESS 4
#define ERROR_TOO_LONG_ADDRESS 5
#define ERROR_NO_ADDRESSES 6

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

    if (actualStart == len) {

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
 *                  Used purely for error messages.
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
 * @brief Prints the allowed characters from the corresponding character map.
 *
 * @param charMap The symbols / characters to be printed.
 */
void print_allowed_chars(const bool* charMap) {

    for (int charIndex = 0; charIndex < PRINTABLE_ASCII_LENGTH; ++charIndex) {
        if (charMap[charIndex]) {

            printf("%c", charIndex);
        }
    }

    printf("\n");
}

int main(int argc, char *argv[]) {

    //**************************************
    // Parse & check the provided arguments.
    //**************************************

    int parseResultCode = parse_args(argc, argv);
    if (parseResultCode != 0) {

        return parseResultCode;
    }

    char *userInput = "";
    unsigned userInputLen = 0;

    if (argc == 2) {

        userInput = argv[1];
        userInputLen = strlen(userInput);
    }

    //******************************************************
    // Convert the input to uppercase for later comparisons.
    //******************************************************

    to_upper(userInput);

    //******************************************************************
    // The ASCII characters are sorted in alphabetical order, so we take
    // advantage of this and store the allowed characters in the map.
    //******************************************************************

    bool charMap[PRINTABLE_ASCII_LENGTH] = {false};
    char matchedAddress[LINE_BUFFER_LENGTH] = "";
    char currentLine[LINE_BUFFER_LENGTH];
    int lineIndex = 0;
    int foundNum = 0;

    //**********************************************
    // Iterate over the lines of the file until EOF.
    //**********************************************

    while (fgets(currentLine, LINE_BUFFER_LENGTH, stdin) != NULL) {

        lineIndex++;
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

        if (!foundNum) {

            strcpy(matchedAddress, currentLine);
        }

        //*********************************************************
        // If user input length equals current line length, we can
        // not store continuing character, because there is none.
        // Also, if current line equals the matched address, we can
        // not increment foundNum, because the logic would break.
        //*********************************************************

        if (foundNum && (userInputLen == strlen(currentLine) || strcmp(matchedAddress, currentLine) == 0)) {

            continue;
        }

        foundNum++;

        //***********************************************************
        // If continuing character is printable (interval <32, 126>),
        // we can enable the index corresponding to the character.
        //***********************************************************

        int charToAdd = toupper(currentLine[userInputLen]);
        if (' ' <= charToAdd && charToAdd <= '~') {

            charMap[charToAdd] = true;
        }
    }

    if (!lineIndex) {

        fprintf(stderr, "ERROR: the provided address book is empty.\n");

        return ERROR_NO_ADDRESSES;
    }

    if (foundNum == 1) {

        printf("Found: %s\n", matchedAddress);
    } else if (foundNum) {

        printf("Enable: ");
        print_allowed_chars(charMap);
    } else {

        printf("Not found\n");
    }

    return 0;
}
