#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>
using namespace std;

// Get Current Time
time_t seconds = time(NULL);
struct tm timeFormat = *localtime(&seconds);

// Standard Template Library[STL] Containers
map<string, string> loginCredentials;
vector<string> ISBNList;
map<string, unique_ptr<string>> bookList;
map<unique_ptr<string>, unique_ptr<string>> borrowHistory;
map<unique_ptr<string>, unique_ptr<string>> returnHistory;

/* UTILITIES*/
// UTILITY: Mimic a delay within `ms` milliseconds
void sleepAnimation(int ms_delay) {
    this_thread::sleep_for(std::chrono::milliseconds(ms_delay));
}

// UTILITY: Mimic a line delay within `ms` milliseconds (for ASCII art animation)
mutex MUTEX;
void sleepAnimation(const string& line, int delay) {
    this_thread::sleep_for(chrono::milliseconds(delay));
    lock_guard<mutex> lock(MUTEX);
    cout << line << '\n';
}

// UTLITY: Prompt user to Press 'Enter' key before proceeding
void pressEnterToContinue() {
    cout << "Press 'Enter' to continue...";
    cin.get();
}

// UTILITY: Display ASCII art animation
void ASCIIArtAnimation() {
    vector<string> asciiArt = {
        "                  ___       ___           ___     ",
        "      ___        /\\__\\     /\\__\\         /\\  \\    ",
        "     /\\  \\      /:/  /    /::|  |       /::\\  \\   ",
        "     \\:\\  \\    /:/  /    /:|:|  |      /:/\\ \\  \\  ",
        "     /::\\__\\  /:/  /    /:/|:|__|__   _\\:\\~\\ \\  \\ ",
        "  __/:/\\/__/ /:/__/    /:/ |::::\\__\\ /\\ \\:\\ \\ \\__\\",
        " /\\/:/  /    \\:\\  \\    \\/__/~~/:/  / \\:\\ \\:\\ \\/__/",
        " \\::/__/      \\:\\  \\         /:/  /   \\:\\ \\:\\__\\  ",
        "  \\:\\__\\       \\:\\  \\       /:/  /     \\:\\/:/  /  ",
        "   \\/__/        \\:\\__\\     /:/  /       \\::/  /   ",
        "                 \\/__/     \\/__/         \\/__/    "
    };

    cout << "";
    this_thread::sleep_for(chrono::seconds(1));

    vector<thread> threads;
    int ms_delay = 25;

    for (size_t i = 0; i < asciiArt.size(); ++i) {
        threads.emplace_back([&, i]() {
            sleepAnimation(asciiArt[i], ms_delay * i);
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    this_thread::sleep_for(chrono::seconds(2));
    system("cls");
}

// UTILITY: Mimic loading animation '...'
void loadingAnimation(int ms_delay, size_t iterations) {
    for (size_t i = 0; i < iterations; i++) {
        cout << '.'; sleepAnimation(ms_delay);
    }
}

// UTILITY: Display TUI header
void displayHeader() {
    cout << "+==+==+==+==+==+==+== iSort ==+==+==+==+==+==+==+\n" << flush;
    sleepAnimation(100);
    cout << "  |        -Library-Management-System-        |\n" << flush;
    sleepAnimation(100);
}

// UTILITY: Dispaly TUI formatting
void displayFormat(size_t iterations) {
    for (size_t i = 0; i < iterations; i++) {
        cout << '-';
    } cout << '\n';
}

void displayNoSpaceFormat(size_t iterations) {
    for (size_t i = 0; i < iterations; i++) {
        cout << '-';
    }
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
            cout << "Create | Book Title: ";
            getline(cin, fileName);

            // Input Handling: Exit Loop
            if (fileName == "/exit") {
                return;
            }

            // Error Handling: isEmpty
            if (fileName.empty() || isspace(fileName[0])) {
                cerr << "ERROR | blank_title_input\n";
                sleepAnimation(1000);
                system("cls");
            }

        } while (fileName.empty());
    }

    string validateBookAuthor() {
        string inputAuthor;

        do { // Error Loop
            // Prompt
            cout << "Author: ";
            getline(cin, inputAuthor);

            // Input Handling: Exit Loop
            if (inputAuthor == "/exit") {
                return "";
            }

            // Error Handling: isEmpty
            if (inputAuthor.empty() || isspace(inputAuthor[0])) {
                cerr << "ERROR | blank_author_input\n";
                sleepAnimation(1000);
                system("cls");
            }

        } while (inputAuthor.empty() || isspace(inputAuthor[0]));

        return inputAuthor;
    }

    string validateBookGenre() {
        string inputGenre;

        do { // Error Loop
            // Prompt
            cout << "Genre: ";
            getline(cin, inputGenre);

            // Input Handling: Exit Loop
            if (inputGenre == "/exit") {
                return "";
            }

            // Error Handling: isEmpty
            if (inputGenre.empty() || isspace(inputGenre[0])) {
                cerr << "ERROR | blank_genre_input\n";
                sleepAnimation(1000);
                system("cls");
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
            cout << "ISBN(10-digits): ";
            getline(cin, inputISBN);

            // Input Handling: Exit Loop
            if (inputISBN == "/exit") {
                return "";
            }

            // Error Handling: isEmpty
            if (inputISBN.empty() || isspace(inputISBN[0])) {
                isValidISBN = false;
                cerr << "ERROR | blank_ISBN_input\n";
                sleepAnimation(1000);
                system("cls");
                continue;
            }

            // Error Handling: validLength
            if (inputISBN.length() != 10) {
                isValidISBN = false;
                cerr << "ERROR | ISBN_must_be_10_digits\n";
                sleepAnimation(1000);
                system("cls");
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
                    cerr << "ERROR | existing_ISBN\n";
                    sleepAnimation(1000);
                    system("cls");
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
            cout << "Publication Date(MM/DD/YYYY): ";
            getline(cin, inputPublicationDate);

            // Input Handling: Exit Loop
            if (inputPublicationDate == "/exit") {
                return "";
            }

            // Error Handling: isEmpty
            if (inputPublicationDate.empty() || isspace(inputPublicationDate[0])) {
                isValidPublicationDate = false;
                cerr << "ERROR | blank_publication_date_input\n";
                sleepAnimation(1000);
                system("cls");
            }

            // Error Handling: validLength
            else if (inputPublicationDate.length() != 10) {
                isValidPublicationDate = false;
                cerr << "ERROR | invalid_publication_date_length\n";
                sleepAnimation(1000);
                system("cls");
            }

            else {
                // Erorr Handling: Date Separators
                if (inputPublicationDate[2] != '/' || inputPublicationDate[5] != '/') {
                    isValidPublicationDate = false;
                    cerr << "ERROR | invalid_date_separators\n";
                    sleepAnimation(1000);
                    system("cls");
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
                            cerr << "ERROR | non_digit_publication_date_input\n";
                            sleepAnimation(1000);
                            system("cls");
                            break;
                        }
                    }

                    int currentYear = timeFormat.tm_year + 1900; // Current Year
                    int currentMonth = timeFormat.tm_mon + 1; // Current Month
                    int currentDay = timeFormat.tm_mday; // Current Date

                    int year = (inputPublicationDate[6] - '0') * 1000 + (inputPublicationDate[7] - '0') * 100 + (inputPublicationDate[8] - '0') * 10 + (inputPublicationDate[9] - '0');

                    // Error Handling: Invalid Year
                    if (year > currentYear) {
                        isValidPublicationDate = false;
                        cerr << "ERROR | invalid_year\n";
                        sleepAnimation(1000);
                        system("cls");
                    }

                    // Error Handling: Invalid Month
                    int month = (inputPublicationDate[0] - '0') * 10 + (inputPublicationDate[1] - '0');
                    if (month < 1 || month > 12) {
                        isValidPublicationDate = false;
                        cerr << "ERROR | invalid_month\n";
                        sleepAnimation(1000);
                        system("cls");
                    }

                    // Error Handling: Invalid Day
                    int day = (inputPublicationDate[3] - '0') * 10 + (inputPublicationDate[4] - '0');
                    if (day < 1 || day > 31) { // Simplistic day check
                        isValidPublicationDate = false;
                        cerr << "ERROR | invalid_day\n";
                        sleepAnimation(1000);
                        system("cls");
                    }

                    // Error Handling: Current Month & Year
                    if (year == currentYear && month == currentMonth) {
                        if (day > currentDay) {
                            isValidPublicationDate = false;
                            cerr << "ERROR | invalid_day_for_current_month\n";
                            sleepAnimation(1000);
                            system("cls");
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
            cout << "Edition(blank if not applicable): ";
            getline(cin, inputBookEdition);

            // Input Handling: Exit Loop
            if (inputBookEdition == "/exit") {
                return "";
            }

            // Input Handling: isEmpty
            if (inputBookEdition.empty() || isspace(inputBookEdition[0])) {
                return "N/A";
            }

            // Error Handling: validLength
            if (inputBookEdition.length() > 2) {
                isValidEdition = false;
                cerr << "ERROR | exceeded_edition_input_limit_of_two_digits\n";
                sleepAnimation(1000);
                system("cls");
            }

            // Loop Checking: isDigit
            for (char &c : inputBookEdition) {
                // Error Handling: isDigit
                if (!isdigit(c)) {
                    isValidEdition = false;
                    cerr << "ERROR | non_digit_edition_input\n";
                    sleepAnimation(1000);
                    system("cls");
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
        cout << "[ English[1] |";
        sleepAnimation(100);
        cout << " Filipino[2] |";
        sleepAnimation(100);
        cout <<" Japanese[3] |";
        sleepAnimation(100);
        cout << " French[4] ]\n";
        sleepAnimation(100);
        cout << " [ Italian[5] |";
        sleepAnimation(100);
        cout << " German[6] |";
        sleepAnimation(100);
        cout << " Arabic[7] |";
        sleepAnimation(100);
        cout << " Chinese[8] ]\n";
        sleepAnimation(100);

        do { // Error Loop
            isValidLanguage = true;

            // Prompt
            cout << "Language: ";
            cin >> languageIndex;

            // Error Handling: isEmpty
            if (cin.fail()) {
                isValidLanguage = false;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cerr << "ERROR | blank_language_input\n";
            }

            // Error Handling: outOfBoundsIndex
            else if (languageIndex < 1 || languageIndex > 8) {
                isValidLanguage = false;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cerr << "ERROR | out_of_bounds_language_index\n";
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
                    cerr << "ERROR | invalid_language_index\n";
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
            cout << "# of Pages: ";
            cin >> inputPageCount;
            cout << '\n';

            // Error Handling: isEmpty
            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cerr << "ERROR | blank_page_count\n";
                sleepAnimation(1000);
                system("cls");
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
            cout << "Content Specifications:\n";
            sleepAnimation(100);
            cout << "* 10 Words < Word Length < 300 Words\n";
            sleepAnimation(100);
            cout << "* Ending With '|'\n\n";
            sleepAnimation(100);
            cout << "Content:\n";
            sleepAnimation(100);

            getline(cin, inputBookContent); // Input Content

            // Input Handling: Exit Loop
            if (inputBookContent == "/exit") {
                return "";
            }

            // Error Handling: isEmpty
            if (inputBookContent.empty() || isspace(inputBookContent[0])) {
                isValidContent = false;
                cerr << "ERROR | blank_book_content\n";
                continue;
            }

            // Error Handling: No Sentinel
            if (inputBookContent.back() != '|') {
                isValidContent = false;
                cerr << "ERROR | no_ending_'|'_symbol\n";
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
                            cerr << "ERROR | double_spaces\n";
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
                        cerr << "ERROR | under_description_length\n";
                    } else {
                        cerr << "ERROR | exceeded_description_length\n";
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
        system("cls");
        bookGenre = validateBookGenre();
        system("cls");
        bookISBN = validateBookISBN();
        system("cls");
    }

    void validatePublicationDetails(string &bookPublicationDate, string &bookEdition) {
        bookPublicationDate = validateBookPublicationDate();
        system("cls");
        bookEdition = validateBookEdition();
        system("cls");
    }

    void validatePhysicalDetails(string &bookLanguage, unsigned int &bookPageCount, string &bookContent) {
        bookLanguage = validateBookLanguage();
        system("cls");
        bookPageCount = validateBookPageCount();
        system("cls");
        bookContent = validateBookContent();
        system("cls");
    }

    void validateLibraryBookInformation(bool &availability) {
        availability = validateBookAvailability();
        system("cls");
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

        displayFormat(49);
        sleepAnimation(100);
        cout << '\n';
        cout << "*  Book Title " << right << setw(36) << "[0000000000]\n";
        displayFormat(49);
        sleepAnimation(100);
        cout << '\n';

        // Loop Iteration: Display
        size_t counter = 1;
        for (const auto& book : bookList) {
            cout << counter++ << ". " << left << setw(25) << book.first  << right << setw(10) << "[" << *book.second << "]\n";
            sleepAnimation(100);
        }

        displayFormat(49);
        cout << '\n';

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

                    loadingAnimation(500, 3);
                    cout << "bookfile created successfully!\n";
                    sleepAnimation(100);
                    displayFormat(49);
                    cout << '\n';
                    cout << "Note: Enter new book info after creating\n";
                    sleepAnimation(100);
                    displayFormat(49);
                    cout << '\n';
                    break;
                }

                // Error Message: Book File is Not Accessable
                else  {
                    cerr << "ERROR | cannot_open_file\n";
                    sleepAnimation(1000);
                    system("cls");
                    break;
                }
            }

            // Error Message: Book File Already Exists
            else {
                cerr << "ERROR | book_already_exists\n";
            }

        } while (bookAlreadyExists);
    }
};

// NOTE: FOR FUTURE VERSIONS IMPLEMENTATIONS: Add Genre as Derived Classes
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
        cout << "locating book file";
        loadingAnimation(500, 3);
        cout << '\n';

        string filePath = getFilePath(bookTitle); // Get File Path
        ifstream fileSearcher(filePath); // File Search

        // Error Handling: Accessable File
        if (!fileSearcher.is_open()) {
            fileFound = false;
            return;
        }

        fileFound = true;
        loadingAnimation(500, 3);
        cout << "bookfile found!\n";
        sleepAnimation(500);
        system("cls");
    }

    // FILE: Input
    void inputValidatedInformation(const string &filePath, const string &bookTitle, const string &bookAuthor, const string &bookGenre, const string &bookISBN, const string &bookPublicationDate, const string &bookEdition, const string &bookLanguage, const string &bookContent, const unsigned int &bookPageCount, const bool &availability) {
        cout << "updating book file information";
        sleepAnimation(100);
        loadingAnimation(500, 3);
        cout << '\n';

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
        cout << "reading book file";
        sleepAnimation(100);
        loadingAnimation(500, 3);
        cout << '\n';

        string line, filePath = getFilePath(bookTitle); // Getter
        ifstream bookFileReader(filePath); // File Reader

        // Read Text File: Accessable File
        if (bookFileReader) {
            loadingAnimation(500, 3);
            cout << "bookfile found!";
            sleepAnimation(500);
            cout << '\n';
            system("cls");

            displayFormat(25);
            cout << '\n';

            while (getline(bookFileReader, line)) {
                cout << line << "\n";
                sleepAnimation(100);
            }
            displayFormat(25);
            cout << '\n';
            bookFileReader.close();
        }

        // Error Handling: Inaccessable File
        else {
            cerr << "ERROR | inaccessible_book_file\n";
            sleepAnimation(1000);
            system("cls");
        }
    }

    // >> [C]REATE Book [1]
    void createBookfile() {
        displayHeader();
        cout << "_-_-_-_-_-_-_-_-_-[CREATE BOOK]-_-_-_-_-_-_-_-_-_\n";
        sleepAnimation(100);

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

        displayHeader();
        cout << "_-_-_-_-_-_-_-_-[INPUT BOOK INFO]-_-_-_-_-_-_-_-_-\n" << flush;
        sleepAnimation(100);

        displayFormat(49);
        sleepAnimation(100);
        cout << '\n';
        cout << "*  Book Title " << right << setw(36) << "[0000000000]\n";
        displayFormat(49);
        sleepAnimation(100);
        cout << '\n';

        if (bookList.size() == 0) {
            char choice;
            cout << "There are currently no books in the list...\n";
            sleepAnimation(100);
            return;
        } else {
            // Loop Iteration: Display
            size_t counter = 1;

            for (const auto& book : bookList) {
                cout << counter++ << ". " << left << setw(25) << book.first  << right << setw(10) << "[" << *book.second << "]\n";
                sleepAnimation(100);
            }
            displayFormat(49);
            cout << '\n';
        }

        do { // Error Loop
            // Prompt
            cout << "Search Book | Title: ";
            getline(cin, bookTitle);
            cout << '\n';

            // Input Handling: Exit Loop
            if (bookTitle == "/exit") {
                return;
            }

            // Error Handling: isEmpty
            if (bookTitle.empty() || isspace(bookTitle[0])) {
                cerr << "ERROR | blank_book_title\n";
                continue;
            }

            system("cls");
            locateBookFile(fileFound, bookTitle); // Search for File
            sleepAnimation(1000);

            // Validate Information: Accessable File
            if (fileFound) {
                string filePath = getFilePath(bookTitle);
                cout << "BASIC INFORMATION |\n";
                bookReference.validateBasicInformation(bookAuthor, bookGenre, bookISBN);
                system("cls");
                cout << "PUBLICATION INFORMATION |\n";
                bookReference.validatePublicationDetails(bookPublicationDate, bookEdition);
                system("cls");
                cout << "PHYSICAL INFORMATION |\n";
                bookReference.validatePhysicalDetails(bookLanguage, bookPageCount, bookContent);
                // Input Validated Information to Book File
                system("cls");
                inputValidatedInformation(filePath, bookTitle, bookAuthor, bookGenre, bookISBN, bookPublicationDate, bookEdition, bookLanguage, bookContent, bookPageCount, bookAvailability);
                readBookfile(bookTitle);
                bookList[bookTitle] = make_unique<string>(bookISBN);
            }

            // Error Handling: Accessable File
            else {
                cerr << "ERROR | file_does_not_exist\n";
                sleepAnimation(1000);
                system("cls");
            }

        } while (bookTitle.empty() || isspace(bookTitle[0]) || !fileFound);
    }


    // >> [R]EAD Book [3]
    void readBookfile() {
        string bookTitle, line;
        bool fileFound = true;

        displayHeader();
        cout << "_-_-_-_-_-_-_-_-_-_-[READ BOOK]-_-_-_-_-_-_-_-_-_\n";
        sleepAnimation(100);

        displayFormat(49);
        sleepAnimation(100);
        cout << '\n';
        cout << "*  Book Title " << right << setw(36) << "[0000000000]\n";
        displayFormat(49);
        sleepAnimation(100);
        cout << '\n';

        // Loop Iteration: Display
        size_t counter = 1;
        for (const auto& book : bookList) {
            cout << counter++ << ". " << left << setw(25) << book.first  << right << setw(10) << "[" << *book.second << "]\n";
            sleepAnimation(100);
        }

        displayFormat(49);
        cout << '\n';

        do { // Error Loop
            // Prompt
            cout << "Search | Book Title: ";
            getline(cin, bookTitle);
            cout << '\n';

            // Input Handling: Exit Loop
            if (bookTitle == "/exit") {
                return;
            }

            // Error Handling: isEmpty
            if (bookTitle.empty() || isspace(bookTitle[0])) {
                cerr << "ERROR | blank_book_title_input\n";
                sleepAnimation(1000);
                system("cls");
            }

        } while (bookTitle.empty() || isspace(bookTitle[0]));

        string filePath = getFilePath(bookTitle); // Getter
        ifstream bookFileReader(filePath); // File Reader

        // Read File: Accessable File
        if (bookFileReader) {
            system("cls");
            loadingAnimation(500, 3);
            cout << "bookfile found!\n";
            sleepAnimation(500);
            system("cls");
            cout << "File Name: " << bookTitle << ".txt\n";
            sleepAnimation(100);
            displayFormat(21);
            cout << "CONTENT";
            displayFormat(21);
            cout << '\n';
            sleepAnimation(100);

            // Display Content
            while (getline(bookFileReader, line)) {
                cout << line << "\n";
                sleepAnimation(100);
            }

            displayFormat(49);
            cout << '\n';
            sleepAnimation(100);
            bookFileReader.close();
        }

        // Error Handling: Accessable File
        else {
            cerr << "ERROR | inaccessible_book_file\n";
            sleepAnimation(1000);
            system("cls");
        }
    }

    // >> [U]PDATE Book Information [4]
    void updateBookInformation() {
        string bookTitle;
        vector<string> currentLines;
        string line;
        int lineNumber;

        displayHeader();
        cout << "_-_-_-_-_-_-[UPDATE BOOK INFORMATION]-_-_-_-_-_-_\n";
        sleepAnimation(100);

        displayFormat(49);
        sleepAnimation(100);
        cout << '\n';
        cout << "*  Book Title " << right << setw(36) << "[0000000000]\n";
        displayFormat(49);
        sleepAnimation(100);
        cout << '\n';

        // Loop Iteration: Display
        size_t counter = 1;
        for (const auto& book : bookList) {
            cout << counter++ << ". " << left << setw(25) << book.first  << right << setw(10) << "[" << *book.second << "]\n";
            sleepAnimation(100);
        }

        displayFormat(49);
        cout << '\n';

        do { // Error Loop
            // Prompt
            cout << "Search Book | Title: ";
            getline(cin, bookTitle);
            cout << '\n';

            // Input Handling: Exit Loop
            if (bookTitle == "/exit") {
                return;
            }

            // Error Handling: isEmpty
            if (bookTitle.empty() || isspace(bookTitle[0])) {
                cerr << "ERROR | blank_book_title_input\n";
                sleepAnimation(1000);
                system("cls");
            }

        } while (bookTitle.empty() || isspace(bookTitle[0]));

        string filePath = getFilePath(bookTitle); // Getter
        ifstream bookFileReader(filePath); // File Reader

        unordered_set<int> invalidLineNumbers = {1, 6, 7, 10, 11, 14, 15, 17, 18}; // Invalid Line Numbers

        // Display Current Content
        if (bookFileReader) {
            system("cls");
            loadingAnimation(500, 3);
            cout << "bookfile found!\n";
            sleepAnimation(500);
            system("cls");
            cout << "File Name: " << bookTitle << ".txt\n";
            sleepAnimation(100);
            displayFormat(21);
            cout << "CONTENT";
            displayFormat(21);
            cout << '\n';
            sleepAnimation(100);

            int numTrack = 1;

        // Read Content
        while (getline(bookFileReader, line)) {
            currentLines.push_back(line);
            if (invalidLineNumbers.count(numTrack)) {
                cout << line << "\n"; // Print Line /wo Line Number
                sleepAnimation(100);
            } else if (numTrack > 1 && numTrack < 10) {
                cout << "  " << currentLines.size() << " | " << line << "\n"; // Print Line /w Line Number
                sleepAnimation(100);
            } else {
                cout << ' ' << currentLines.size() << " | " << line << "\n"; // Print Line /w Line Number
            }
            numTrack++;
        }
            bookFileReader.close();
            displayFormat(49);
            sleepAnimation(100);
            cout << '\n';

        bool invalidLineNumber;

        do { // Error Loop
            invalidLineNumber = false;
            // Prompt
            cout << "Change Line No.: ";
            cin >> lineNumber;

            // Error Handling: invalidInput
            if (cin.fail() || invalidLineNumbers.count(lineNumber) || lineNumber > currentLines.size()) {
                invalidLineNumber = true;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cerr << "ERROR | invalid_line_number_input\n";
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
            cout << "Current Content: " << currentLines[lineNumber - 1];
            cout << '\n';
            sleepAnimation(100);
            cin.ignore();
            cout << "Change Content(Line " << lineNumber << "): ";
            sleepAnimation(100);
            getline(cin, currentLines[lineNumber - 1]);
            cout << '\n';

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
            loadingAnimation(500, 3);
            cout << "line " << lineNumber << " updated successfully!\n";
            sleepAnimation(100);
        }

        else {
            cerr << "ERROR | cannot_open_file\n";
            sleepAnimation(1000);
            system("cls");
        }
    }

    // >> [D]ELETE Book [5]
    void deleteBookfile() {
        string bookTitle;
        string filePath = getFilePath(bookTitle);

        displayHeader();
        cout << "_-_-_-_-_-_-_-_-_-[DELETE BOOK]-_-_-_-_-_-_-_-_-_\n";
        sleepAnimation(100);
        displayFormat(49);
        cout << '\n';

        // Loop Iteration: Display
        size_t counter = 1;
        for (const auto& book : bookList) {
            cout << counter++ << ". " << left << setw(25) << book.first  << right << setw(10) << "[" << *book.second << "]\n";
            sleepAnimation(100);
        }

        displayFormat(49);
        cout << '\n';

        do { // Error Loop
            // Prompt
            cout << "Book Title: ";
            getline(cin, bookTitle);

            // Input Handling: Exit Loop
            if (bookTitle == "/exit") {
                return;
            }

            // Error Handling: isEmpty
            if (bookTitle.empty() || isspace(bookTitle[0])) {
                cerr << "ERROR | blank_book_title_input\n";
                sleepAnimation(1000);
                system("cls");
            }

        } while (bookTitle.empty() || isspace(bookTitle[0]));

        char choice;
        // Confirmation Message
        cout << "WARNING: Deleting bookfile '" << bookTitle << "' is permanent and cannot be undone.\n";
        cout << "Are you sure you want to proceed?[y/n]: ";

        do {
            cin.get(choice);
            choice = tolower(choice);

            if (choice == 'y') {
                break;
            } else if (choice == 'n') {
                cout << "cancelling deletion";
                loadingAnimation(500, 3);
                cout << '\n';
                return;
            } else {
                cerr << "ERROR | invalid_input";
                sleepAnimation(1000);
                system("cls");
            }
        } while (true);

        system("cls");
        filePath = getFilePath(bookTitle); // Getter
        if (remove(filePath.c_str()) == 0) { // Converts File Path to C-Style String, Removes File Using File Path, Returns 0 or 1
            loadingAnimation(500, 3);
            cout << "bookfile deleted!\n";

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
            cerr << "ERROR | file_does_not_exist\n";
            sleepAnimation(1000);
            system("cls");
        }
    }

    // >> Show Booklist [6]
    void showBooklist() {
        displayHeader();
        cout << "-_-_-_-_-_-_-_-_-_-[BOOK LIST]-_-_-_-_-_-_-_-_-_-\n" << flush;
        sleepAnimation(100);

        // Display If No Book Files in Booklist
        if (bookList.size() == 0) {
            char choice;
            cout << "There are currently no books in the list...\n";
            sleepAnimation(100);

            do { // Error Loop
                // Prompt
                cout << "Would you like to add one?[y/n]:\n";
                sleepAnimation(100);
                cout << ">> ";
                sleepAnimation(100);

                cin >> choice;
                cout << '\n';
                cin.ignore();

                // Error Handling
                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cerr << "ERROR | invalid_input\n";
                    sleepAnimation(1000);
                    system("cls");
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
                    cout << "Invalid Input.\n";
                }

            } while (cin.fail());
        }

        displayFormat(49);
        sleepAnimation(100);
        cout << '\n';
        cout << "*  Book Title " << right << setw(36) << "[0000000000]\n";
        displayFormat(49);
        sleepAnimation(100);
        cout << '\n';

        // Loop Iteration: Display
        size_t counter = 1;
        for (const auto& book : bookList) {
            cout << counter++ << ". " << left << setw(25) << book.first  << right << setw(10) << "[" << *book.second << "]\n";
            sleepAnimation(100);
        }
        displayFormat(49);
        cout << '\n';
    }

    // >> Borrow Book [7]
    void borrowBook() {
        string bookTitle, bookISBN, line;
        int lineNumber = 16;
        bool bookExists = false, invalidBorrow = false;
        vector<string> currentLines;

        displayHeader();
        cout << "_-_-_-_-_-_-_-_-_-[BORROW BOOK]-_-_-_-_-_-_-_-_-_\n";
        sleepAnimation(100);
        displayFormat(49);
        sleepAnimation(100);
        cout << '\n';
        cout << "*  Book Title " << right << setw(36) << "[0000000000]\n";
        displayFormat(49);
        sleepAnimation(100);
        cout << '\n';

        // Loop Iteration: Display
        size_t counter = 1;
        for (const auto& book : bookList) {
            cout << counter++ << ". " << left << setw(25) << book.first  << right << setw(10) << "[" << *book.second << "]\n";
            sleepAnimation(100);
        }

        displayFormat(49);
        cout << '\n';

        do { // Error Handling
            cout << "Borrow | Book Title: ";
            getline(cin, bookTitle);

            // Input Handling: Exit Code
            if (bookTitle == "/exit") {
                return;
            }

            // Error Handling: isEmpty
            if (bookTitle.empty()) {
                cerr << "ERROR | blank_book_title\n";
                sleepAnimation(1000);
                system("cls");
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
            cerr << "ERROR | book_does_not_exist\n";
            sleepAnimation(1000);
            system("cls");
            return;
        }

        // Directories
        string currentPath = "txt_files\\" + bookTitle + ".txt";
        string borrowedBookFolder = "txt_files\\borrowed_books\\" + bookTitle + ".txt";

        // File Handling: isInCurrentPath
        if (ifstream(currentPath)) {

            // File Handling: isMovable
            if (rename(currentPath.c_str(), borrowedBookFolder.c_str()) != 0) {
                cerr << "ERROR | could_not_move_file\n";
                sleepAnimation(1000);
                system("cls");
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
            loadingAnimation(500, 3);
            cout << "successfully borrowed '" << bookTitle << "'\n";
            sleepAnimation(100);
            cout << "Note: Please return the book within a week to avoid late fees.\n";
            *bookList[bookTitle] = "*borrowed*"; // Change ISBN to *borrowed*

            ofstream writeHistory(".borrow_history.txt", ios::app);
            if (writeHistory.is_open()) {
                writeHistory << "* " << bookTitle << "[" << bookISBN << "] borrowed | "
                << setw(11) << right << timeFormat.tm_mon + 1 << " / " << timeFormat.tm_mday << " / " << timeFormat.tm_year + 1900 << '\n';
                writeHistory.close();
            } else {
                cout << "ERROR | missing_history_file\n";
                sleepAnimation(1000);
                system("cls");
            }
        }

            // Error Handling: borrowedBook
            else {
                cerr << "ERROR | book_is_currently_borrowed";
                sleepAnimation(1000);
                system("cls");
            }
    }

    // >> Return Book [8]
    void returnBook() {
        string bookTitle, bookISBN, line;
        int lineNumber = 16;
        bool bookExists = false, invalidReturn = false;
        vector<string> currentLines;

        displayHeader();
        cout << "_-_-_-_-_-_-_-_-_-[RETURN BOOK]-_-_-_-_-_-_-_-_-_\n";
        sleepAnimation(100);
        string filePath = "txt_files\\borrowed_books\\";

        for (const auto& file : filesystem::directory_iterator(filePath)) {
            if (file.path().extension() == ".txt") {
                bookExists = true;
                break;
            }
        }

        // Error Handling: Non-Existing Book
        if (!bookExists) {
            cout << "There are currently no books borrowed...\n";
            return;
        }

        size_t counter = 1;

        displayFormat(49);
        sleepAnimation(100);
        cout << '\n';
        cout << "*  Book Title " << right << setw(27) << "[0000000000]\n";
        displayFormat(49);
        sleepAnimation(100);
        cout << '\n';


        for (const auto& file : filesystem::directory_iterator(filePath)) {
            cout << counter << ". " << left << setw(25) << file.path().stem().string() << "[*borrowed*]" << "\n";
        }

        displayFormat(49);
        sleepAnimation(100);
        cout << '\n';

        do { // Error Loop
            // Prompt
            cout << "Return | Book Title: ";
            getline(cin, bookTitle);
            cout << '\n';

            // Input Handling: Exit Code
            if (bookTitle == "/exit") {
                return;
            }

            // Error Handling: isEmpty
            if (bookTitle.empty()) {
                cerr << "ERROR | blank_book_title\n";
                sleepAnimation(1000);
                system("cls");
            }

        } while (bookTitle.empty());

        for (const auto& file : filesystem::directory_iterator(filePath)) {
            if (file.path().stem().string() == bookTitle) {
                bookExists = true;
                ifstream fileReader(file.path());

                if (fileReader.is_open()) {
                    string line;
                    while (getline(fileReader, line)) {
                        size_t position = line.find("ISBN: ");
                        if (position != string::npos) {
                            bookISBN = line.substr(position + 6, 10); // Get ISBN
                            break;
                        }
                    }
                    fileReader.close();
                } else {
                    cout << "Error loading book files.\n";
                }
            }
        }

        // Error Handling: Non-Existing Book
        if (!bookExists) {
            cerr << "ERROR | book_does_not_exist\n";
            sleepAnimation(1000);
            system("cls");
            return;
        }

        // Directories
        string currentPath = "txt_files\\borrowed_books\\" + bookTitle + ".txt";
        string textFileFolder = "txt_files\\" + bookTitle + ".txt";

        // File Handling: isInCurrentPath
        if (ifstream(currentPath)) {
            // File Handling: isMovable
            if (rename(currentPath.c_str(), textFileFolder.c_str()) != 0) {
                cerr << "ERROR | could_not_move_file\n";
                sleepAnimation(1000);
                system("cls");
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
            loadingAnimation(500, 3);
            cout << "successfully returned '" << bookTitle << "'\n";
            sleepAnimation(100);

            if (!bookList[bookTitle]) {
                bookList[bookTitle] = make_unique<string>(); // Avoid Null Pointer Dereferencing
            }
            *bookList[bookTitle] = bookISBN; // Return ISBN

            ofstream returnHistory(".return_history.txt", ios::app);
            if (returnHistory.is_open()) {
                returnHistory << "* " << bookTitle << "[" << bookISBN << "] returned | "
                << setw(11) << right << timeFormat.tm_mon + 1 << " / " << timeFormat.tm_mday << " / " << timeFormat.tm_year + 1900 << '\n';
                returnHistory.close();
            } else {
                cout << "ERROR | missing_history_file\n";
                sleepAnimation(1000);
                system("cls");
            }
        }

        // Error Handling: isAlreadyReturned
        else {
            cerr << "ERROR | book_already_returned\n";
            sleepAnimation(1000);
            system("cls");
        }
    }

    // >> Search Book [9]
    void searchBook() {
        string bookTitle;
        bool bookExists;

        displayHeader();
        cout << "_-_-_-_-_-_-_-_-_-[SEARCH BOOK]-_-_-_-_-_-_-_-_-_\n";
        sleepAnimation(100);

        size_t counter = 1;

        displayFormat(49);
        sleepAnimation(100);
        cout << '\n';
        cout << "*  Book Title " << right << setw(36) << "[0000000000]\n";
        displayFormat(49);
        sleepAnimation(100);
        cout << '\n';

        // Loop Iteration: Display
        for (const auto& book : bookList) {
            cout << counter++ << ". " << left << setw(25) << book.first  << right << setw(10) << "[" << *book.second << "]\n";
            sleepAnimation(100);
        }

        displayFormat(49);
        cout << '\n';

        do { // Error Loop
            bookExists = false;
            // Prompt
            cout << "Search Book | Book Title: ";
            getline(cin, bookTitle);

            // Input Handling: Exit Loop
            if (bookTitle == "/exit") {
                return;
            }

            // Error Handling: isEmpty
            if (bookTitle.empty() || bookTitle[0] == ' ') {
                cerr << "ERROR | blank_book_title_input\n";
                sleepAnimation(1000);
                system("cls");
            }

        } while (bookTitle.empty() || bookTitle[0] == ' ');

        cout << "searching book";
        loadingAnimation(500, 3);
        cout << '\n';

        string filePath = "txt_files\\";

        for (const auto& file : filesystem::directory_iterator(filePath)) {
            if (file.path().stem().string() == bookTitle) {
                bookExists = true;

                    int choice;
                    cout << "bookfile '" << bookTitle << "' found!\n";  // Display Book File

                    do { // Error Loop
                        // Prompt
                        cout << "Show Contents?:\n";
                        sleepAnimation(100);
                        cout << "[1] | Yes\n";
                        sleepAnimation(100);
                        cout << "[2] | No\n";
                        sleepAnimation(100);
                        cout << "---------\n";
                        sleepAnimation(100);
                        cout << ">> ";
                        sleepAnimation(100);
                        cin >> choice;

                        // Error Handling: invalidInput
                        if (cin.fail()) {
                            cin.clear();
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                            cerr << "ERROR | invalid_input\n";
                            sleepAnimation(1000);
                            system("cls");
                        }

                    } while (cin.fail());

                    // Input Handling: 1(Display Content)
                    if (choice == 1) {
                        string line;
                        string filePath = getFilePath(bookTitle); // Getter
                        ifstream bookFileReader(filePath); // File Reader

                        // Read File: Accessible File
                        if (bookFileReader) {
                            system("cls");
                            cout << "File Name: " << bookTitle << ".txt\n";
                            sleepAnimation(100);
                            displayFormat(21);
                            cout << "CONTENT";
                            displayFormat(21);
                            cout << "\n";
                            sleepAnimation(100);

                            while (getline(bookFileReader, line)) {
                                cout << line << "\n";
                                sleepAnimation(100);
                            }

                            displayFormat(49);
                            cout << "\n";
                            sleepAnimation(100);
                            bookFileReader.close(); // Close File Reader
                        }

                        // Error Handling: Accessible File
                        else {
                            cerr << "ERROR | inaccessible_book_file\n";
                            sleepAnimation(1000);
                            system("cls");
                        }
                    }

                    // Input Handling: 2(Break Loop)
                    else if (choice == 2) {
                        cout << "finishing search";
                        loadingAnimation(500, 3);
                        cout << '\n';
                        break;
                    }
            }
        }
    }

    // >> Print Borrow History [10]
    void printBorrowHistory() {
        displayHeader();
        cout << "_-_-_-_-_-_-_-_-[BORROW HISTORY]-_-_-_-_-_-_-_-_\n";
        sleepAnimation(100);

        // Read Borrow History
        ifstream borrowHistory(".borrow_history.txt");
        if (borrowHistory) {
            string line;

            displayFormat(49);
            cout << "Book Title " << right << setw(26) << "[0000000000]" << "        Date" << "\n";
            displayFormat(49);

            while (getline(borrowHistory, line)) {
                // Parse the line
                stringstream ss(line);
                string title, isbn, date;
                ss >> ws; // Ignore leading whitespace
                getline(ss, title, '['); // Extract Title
                getline(ss, isbn, ']'); // Extract ISBN
                ss >> date;   // Extract "borrowed | MM / DD / YYYY"
                getline(ss, date);

                // Output with formatting in your desired format
                cout << left << setw(25) << title  // Title
                << "[" << isbn << "]" // ISBN
                << date << "\n"; // Date
                sleepAnimation(100);
            }
        } else {
            cerr << "ERROR | missing_history_file\n";
            sleepAnimation(1000);
            system("cls");
        }

        displayFormat(49);
    }

    // >> Print Return History [11]
    void printReturnHistory() {
        displayHeader();
        cout << "_-_-_-_-_-_-_-_-[RETURN HISTORY]-_-_-_-_-_-_-_-_\n";
        sleepAnimation(100);

        // Read Return History
        ifstream returnHistory(".return_history.txt");

        if (returnHistory) {
            string line;

            displayFormat(49);
            cout << "Book Title " << right << setw(26) << "[0000000000]" << "        Date" << "\n";
            displayFormat(49);

            while (getline(returnHistory, line)) {
                // Parse the line
                stringstream ss(line);
                string title, isbn, date;
                ss >> ws; // Ignore leading whitespace
                getline(ss, title, '['); // Extract Title
                getline(ss, isbn, ']'); // Extract ISBN
                ss >> date;   // Extract "borrowed | MM / DD / YYYY"
                getline(ss, date);

                // Output with formatting in your desired format
                cout << left << setw(25) << title  // Title
                << "[" << isbn << "]" // ISBN
                << date << "\n"; // Date
                sleepAnimation(100);
            }
        } else {
            cerr << "ERROR | missing_history_file\n";
            sleepAnimation(1000);
            system("cls");
        }

        displayFormat(49);
    }

    // >> Display Table Of Contents [12]
    void displayTableOfContents() {
        displayHeader();
        cout << "-_-_-_-_-_-_-_-[TABLE OF CONTENTS]-_-_-_-_-_-_-_-\n";
        sleepAnimation(100);
        displayNoSpaceFormat(19);
        cout << " FUNCTIONS ";
        displayNoSpaceFormat(19);
        cout << '\n';
        sleepAnimation(100);
        cout << "  1. createBookfile() [C#1]\n";
        sleepAnimation(100);
        cout << "   - creates a book file(.txt)\n";
        sleepAnimation(100);
        displayFormat(49);
        sleepAnimation(100);
        cout << "  2. inputBookInformation() [C#2]\n";
        sleepAnimation(100);
        cout << "   - stores book details inside bookfile\n";
        sleepAnimation(100);
        displayFormat(49);
        sleepAnimation(100);
        cout << "  3. readBookfile() [R]\n";
        sleepAnimation(100);
        cout << "   - reads book file line-by-line\n";
        sleepAnimation(100);
        displayFormat(49);
        sleepAnimation(100);
        cout << "  4. updateBookInformation() [U]\n";
        sleepAnimation(100);
        cout << "   - edits/updates book detail/information\n";
        sleepAnimation(100);
        displayFormat(49);
        sleepAnimation(100);
        cout << "  5. deleteBookfile() [D]\n";
        sleepAnimation(100);
        cout << "   - deletes book file\n";
        sleepAnimation(100);
        displayFormat(49);
        sleepAnimation(100);
        cout << "  6. showBooklist()\n";
        sleepAnimation(100);
        cout << "   - displays book list /w proper format\n";
        sleepAnimation(100);
        displayFormat(49);
        sleepAnimation(100);
        cout << "  7. borrowBook()\n";
        sleepAnimation(100);
        cout << "   - book is transferred to 'borrowed_books'\n";
        sleepAnimation(100);
        displayFormat(49);
        sleepAnimation(100);
        cout << "  8. returnBook()\n";
        sleepAnimation(100);
        cout << "   - book is transferred back to the 'main'\n";
        sleepAnimation(100);
        displayFormat(49);
        sleepAnimation(100);
        cout << "  9. searchBook()\n";
        sleepAnimation(100);
        cout << "   - locates book(if exists)\n";
        sleepAnimation(100);
        displayFormat(49);
        sleepAnimation(100);
        cout << "  10. printBorrowHistory()\n";
        sleepAnimation(100);
        cout << "   - prints transcript of borrow_history\n";
        sleepAnimation(100);
        displayFormat(49);
        sleepAnimation(100);
        cout << "  11. printReturnHistory()\n";
        sleepAnimation(100);
        cout << "   - prints transcript of return_history\n";
        sleepAnimation(100);
        displayFormat(49);
        sleepAnimation(100);
        cout << "  >> /exit\n";
        sleepAnimation(100);
        cout << "   - exits the loop validation\n ";
        sleepAnimation(100);
        displayFormat(49);
    }
};

class Menu { // Menu Class: Recursive
protected:
    Library &libraryReference;
public:
    Menu(Library &libraryReference) : libraryReference(libraryReference) { /* Menu Constructor */ }

    void loadBooklist() {
        string filePath = "txt_files\\";

        for (const auto& file : filesystem::directory_iterator(filePath)) {
            if (file.path().extension() == ".txt") {
                ifstream fileReader(file.path());

                if (fileReader.is_open()) {
                string line;
                    while (getline(fileReader, line)) {
                        size_t position = line.find("ISBN: ");
                        if (position != string::npos) {
                            string fileISBN = line.substr(position + 6, 10);

                            string fileName = file.path().filename().string();
                            fileName = fileName.substr(0, fileName.size() - 4); // Remove '.txt' extension

                            bookList[fileName] = make_unique<string>(fileISBN); // Add to Booklist
                            break;
                        }
                    }
                    fileReader.close();
                } else {
                    cout << "Error loading book files.";
                }
            }
        }
    }

    void displayRegisterMenu() {
        string username, password;
        bool isRegisteredUsername, isValidUsername, isValidPassword, onRegisterMenu;
        bool hasLetter, hasNumber;

        displayHeader();
        cout << "-_-_-_-_-_-_-_-_-_-[REGISTER]-_-_-_-_-_-_-_-_-_-\n";
        sleepAnimation(100);
        displayFormat(49);

        do { // Error Loop
            isRegisteredUsername = false, isValidUsername = true, hasLetter = false, hasNumber = false, onRegisterMenu = true;
            // Prompt
            cout << " [ Username must be: ]\n";
            cout << "* 5-15 characters\n";
            sleepAnimation(250);
            cout << "* a-z, A-Z, 1-9\n";
            sleepAnimation(250);
            cout << "* no spaces\n";
            sleepAnimation(100);
            cout << "Username: ";
            sleepAnimation(100);
            getline(cin, username);
            cout << '\n';

            // Input Handling: Exit Loop
            if (username == "/exit") {
                return;
            }

            // Error Handling: isEmpty
            if (username.empty() || username[0] == ' ') {
                isValidUsername = false;
                cerr << "ERROR | blank_username_input\n";
                sleepAnimation(1000);
                system("cls");
            }

            // Error Handling: validLength
            if (username.length() < 5 || username.length() > 15) {
                isValidUsername = false;
                if (username.length() < 5) {
                    cerr << "ERROR | under_minimum_username_length\n";
                    sleepAnimation(1000);
                    system("cls");
                }

                // Error Handling: isValidUsernameLength
                else {
                    cerr << "ERROR | exceeded_maximum_username_length\n";
                    sleepAnimation(1000);
                    system("cls");
                }
            }

            // Error Handling: isValidUsername
            for (size_t i = 0; i < username.length(); i++) {

                // Error Handling: isSpace
                if (isspace(username[i])) {
                    isValidUsername = false;
                    cerr << "ERROR | username_must_not_contain_spaces\n";
                    sleepAnimation(1000);
                    system("cls");
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
                    cerr << "ERROR | username_must_not_contain_characters\n";
                    sleepAnimation(1000);
                    system("cls");
                    break;
                }
            }

            // Error Handling: noLetters
            if (!hasLetter) {
                isValidUsername = false;
                cerr << "ERROR | username_must_contain_a_letter\n";
                sleepAnimation(1000);
                system("cls");
            }

            // Error Handling: noNumbers
            if (!hasNumber) {
                isValidUsername = false;
                cerr << "ERROR | username_must_contain_a_number\n";
                sleepAnimation(1000);
                system("cls");
            }

            // Error Handling: isNotExisting
            for (const auto& existingUsername : loginCredentials) {
                if (existingUsername.first == username) {
                    isRegisteredUsername = true;
                    isValidUsername = false;
                    cerr << "ERROR | username_already_registered\n";
                    sleepAnimation(1000);
                    system("cls");
                    break;
                }
            }

        } while (!isValidUsername);

        do { // Error Loop
            isValidPassword = true, hasLetter = false, hasNumber = false;
            // Prompt
            cout << "[ Password must be: ]\n";
            cout << "* 5-15 characters\n";
            sleepAnimation(250);
            cout << "* a-z, A-Z, 1-9\n";
            sleepAnimation(250);
            cout << "* no spaces\n";
            sleepAnimation(100);
            cout << "Password: ";
            sleepAnimation(100);
            getline(cin, password);
            cout << '\n';

            // Input Handling: Exit Loop
            if (password == "/exit") {
                return;
            }

            // Error Handling: isEmpty
            if (password.empty() || password[0] == ' ') {
                isValidPassword = false;
                cerr << "ERROR | blank_password_input\n";
                sleepAnimation(1000);
                system("cls");
            }

            // Error Handling: validLength
            if (password.length() < 5 || password.length() > 15) {
                isValidPassword = false;

                // Error Handling: isValidPasswordLength
                if (password.length() < 5) {
                    cerr << "ERROR | under_minimum_password_length\n";
                    sleepAnimation(1000);
                    system("cls");
                }

                // Error Handling: isValidPasswordLength
                else {
                    cerr << "ERROR | exceeded_maximum_password_length\n";
                    sleepAnimation(1000);
                    system("cls");
                }
            }

            // Error Handling: isSpace
            for (size_t i = 0; i < password.length(); i++) {
                if (isspace(password[i])) {
                    isValidPassword = false;
                    cerr << "ERROR | password_must_not_contain_spaces\n";
                    sleepAnimation(1000);
                    system("cls");
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
                    cerr << "ERROR | password_must_not_contain_characters\n";
                    sleepAnimation(1000);
                    system("cls");
                    break;
                }
            }

            // Error Handling: noLetters
            if (!hasLetter) {
                isValidPassword = false;
                cerr << "ERROR | password_must_contain_a_letter\n";
                sleepAnimation(1000);
                system("cls");
            }

            // Error Handling: noNumbers
            if (!hasNumber) {
                isValidPassword = false;
                cerr << "ERROR | password_must_contain_a_number\n";
                sleepAnimation(1000);
                system("cls");
            }

        } while (!isValidPassword);

        // Register Using Login Credentials
        loginCredentials[username] = password;

        loadingAnimation(500, 3);
        cout << "user '" << username << "' registered successfully!\n";
        pressEnterToContinue();
        system("cls");

        int registerMenuChoice;

        do { // Error Loop
            // Prompt
            cout << "| -Library-Management-System- |\n";
            sleepAnimation(100);
            cout << " -_-_-_-_-[REGISTER]-_-_-_-_-\n";
            sleepAnimation(100);
            displayFormat(24);
            cout << '\n';
            sleepAnimation(100);
            cout << " Proceed to Login Page?\n";
            sleepAnimation(100);
            cout << "      [1] | Yes\n";
            sleepAnimation(100);
            cout << "      [2] | No\n";
            sleepAnimation(100);
            displayFormat(24);
            cout << '\n';
            sleepAnimation(100);
            cout << ">> ";
            sleepAnimation(100);
            cin >> registerMenuChoice;
            cout << '\n';
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

        displayHeader();
        cout << "_-_-_-_-_-_-_-_-_-_-[LOGIN]-_-_-_-_-_-_-_-_-_-_\n";
        sleepAnimation(100);
        displayFormat(49);
        cout << '\n';

        do { // Error Loop
            usernameExists = false;
            // Prompt
            cout << "Username: ";
            sleepAnimation(100);
            getline(cin, username);

            // Input Handling: Exit Loop
            if (username == "/exit") {
                return;
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
                cerr << "ERROR | username_does_not_exist\n";
                sleepAnimation(1000);
                system("cls");
            }

        } while (!usernameExists);

        displayFormat(49);
        cout << '\n';

        do { // Error Loop
            correctPassword = false;
            // Prompt
            cout << "Password: ";
            sleepAnimation(100);
            getline(cin, password);

            // Input Handling: Exit Loop
            if (password == "/exit") {
                return;
            }

            // Input Handling: correctPassword
            if (loginCredentials[username] == password) {
                correctPassword = true;
            }

            // Error Handling: invalidPassword
            else {
                cerr << "ERROR | invalid_password\n";
                sleepAnimation(1000);
                system("cls");
            }

        } while (!correctPassword);

        loadingAnimation(500, 3);
        cout << "login successful!\n";
        sleepAnimation(250);
        pressEnterToContinue();
        system("cls");

        cout << "Welcome, " << username << ".\n";
        sleepAnimation(1000);
        system("cls");
        displayLibraryMenu();
    }

    void displayMenu() {
        int loginMenuChoice;
        do { // Error Loop
            // Prompt
            displayHeader();
            cout << "-_-_-_-_-_-_-_-_-_-_-[MENU]-_-_-_-_-_-_-_-_-_-_-\n";
            sleepAnimation(100);
            displayFormat(49);
            cout << '\n';
            sleepAnimation(100);
            cout << "                 [1] | Login\n";
            sleepAnimation(100);
            cout << "                 [2] | Register\n";
            sleepAnimation(100);
            cout << "                 [3] | Exit\n";
            sleepAnimation(100);
            displayFormat(49);
            cout << '\n';
            sleepAnimation(100);
            cout << ">> ";
            sleepAnimation(100);
            cin >> loginMenuChoice;
            cout << '\n';

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
                    cout << "exiting program";
                    loadingAnimation(250, 3);
                    system("cls");
                    ASCIIArtAnimation();
                    exit(0);
                break;
                default:
                    cerr << "ERROR | invalid_input\n";
                sleepAnimation(1000);
                system("cls");
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
            displayHeader();
            cout << "-_-_-_-_-_-_-_-_-[LIBRARY MENU]-_-_-_-_-_-_-_-_-\n" << flush;
            sleepAnimation(25);
            displayFormat(49);
            sleepAnimation(25);
            cout << setw(9) << ' ' << " [1] | Create Book File\n" << flush;
            sleepAnimation(25);
            cout << setw(9) << ' ' << " [2] | Input Book Information\n" << flush;
            sleepAnimation(25);
            cout << setw(9) << ' ' << " [3] | Read Book File\n" << flush;
            sleepAnimation(25);
            cout << setw(9) << ' ' << " [4] | Update Book Information\n" << flush;
            sleepAnimation(25);
            cout << setw(9) << ' ' << " [5] | Delete Book File\n" << flush;
            sleepAnimation(25);
            cout << setw(9) << ' ' << " [6] | Show Booklist\n" << flush;
            sleepAnimation(25);
            cout << setw(9) << ' ' << " [7] | Borrow Book\n" << flush;
            sleepAnimation(25);
            cout << setw(9) << ' ' << " [8] | Return Book\n" << flush;
            sleepAnimation(25);
            cout << setw(9) << ' ' << " [9] | Search Book\n" << flush;
            sleepAnimation(25);
            cout << setw(9) << ' ' << "[10] | Borrow History\n" << flush;
            sleepAnimation(25);
            cout << setw(9) << ' ' << "[11] | Return History\n" << flush;
            sleepAnimation(25);
            cout << setw(9) << ' ' << "[12] | Table of Contents\n" << flush;
            sleepAnimation(25);
            cout << setw(9) << ' ' << "[13] | Exit\n" << flush;
            sleepAnimation(25);
            displayFormat(49);
            cout << '\n' << flush;
            sleepAnimation(25);
            cout << ">> ";
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
                cout << "ERROR | invalid_input\n";
                break;
            }
            pressEnterToContinue();
            system("cls");
        } while (true);
    }

    // MENU: Return
    void pressEnterToContinue() {
        cout << "Press 'Enter' to continue...";
        cin.get();
    }
};

int main() {
    ASCIIArtAnimation();
    Book book; // Default Book Constructor for Library
    Library library(book); // Library Constructor for Menu
    unique_ptr<Menu> startProgram = make_unique<Menu>(library); // Function Wrapper: Smart Pointer -> Function
    startProgram->loadBooklist(); // Load Bookfiles
    //startProgram->displayMenu(); // [PROGRAM]
    startProgram->displayLibraryMenu(); // [DEBUGGING]
    return 0;
}