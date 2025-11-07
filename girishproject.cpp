#include <iostream>
#include <string>
#include <fstream>
using namespace std;

struct Location {
    int shelf;
    int rack;
    Location operator+(const Location& other) const {
        Location temp;
        temp.shelf = shelf + other.shelf;
        temp.rack = rack + other.rack;
        return temp;
    }
};

class Book;
void showPrivateISBN(const Book& b);

class Book {
private:
    string isbn;
    const string libraryName = "CityLibrary";
protected:
    Location loc;
    string status;

public:
    static int totalBooks;
    int bookId;
    string title;
    string author;
    int copies;

    Book() {
        bookId = 0;
        isbn = "0000-DEFAULT";
        title = "Untitled";
        author = "Unknown";
        copies = 0;
        status = "Unavailable";
        loc = {0, 0};
        totalBooks++;
    }

    Book(int id, const string& _isbn, const string& t, const string& a, int c) {
        bookId = id;
        isbn = _isbn;
        title = t;
        author = a;
        copies = c;
        status = (c > 0 ? "Available" : "Unavailable");
        loc = {1, 1};
        totalBooks++;
    }

    Book(const Book& other) {
        bookId = other.bookId;
        isbn = other.isbn;
        title = other.title;
        author = other.author;
        copies = other.copies;
        status = other.status;
        loc = other.loc;
        totalBooks++;
    }

    virtual ~Book() {}

    void setData(int id, const string& _isbn, const string& t, const string& a, int c) {
        bookId = id;
        isbn = _isbn;
        title = t;
        author = a;
        copies = c;
        status = (c > 0 ? "Available" : "Unavailable");
    }

    void setLocation(int shelf, int rack) {
        loc = {shelf, rack};
    }

    void setStatus(const string& s) {
        this->status = s;
    }

    string getStatus() const {      // ✅ getter for protected member
        return status;
    }

    virtual void checkStatus() {
        cout << "Book ID: " << bookId << ", Title: " << title
             << ", Status: " << status << ", Copies: " << copies << endl;
    }

    static int getTotalBooks() {
        return totalBooks;
    }

    void showLibrary() const {
        cout << "Library: " << libraryName << endl;
    }

    friend void showPrivateISBN(const Book& b) {
        cout << "Book ID: " << b.bookId << ", ISBN: " << b.isbn << endl;
    }

    friend ostream& operator<<(ostream& os, const Book& b) {
        os << "Book ID: " << b.bookId << "\nTitle: " << b.title
           << "\nAuthor: " << b.author << "\nCopies: " << b.copies
           << "\nStatus: " << b.status << "\nLocation: Shelf "
           << b.loc.shelf << " Rack " << b.loc.rack << endl;
        return os;
    }

    void borrowBook() {
        if (copies <= 0)
            throw runtime_error("No copies available to borrow!");
        copies--;
        if (copies == 0) status = "Unavailable";
    }

    void returnBook() {
        copies++;
        status = "Available";
    }
};

int Book::totalBooks = 0;

class ReferenceBook : public Book {
public:
    ReferenceBook(int id, const string& _isbn, const string& t, const string& a, int c) : Book() {
        setData(id, _isbn, t, a, c);
        setStatus("Reference only");
    }

    void checkStatus() override {
        cout << "ReferenceBook ID: " << bookId << ", Title: " << title << ", Status: " << getStatus() << endl;
    }
};

template <typename T>
T add(T a, T b) { return a + b; }

int main() {
    Book* books[10];
    int count = 0;
    int choice;

    do {
        cout << "\n========= LIBRARY MENU =========\n";
        cout << "1. Add Book\n";
        cout << "2. View All Books\n";
        cout << "3. Borrow Book\n";
        cout << "4. Return Book\n";
        cout << "5. Show Total Books\n";
        cout << "6. Save Books to File\n";
        cout << "7. Read Books from File\n";
        cout << "8. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                if (count >= 10) {
                    cout << "Book limit reached!\n";
                    break;
                }
                int id, copies;
                string isbn, title, author;
                cout << "Enter Book ID: ";
                cin >> id;
                cout << "Enter ISBN: ";
                cin >> isbn;
                cin.ignore();
                cout << "Enter Title: ";
                getline(cin, title);
                cout << "Enter Author: ";
                getline(cin, author);
                cout << "Enter Copies: ";
                cin >> copies;
                books[count] = new Book(id, isbn, title, author, copies);
                count++;
                cout << "Book added successfully!\n";
                break;
            }
            case 2: {
                if (count == 0) cout << "No books available.\n";
                else {
                    for (int i = 0; i < count; i++) {
                        cout << "\n--- Book " << (i + 1) << " ---\n";
                        cout << *books[i];
                    }
                }
                break;
            }
            case 3: {
                int id;
                cout << "Enter Book ID to borrow: ";
                cin >> id;
                bool found = false;
                for (int i = 0; i < count; i++) {
                    if (books[i]->bookId == id) {
                        try {
                            books[i]->borrowBook();
                            cout << "Borrowed successfully!\n";
                        } catch (exception& e) {
                            cout << e.what() << endl;
                        }
                        found = true;
                        break;
                    }
                }
                if (!found) cout << "Book not found!\n";
                break;
            }
            case 4: {
                int id;
                cout << "Enter Book ID to return: ";
                cin >> id;
                bool found = false;
                for (int i = 0; i < count; i++) {
                    if (books[i]->bookId == id) {
                        books[i]->returnBook();
                        cout << "Book returned successfully!\n";
                        found = true;
                        break;
                    }
                }
                if (!found) cout << "Book not found!\n";
                break;
            }
            case 5:
                cout << "Total Books Created: " << Book::getTotalBooks() << endl;
                break;

            case 6: {
                ofstream fout("library_data.txt");
                if (!fout.is_open()) {
                    cout << "Failed to open file.\n";
                    break;
                }
                for (int i = 0; i < count; i++) {
                    fout << books[i]->bookId << "," << books[i]->title << ","
                         << books[i]->author << "," << books[i]->copies << ","
                         << books[i]->getStatus() << endl;   // ✅ fixed here
                }
                fout.close();
                cout << "Books saved to file.\n";
                break;
            }

            case 7: {
                ifstream fin("library_data.txt");
                if (!fin.is_open()) {
                    cout << "File not found.\n";
                    break;
                }
                string line;
                cout << "\nReading from file:\n";
                while (getline(fin, line)) {
                    cout << line << endl;
                }
                fin.close();
                break;
            }

            case 8:
                cout << "Exiting program... Goodbye!\n";
                break;

            default:
                cout << "Invalid choice! Try again.\n";
        }

    } while (choice != 8);

    for (int i = 0; i < count; i++) delete books[i];

    return 0;
}
