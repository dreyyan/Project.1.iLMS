/* iLMS(iSort Library Management System - v2.3) | Adrian Dominic L. Tan | BSCS 1-A */
// Standard Library Headers
#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <string>
#include <thread>
#include <vector>
using namespace std;

// NOTE: Add folder as working directory if file functions are not working
/* --------------------- CLASSES ---------------------
    1. Book - Constructor, Getters/Setters, Validators
        DERIVED:
            |_ Fiction
            |_ Non-Fiction
            |_ E-Book
            |_ Magazine
            |_ Comic
            |_ Textbook

   2. Library - Book Functions
   3. Menu - Login/Register/Library
   ---------------- TABLE OF CONTENTS ----------------
   ----------------------------- FUNCTIONS -----------------------------
   1. createBookfile() [C#1]
   -  creates a book file(.txt) using bookTitle as filename
   ---------------------------------------------------------------------
   2. inputBookInformation() [C#2]
   -  stores book details/information inside book file using validated
    information input
   -  after validation, book is added to book list
   ---------------------------------------------------------------------
   3. readBook() [R]
   -  reads book file line-by-line
   ---------------------------------------------------------------------
   4. updateBookInformation() [U]
   -  edits/updates one book detail/information inside book file
   ---------------------------------------------------------------------
   5. deleteBookfile() [D]
   -  deletes book file and is removed from the book list
   ---------------------------------------------------------------------
   6. showBooklist()
   -  displays book list /w proper format (book_title[book_ISBN])
   ---------------------------------------------------------------------
   7. borrowBook()
   -  book is transferred to 'borrowed_books' folder, is transcripted to
    'borrow_history', and availability is set to 'No'
   ---------------------------------------------------------------------
   8. returnBook()
   -  book is transferred back to the 'main' folder, is transcripted to
    'return_history', and availability is set back to 'Yes'
   ---------------------------------------------------------------------
   9. searchBook()
   -  locates book(if exists), displays ISBN, and suggests to show book
    file content
   ---------------------------------------------------------------------
   10. printBorrowHistory()
   -  prints transcript of borrow_history from start to current,
    formatted along with borrow date
   ---------------------------------------------------------------------
   11. printReturnHistory()
   -  prints transcript of return_history from start to current,
    formatted along with return date
   ---------------------------------------------------------------------
   /exit
   - exits the loop validation
 */

// Current Time
time_t seconds = time(NULL);
struct tm timeFormat = *localtime(&seconds);

// Standard Template Library[STL] Containers
map<string, string> loginCredentials;
vector<string> ISBNList;
map<string, unique_ptr<string>> bookList;
map<unique_ptr<string>, unique_ptr<string>> borrowHistory;
map<unique_ptr<string>, unique_ptr<string>> returnHistory;

void sleepAnimation(int ms_delay) {
    this_thread::sleep_for(std::chrono::milliseconds(ms_delay));
}

class Book { // Book Class
protected:
    // Basic Information
    unique_ptr<string> title, ISBN, genre, author;

    // Publication Details
    unique_ptr<string> publicationDate, edition;

    // Physical Details
    unique_ptr<string> language, description;
    unsigned int pageCount;

    // Library Book Information
    bool availability = true;

public:
    enum class Language { // Enum Class: Language
        English = 1, // Index starts at 1
        Filipino,
        Japanese,
        French,
        Italian,
        German,
        Arabic,
        Chinese
    };

    /* VALIDATORS */
    // VALIDATOR: Basic Information
    void validateBookTitle(string &fileName) { // File Search
        do { // Error Loop
            // Prompt
            cout << "\nBook Title: ";
            getline(cin, fileName);

            // Input Handling: Exit Loop
            if (fileName == "/exit") {
                exit(0);
            }

            // Error Handling: isEmpty
            if (fileName.empty() || isspace(fileName[0])) {
                cerr << "\nERROR | blank_title_input\n";
            }

        } while (fileName.empty());
    }

    string validateBookAuthor() {
        string inputAuthor;

        do { // Error Loop
            // Prompt
            cout << "\nAuthor: ";
            getline(cin, inputAuthor);

            // Input Handling: Exit Loop
            if (inputAuthor == "/exit") {
                exit(0);
            }

            // Error Handling: isEmpty
            if (inputAuthor.empty() || isspace(inputAuthor[0])) {
                cerr << "\nERROR | blank_author_input\n";
            }

        } while (inputAuthor.empty() || isspace(inputAuthor[0]));

        return inputAuthor;
    }

    string validateBookGenre() {
        string inputGenre;

        do { // Error Loop
            // Prompt
            cout << "\nGenre: ";
            getline(cin, inputGenre);

            // Input Handling: Exit Loop
            if (inputGenre == "/exit") {
                exit(0);
            }

            // Error Handling: isEmpty
            if (inputGenre.empty() || isspace(inputGenre[0])) {
                cerr << "\nERROR | blank_genre_input\n";
            }

        } while (inputGenre.empty() || isspace(inputGenre[0]));

        return inputGenre;
    }

    string validateBookISBN() {
        string inputISBN;
        bool isValidISBN;

        do { // Error Loop
            isValidISBN = true;

            // Prompt
            cout << "\nISBN(10-digits): ";
            getline(cin, inputISBN);

            // Input Handling: Exit Loop
            if (inputISBN == "/exit") {
                exit(0);
            }

            // Error Handling: isEmpty
            if (inputISBN.empty() || isspace(inputISBN[0])) {
                isValidISBN = false;
                cerr << "\nERROR | blank_ISBN_input\n";
                continue;
            }

            // Error Handling: validLength
            if (inputISBN.length() != 10) {
                isValidISBN = false;
                cerr << "\nERROR | ISBN_must_be_10_digits\n";
                continue;
            }

            // Loop Checking: isDigit
            for (char &c : inputISBN) {
                if (!isdigit(c)) {
                    isValidISBN = false;
                    break;
                }
            }

            // Error Checking: existingISBN
            for (const auto& existingISBN : ISBNList) {
                if (inputISBN == existingISBN) {
                    isValidISBN = false;
                    cerr << "\nERROR | existing_ISBN";
                    break;
                }
            }
        } while (!isValidISBN || isspace(inputISBN[0]));

        return inputISBN;
    }

    // Getter: ISBN
    string getISBN(const string &bookISBN) const {
        return bookISBN;
    }

    // VALIDATOR: Publication Details
    string validateBookPublicationDate() {
        string inputPublicationDate;
        bool isValidPublicationDate;

        do { // Error Loop
            isValidPublicationDate = true;

            // Prompt
            cout << "\nPublication Date(MM/DD/YYYY): ";
            getline(cin, inputPublicationDate);

            // Input Handling: Exit Loop
            if (inputPublicationDate == "/exit") {
                exit(0);
            }

            // Error Handling: isEmpty
            if (inputPublicationDate.empty() || isspace(inputPublicationDate[0])) {
                isValidPublicationDate = false;
                cerr << "\nERROR | blank_publication_date_input\n";
            }

            // Error Handling: validLength
            else if (inputPublicationDate.length() != 10) {
                isValidPublicationDate = false;
                cerr << "\nERROR | invalid_publication_date_length\n";
            }

            else {
                // Erorr Handling: Date Separators
                if (inputPublicationDate[2] != '/' || inputPublicationDate[5] != '/') {
                    isValidPublicationDate = false;
                    cerr << "\nERROR | invalid_date_separators";
                }

                // Error Handling: Validity & Format
                else {
                    for (size_t i = 0; i < inputPublicationDate.length(); i++) {
                        if (i == 2 || i == 5) {
                            continue;
                        }

                        // Error Handling: Non-Digit
                        if (!isdigit(inputPublicationDate[i])) {
                            isValidPublicationDate = false;
                            cerr << "\nERROR | non_digit_publication_date_input\n";
                            break;
                        }
                    }

                    int currentYear = timeFormat.tm_year + 1900; // Current Year
                    int currentMonth = timeFormat.tm_mon + 1; // Current Month
                    int currentDay = timeFormat.tm_mday; // Current Date

                    int year = (inputPublicationDate[6] - '0') * 1000 + (inputPublicationDate[7] - '0') * 100 + (inputPublicationDate[8] - '0') * 10 + (inputPublicationDate[9] - '0');

                    // Error Handling: Invalid Year
                    if (year < 1900 || year > currentYear) {
                        isValidPublicationDate = false;
                        cerr << "\nERROR | invalid_year\n";
                    }

                    // Error Handling: Invalid Month
                    int month = (inputPublicationDate[0] - '0') * 10 + (inputPublicationDate[1] - '0');
                    if (month < 1 || month > 12) {
                        isValidPublicationDate = false;
                        cerr << "\nERROR | invalid_month\n";
                    }

                    // Error Handling: Invalid Day
                    int day = (inputPublicationDate[3] - '0') * 10 + (inputPublicationDate[4] - '0');
                    if (day < 1 || day > 31) { // Simplistic day check
                        isValidPublicationDate = false;
                        cerr << "\nERROR | invalid_day\n";
                    }

                    // Error Handling: Current Month & Year
                    if (year == currentYear && month == currentMonth) {
                        if (day > currentDay) {
                            isValidPublicationDate = false;
                            cerr << "\nERROR | invalid_day_for_current_month\n";
                        }
                    }
                }
            }

        } while (!isValidPublicationDate);

        return inputPublicationDate;
    }

