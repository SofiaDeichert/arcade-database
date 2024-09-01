#include <cctype> 
#include <cstdio> 
#include <fstream> 
#include <iomanip> 
#include <iostream> 
#include <sstream>
#include <string> 

using namespace std;

//forward declarations for functions:
bool validateCommand(string commandLine);
void addRecord(string commandLine, fstream& datfile);
void searchRecord(string searchTerm, fstream& datfile);
void editRecord(string commandLine, fstream& datfile);
void deleteRecord(string recordToDelete, fstream& datfile);
string toLowercase(string original);
string cutLeadingZeroes(string original);
string addLeadingZeroes(string original, int fieldWidth);

const string database = "database.dat";
//your filestream for the database will connect to this variable

string toLowercase(string original)  //function to convert string to lowercase
{
    //loop through each character in string
    for (size_t i = 0; i < original.size(); ++i)
    {
        //convert current character to lowercase equivalent,
        //converting to 'char' type for compatibility with 'string' type
        original[i] = static_cast<char>(tolower(original[i]));
    }
    //return modified string with lowercase letters
    return original;
}

string addLeadingZeroes(string original, int fieldWidth)  //function to add leading zeroes to string until it reaches certain length
{
    //create stringstream object to build new string(need a 
    //stringstream to use manipulators like setw and setfill)
    stringstream x;
    
    //set width of field; if original string is shorter 
    //than this width, leading zeroes will be added
    x << setw(fieldWidth) << setfill('0') << original;
    
    //convert stringstream back to string and return it
    return x.str();
}

string cutLeadingZeroes(string original)  //function to remove leading zeros from string
{
    //loop through each charactter in string
    for (size_t i{ 0 }; i < original.size(); ++i)
    {
        //if current character isn't '0'
        if (original[i] != '0')
        {
            //create new string starting from this nonzero character to end of original string
            string withoutLeadingZeros{ original.substr(i) };
            
            //return modified string without leading zeros
            return withoutLeadingZeros;
        }
    }
    //if whole string is '0's or empty, return only one '0' to indicate value is zero
    return "0"; 
}

