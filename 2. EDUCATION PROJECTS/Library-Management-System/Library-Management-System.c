/*
 * Project: Library Management System
 * Category: Education Projects
 * Description: Manages a collection of books, allowing additions, searches, 
 *              issuing, and returning of books with due date tracking.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FILE_NAME "library.dat"

struct Book {
    int bookID;
    char title[100];
    char author[50];
    int available;          // 1 = Available, 0 = Issued

    time_t issueTime;       // System timestamp
    char returnDate[20];    // Expected return date
    char borrower[20];      // Student Reg No
};

// Global variables for formatting issue time
struct tm *tm_info;
char formattedTime[30];

void addBook();
void displayBooks();
void searchBook();
void issueBook();
void returnBook();

int main() {
    int choice;

    while (1) {
        printf("\nLIBRARY MANAGEMENT SYSTEM\n");
        printf("1. Add New Book\n");
        printf("2. Search Book\n");
        printf("3. Issue Book\n");
        printf("4. Return Book\n");
        printf("5. Display All Books\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addBook(); break;
            case 2: searchBook(); break;
            case 3: issueBook(); break;
            case 4: returnBook(); break;
            case 5: displayBooks(); break;
            case 6: exit(0);
            default: printf("Invalid choice!\n");
        }
    }

    return 0;
}

// Add a book ------------------------------------------------------------------
void addBook() {
    FILE *fp = fopen(FILE_NAME, "ab");
    struct Book book;

    if (fp == NULL) {
        printf("File error!\n");
        return;
    }

    printf("Enter Book ID: ");
    scanf("%d", &book.bookID);
    getchar();

    printf("Enter Title: ");
    fgets(book.title, sizeof(book.title), stdin);
    book.title[strcspn(book.title, "\n")] = 0;

    printf("Enter Author: ");
    fgets(book.author, sizeof(book.author), stdin);
    book.author[strcspn(book.author, "\n")] = 0;

    book.available = 1;

    fwrite(&book, sizeof(book), 1, fp);
    fclose(fp);

    printf("Book added successfully!\n");
}
// Show books ------------------------------------------------------------------
void displayBooks() {
    FILE *fp = fopen(FILE_NAME, "rb");
    struct Book book;

    if (fp == NULL) {
        printf("No records found!\n");
        return;
    }

    printf("\nBook List ---------------------------------------\n");
    while (fread(&book, sizeof(book), 1, fp)) {
        printf("ID: %d\nTitle: %s\nAuthor: %s\nStatus: %s\n\n",
               book.bookID, book.title, book.author,
               book.available ? "Available" : "Issued");
    }

    fclose(fp);
}

// Search a Book ---------------------------------------------------------------
void searchBook() {
    FILE *fp = fopen(FILE_NAME, "rb");
    struct Book book;
    int id, found = 0;

    if (fp == NULL) {
        printf("No records found!\n");
        return;
    }

    printf("Enter Book ID to search: ");
    scanf("%d", &id);

    while (fread(&book, sizeof(book), 1, fp)) {

        if (book.bookID == id) {

            printf("\nBook Found! ----------------------------------------\n");
            printf("Title: %s\n", book.title);
            printf("Author: %s\n", book.author);
            printf("Status: %s\n", book.available ? "Available" : "Issued");

            if (!book.available) {

                tm_info = localtime(&book.issueTime);

                strftime(formattedTime, sizeof(formattedTime),
                         "%d/%m/%Y %I:%M%p", tm_info);

                printf("Issued To (Reg No): %s\n", book.borrower);
                printf("Issue Date & Time : %s\n", formattedTime);
                printf("Expected Return Date: %s\n", book.returnDate);
            }

            found = 1;
            break;
        }
    }

    if (!found)
        printf("Book not found!\n");

    fclose(fp);
}

// Issue a book ----------------------------------------------------------------
void issueBook() {
    FILE *fp = fopen(FILE_NAME, "rb+");
    struct Book book;
    int id, found = 0;

    if (fp == NULL) {
        printf("No records found!\n");
        return;
    }

    printf("Enter Book ID to issue: ");
    scanf("%d", &id);

    while (fread(&book, sizeof(book), 1, fp)) {
        if (book.bookID == id) {
            if (book.available) {

                getchar(); // clear buffer

                printf("Enter Student Reg No: ");
                fgets(book.borrower, sizeof(book.borrower), stdin);
                book.borrower[strcspn(book.borrower, "\n")] = 0;

                printf("Enter Expected Return Date (DD/MM/YYYY): ");
                fgets(book.returnDate, sizeof(book.returnDate), stdin);
                book.returnDate[strcspn(book.returnDate, "\n")] = 0;

                book.issueTime = time(NULL);   // Capture current system time
                book.available = 0;

                fseek(fp, -sizeof(book), SEEK_CUR);
                fwrite(&book, sizeof(book), 1, fp);

                printf("Book issued successfully!\n");
            } else {
                printf("Book is already issued!\n");
            }
            found = 1;
            break;
        }
    }

    if (!found)
        printf("Book not found!\n");

    fclose(fp);
}

// Return a book ---------------------------------------------------------------
void returnBook() {
    FILE *fp = fopen(FILE_NAME, "rb+");
    struct Book book;
    int id, found = 0;

    if (fp == NULL) {
        printf("No records found!\n");
        return;
    }

    printf("Enter Book ID to return: ");
    scanf("%d", &id);

    while (fread(&book, sizeof(book), 1, fp)) {
        if (book.bookID == id) {
            if (!book.available) {
                book.available = 1;
                fseek(fp, -sizeof(book), SEEK_CUR);
                fwrite(&book, sizeof(book), 1, fp);
                printf("Book returned successfully!\n");
            } else {
                printf("Book was not issued!\n");
            }
            found = 1;
            break;
        }
    }

    if (!found)
        printf("Book not found!\n");

    fclose(fp);
}