    string validateBookEdition() {
        string inputBookEdition;
        bool isValidEdition;

        do { // Error Loop
            isValidEdition = true;
            // Prompt
            cout << "\nEdition(blank if not applicable): ";
            getline(cin, inputBookEdition);

            // Input Handling: Exit Loop
            if (inputBookEdition == "/exit") {
                exit(0);
            }

            // Input Handling: isEmpty
            if (inputBookEdition.empty() || isspace(inputBookEdition[0])) {
                return "N/A";
            }

            // Error Handling: validLength
            if (inputBookEdition.length() > 2) {
                isValidEdition = false;
                cerr << "\nERROR | exceeded_edition_input_limit_of_two_digits";
            }

            // Loop Checking: isDigit
            for (char &c : inputBookEdition) {
                // Error Handling: isDigit
                if (!isdigit(c)) {
                    isValidEdition = false;
                    cerr << "\nERROR | non_digit_edition_input";
                    break;
                }
            }

        } while (!isValidEdition);

        return inputBookEdition;
    }

    // VALIDATOR: Physical Details
    string validateBookLanguage() {
        unsigned int languageIndex;
        string languageInput;
        bool isValidLanguage;

        // Display Book Languages[Common]
        cout << "\nEnglish[1] |";
        sleepAnimation(250);
        cout << "\nFilipino[2] |";
        cout <<"\nJapanese[3] |";
        cout << "\nFrench[4] |";
        cout << "\nItalian[5] |";
        cout << "\nGerman[6] |";
        cout << "\nArabic[7] |";
        cout << "\nChinese[8]";

        do { // Error Loop
            isValidLanguage = true;

            // Prompt
            cout << "\nLanguage: ";
            cin >> languageIndex;

            // Error Handling: isEmpty
            if (cin.fail()) {
                isValidLanguage = false;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cerr << "\nERROR | blank_language_input\n";
            }

            // Error Handling: outOfBoundsIndex
            else if (languageIndex < 1 || languageIndex > 8) {
                isValidLanguage = false;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cerr << "\nERROR | out_of_bounds_language_index\n";
            }

            // Conversion: indexToEnum
            if (isValidLanguage) {
                switch (languageIndex) {
                case 1:
                    languageInput = "English";
                    break;
                case 2:
                    languageInput = "Filipino";
                    break;
                case 3:
                    languageInput = "Japanese";
                    break;
                case 4:
                    languageInput = "French";
                    break;
                case 5:
                    languageInput = "Italian";
                    break;
                case 6:
                    languageInput = "German";
                    break;
                case 7:
                    languageInput = "Arabic";
                    break;
                case 8:
                    languageInput = "Chinese";
                    break;
                default:
                    // Error Handling: nonIndex
                    isValidLanguage = false;
                    cerr << "\nERROR | invalid_language_index\n";
                    break;
                }
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

        } while (!isValidLanguage);

        return languageInput;
    }

    unsigned int validateBookPageCount() {
        unsigned int inputPageCount;

        do { // Error Loop
            // Prompt
            cout << "\n# of Pages: ";
            cin >> inputPageCount;

            // Error Handling: isEmpty
            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cerr << "\n ERROR | blank_page_count\n";
            }

        } while (cin.fail());
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        return inputPageCount;
    }

    string validateBookContent() {
        string inputBookContent;
        bool isValidContent;

        do { // Error Loop
            isValidContent = true;
            // Prompt
            cout << "\nContent Must Be...: ";
            sleepAnimation(250);
            cout << "\n* 10 Words < Word Length < 300 Words";
            sleepAnimation(250);
            cout << "\n* Ending With '|': ";
            sleepAnimation(250);
            cout << "\nContent:\n";
            sleepAnimation(250);

            getline(cin, inputBookContent); // Input Content

            // Input Handling: Exit Loop
            if (inputBookContent == "/exit") {
                exit(0);
            }

            // Error Handling: isEmpty
            if (inputBookContent.empty() || isspace(inputBookContent[0])) {
                isValidContent = false;
                cerr << "\nERROR | blank_book_content\n";
                continue;
            }

            // Error Handling: No Sentinel
            if (inputBookContent.back() != '|') {
                isValidContent = false;
                cerr << "\nERROR | no_ending_'|'_symbol";
                continue;
            }

            inputBookContent.pop_back(); // Remove Sentinel '|' Temporarily

            unsigned int wordCount = 0;
            bool isWord = false;

                // Loop Checking: Word Count
                for (size_t i = 0; i < inputBookContent.length(); i++) {
                    // Word Checking: Space Handling
                    if (inputBookContent[i] == ' ') {
                        if (isWord) {
                            isWord = false;
                        }

                        // Error Handling: Double Spaces
                        if (i + 1 < inputBookContent.length() && inputBookContent[i + 1] == ' ') {
                            isValidContent = false;
                            cerr << "\nERROR | double_spaces\n";
                            break;
                        }
                    }

                    // Word Checking: Afterspace Handling
                    else {
                        if (!isWord) {
                            isWord = true;
                            wordCount++; // Every After Whitespace
                        }
                    }
                }

                if (wordCount < 10 || wordCount > 301) { // Max Limit: 300 + 1 [Margin of Error]
                    isValidContent = false;

                    // Error Handling: validLength
                    if (wordCount < 10) {
                        cerr << "\nERROR | under_description_length\n";
                    } else {
                        cerr << "\nERROR | exceeded_description_length\n";
                    }
                }

        } while (!isValidContent);

        return inputBookContent;
    }

    bool validateBookAvailability() const {
        return availability;
    }

    // VALIDATOR: Abstraction
    void validateBasicInformation(string &bookAuthor, string &bookGenre, string &bookISBN) {
        bookAuthor = validateBookAuthor();
        bookGenre = validateBookGenre();
        bookISBN = validateBookISBN();
    }

    void validatePublicationDetails(string &bookPublicationDate, string &bookEdition) {
        bookPublicationDate = validateBookPublicationDate();
        bookEdition = validateBookEdition();
    }

    void validatePhysicalDetails(string &bookLanguage, unsigned int &bookPageCount, string &bookContent) {
        bookLanguage = validateBookLanguage();
        bookPageCount = validateBookPageCount();
        bookContent = validateBookContent();
    }

    void validateLibraryBookInformation(bool &availability) {
        availability = validateBookAvailability();
    }

    // Error Handling: Book File Already Exists
    bool alreadyExists(const string &bookTitle) {
        for (const auto&  book : bookList) {
            if (book.first == bookTitle) {
                return true;
                break;
            }
        }

        return false;
    }

    /* CONSTRUCTOR */
    // CONSTRUCTOR: Default
    Book()
        : title(make_unique<string>("")), author(make_unique<string>("")), genre(make_unique<string>("")), ISBN(make_unique<string>("")), publicationDate(make_unique<string>("")), edition(make_unique<string>("")), language(make_unique<string>("")), description(make_unique<string>("")), pageCount(0), availability(true) {}