bool validateCommand(string commandLine)  //function to ensure commands from batch file follow correct format
{
    //check if first character is valid number(1,2,3, or 4)
    if (commandLine[0] != '1' && commandLine[0] != '2' && commandLine[0] != '3' && commandLine[0] != '4')
    {
        return false; //invalid if 1st character isn't valid number
    }
    
    //check if second character is a space
    if(commandLine[1] != ' ')
    {
        return false; //invalid if 2nd character isn't space
    }

    //if first character is '1', perform validation for 'add record' batch file command:
    if(commandLine[0] == '1')
    {
        //check for double quotes around name:
        //use .find() to get position of first double quote in command line and store in variable
        size_t doubleQuote1{ commandLine.find('\"') }; 
        if (doubleQuote1 == string::npos)
        {
            return false; //invalid if 1st quote not found
        }
        
        //create variable for position of 2nd double quote(.find() searches for first double quote after doubleQuote1)
        size_t doubleQuote2{ commandLine.find('\"', doubleQuote1 + 1) }; 
        if (doubleQuote2 == string::npos)
        {
            return false; //invalid if 2nd quote not found
        }

        //extract name starting from right after first double quote all the way to before second double quote
        //doubleQuote2 - (doubleQuote1 + 1) calculates the correct number of characters to extract for name
        string name{ commandLine.substr(doubleQuote1 + 1, doubleQuote2 - (doubleQuote1 + 1)) };
        
        //check if name contains only alphanumeric characters, punctuation, and spaces by iteratating through each character
        for (size_t i = 0; i < name.size(); ++i)
        {
            if (!isalnum(name[i]) && !ispunct(name[i]) && !isspace(name[i]))
            {
                return false; //invalid if name contains any invalid characters
            }
        }

        //check for 4 spaces after 2nd double quote
        int numSpaces{}; //variable to track spaces
        
        //iterate through each character after 2nd double quote and checks if it's a space 
        for (size_t i = doubleQuote2 + 1; i < commandLine.size(); ++i)
        {
            if (commandLine[i] == ' ')
            {
                ++numSpaces; //if the character is a space, add one to our counter
            }
        }
        if (numSpaces != 4)
        {
            return false; //invalid if there are not exactly 4 spaces after 2nd double quote
        }

        //create variables for each space (to use their positions as dilimiters for our fields)
        size_t space1{ commandLine.find(' ', doubleQuote2) }; //get position of first space after 2nd double quote
        size_t space2{ commandLine.find(' ', space1 + 1) };   //get position of first space after space1
        size_t space3{ commandLine.find(' ', space2 + 1) };   //get position of first space after space2
        size_t space4{ commandLine.find(' ', space3 + 1) };   //get position of first space after space3
        
        if (space1 == string::npos || space2 == string::npos || space3 == string::npos || space4 == string::npos) //if any of our spaces cannot be found
        {
            return false; //invalid because of incorrect spacing between fields
        }
        
        /*check if second field(highscore) is all digits and is less than 10 digits:
          extract high score from commandline by getting portion starting after space1 all the way to before space2
          space2 - (space1 +1 ) calculates the exact number of characters to extract for the highscore */
        string highScore{ commandLine.substr(space1 + 1, space2 - (space1 + 1)) }; 
    
        //iterate through each character in high score field
        for (size_t i = 0; i < highScore.size(); ++i)
        {
            if (!isdigit(highScore[i]))
            {
                return false; //invalid because highscore field has at least 1 character that is not a digit
            }
            if (highScore.size() < 1 || highScore.size() > 9)
            {
                return false; //invalid because highscore field has incorrect length
            }
        }

        /*check if third field(initials) has no white space and is exactly 3 characters
          extract initials by getting portion of commandline starting after space2 all the way to before space3
          space3 - (space2 + 1) calculates the exact number of characters to extract for the initials */
        string initials{ commandLine.substr(space2 + 1, space3 - (space2 + 1)) }; 
        
        //iterate through each character in initials field
        for (size_t i = 0; i < initials.size(); ++i)
        {
            if (isspace(initials[i]))
            {
                return false; //invalid because initials field contains a space
            } 
        }
        if (initials.size() != 3)
        {
            return false; //invalid because initials field is not exactly 3 characters
        }

        /*check fourth field(plays) is all digits, mimimum 1 character and maximum 4 characters
          extract plays by getting portion of commandline starting after space3 all the way to before space4
          space4 - (space3 + 1) calculates the exact number of characters to extract for plays */
        string plays{ commandLine.substr(space3 + 1, space4 - (space3 + 1)) }; 
        
        //iterate through each character in plays field
        for (size_t i = 0; i < plays.size(); ++i)
        {
            if (!isdigit(plays[i]))
            {
                return false; //invalid because fourth field has at least 1 character that is not a digit
            }
            if ((plays.size() < 1) || (plays.size() > 4))
            {
                return false; //invalid because fourth field has incorrect length
            }
        }

        //check fifth field(revenue) has dollar sign, decimal point, 1-4 digits between decimal point and $, and exactly 2 digits after decimal point:
        //extract revenue by getting portion from right after space4 all the way to end of commandline
        string revenue{ commandLine.substr(space4 + 1) };      
        
        //extract position of decimal point in revenue by finding first occurence of '.' in our revenue variable
        size_t decimal{ revenue.find('.') };                    
        
        //extract revenue part before decimal excluding '$'(dollars) by getting portion of revenue string from after first character('$') to before decimal point
        //(decimal - 1) calculates the exact amount of characters to extract for our dollars part
        string beforeDecimal{ revenue.substr(1, decimal - 1) }; 
        
        //extract revenue part after decimal(cents) by getting portion starting from after decimal point all the way to end of revenue string
        string afterDecimal{ revenue.substr(decimal + 1) };    
    
        //if first character of revenue field is not '$', decimal doesn't exist, dollars part is not between 1-4 characters, or cents part is not 2 characters
        if (revenue[0] != '$' || decimal == string::npos || beforeDecimal.size() < 1 || beforeDecimal.size() > 4 || afterDecimal.size() != 2)
        {
            return false; //invalid if revenue format is incorrect
        }
    }
    
    //if first character is '3', perform validation for 'edit record' batch file command:
    if(commandLine[0] == '3')
    {
        //create variable for position of 1st double quote
        size_t doubleQuote1{ commandLine.find('\"') };
        if (doubleQuote1 == string::npos)
        {
            return false; //invalid if first double quote not found
        }
        //create variable for position of 2nd double quote
        size_t doubleQuote2{ commandLine.find('\"', doubleQuote1 + 1) };
        if (doubleQuote2 == string::npos)
        {
            return false; //invalid if second double quote not found
        }

        //extract name starting from right after first double quote all the way to before second double quote
        //doubleQuote2 - (doubleQuote1 + 1) calculates the exact number of characters to extract for name
        string name{ commandLine.substr(doubleQuote1 + 1, doubleQuote2 - (doubleQuote1 + 1)) }; 
        //check if name contains only alphanumeric characters, punctuation, and spaces by iteratating through each character
        for (size_t i = 0; i < name.size(); ++i)
        {
            if (!isalnum(name[i]) && !ispunct(name[i]) && !isspace(name[i]))
            {
                return false; //invalid if name has invalid characters
            }
        }

        //check for 2 spaces after double quote to ensure proper field separation:
        //create numSpaces variable to track spaces in commandline
        int numSpaces{};
        //iterates through each character after the double quote and checks if it's a space 
        for (size_t i = doubleQuote2 + 1; i < commandLine.size(); ++i)
        {
            if (commandLine[i] == ' ')
            {
                ++numSpaces;  //if character is space, add to our numSpaces count
            }
        }
        if (numSpaces != 2)
        {
            return false; //invalid if there are not exactly 2 spaces after double quote
        }

        //create variables for each space (to use their positions as dilimiters for our fields)
        size_t space1{ commandLine.find(' ', doubleQuote2) };
        size_t space2{ commandLine.find(' ', space1 + 1) };
        
        if (space1 == string::npos || space2 == string::npos) //if either space couldn't be found
        {
            return false; //invalid because of incorrect spacing between fields.
        }

        //extract field number by getting portion of commandline starting from right after space1, and extracting only 1 character
        string fieldNumber{ commandLine.substr(space1 + 1, 1) }; 
   
        //check if field number is valid (1, 2, or 3); otherwise, invalid command
        if ((fieldNumber[0] != '1') && (fieldNumber[0] != '2') && (fieldNumber[0] != '3'))
        {
            return false;
        }

        //extract new value by getting portion starting from right after space2 all the way to end of commandline
        string newValue{ commandLine.substr(space2 + 1) }; 

        //if field number is '1', check if new value adheres to high score validation rules
        if (fieldNumber == "1")
        {
            //iterates through each character of new value and checks if it is not a digit
            for (size_t i = 0; i < newValue.size(); ++i) {
                if (!isdigit(newValue[i])) {
                    return false; //new value invalid if any character is not a digit, thus command line invalid
                }
                if (newValue.size() < 1 || newValue.size() > 9) {
                    return false; //new value invalid if not correct length, thus command line invalid
                }
            }
        }
        
        //if field number is '2', check if new value adheres to initials validation rules
        if (fieldNumber == "2")
        {
           //iterates through each character in new value and checks if it is a space
            for (size_t i = 0; i < newValue.size(); ++i)
            {
                if (isspace(newValue[i]))
                {
                    return false; //new value invalid if any character is a space, thus command line invalid
                }
            }
            if (newValue.size() != 3)
            {
                return false; //new value invalid if it is not exactly 3 characters, thus command line invalid
            }
        }
        
        //if field number is '3', check if new value adheres to number of plays validation rules
        if (fieldNumber == "3")
        {
            //iterates through each character in new value and checks if it is not a digit
            for (size_t i = 0; i < newValue.size(); ++i)
            {
                if (!isdigit(newValue[i]))
                {
                    return false; //new value invalid if any character is not a digit, thus command line invalid
                }
                if ((newValue.size() < 1) || (newValue.size() > 4))
                {
                    return false; //new valid invalid if not correct length, thus command line invalid
                }
            }
        }
    } 
    
    //if first character is '4', perform validation for 'delete record' batchfile command:
    if(commandLine[0] == '4')
    {
        //extract name to delete from command line, get section starting after first space(position 2)
        string name{ commandLine.substr(2) };
        //iterate through each character of name 
        for (size_t i = 0; i < name.size(); ++i)
        {
            if (!isalnum(name[i]) && !ispunct(name[i]) && !isspace(name[i]))
            {
                return false; //invalid if any character is not alphanumeric, punctuation, or space
            }
        }
    }
    return true; //command line from batchfile valid if all previous checks were successful
}

