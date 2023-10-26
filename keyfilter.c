/**
 * @file keyfilter.c
 * @brief Implementation of virtual keyboard filtering in car dashboard navigation.
 * @author Ondřej Vomáčka (xvomaco00)
 * @date 10/10/2023
 *
 * @note First IZP project with stream processing / premium functionality.
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#define PRINTABLE_ASCII_LENGTH 128
#define LINE_BUFFER_SIZE 104 // The buffer needs to be a bit bigger, because each line ends with \n. (+ \r)
#define MAX_LINE_LENGTH 100

#define ERROR_NO_ARGUMENTS 1
#define ERROR_TOO_MANY_ARGUMENTS 2
#define ERROR_TOO_LONG_USER_INPUT 3
#define ERROR_TOO_LONG_ADDRESS 4

/**
 * @brief Converts string characters to uppercase.
 *
 * @param strToUpper The string to be converted.
 * @param strLength The length of given string.
 */
void to_upper(char *strToUpper, unsigned strLength) {

    for (unsigned charIndex = 0; charIndex < strLength; ++charIndex) {

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
int check_args(int argc, char **argv) {

    // Just a sanity check, normally this should not happen.
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
 * @brief Checks whether the first characters of userInput match the first
 *        characters of storedAddress.
 *
 * @param userInput The user input parsed from arguments.
 * @param userInputLen The length of given user input.
 * @param storedAddress The stored address parsed from file.
 * @param storedAddressLen The length of given stored address.
 *
 * @return A bool.
 */
bool matches(const char* userInput, unsigned userInputLen, const char* storedAddress, unsigned storedAddressLen) {

    if (userInputLen > storedAddressLen) {

        return false;
    }

    for (unsigned charIndex = 0; charIndex < userInputLen; ++charIndex) {
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

    int checkResultCode = check_args(argc, argv);
    if (checkResultCode != 0) {

        return checkResultCode;
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

    to_upper(userInput, userInputLen);

    //****************************************************************
    // The ASCII characters are sorted in alphabetical order, so we
    // take advantage of this and store the allowed characters in map.
    //****************************************************************

    bool charMap[PRINTABLE_ASCII_LENGTH] = {false};
    char matchedAddress[LINE_BUFFER_SIZE] = "";
    char currentLine[LINE_BUFFER_SIZE];
    int lineIndex = 0;
    int foundNum = 0;

    //**********************************************
    // Iterate over the lines of the file until EOF.
    //**********************************************

    while (fgets(currentLine, LINE_BUFFER_SIZE, stdin) != NULL) {

        lineIndex++;

        //*********************************************
        // Remove newline character at the end of line.
        //*********************************************

        unsigned newlineIndex = strcspn(currentLine, "\n");

        if (newlineIndex < LINE_BUFFER_SIZE) {

            currentLine[newlineIndex] = 0;

#ifdef _WIN32
            currentLine[strcspn(currentLine, "\r")] = 0;
#endif
        }

        unsigned currentLineLen = strlen(currentLine);

        to_upper(currentLine, currentLineLen);

        if (currentLineLen > MAX_LINE_LENGTH) {

            fprintf(stderr, "ERROR: invalid data found on line number %d.\n", lineIndex);
            fprintf(stderr, "A line can contain a maximum of 100 characters.\n");

            return ERROR_TOO_LONG_ADDRESS;
        }

        //***************************************************
        // If the userInput is not substring, or the line is
        // empty, we can continue to the next loop iteration.
        //***************************************************

        if (!matches(userInput, userInputLen, currentLine, currentLineLen) || !currentLineLen) {

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

        if (foundNum && (userInputLen == currentLineLen || strcmp(matchedAddress, currentLine) == 0)) {

            continue;
        }

        foundNum++;

        //*************************************************
        // If the continuing character is printable, we can
        // enable the index corresponding to the character.
        //*************************************************

        int charToAdd = toupper(currentLine[userInputLen]);
        if (charToAdd < PRINTABLE_ASCII_LENGTH) {

            charMap[charToAdd] = true;
        }
    }

    //******************
    // Print the result.
    //******************

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