    // CONSTRUCTOR: Initialization [Debugging]
    Book(unique_ptr<string> bookTitle,
         unique_ptr<string> bookAuthor,
         unique_ptr<string> bookGenre,
         unique_ptr<string> bookISBN,
         unique_ptr<string> bookPublicationDate,
         unique_ptr<string> bookEdition,
         unique_ptr<string> bookLanguage,
         unique_ptr<string> bookContent,
         unsigned int bookPageCount,
         bool bookAvailability)
        : title(move(bookTitle)), author(move(bookAuthor)), genre(move(bookGenre)), ISBN(move(bookISBN)), publicationDate(move(bookPublicationDate)), edition(move(bookEdition)), language(move(bookLanguage)), description(move(bookContent)), pageCount(bookPageCount), availability(bookAvailability) {
    }

    // CONSTRUCTOR: Book File <- addBook();
    Book(const string &textDirectory) {
        string bookTitle;
        bool bookAlreadyExists;

        // Prompt Book Title as File Name
        do { // Error Loop
            validateBookTitle(bookTitle);
            bookAlreadyExists = alreadyExists(bookTitle);

            // Create File Path: If Non-Existing Book
            if (!bookAlreadyExists) {
                ofstream openFile(textDirectory + "\\" + bookTitle + ".txt"); // Create File Path

                // Create Book: Book File is Accessable && Book File Doesn't Exist
                if (openFile) {
                        bookList[bookTitle] = make_unique<string>("N/A");

                    cout << "\n>> Book created successfully <<";
                    sleepAnimation(250);
                    break;
                }

                // Error Message: Book File is Not Accessable
                else  {
                    cerr << "\nERROR | cannot_open_file\n";
                    break;
                }
            }

            // Error Message: Book File Already Exists
            else {
                cerr << "\nERROR | book_already_exists\n";
            }

        } while (bookAlreadyExists);
    }
};

// CONSTRUCTOR: Initialization [Debugging]
class Fiction : public Book {
public:
    Fiction(unique_ptr<string> fictionBookTitle,
     unique_ptr<string> fictionBookAuthor,
     unique_ptr<string> fictionBookGenre,
     unique_ptr<string> fictionBookISBN,
     unique_ptr<string> fictionBookPublicationDate,
     unique_ptr<string> fictionBookEdition,
     unique_ptr<string> fictionBookLanguage,
     unique_ptr<string> fictionBookContent,
     unsigned int fictionBookPageCount,
     bool fictionBookAvailability)
    : Book(move(fictionBookTitle), move(fictionBookAuthor), move(fictionBookGenre), move(fictionBookISBN), move(fictionBookPublicationDate), move(fictionBookEdition), move(fictionBookLanguage), move(fictionBookContent), fictionBookPageCount, fictionBookAvailability) {}
};

// CONSTRUCTOR: Initialization [Debugging]
class NonFiction : public Book {
public:
    NonFiction(unique_ptr<string> nonFictionBookTitle,
     unique_ptr<string> nonFictionBookAuthor,
     unique_ptr<string> nonFictionBookGenre,
     unique_ptr<string> nonFictionBookISBN,
     unique_ptr<string> nonFictionBookPublicationDate,
     unique_ptr<string> nonFictionBookEdition,
     unique_ptr<string> nonFictionBookLanguage,
     unique_ptr<string> nonFictionBookContent,
     unsigned int nonFictionBookPageCount,
     bool nonFictionBookAvailability)
    : Book(move(nonFictionBookTitle), move(nonFictionBookAuthor), move(nonFictionBookGenre), move(nonFictionBookISBN), move(nonFictionBookPublicationDate), move(nonFictionBookEdition), move(nonFictionBookLanguage), move(nonFictionBookContent), nonFictionBookPageCount, nonFictionBookAvailability) {}
};

// CONSTRUCTOR: Initialization [Debugging]
class EBook : public Book {
public:
    EBook(unique_ptr<string> ebookBookTitle,
     unique_ptr<string> ebookBookAuthor,
     unique_ptr<string> ebookBookGenre,
     unique_ptr<string> ebookBookISBN,
     unique_ptr<string> ebookBookPublicationDate,
     unique_ptr<string> ebookBookEdition,
     unique_ptr<string> ebookBookLanguage,
     unique_ptr<string> ebookBookContent,
     unsigned int ebookBookPageCount,
     bool ebookBookAvailability)
    : Book(move(ebookBookTitle), move(ebookBookAuthor), move(ebookBookGenre), move(ebookBookISBN), move(ebookBookPublicationDate), move(ebookBookEdition), move(ebookBookLanguage), move(ebookBookContent), ebookBookPageCount, ebookBookAvailability) {}
};

// CONSTRUCTOR: Initialization [Debugging]
class Magazine : public Book {
public:
    Magazine(unique_ptr<string> magazineBookTitle,
     unique_ptr<string> magazineBookAuthor,
     unique_ptr<string> magazineBookGenre,
     unique_ptr<string> magazineBookISBN,
     unique_ptr<string> magazineBookPublicationDate,
     unique_ptr<string> magazineBookEdition,
     unique_ptr<string> magazineBookLanguage,
     unique_ptr<string> magazineBookContent,
     unsigned int magazineBookPageCount,
     bool magazineBookAvailability)
    : Book(move(magazineBookTitle), move(magazineBookAuthor), move(magazineBookGenre), move(magazineBookISBN), move(magazineBookPublicationDate), move(magazineBookEdition), move(magazineBookLanguage), move(magazineBookContent), magazineBookPageCount, magazineBookAvailability) {}
};

// CONSTRUCTOR: Initialization [Debugging]
class Comic : public Book {
public:
    Comic(unique_ptr<string> comicBookTitle,
     unique_ptr<string> comicBookAuthor,
     unique_ptr<string> comicBookGenre,
     unique_ptr<string> comicBookISBN,
     unique_ptr<string> comicBookPublicationDate,
     unique_ptr<string> comicBookEdition,
     unique_ptr<string> comicBookLanguage,
     unique_ptr<string> comicBookContent,
     unsigned int comicBookPageCount,
     bool comicBookAvailability)
    : Book(move(comicBookTitle), move(comicBookAuthor), move(comicBookGenre), move(comicBookISBN), move(comicBookPublicationDate), move(comicBookEdition), move(comicBookLanguage), move(comicBookContent), comicBookPageCount, comicBookAvailability) {}
};

// CONSTRUCTOR: Initialization [Debugging]
class Textbook : public Book {
public:
    Textbook(unique_ptr<string> textbookBookTitle,
     unique_ptr<string> textbookBookAuthor,
     unique_ptr<string> textbookBookGenre,
     unique_ptr<string> textbookBookISBN,
     unique_ptr<string> textbookBookPublicationDate,
     unique_ptr<string> textbookBookEdition,
     unique_ptr<string> textbookBookLanguage,
     unique_ptr<string> textbookBookContent,
     unsigned int textbookBookPageCount,
     bool textbookBookAvailability)
    : Book(move(textbookBookTitle), move(textbookBookAuthor), move(textbookBookGenre), move(textbookBookISBN), move(textbookBookPublicationDate), move(textbookBookEdition), move(textbookBookLanguage), move(textbookBookContent), textbookBookPageCount, textbookBookAvailability) {}
};

class Library { // Library Class
protected:
    Book &bookReference; // Reference to Book

public:
    Library(Book &bookReference) : bookReference(bookReference) { /* Library Constructor */ }

    //  Getter: File Path
    string getFilePath(const string &bookTitle) const {
        return "txt_files\\" + bookTitle + ".txt";
    }

    /* FILE */
    // FILE: Locate
    void locateBookFile(bool &fileFound, const string &bookTitle) {
        cout << "\n>> locating book file...";

        string filePath = getFilePath(bookTitle); // Get File Path
        ifstream fileSearcher(filePath); // File Search

        // Error Handling: Accessable File
        if (!fileSearcher.is_open()) {
            fileFound = false;
            return;
        }

        fileFound = true;
        cout << "\nBook File Found!\n";
    }