void addRecord(string commandLine, fstream& datfile)  //function to add new record to end of database file
{
    //reopen database file in the correct modes (writing to database file without deleting existing contents)
    datfile.open(database, ios::in | ios::out | ios::binary);
    
    //clear any error flags in filestream
    datfile.clear();
    
    //print error if datafile cannot be opened correctly
    if (!datfile)
    {
        cerr << "data file could not be opened properly.\n";
    }
    
    //move file pointer write position to the end of file
    datfile.seekp(0, ios::end);
    
    //same batch command parsing logic as in our validateCommand function
    //find the first and second double quote to isolate name field
    size_t doubleQuote1{ commandLine.find('\"') };
    size_t doubleQuote2{ commandLine.find('\"', doubleQuote1 + 1) };

    /*use spaces as dilimiters for our fields
      start searching at double quote instead of beginning of command line, because our name 
      can include spaces and punctuation(which would mistake our in-field comma/space for delimiter) */
    size_t space1{ commandLine.find(' ', doubleQuote2) };
    size_t space2{ commandLine.find(' ', space1 + 1) };
    size_t space3{ commandLine.find(' ', space2 + 1) };
    size_t space4{ commandLine.find(' ', space3 + 1) };

    //parse commandline string to extract fields (based on positions of quotes and spaces after quotes)
    string name{ commandLine.substr(doubleQuote1 + 1, doubleQuote2 - (doubleQuote1 + 1)) };
    string highScore{ commandLine.substr(space1 + 1, space2 - (space1 + 1)) };
    string initials{ commandLine.substr(space2 + 1, space3 - (space2 + 1)) };
    string plays{ commandLine.substr(space3 + 1, space4 - (space3 + 1)) };
    string revenue{ commandLine.substr(space4 + 1) };

    //formatting for database file: add leading zeroes if high score is less than 9 digits
    string formattedHighScore{addLeadingZeroes (highScore, 9)};

    //add leading zeroes if plays has less than 4 digits
    string formattedPlays{addLeadingZeroes (plays, 4)} ;

    //split revenue into dollars and cents for formatting
    string dollars{ revenue.substr(1, revenue.find('.') - 1) };
    string cents{ revenue.substr(revenue.find('.') + 1) };
    
    //add leading zeroes if value of first four digits in revenue is less than 1000
    string formattedRevenue {'$' + addLeadingZeroes(dollars, 4) + '.' + cents};  
    
    //append new record to file with formatted values
    datfile << name << ", " << formattedHighScore << ", " << initials << ", " << formattedPlays << ", " << formattedRevenue << '\n';

    //output added record's details to console(using cutLeadingZeroes for correct format)
    cout << "RECORD ADDED\n" << "Name: " << name << '\n'
         << "High Score: " << cutLeadingZeroes(highScore) << '\n'
         << "Initials: " << initials << '\n'
         << "Plays: " << cutLeadingZeroes(plays) << '\n'
         << "Revenue: " << cutLeadingZeroes(revenue) << '\n' << '\n';

    //close file stream after adding record
    datfile.close();
}

