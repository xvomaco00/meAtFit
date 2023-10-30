[English Version](ASSIGNMENT.md) | [Czech Version](ASSIGNMENT_CS.md)

<a href="./ASSIGNMENT.md"><img src="https://upload.wikimedia.org/wikipedia/commons/thumb/f/f2/Flag_of_Great_Britain_%281707%E2%80%931800%29.svg/640px-Flag_of_Great_Britain_%281707%E2%80%931800%29.svg.png" alt="drawing" width="50"></a>
<a href="./ASSIGNMENT_CS.md"><img src="https://upload.wikimedia.org/wikipedia/commons/thumb/c/cb/Flag_of_the_Czech_Republic.svg/255px-Flag_of_the_Czech_Republic.svg.png" alt="drawing" width="50"></a>

### Project 1 - Working with text

#### Motivational example

Imagine a virtual keyboard in the navigation on the dashboard of a car. The navigation system has a whole database of addresses and expects the user to input using the virtual keyboard. To make it easier for the user to enter, only selected keys are enabled on the keyboard at any given time - those keys that, if used, would still lead the search string to a known address. For example, the navigation knows the cities of Brno, Beroun and Bruntál, the user has already entered the letters "BR" and therefore only the keys "N" (leading to "Brno") and "U" (leading to "Bruntál") will be allowed on the navigation.

#### Project description

The aim of the project is to create a program that would emulate the algorithm for calculating the enabling and disabling of keys on the virtual keyboard of the navigation. The input to the program is data of known addresses and a user-supplied string. The output of the program will contain the characters to be enabled on the keyboard.

#### Detailed specification

Implement the program in a single _keyfilter.c_ source file. Input data will be read from standard input (stdin), output will be printed to standard output (stdout).

#### Translation and commit of the source file

Commit: Commit the source file _keyfilter.c_ via the information system.

Compile the program with the following arguments

```gcc -std=c11 -Wall -Wextra -Werror keyfilter.c -o keyfilter```

#### Execution syntax

The program executes as follows: (./keyfilter denotes the location and name of the program):

```./keyfilter ADDRESS```

If the program is run without arguments, it takes the specified address as an empty string.

#### Implementation details

##### Input address database

Address databases are text data where each line denotes one address. Each row contains a maximum of 100 characters. The list of addresses is unordered. All data is case-insensitive. The program must support at least 42 addresses.

##### Program output

The program output can be of three types:

1. address found,
2. address requires specification,
3. address not found.

**1. Address found**

Found.

This output is printed if a single address S is found in the address database whose prefix matches the user-specified address ADDRESS. (Note: The [prefix](https://en.oxforddictionaries.com/definition/prefix "https://en.oxforddictionaries.com/definition/prefix") P of an S string is a string where the S string starts with the P string).

**2. Address requires specification**

Enable: CHARS

If multiple addresses matching a given ADDRESS prefix are found in the address database, the program prints a list of enabled CHARS keys using this formatted line. CHARS is an alphabetically ordered list of characters for which, for each character C, there is an address in the address database whose prefix matches the ADDRESS string associated with that character C.

**3. Address not found**

Not found

If there is no address in the address database whose prefix matches the specified ADDRESS string, the program prints this message.

##### Restrictions in the project

It is forbidden to use the following features:

- calls from malloc and free families - working with dynamic memory is not needed in this project,
- calls from the fopen, fclose, fscanf, ... family - working with (temporary) files is not required in this project,
- calls qsort, lsearch, bsearch and hsearch - the goal is to think about algorithmization and data structure.

#### Unexpected behaviour

Respond to runtime errors in the usual way: Respond to unexpected input data, input data format, or function call errors by aborting the program with a brief and concise error message to the appropriate output and the appropriate return code. Messages will be in ASCII encoded Czech or English.

#### Examples of inputs and outputs

Auxiliary address file:

```
$ cat addresses.txt
Prague
Brno
Bruntal
Bratislava
```

Example of searching for the word Brno:

```
$ ./keyfilter <addresses.txt
Enable: BP

$ ./keyfilter b <addresses.txt
Enable: R

$ ./keyfilter br <addresses.txt
Enable: ANU

$ ./keyfilter brn <addresses.txt
Found.

$ ./keyfilter be <addresses.txt
Not found
```

#### Ratings

The following factors have a major influence on the final rating:

- The translatability of the source file,
- the format of the source file (layout, alignment, comments, appropriately chosen identifiers),
- decomposition of the problem into subproblems (appropriate functions, appropriate function length and function parameters),
- correct choice of data types, or creation of new types,
- proper search functionality and
- handling of error conditions.

##### Priority functionalities

1. Finding prefixes in a string and corresponding addresses.
2. Data structures for enabled keys and key lookup.
3. Stream processing of input addresses and printing of search results.