    // FILE: Input
    void inputValidatedInformation(const string &filePath, const string &bookTitle, const string &bookAuthor, const string &bookGenre, const string &bookISBN, const string &bookPublicationDate, const string &bookEdition, const string &bookLanguage, const string &bookContent, const unsigned int &bookPageCount, const bool &availability) {
        cout << "\n>> updating basic information in book file...";
        sleepAnimation(250);
        ofstream enterText(filePath); // Open File

        // Validation Message
        if (enterText.is_open()) {
            enterText << "BASIC INFORMATION |\n";
            sleepAnimation(250);
            enterText << "Title: " << bookTitle << "\n";
            sleepAnimation(250);
            enterText << "Author: " << bookAuthor << "\n";
            sleepAnimation(250);
            enterText << "Genre: " << bookGenre << "\n";
            sleepAnimation(250);
            enterText << "ISBN: " << bookISBN << "\n";
            sleepAnimation(250);
            enterText << "\nPUBLICATION INFORMATION |\n";
            sleepAnimation(250);
            enterText << "Publication Date: " << bookPublicationDate << "\n";
            sleepAnimation(250);
            enterText << "Edition: " << bookEdition << "\n";
            sleepAnimation(250);
            enterText << "\nPHYSICAL INFORMATION |\n";
            sleepAnimation(250);
            enterText << "Language: " << bookLanguage << "\n";
            sleepAnimation(250);
            enterText << "# of Pages: " << bookPageCount << "\n";
            sleepAnimation(250);
            enterText << "\nLIBRARY INFORMATION |\n";
            sleepAnimation(250);
            enterText << "Available?: " << (availability? "/" : "X") << "\n";
            sleepAnimation(250);
            enterText << "\nContent:\n" << bookContent << "\n";
            sleepAnimation(250);
        }
    }

    // FILE: Read
    void readBookfile(const string &bookTitle) {
        cout << "\n>> reading book file...";
        sleepAnimation(250);
        string line, filePath = getFilePath(bookTitle); // Getter
        ifstream bookFileReader(filePath); // File Reader


        // Read Text File: Accessable File
        if (bookFileReader) {
            cout << "\nBook File Found!\n";
            sleepAnimation(250);
            while (getline(bookFileReader, line)) {
                cout << line << "\n";
            }
            bookFileReader.close();
        }

        // Error Handling: Inaccessable File
        else {
            cerr << "\nERROR | inaccessible_book_file";
        }
    }

    // >> [C]REATE Book [1]
    void createBookfile() {
        cout << "\n===+==+==+== iSort ==+==+==+===";
        sleepAnimation(250);
        cout << "\n| -Library-Management-System- |";
        sleepAnimation(250);
        cout << "\n  -_-_-_-[CREATE BOOK]-_-_-_-";
        sleepAnimation(250);
        cout << "\n['/exit' to Exit Function Validations ]";
        sleepAnimation(250);
        Book("txt_files");
    }

    // >> Input Book Information [2]
    void inputBookInformation() {
        // Book Search Variables
        string bookTitle;
        bool fileFound;

        // Book File Variables
        string bookAuthor, bookGenre, bookISBN, bookPublicationDate, bookLanguage, bookContent, bookEdition;
        unsigned int bookPageCount, bookAvailability = true;

        cout << "\n===+==+==+== iSort ==+==+==+===";
        sleepAnimation(250);
        cout << "\n| -Library-Management-System- |";
        sleepAnimation(250);
        cout << "\n -_-_-[UPDATE BOOK INFO]-_-_-";
        sleepAnimation(250);
        cout << "\n['/exit' to Exit Function Validations ]";
        sleepAnimation(250);

        do { // Error Loop
            // Prompt
            cout << "\nSearch Book | Title: ";
            getline(cin, bookTitle);

            // Input Handling: Exit Loop
            if (bookTitle == "/exit") {
                exit(0);
            }

            // Error Handling: isEmpty
            if (bookTitle.empty() || isspace(bookTitle[0])) {
                cerr << "\nERROR | blank_book_title\n";
                continue;
            }

            locateBookFile(fileFound, bookTitle); // Search for File

            // Validate Information: Accessable File
            if (fileFound) {
                system("cls");
                string filePath = getFilePath(bookTitle);
                cout << "\nBASIC INFORMATION |";
                bookReference.validateBasicInformation(bookAuthor, bookGenre, bookISBN);
                system("cls");
                cout << "\nPUBLICATION INFORMATION |";
                bookReference.validatePublicationDetails(bookPublicationDate, bookEdition);
                system("cls");
                cout << "\nPHYSICAL INFORMATION |";
                bookReference.validatePhysicalDetails(bookLanguage, bookPageCount, bookContent);
                // Input Validated Information to Book File
                system("cls");
                inputValidatedInformation(filePath, bookTitle, bookAuthor, bookGenre, bookISBN, bookPublicationDate, bookEdition, bookLanguage, bookContent, bookPageCount, bookAvailability);
                readBookfile(bookTitle);
                bookList[bookTitle] = make_unique<string>(bookISBN);
            }

            // Error Handling: Accessable File
            else {
                cerr << "\nERROR | file_does_not_exist\n";
            }

        } while (bookTitle.empty() || isspace(bookTitle[0]) || !fileFound);
    }

    // >> [U]PDATE Book Information [3]
    void updateBookInformation() {
        string bookTitle;
        vector<string> currentLines;
        string line;
        int lineNumber;

        cout << "\n===+==+==+== iSort ==+==+==+===";
        sleepAnimation(250);
        cout << "\n| -Library-Management-System- |";
        sleepAnimation(250);
        cout << "\n-_-[UPDATE BOOK INFORMATION]-_-";
        sleepAnimation(250);
        cout << "\n['/exit' to Exit Function Validations ]";
        sleepAnimation(250);

        do { // Error Loop
            // Prompt
            cout << "\nSearch Book | Title: ";
            getline(cin, bookTitle);

            // Input Handling: Exit Loop
            if (bookTitle == "/exit") {
                exit(0);
            }

            // Error Handling: isEmpty
            if (bookTitle.empty() || isspace(bookTitle[0])) {
                cerr << "\nERROR | blank_book_title_input";
            }

        } while (bookTitle.empty() || isspace(bookTitle[0]));

        string filePath = getFilePath(bookTitle); // Getter
        ifstream bookFileReader(filePath); // File Reader

        // Display Current Content
        if (bookFileReader) {
            system("cls");
            cout << "\nBook File Found!\n\n";
            sleepAnimation(250);
            cout << "File Name: " << bookTitle << ".txt";
            sleepAnimation(250);
            cout << "\n-------------------------CONTENT-------------------------\n";
            sleepAnimation(250);

        int numTrack = 1;

        // Read Content
        while (getline(bookFileReader, line)) {
            currentLines.push_back(line);
            if (numTrack == 1 || numTrack == 7 || numTrack == 11 || numTrack == 15 || numTrack == 18) {
                cout << line << "\n"; // Print Line /wo Line Number
                sleepAnimation(250);
            } else {
                cout << currentLines.size() << " | " << line << "\n"; // Print Line /w Line Number
            }
            numTrack++;
        }
            bookFileReader.close();
            cout << "---------------------------------------------------------\n";
            sleepAnimation(250);

        bool invalidLineNumber;

        do { // Error Loop
            invalidLineNumber = false;
            // Prompt
            cout << "\nChange Line No.: ";
            cin >> lineNumber;

            // Error Handling: invalidInput
            if (cin.fail() || lineNumber < 1 || lineNumber == 1 || lineNumber == 7 || lineNumber == 11 || lineNumber == 15 || lineNumber == 18 || lineNumber > currentLines.size()) {
                invalidLineNumber = true;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cerr << "\nERROR | invalid_line_number_input\n";
            }

        } while (invalidLineNumber || lineNumber > currentLines.size());

            string category;
            bool isPlaced = false;

            switch(lineNumber) {
                case 2:
                    category = "Title: ";
                    break;
                case 3:
                    category = "Author: ";
                break;
                case 4:
                    category = "Genre: ";
                break;
                case 5:
                    category = "ISBN: ";
                break;
                case 8:
                    category = "Publication Date: ";
                break;
                case 9:
                    category = "Edition: ";
                break;
                case 12:
                    category = "Language: ";
                break;
                case 13:
                    category = "# of Pages: ";
                break;
                case 16:
                    category = "Available?: ";
                break;
                default:
                    category = "N/A";
            }

            // Prompt New Content
            system("cls");
            cout << "\nCurrent Content: " << currentLines[lineNumber - 1];
            sleepAnimation(250);
            cin.ignore();
            cout << "\nChange Content(Line " << lineNumber << "): ";
            sleepAnimation(250);
            getline(cin, currentLines[lineNumber - 1]);

            // Change Content
            ofstream bookFileWriter(filePath);
            for (size_t i = 0; i < currentLines.size(); ++i) {
                if (i == lineNumber - 1 && !isPlaced && !category.empty()) {
                    bookFileWriter << category << currentLines[i] << "\n";
                    isPlaced = true;
                } else {
                    bookFileWriter << currentLines[i] << "\n";
                }
            }

            // Close Book
            bookFileWriter.close();
            system("cls");
            cout << "\nLine " << lineNumber << " updated successfully!";
            sleepAnimation(250);
        }

        else {
            cerr << "\nERROR | cannot_open_file ";
        }
    }

