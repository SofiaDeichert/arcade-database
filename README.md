# Arcade Database Management System

## Project Overview
This C++ program implements a database management system for an arcade, allowing users to manage game records including high scores, player initials, number of plays, and revenue generated.

## Features
- Add new game records
- Search for existing records
- Edit record fields (high score, initials, number of plays)
- Delete records
- Process commands from a batch file
- Maintain data integrity and format consistency

## File Structure
- `main.cpp`: Contains the entire program code
- `db.txt`: Initial database file with predefined records
- `samplebatch.txt`: Sample batch file containing test commands
- `database.dat`: Resulting database file after processing batch commands

## How to Use
1. Compile the program using a C++ compiler (C++11 or later)
2. Run the executable
3. When prompted, enter the name of the initial database file (e.g., `db.txt`)
4. Enter the name of the batch file containing commands (e.g., `samplebatch.txt`)
5. The program will process the commands and update the `database.dat` file

## Command Format
The program processes the following command types from the batch file:

1. Add Record: `1 "Game Name" HighScore Initials Plays Revenue`
2. Search Record: `2 SearchTerm`
3. Edit Record: `3 "Game Name" FieldNumber NewValue`
4. Delete Record: `4 Game Name`

## Key Functions
- `validateCommand()`: Ensures commands from the batch file are correctly formatted
- `addRecord()`: Adds a new record to the database
- `searchRecord()`: Searches for a record in the database
- `editRecord()`: Modifies a specific field in a record
- `deleteRecord()`: Removes a record from the database

## Data Formatting
- High scores are stored with leading zeros (9 digits)
- Number of plays are stored with leading zeros (4 digits)
- Revenue is stored in the format $XXXX.XX

## Error Handling
The program includes error checking for file operations and data validation to ensure robust performance.

## Implementation Details

This project was implemented using C++ with a focus on file I/O operations. Here are some key aspects of the implementation:

1. Random File I/O: The program uses random-access file I/O to read from and write to the database file. This allows for efficient editing and updating of specific records without reading the entire file.

2. File Streams: Various file stream objects are used:
   - `ifstream` for reading the initial database and batch files
   - `fstream` for reading from and writing to the main database file
   - `ofstream` for creating temporary files during delete operations

3. File Positioning: The `seekg()` and `seekp()` functions are used to move the file pointer to specific positions, enabling precise reading and writing of records.

4. Binary File Operations: The main database file is opened in binary mode (`ios::binary`) to ensure exact byte-level control over the file content.

5. String Parsing: Extensive string manipulation is used to parse commands from the batch file and to extract fields from database records.

6. In-Place Editing: For edit operations, the program uses file positioning to overwrite specific fields within a record, avoiding the need to rewrite the entire file for small changes.

7. Temporary File Usage: When deleting records, a temporary file is created to store the updated database content before replacing the original file.

These techniques allow for efficient and robust management of the arcade database, providing flexibility in handling various operations while maintaining data integrity.

## Linked List Version
github.com/SofiaDeichert/arcade-database-linkedlist-version