void searchRecord(string searchTerm, fstream& datfile)  //function to search for record given a search term
{
    //create flag to track if search term is found in database file
    bool searchTermFound{ false };
    
    //create variable to store each line read from database file
    string datfileLine{};

    //open database file for reading and clear any error flags that could be set on filestream
    datfile.open(database, ios::in | ios::binary); 
    datfile.clear();                               
    
    //print error if database file cannot be opened correctly
    if (!datfile)
    {
        cerr << "data file cannot be opened properly.\n";
    }
    
    //move file pointer read position to beginning of file
    datfile.seekg(0, ios::beg); 

    //keep reading lines from our database file until we get to end of file
    while (getline(datfile, datfileLine))
    {
        /*use reverse find to extract the position of the last occurence of '$'
         *not using ' ' ',' or ', ' as delimiters since name can contain punctuation/spaces, and initials can contain punctuation
         *since fields(except name) have a fixed length, we can count backwards from '$' to extract each one */
        size_t revenueDollarSign{ datfileLine.rfind('$') };
        
        /*extract name part of record:
         *gets portion of database line from first character(position 0) to 24 places before the '$'
         *24 is the number of characters between the end of the name and the '$' */
        string name{ datfileLine.substr(0, revenueDollarSign - 24) };

        //if search term is in name(using toLowercase functions on both names for case-insensitivity), we parse the rest of the fields out of the database line
        if (toLowercase(name).find(toLowercase(searchTerm)) != string::npos)
        {
            searchTermFound = true; //set flag to true as we have found the search term
            
            //extract revenue by getting part starting at the revenueDollarSign all the way to the end of the database file line
            string revenue{ datfileLine.substr(revenueDollarSign) }; 
            
            //extract dollars part of revenue (getting exactly 4 characters starting from right after '$' in revenue)
            string revenueDollars{ datfileLine.substr(revenueDollarSign + 1, 4) }; 
            
            //extract cents part of revenue (getting portion of revenue starting from right after decimal point)
            string revenueCents{ revenue.substr(revenue.find('.') + 1) }; 

            //extract plays field using position of '$' to find where plays field starts(position of revenueDollarSign - 6) and extracting exactly 4 characters
            string plays{ datfileLine.substr(revenueDollarSign - 6, 4) };

            //extract initials field using position of '$' to find where initials field starts(position of revenueDollarSign - 11) and extracting exactly 3 characters
            string initials{ datfileLine.substr(revenueDollarSign - 11, 3) };

            //extract highscore field using position of '$' to find where highscore field starts(position of revenueDollarSign - 22) and extracting exactly 9 characters
            string highScore{ datfileLine.substr(revenueDollarSign - 22, 9) };

            //output found record's details to console (using cutLeadingZeroes function to ensure correct format)
            cout << name << " FOUND\n"
                 << "High Score: " << cutLeadingZeroes(highScore) << '\n'
                 << "Initials: " << initials << '\n'
                 << "Plays: " << cutLeadingZeroes(plays) << '\n'
                 << "Revenue: " << '$' << cutLeadingZeroes(revenueDollars) << '.' << revenueCents << '\n' << '\n';
        }
    }
    
    //if search term not found after going through all database lines, print message accordingly
    if (!searchTermFound)
    {
        cout << searchTerm << " NOT FOUND\n\n"; 
    }
    
    //close filestream after completing search operation
    datfile.close();
}