    // >> [R]EAD Book [4]
    void readBookfile() {
        string bookTitle, line;
        bool fileFound = true;

        cout << "\n===+==+==+== iSort ==+==+==+===";
        sleepAnimation(250);
        cout << "\n| -Library-Management-System- |";
        sleepAnimation(250);
        cout << "\n -_-_-_-_-[READ BOOK]-_-_-_-_-";
        sleepAnimation(250);
        cout << "\n['/exit' to Exit Function Validations ]";
        sleepAnimation(250);

        // Loop Iteration: Display
        size_t counter = 1;
        for (const auto& book : bookList) {
            cout << "\n" << counter << ". " << book.first << " [" << *book.second << "]";
            counter++;
        }

        do { // Error Loop
            // Prompt
            cout << "\nBook Title: ";
            getline(cin, bookTitle);

            // Input Handling: Exit Loop
            if (bookTitle == "/exit") {
                exit(0);
            }

            // Error Handling: isEmpty
            if (bookTitle.empty() || isspace(bookTitle[0])) {
                cerr << "\nERROR | blank_book_title_input";
            }

        } while (bookTitle.empty() || isspace(bookTitle[0]));

        string filePath = getFilePath(bookTitle); // Getter
        ifstream bookFileReader(filePath); // File Reader

        // Read File: Accessable File
        if (bookFileReader) {
            system("cls");
            cout << "\nBook File Found!\n\n";
            sleepAnimation(250);
            cout << "\nFile Name: " << bookTitle << ".txt";
            sleepAnimation(250);
            cout << "\n-------------------------CONTENT-------------------------|\n";
            sleepAnimation(250);

            // Display Content
            while (getline(bookFileReader, line)) {
                cout << line << "\n";
                sleepAnimation(250);
            }

            cout << "---------------------------------------------------------|\n";
            sleepAnimation(250);
            bookFileReader.close();
        }

        // Error Handling: Accessable File
        else {
            cerr << "\nERROR | inaccessible_book_file";
        }
    }

    // >> [D]ELETE Book [5]
    void deleteBookfile() {
        string bookTitle;
        string filePath = getFilePath(bookTitle);

        cout << "\n===+==+==+== iSort ==+==+==+===";
        sleepAnimation(250);
        cout << "\n| -Library-Management-System- |";
        sleepAnimation(250);
        cout << "\n-_-_-_-_-[DELETE BOOK]-_-_-_-_-";
        sleepAnimation(250);
        cout << "\n['/exit' to Exit Function Validations ]";
        sleepAnimation(250);

        // Loop Iteration: Display
        size_t counter = 1;
        for (const auto& book : bookList) {
            cout << "\n" << counter << ". " << book.first << " [" << *book.second << "]";
            counter++;
        }

        do { // Error Loop
            // Prompt
            cout << "\nBook Title: ";
            getline(cin, bookTitle);

            // Input Handling: Exit Loop
            if (bookTitle == "/exit") {
                exit(0);
            }

            // Error Handling: isEmpty
            if (bookTitle.empty() || isspace(bookTitle[0])) {
                cerr << "\nERROR | blank_book_title_input";
            }

        } while (bookTitle.empty() || isspace(bookTitle[0]));

        system("cls");
        filePath = getFilePath(bookTitle); // Getter
        if (remove(filePath.c_str()) == 0) { // Converts File Path to C-Style String, Removes File Using File Path, Returns 0 or 1
            cout << "\nBook File Deleted!\n\n";

            // Map Handling: Erase Book File in Booklist
            for (const auto& book : bookList) {
                if (book.first == bookTitle) {
                    bookList.erase(book.first);
                    break;
                }
            }
        }

        // Error Handling: File Non-Existing
        else {
            cerr << "\nERROR | file_does_not_exist";
        }
    }

    // >> Show Booklist [6]
    void showBooklist() {
        cout << "\n===+==+==+== iSort ==+==+==+===";
        sleepAnimation(250);
        cout << "\n| -Library-Management-System- |";
        sleepAnimation(250);
        cout << "\n -_-_-_-_-[BOOK LIST]-_-_-_-_-";
        sleepAnimation(250);

        // Display If No Book Files in Booklist
        if (bookList.size() == 0) {
            char choice;
            cout << "\nThere are currently no books in the booklist.";
            sleepAnimation(250);

            do { // Error Loop
                // Prompt
                cout << "\nWould you like to add one?[y/n]:";
                sleepAnimation(250);
                cout << "\n>> ";
                sleepAnimation(250);

                cin >> choice;
                cin.ignore();

                // Error Handling
                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cerr << "\nERROR | invalid_input";
                }

                // Input Handling: Create Book File
                if (choice == 'y' || choice == 'Y') {
                    system("cls");
                    createBookfile();
                }

                // Input Handling: Return Void
                else if (choice == 'n' || choice == 'N') {
                    return;
                }

                else {
                    cout << "\nInvalid Input.\n";
                }

            } while (cin.fail());
        }

        // Loop Iteration: Display
        size_t counter = 1;
        for (const auto& book : bookList) {
            cout << "\n" << counter << ". " << book.first << " [" << *book.second << "]";
            counter++;
        }
    }

    // >> Borrow Book [7]
    void borrowBook() {
        string bookTitle, bookISBN, line;
        int lineNumber = 16;
        bool bookExists = false, invalidBorrow = false;
        vector<string> currentLines;

        cout << "\n===+==+==+== iSort ==+==+==+===";
        sleepAnimation(250);
        cout << "\n| -Library-Management-System- |";
        sleepAnimation(250);
        cout << "\n-_-_-_-_-[BORROW BOOK]-_-_-_-_-";
        sleepAnimation(250);
        cout << "\n['/exit' to Exit Function Validations ]";
        sleepAnimation(250);

        // Display available books
        size_t counter = 1;
        for (const auto& book : bookList) {
            cout << "\n" << counter << ". " << book.first << " [" << *book.second << "]";
            counter++;
        }

        do { // Error Handling
            cout << "\nBook Title: ";
            getline(cin, bookTitle);

            // Input Handling: Exit Code
            if (bookTitle == "/exit") {
                exit(0);
            }

            // Error Handling: isEmpty
            if (bookTitle.empty()) {
                cerr << "\nERROR | blank_book_title";
            }

        } while (bookTitle.empty());

        // Check if the book exists
        for (const auto& book : bookList) {
            if (book.first == bookTitle) {
                bookExists = true;
                bookISBN = *book.second;
                break;
            }
        }

        // Error Handling: Book Non-Existing
        if (!bookExists) {
            cerr << "\nERROR | book_does_not_exist";
            return;
        }

        // Directories
        string currentPath = "txt_files\\" + bookTitle + ".txt";
        string borrowedBookFolder = "txt_files\\borrowed_books\\" + bookTitle + ".txt";

        // File Handling: isInCurrentPath
        if (ifstream(currentPath)) {

            // File Handling: isMovable
            if (rename(currentPath.c_str(), borrowedBookFolder.c_str()) != 0) {
                cerr << "\nERROR | could_not_move_file";
                return;
            }

            ifstream bookFileReader(borrowedBookFolder); // File Reader

            // Read Content
            while (getline(bookFileReader, line)) {
                currentLines.push_back(line); // Store Content
            }

            bookFileReader.close(); // Close File Reader

            currentLines[lineNumber - 1] = "Availability: X"; // Set Availability

            ofstream bookFileWriter(borrowedBookFolder); // File Writer

            // Display Content
            for (const auto& modifiedLine : currentLines) {
                bookFileWriter << modifiedLine << "\n";
            }

            bookFileWriter.close(); // Close File Writer

            system("cls");
            cout << "\nSuccessfully borrowed: " << bookTitle;
            sleepAnimation(250);
            borrowHistory[make_unique<string>(bookTitle)] = make_unique<string>(bookISBN); // Add to Borrow History
        }

            // Error Handling: borrowedBook
            else {
                cerr << "\nERROR | book_is_currently_borrowed";
            }
    }

    // >> Return Book [8]
    void returnBook() {
        string bookTitle, bookISBN, line;
        int lineNumber = 16;
        bool bookExists = false, invalidReturn = false;
        vector<string> currentLines;

        cout << "\n===+==+==+== iSort ==+==+==+===";
        sleepAnimation(250);
        cout << "\n| -Library-Management-System- |";
        sleepAnimation(250);
        cout << "\n-_-_-_-_-[RETURN BOOK]-_-_-_-_-";
        sleepAnimation(250);
        cout << "\n['/exit' to Exit Function Validations ]";
        sleepAnimation(250);

        do { // Error Loop
            // Prompt
            cout << "\nBook Title: ";
            getline(cin, bookTitle);

            // Input Handling: Exit Code
            if (bookTitle == "/exit") {
                exit(0);
            }

            // Error Handling: isEmpty
            if (bookTitle.empty()) {
                cerr << "\nERROR | blank_book_title";
            }

        } while (bookTitle.empty());

        // Check if the book exists
        for (const auto& book : bookList) {
            if (book.first == bookTitle) {
                bookExists = true;
                bookISBN = *book.second;
                break;
            }
        }

        // Error Handling: Non-Existing Book
        if (!bookExists) {
            cerr << "\nERROR | book_does_not_exist";
            return;
        }

        // Directories
        string currentPath = "txt_files\\borrowed_books\\" + bookTitle + ".txt";
        string textFileFolder = "txt_files\\" + bookTitle + ".txt";

        // File Handling: isInCurrentPath
        if (ifstream(currentPath)) {

            // File Handling: isMovable
            if (rename(currentPath.c_str(), textFileFolder.c_str()) != 0) {
                cerr << "\nERROR | could_not_move_file";
                return;
            }

            ifstream bookFileReader(textFileFolder); // File Reader

            // Read Content
            while (getline(bookFileReader, line)) {
                currentLines.push_back(line); // Store Content
            }

            bookFileReader.close(); // Close File Reader

            currentLines[lineNumber - 1] = "Availability: /"; // Set Availability

            ofstream bookFileWriter(textFileFolder); // File Writer

            // Display Content
            for (const auto& modifiedLine : currentLines) {
                bookFileWriter << modifiedLine << "\n";
            }

            bookFileWriter.close(); // Close File Writer

            system("cls");
            cout << "\nSuccessfully returned: " << bookTitle;
            sleepAnimation(250);
            bookList[bookTitle] = make_unique<string>(bookISBN);
            returnHistory[make_unique<string>(bookTitle)] = make_unique<string>(bookISBN);

            // Display available books
            size_t counter = 1;
            for (const auto& book : bookList) {
                cout << "\n" << counter << ". " << book.first << " [" << *book.second << "]";
                counter++;
            }
        }

        // Error Handling: isAlreadyReturned
        else {
            cerr << "\nERROR | book_already_returned";
        }
    }

    // >> Search Book [9]
    void searchBook() {
        string bookTitle;
        bool bookExists;

        cout << "\n===+==+==+== iSort ==+==+==+===";
        sleepAnimation(250);
        cout << "\n| -Library-Management-System- |";
        sleepAnimation(250);
        cout << "\n-_-_-_-_-[SEARCH BOOK]-_-_-_-_-";
        sleepAnimation(250);
        cout << "\n['/exit' to Exit Function Validations ]";
        sleepAnimation(250);

        // Loop Iteration: Display
        size_t counter = 1;
        for (const auto& book : bookList) {
            cout << "\n" << counter << ". " << book.first << " [" << *book.second << "]";
            counter++;
        }

        do { // Error Loop
            bookExists = false;
            // Prompt
            cout << "\nSearch Book | Book Title: ";
            getline(cin, bookTitle);

            // Input Handling: Exit Loop
            if (bookTitle == "/exit") {
                exit(0);
            }

            // Error Handling: isEmpty
            if (bookTitle.empty() || bookTitle[0] == ' ') {
                cerr << "\nERROR | blank_book_title_input";
            }

        } while (bookTitle.empty() || bookTitle[0] == ' ');

        // Loop Handling: bookExists
        for (const auto& book : bookList) {
            if (book.first == bookTitle) {
                bookExists = true;

                if (bookExists) {
                    int choice;
                    system("cls");
                    cout << "\nBook '" << bookTitle << "' found!" << " >> ISBN[" << *book.second << "]"; // Display Book File

                    do { // Error Loop
                        // Prompt
                        cout << "\nShow Contents?: ";
                        sleepAnimation(250);
                        cout << "\n[1] | Yes";
                        sleepAnimation(250);
                        cout << "\n[2] | No";
                        sleepAnimation(250);
                        cout << "\n---------";
                        sleepAnimation(250);
                        cout << "\n>> ";
                        sleepAnimation(250);
                        cin >> choice;

                        // Error Handling: invalidInput
                        if (cin.fail()) {
                            cin.clear();
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            cerr << "\nERROR | invalid_input";
                        }

                    } while (cin.fail());

                    // Input Handling: 1(Display Content)
                    if (choice == 1) {
                        string line;
                        string filePath = getFilePath(bookTitle); // Getter
                        ifstream bookFileReader(filePath); // File Reader

                        // Read File: Accessable File
                        if (bookFileReader) {
                            system("cls");
                            cout << "\nFile Name: " << bookTitle << ".txt";
                            sleepAnimation(250);
                            cout << "\n-------------------------CONTENT-------------------------|\n";
                            sleepAnimation(250);

                            while (getline(bookFileReader, line)) {
                                cout << line << "\n";
                                sleepAnimation(250);
                            }

                            cout << "---------------------------------------------------------|\n";
                            sleepAnimation(250);
                            bookFileReader.close(); // Close File Reader
                        }

                        // Error Handling: Accessable File
                        else {
                            cerr << "\nERROR | inaccessible_book_file";
                        }
                    }

                    // Input Handling: 2(Break Loop)
                    else if (choice == 2) {
                        cout << "\nfinishing search...";
                        break;
                    }
                }
            }
        }
    }

    // >> Print Borrow History [10]
    void printBorrowHistory() {
        cout << "\n===+==+==+== iSort ==+==+==+===";
        sleepAnimation(250);
        cout << "\n| -Library-Management-System- |";
        sleepAnimation(250);
        cout << "\n-_-_-_-[BORROW HISTORY]-_-_-_-";
        sleepAnimation(250);

        // Loop Handling: Print Borrow History
        for (const auto& bHistory : borrowHistory) {
            cout << "\n* " << *bHistory.first << "[" << *bHistory.second << "] borrowed | "
            << timeFormat.tm_mon + 1 << " / " << timeFormat.tm_mday << " / " << timeFormat.tm_year + 1900;
            sleepAnimation(250);
        }
    }

    // >> Print Return History [11]
    void printReturnHistory() {
        cout << "\n===+==+==+== iSort ==+==+==+===";
        cout << "\n| -Library-Management-System- |";
        cout << "\n-_-_-_-[RETURN HISTORY]-_-_-_-";

        // Loop Handling: Print Return History
        for (const auto& rHistory : returnHistory) {
            cout << "\n* " << *rHistory.first << "[" << *rHistory.second << "] returned | "
            << timeFormat.tm_mon + 1 << " / " << timeFormat.tm_mday << " / " << timeFormat.tm_year + 1900;
            sleepAnimation(250);
        }
    }

    // >> Display Table Of Contents [12]
    void displayTableOfContents() {
        cout << "\n              ==+==+==+==+==+== iSort ==+==+==+==+==+==";
        sleepAnimation(250);
        cout << "\n              |       -Library-Management-System-     |\n";
        sleepAnimation(250);
        cout << "_-_-_-_-_-_-_-_-_-_-_-_-_-[TABLE OF CONTENTS]-_-_-_-_-_-_-_-_-_-_-_\n";
        sleepAnimation(250);
        cout << "----------------------------- FUNCTIONS ----------------------------\n";
        sleepAnimation(250);
        cout << "1. createBookfile() [C#1]\n";
        sleepAnimation(250);
        cout << "   - creates a book file(.txt) using bookTitle as filename\n";
        sleepAnimation(250);
        cout << "---------------------------------------------------------------------\n";
        sleepAnimation(250);
        cout << "2. inputBookInformation() [C#2]\n";
        sleepAnimation(250);
        cout << "   - stores book details/information inside book file using";
        sleepAnimation(250);
        cout << "\n\tvalidated information input\n";
        sleepAnimation(250);
        cout << "   - after validation, book is added to book list\n";
        sleepAnimation(250);
        cout << "---------------------------------------------------------------------\n";
        sleepAnimation(250);
        cout << "3. readBookfile() [R]\n";
        sleepAnimation(250);
        cout << "   - reads book file line-by-line\n";
        sleepAnimation(250);
        cout << "---------------------------------------------------------------------\n";
        sleepAnimation(250);
        cout << "4. updateBookInformation() [U]\n";
        sleepAnimation(250);
        cout << "   - edits/updates one book detail/information inside book file\n";
        sleepAnimation(250);
        cout << "---------------------------------------------------------------------\n";
        sleepAnimation(250);
        cout << "5. deleteBookfile() [D]\n";
        sleepAnimation(250);
        cout << "   - deletes book file and is removed from the book list\n";
        sleepAnimation(250);
        cout << "---------------------------------------------------------------------\n";
        sleepAnimation(250);
        cout << "6. showBooklist()\n";
        sleepAnimation(250);
        cout << "   - displays book list /w proper format (book_title[book_ISBN])\n";
        sleepAnimation(250);
        cout << "---------------------------------------------------------------------\n";
        sleepAnimation(250);
        cout << "7. borrowBook()\n";
        sleepAnimation(250);
        cout << "   - book is transferred to 'borrowed_books' folder, is";
        sleepAnimation(250);
        cout << "\n\ttranscripted to 'borrow_history', and availability is set to 'No'\n";
        sleepAnimation(250);
        cout << "---------------------------------------------------------------------\n";
        sleepAnimation(250);
        cout << "8. returnBook()\n";
        sleepAnimation(250);
        cout << "   - book is transferred back to the 'main' folder, is transcripted";
        sleepAnimation(250);
        cout << "\n\tto 'return_history', and availability is set back to 'Yes'\n";
        sleepAnimation(250);
        cout << "---------------------------------------------------------------------\n";
        sleepAnimation(250);
        cout << "9. searchBook()\n";
        sleepAnimation(250);
        cout << "   - locates book(if exists), displays ISBN, and suggests to show";
        sleepAnimation(250);
        cout << "\n\tbook file content\n";
        sleepAnimation(250);
        cout << "---------------------------------------------------------------------\n";
        sleepAnimation(250);
        cout << "10. printBorrowHistory()\n";
        sleepAnimation(250);
        cout << "   - prints transcript of borrow_history from start to current,";
        sleepAnimation(250);
        cout << "\n\tformatted along with borrow date\n";
        sleepAnimation(250);
        cout << "---------------------------------------------------------------------\n";
        sleepAnimation(250);
        cout << "11. printReturnHistory()\n";
        sleepAnimation(250);
        cout << "   - prints transcript of return_history from start to current,";
        sleepAnimation(250);
        cout << "\n\tformatted along with return date\n";
        sleepAnimation(250);
        cout << "---------------------------------------------------------------------\n";
        sleepAnimation(250);
        cout << ">> /exit\n";
        sleepAnimation(250);
        cout << "   - exits the loop validation\n";
        sleepAnimation(250);
    }
};