void editRecord(string commandLine, fstream& datfile)  //function to edit specific record in database file
{
    //create flag to track if name to edit can be found in database file
    bool foundNameToEdit{false};

    //extract all fields from batch file(same parsing logic as validateCommand function):
    //find positions of double quotes to extract game name
    size_t doubleQuote1{ commandLine.find('\"') };
    size_t doubleQuote2{ commandLine.find('\"', doubleQuote1 + 1) };

    //extract name to edit from batchfile command line
    string batchfileName{ commandLine.substr(doubleQuote1 + 1, doubleQuote2 - (doubleQuote1 + 1)) };

    //create variables for each space after second double quote(to use their positions as dilimiters for our fields)
    size_t space1{ commandLine.find(' ', doubleQuote2) };
    size_t space2{ commandLine.find(' ', space1 + 1) };

    //extract field number and new value
    string fieldNumber{ commandLine.substr(space1 + 1, 1) }; 
    string newValue{ commandLine.substr(space2 + 1) }; 

    //ensure filestream is ready for both reading and writing
    datfile.open(database, ios::in | ios::out | ios::binary);
    datfile.clear();  //clear any set error flags on our filestream
    
    //print error if database file cannot be opened correctly
    if (!datfile)
    {
        cerr << "data file could not be opened properly.\n";
    }
    
    datfile.seekg(0, ios::beg); //move read position of file pointer to beginning of datafile

    string datfileLine;                  //create string variable to store each line from database file
    streampos prevPos = datfile.tellg(); //variable to keep track of position of file pointer

    //loop through database file lines to search for record to edit
    while (getline(datfile, datfileLine))
    {
        //extract fields of record using known positions and delimiters:
        //same logic used previously of working backwards from the revenue '$'
        size_t revenueDollarSign{ datfileLine.rfind('$') };                  //find the last '$' to parse revenue
        string datfileName{ datfileLine.substr(0, revenueDollarSign - 24) }; //parse game name from database line

        //parse other fields (to use in our console output display)
        string revenue{ datfileLine.substr(revenueDollarSign) };
        string revenueDollars{ datfileLine.substr(revenueDollarSign + 1, 4) };
        string revenueCents{ revenue.substr(revenue.find('.') + 1) };

        //extract plays field using position of '$' to find where plays field starts(position of revenueDollarSign - 6) and extracting exactly 4 characters
        string plays{ datfileLine.substr(revenueDollarSign - 6, 4) };

        //extract initials field using position of '$' to find where initials field starts(position of revenueDollarSign - 11) and extracting exactly 3 characters
        string initials{ datfileLine.substr(revenueDollarSign - 11, 3) };

        //extract highscore field using position of '$' to find where highscore field starts(position of revenueDollarSign - 22) and extracting exactly 9 characters
        string highScore{ datfileLine.substr(revenueDollarSign - 22, 9) };
       
        //if name from batchfile command matches current record's name
        if (batchfileName == datfileName)
        {
            foundNameToEdit = true; //set flag to true as we have found the name to edit
            
            //update correct field with new value depending on the field number:
            
            if (fieldNumber == "1") //if field number is 1, update high score
            {
                /*move write position of file pointer right before high score field 
                  prevPos moves file pointer to beginning of line record is in
                  adding datafileName length and 2(accounting for comma and space) moves it exactly before high score */
                datfile.seekp(prevPos + static_cast<streampos>(datfileName.size()) + static_cast<streampos>(2)); 
                
                //write over/update datafile line with new value(highscore), formatted with leading zeroes
                datfile << addLeadingZeroes(newValue, 9);                                                        

                //output edited record's details to console(using cutLeadingZeroes function to ensure correct format)
                cout << datfileName << " UPDATED\n"
                     << "UPDATE TO high score - VALUE " << cutLeadingZeroes(newValue) << '\n'
                     << "Name: " << datfileName << '\n'
                     << "High Score: " << cutLeadingZeroes(newValue) << '\n'
                     << "Initials: " << initials << '\n'
                     << "Plays: " << cutLeadingZeroes(plays) << '\n'
                     << "Revenue: " << '$' << cutLeadingZeroes(revenueDollars) << '.' << revenueCents << '\n' << '\n';
                    
                //exit loop since record has been found and updated
                break;
            }
            if (fieldNumber == "2") //if field number is 2, update initials
            {
                /*move write position of file pointer right before initials field 
                  prevPos moves file pointer to beginning of line record is in
                  adding datafileName length and 13(accounting for all characters between name and initials) moves it exactly before initials */
                datfile.seekp(prevPos + static_cast<streampos>(datfileName.size()) + static_cast<streampos>(13)); 
                
                //write over/update datafile line with new value(initials)
                datfile << newValue;

                //output edited record's details to console(using cutLeadingZeroes function to ensure correct format)
                cout << datfileName << " UPDATED\n"
                     << "UPDATE TO initials - VALUE " << newValue << '\n'
                     << "Name: " << datfileName << '\n'
                     << "High Score: " << cutLeadingZeroes(highScore) << '\n'
                     << "Initials: " << newValue << '\n'
                     << "Plays: " << cutLeadingZeroes(plays) << '\n'
                     << "Revenue: " << '$' << cutLeadingZeroes(revenueDollars) << '.' << revenueCents << '\n' << '\n';
                    
                //exit loop since record has been found and updated
                break;
            }
            if (fieldNumber == "3") //if field number is 3, update plays(and thus revenue)
            {
                //convert newValue string holding number of plays to double value so we can perform multiplication 
                //multiply by 0.25 to get new value for revenue
                double newRevenue {stod(newValue) * 0.25 };
                
                //get dollar part by truncating fractional part(cents) - does this by converting to int type(which has no fractional part)
                int newDollars{ static_cast<int>(newRevenue) }; 
                
                //get cents part alone by subtracting dollar amount from value
                double newCents{newRevenue - newDollars};       
                
                stringstream x;
                x << fixed << setprecision(2) << newCents;     //use stringstream object to format cents to 2 decimal places
                string formattedNewCents = x.str().substr(1);  //remove leading '0' from formatted string

                /*move write position of file pointer right before plays field 
                  prevPos moves file pointer to beginning of line the record is in, 
                  adding the datafileName length and 18(accounting for all characters between name and plays) moves it exactly before plays */
                datfile.seekp(prevPos + static_cast<streampos>(datfileName.size()) + static_cast<streampos>(18)); 
                
                //write over/update datafile line with new value(plays), formatted with leading zeroes
                datfile << addLeadingZeroes(newValue, 4);
                
                /*move write position of file pointer right before revenue field 
                  prevPos moves file pointer to beginning of line the record is in, 
                  adding the datafileName length and 24(accounting for all characters bewteen name and revenue) moves it exactly before revenue */
                datfile.seekp(prevPos + static_cast<streampos>(datfileName.size()) + static_cast<streampos>(24));
                
                //write over/update datafile line with new value(revenue), formatted with leading zeroes
                datfile << '$' << addLeadingZeroes(to_string(newDollars), 4) << formattedNewCents; 

                //output edited record's details to console(using cutLeadingZeroes function to ensure correct format)
                cout << datfileName << " UPDATED\n"
                     << "UPDATE TO plays - VALUE " << newValue << '\n'
                     << "Name: " << datfileName << '\n'
                     << "High Score: " << cutLeadingZeroes(highScore) << '\n'
                     << "Initials: " << initials << '\n'
                     << "Plays: " << cutLeadingZeroes(newValue) << '\n'
                     << "Revenue: " << '$' << fixed << setprecision(2) << newRevenue << '\n' << '\n'; 
                    
                //exit loop since record has been found and updated
                break;
            }

        }
        //update position to beginning of next line
        prevPos = datfile.tellg(); 
    }
    
    //if batchfile name to edit cannot be found in the database, print an error message
    if(foundNameToEdit == false)
    {
        cout << "Record to edit was not found in the database file.\n";
    }
    
    //close file after edit operation is complete
    datfile.close();
}

 void deleteRecord(string recordToDelete, fstream& datfile)  //function to delete a record from database file, given a game name
 {
    //open database file
    datfile.open(database, ios::in | ios::out | ios::binary);
    
    //create temporary new file where we will write the non-deleted records:
    string newfileName = "newfile.txt";                     //create string variable to hold filename
    ofstream newfile (newfileName, ios::out | ios::binary); //create the filestream with that filename
    
    //check if temporary new file can be opened successfully
    //if not, print an error to the console
    if(!newfile)
    {
        cerr << "Error opening temporary new file.\n";
    }
    
    //check if database file can be opened successfully
    //if not, print an error to the console
    if(!datfile)
    {
        cerr << "Error opening database file.\n";
    }
   
    //create variable to store each line read from database file
    string datfileLine{};
    
    //create flag to track if the record to delete is found
    bool recordFound = false;
    
    //loop through each line until end of database file
    while (getline(datfile, datfileLine))
    {
        //use reverse find to extract the position of the last occurence of the '$'(same parsing logic previously used)
        size_t revenueDollarSign{ datfileLine.rfind('$') };
        
        //extract name from database line based on position of '$' from revenue field
        string datfileName{ datfileLine.substr(0, revenueDollarSign - 24) };

        //if record to delete does not match name, write record to new file (use toLowercase function to ensure case-insensitivity)
        if (toLowercase(recordToDelete) != toLowercase(datfileName)) 
        {
            newfile << datfileLine << '\n';
        }   
        
        else  //if we find record to delete
        {
            //set recordFound flag to true and skip writing this record to new file
            recordFound = true;
            
            //extract rest of fields of deleted record to print in our console output:
            //extract revenue using position of '$'
            string revenue{ datfileLine.substr(revenueDollarSign) };
            
            //split revenue into dollars and cents parts to ensure correct formatting in output
            string revenueDollars{ datfileLine.substr(revenueDollarSign + 1, 4) }; 
            string revenueCents{ revenue.substr(revenue.find('.') + 1) };
            
            //extract plays field using position of '$' to find where plays field starts(position of revenueDollarSign - 6) and extracting exactly 4 characters
            string plays{ datfileLine.substr(revenueDollarSign - 6, 4) };

            //extract initials field using position of '$' to find where initials field starts(position of revenueDollarSign - 11) and extracting exactly 3 characters
            string initials{ datfileLine.substr(revenueDollarSign - 11, 3) };

            //extract highscore field using position of '$' to find where highscore field starts(position of revenueDollarSign - 22) and extracting exactly 9 characters
            string highScore{ datfileLine.substr(revenueDollarSign - 22, 9) };

            //output deleted record's details to console (using cutLeadingZeroes function to ensure correct format)
            cout << "RECORD DELETED\n"
                 << "Name: " << datfileName << '\n'
                 << "High Score: " << cutLeadingZeroes(highScore) << '\n'
                 << "Initials: " << initials << '\n'
                 << "Plays: " << cutLeadingZeroes(plays) << '\n'
                 << "Revenue: " << '$' << cutLeadingZeroes(revenueDollars) << '.' << revenueCents << '\n' << '\n';
        }
    }
    
    if(recordFound)  //after processing each record, if record was found and 'deleted'
    {
        remove(database.c_str()); //delete old file
        rename(newfileName.c_str(), database.c_str()); //rename new file to original database name
    }
    
    else  //if record not found, delete temporary file and print message to console
    {
        remove(newfileName.c_str()); //delete temporary file
        cout << "Record to delete was not found in the database file.\n\n";
    }
    
    //close database file after completing delete operation
    datfile.close();
 }

int main()
{
    /* ################  DO NOT MODIFY CODE IN THIS BLOCK ###################### */
    string temp;  //variable for database filename
    string batch; //variable for batch filename

    cout << "Enter Database Name: ";
    cin >> temp;

    ifstream infile(temp, ios::binary);
    ofstream outfile(database, ios::binary);
    string line;
    if (infile)
        while (infile.good())
        {
            getline(infile, line);
            if (line != "")
                outfile << line << "\n";
        }

    infile.close();
    outfile.close();
    /* ################  DO NOT MODIFY CODE IN THIS BLOCK ###################### */

    //prompt the user to enter name of batch file, and store it in batch variable
    cout << "Enter batch file name: ";
    cin >> batch;
    cout << '\n';

    //create an input filestream object for the batch file (reading)
    ifstream batchfile(batch, ios::in);

    //create an input/output filestream object for the datafile (reading and writing)
    fstream datfile(database, ios::in | ios::out | ios::binary);

    //check if data file can be opened; if it cannot, print an error to the console and exit
    if (!datfile)
    {
        cerr << "datafile could not be opened for reading/writing.\n";
        return 1;
    }

    //check if batch file can be opened; if it cannot, print an error to the console and exit
    if (!batchfile)
    {
        cerr << "batchfile could not be opened for reading.\n";
        return 1;
    }
    
    //create a string object to store our batch file command lines in
    string commandLine{}; 

    //keep reading in lines from our batch file until we get to end of file
    while (!batchfile.eof()) 
    {
        //while there are still lines to process, store each batchfile line in our commandLine string object
        while(getline(batchfile, commandLine)) 
        {
        //if our command line follows the correct format (which is checked for in our validateCommand function)
        if (validateCommand(commandLine))  
        {
            //if first character is the number '1', call function to add a record to our database file
            if (commandLine[0] == '1')                  
            {
                addRecord(commandLine, datfile);
            }
            //if first character is the number '2', parse our command line for search term, and call function to search for record in our database file
            if (commandLine[0] == '2')                  
            {
                string searchTerm{ commandLine.substr(2) }; //get portion of command line starting from position 2(after space)        
                searchRecord(searchTerm, datfile);
            }
            //if first character is the number '3', call function to edit our record in the database file
            if (commandLine[0] == '3')                
            {
                editRecord(commandLine, datfile);         
            }
            //if first character is the number '4'
            if(commandLine[0] == '4')                 
            {
                //parse our command line for record to delete(get portion of command line starting from position 2-after space)  
                string recordToDelete { commandLine.substr(2) };
                
                //close the data file to prepare for file operations (delete/rename)
                datfile.close();    
                
                //call function to delete the record from our database file
                deleteRecord(recordToDelete, datfile); 
                
                //To resume operations, reopen database file (to ensure file stream is appropriately linked to new file after our file operations)
                datfile.open(database, ios::in | ios::out | ios::binary);      
                
                //check if file has opened sucessfully, printing an error to the console and exiting if it hasn't
                if (!datfile) 
                {
                    cerr << "Failed to reopen the database file.\n";
                    return 1;
                }
            }
        }
        }
    }
    //close both files after all operations are completed
    batchfile.close();
    datfile.close();
    return 0;
}