class Menu { // Menu Class: Recursive
protected:
    Library &libraryReference;
public:
    Menu(Library &libraryReference) : libraryReference(libraryReference) { /* Menu Constructor */ }

    void displayRegisterMenu() {
        string username, password;
        bool isRegisteredUsername, isValidUsername, isValidPassword, onRegisterMenu;
        bool hasLetter, hasNumber;

        cout << "\n===+==+==+== iSort ==+==+==+===";
        sleepAnimation(250);
        cout << "\n| -Library-Management-System- |";
        sleepAnimation(250);
        cout << "\n -_-_-_-_-[REGISTER]-_-_-_-_-";
        sleepAnimation(250);

        do { // Error Loop
            isRegisteredUsername = false, isValidUsername = true, hasLetter = false, hasNumber = false, onRegisterMenu = true;
            // Prompt
            cout << "\n[ Character Length: min. 5 / max. 15 | a-z, 1-3 | No Spaces ]";
            sleepAnimation(250);
            cout << "\nUsername: ";
            sleepAnimation(250);
            getline(cin, username);

            // Input Handling: Exit Loop
            if (username == "/exit") {
                exit(0);
            }

            // Error Handling: isEmpty
            if (username.empty() || username[0] == ' ') {
                isValidUsername = false;
                cerr << "\nERROR | blank_username_input";
            }

            // Error Handling: validLength
            if (username.length() < 5 || username.length() > 15) {
                isValidUsername = false;
                if (username.length() < 5) {
                    cerr << "\nERROR | under_minimum_username_length";
                }

                // Error Handling: isValidUsernameLength
                else {
                    cerr << "\nERROR | exceeded_maximum_username_length";
                }
            }

            // Error Handling: isValidUsername
            for (size_t i = 0; i < username.length(); i++) {

                // Error Handling: isSpace
                if (isspace(username[i])) {
                    isValidUsername = false;
                    cerr << "\nERROR | username_must_not_contain_spaces";
                    break;
                }

                // Counter: isLetter
                if (isalpha(username[i])) {
                    hasLetter = true;
                }

                // Counter: isNumber
                if (isdigit(username[i])) {
                    hasNumber = true;
                }

                // Error Handling: isCharacter
                if (!isalnum(username[i])) {
                    isValidUsername = false;
                    cerr << "\nERROR | username_must_not_contain_characters";
                    break;
                }
            }

            // Error Handling: noLetters
            if (!hasLetter) {
                isValidUsername = false;
                cerr << "\nERROR | username_must_contain_a_letter";
            }

            // Error Handling: noNumbers
            if (!hasNumber) {
                isValidUsername = false;
                cerr << "\nERROR | username_must_contain_a_number";
            }

            // Error Handling: isNotExisting
            for (const auto& existingUsername : loginCredentials) {
                if (existingUsername.first == username) {
                    isRegisteredUsername = true;
                    isValidUsername = false;
                    cerr << "\nERROR | username_already_registered";
                    break;
                }
            }

        } while (!isValidUsername);

        do { // Error Loop
            isValidPassword = true, hasLetter = false, hasNumber = false;
            // Prompt
            cout << "\n[ Character Length: min. 5 / max. 15 | a-z, 1-3 | No Spaces ]";
            sleepAnimation(250);
            cout << "\nPassword: ";
            sleepAnimation(250);
            getline(cin, password);

            // Input Handling: Exit Loop
            if (password == "/exit") {
                exit(0);
            }

            // Error Handling: isEmpty
            if (password.empty() || password[0] == ' ') {
                isValidPassword = false;
                cerr << "\nERROR | blank_password_input";
            }

            // Error Handling: validLength
            if (password.length() < 5 || password.length() > 15) {
                isValidPassword = false;

                // Error Handling: isValidPasswordLength
                if (password.length() < 5) {
                    cerr << "\nERROR | under_minimum_password_length";
                }

                // Error Handling: isValidPasswordLength
                else {
                    cerr << "\nERROR | exceeded_maximum_password_length";
                }
            }

            // Error Handling: isSpace
            for (size_t i = 0; i < password.length(); i++) {
                if (isspace(password[i])) {
                    isValidPassword = false;
                    cerr << "\nERROR | password_must_not_contain_spaces";
                    break;
                }

                // Counter: isLetter
                if (isalpha(password[i])) {
                    hasLetter = true;
                }

                // Counter: isNumber
                if (isdigit(password[i])) {
                    hasNumber = true;
                }

                // Error Handling: isCharacter
                if (!isalnum(password[i])) {
                    isValidPassword = false;
                    cerr << "\nERROR | password_must_not_contain_characters";
                    break;
                }
            }

            // Error Handling: noLetters
            if (!hasLetter) {
                isValidPassword = false;
                cerr << "\nERROR | password_must_contain_a_letter";
            }

            // Error Handling: noNumbers
            if (!hasNumber) {
                isValidPassword = false;
                cerr << "\nERROR | password_must_contain_a_number";
            }

        } while (!isValidPassword);

        system("cls");
        // Register Using Login Credentials
        loginCredentials[username] = password;

        cout << "\nuser '" << username << "' registered successfully!";
        int registerMenuChoice;

        do { // Error Loop
            // Prompt
            cout << "\n| -Library-Management-System- |";
            sleepAnimation(250);
            cout << "\n -_-_-_-_-[REGISTER]-_-_-_-_-";
            sleepAnimation(250);
            cout << "\n------------------------";
            sleepAnimation(250);
            cout << "\nProceed to Login Page?";
            sleepAnimation(250);
            cout << "\n[1] | Yes";
            sleepAnimation(250);
            cout << "\n[2] | No";
            sleepAnimation(250);
            cout << "\n------------------------";
            sleepAnimation(250);
            cout << "\n>> ";
            sleepAnimation(250);
            cin >> registerMenuChoice;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            system("cls");

            // Input Handling: 1(Display Login Menu)
            if (registerMenuChoice == 1) {
                onRegisterMenu = false;
                displayLoginMenu();
            }

            // Input Handling: 2(Display Menu)
            else if (registerMenuChoice == 2) {
                onRegisterMenu = false;
                displayMenu();
            }

        } while (onRegisterMenu);
    }

    void displayLoginMenu() {
        string username, password;
        bool usernameExists, correctPassword;

        cout << "\n===+==+==+== iSort ==+==+==+===";
        sleepAnimation(250);
        cout << "\n| -Library-Management-System- |";
        sleepAnimation(250);
        cout << "\n -_-_-_-_-_-[LOGIN]-_-_-_-_-_-";
        sleepAnimation(250);
        do { // Error Loop
            usernameExists = false;
            // Prompt
            cout << "\nUsername: ";
            sleepAnimation(250);
            getline(cin, username);

            // Input Handling: Exit Loop
            if (username == "/exit") {
                exit(0);
            }

            // Input Handling: usernameExists
            for (const auto& existingUsername : loginCredentials) {
                if (existingUsername.first == username) {
                    usernameExists = true;
                    break;
                }
            }

            // Error Handling: noExistingUsername
            if (!usernameExists) {
                cerr << "\nERROR | username_does_not_exist";
            }

        } while (!usernameExists);

        do { // Error Loop
            correctPassword = false;
            // Prompt
            cout << "\nPassword: ";
            sleepAnimation(250);
            getline(cin, password);

            // Input Handling: Exit Loop
            if (password == "/exit") {
                exit(0);
            }

            // Input Handling: correctPassword
            if (loginCredentials[username] == password) {
                correctPassword = true;
            }

            // Error Handling: invalidPassword
            else {
                cerr << "\nERROR | invalid_password";
            }

        } while (!correctPassword);

        system("cls");
        cout << "\nLogin Successful!";
        sleepAnimation(250);
        displayLibraryMenu();
    }

    void displayMenu() {
        int loginMenuChoice;
        do { // Error Loop
            // Prompt
            cout << "\n===+==+==+== iSort ==+==+==+===";
            sleepAnimation(250);
            cout << "\n| -Library-Management-System- |";
            sleepAnimation(250);
            cout << "\n -_-_-_-_-_-[MENU]-_-_-_-_-_-";
            sleepAnimation(250);
            cout << "\n---------------";
            sleepAnimation(250);
            cout << "\n[1] | Login";
            sleepAnimation(250);
            cout << "\n[2] | Register";
            sleepAnimation(250);
            cout << "\n[3] | Exit";
            sleepAnimation(250);
            cout << "\n---------------";
            sleepAnimation(250);
            cout << "\n>> ";
            sleepAnimation(250);
            cin >> loginMenuChoice;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            system("cls");

            // Input Handling: loginMenuChoice
            switch(loginMenuChoice) {
                case 1:
                    displayLoginMenu();
                break;
                case 2:
                    displayRegisterMenu();
                break;
                case 3:
                    cout << "exiting program...\n";
                    exit(0);
                break;
                default:
                    cerr << "Invalid Input. Please try again.";
                break;
            }

        } while(true);
    }

    /* MENU */
    // MENU: Display
    void displayLibraryMenu() {
        int choice;
        do { // Error Loop
            // Prompt
            cout << "\n===+==+==+== iSort ==+==+==+===" << flush;
            sleepAnimation(250);
            cout << "\n| -Library-Management-System- |" << flush;
            sleepAnimation(250);
            cout << "\n-_-_-_-_-_-[LIBRARY]-_-_-_-_-_-" << flush;
            sleepAnimation(250);
            cout << "\n-------------------------------" << flush;
            sleepAnimation(250);
            cout << "\n [1] | Create Book File" << flush;
            sleepAnimation(250);
            cout << "\n [2] | Input Book Information" << flush;
            sleepAnimation(250);
            cout << "\n [3] | Read Book File" << flush;
            sleepAnimation(250);
            cout << "\n [4] | Update Book Information" << flush;
            sleepAnimation(250);
            cout << "\n [5] | Delete Book File" << flush;
            sleepAnimation(250);
            cout << "\n [6] | Show Booklist" << flush;
            sleepAnimation(250);
            cout << "\n [7] | Borrow Book" << flush;
            sleepAnimation(250);
            cout << "\n [8] | Return Book" << flush;
            sleepAnimation(250);
            cout << "\n [9] | Search Book" << flush;
            sleepAnimation(250);
            cout << "\n[10] | Borrow History" << flush;
            sleepAnimation(250);
            cout << "\n[11] | Return History" << flush;
            sleepAnimation(250);
            cout << "\n[12] | Table of Contents" << flush;
            sleepAnimation(250);
            cout << "\n[13] | Exit" << flush;
            sleepAnimation(250);
            cout << "\n-------------------------------" << flush;
            sleepAnimation(250);
            cout << "\n>> ";
            cin >> choice;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            // Input Handling: Choice
            switch (choice) {
            case 1:
                system("cls");
                libraryReference.createBookfile();
                break;
            case 2:
                system("cls");
                libraryReference.inputBookInformation();
                break;
            case 3:
                system("cls");
                libraryReference.readBookfile();
                break;
            case 4:
                system("cls");
                libraryReference.updateBookInformation();
                break;
            case 5:
                system("cls");
                libraryReference.deleteBookfile();
                break;
            case 6:
                system("cls");
                libraryReference.showBooklist();
                break;
            case 7:
                system("cls");
                libraryReference.borrowBook();
                break;
            case 8:
                system("cls");
                libraryReference.returnBook();
                break;
            case 9:
                system("cls");
                libraryReference.searchBook();
                break;
            case 10:
                system("cls");
                libraryReference.printBorrowHistory();
                break;
            case 11:
                system("cls");
                libraryReference.printReturnHistory();
                break;
            case 12:
                system("cls");
                libraryReference.displayTableOfContents();
                break;
            case 13:
                system("cls");
                displayMenu();
                break;
            default:
                system("cls");
                cout << "\nInvalid Input. Please try again.";
                break;
            }
            backToMenu();
            system("cls");
        } while (true);
    }

    // MENU: Return
    void backToMenu() {
        // Prompt
        char choice;
        cout << "\nReturn to menu[y/n]?: ";
        sleepAnimation(250);
        cin >> choice;

        // Input Handling: Return Void
        if (choice == 'y' || choice == 'Y') {
            return;
        }

        // Input Handling: Exit Program
        else if (choice == 'n' || choice == 'N') {
            cout << "\nProgram Exited.";
            exit(0);
        }

        // Input Handling: invalidInput
        else {
            cout << "\nInvalid Input.\n";
            cin >> choice;
        }
    }
};

int main() {
    Book book; // Default Book Constructor for Library
    Library library(book); // Library Constructor for Menu
    unique_ptr<Menu> startProgram = make_unique<Menu>(library); // Function Wrapper: Smart Pointer -> Function
    startProgram->displayMenu(); // [PROGRAM]
    //startProgram->displayLibraryMenu(); // [DEBUGGING]
    return 0;
}
