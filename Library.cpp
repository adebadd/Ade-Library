/****************************************
ADS2 Assignment 1
Library System
Student Name: Abdul Badmos
Student Number: C21325446
 ****************************************/
    #include <stdlib.h>
    #include <cstdio>
    #include <string.h>
    #include <stdio.h>
    #include <conio.h>
    #include <windows.h> 
    #include <stdbool.h>
    #include <time.h>

#define _CRT_SECURE_NO_WARNINGS

    // Defines maximum length of book name and author name
    #define MAX_BOOKNAME_LENGTH 50

    // Defines maximum length of ajuthorname
    #define MAX_AUTHORNAME_LENGTH 50


    // Defines maximum length of customers name
    #define MAX_CUSTOMER_NAME_LENGTH 100


    // Defines the maximum number of books that can be stored in the list
    #define MAX_BOOKS 5


    // Defines the maximum duration a book can be borrowed for
    #define MAX_BORROW_DURATION 4 // set this number to any value to test overdue function but I will set it to 30 days  2,628,288 seconds in 30 days
                            

    //Structure template for data part of the linked list
    struct Book {
        char identifier[9]; // 8 digit number in the form xxxx-xxxx
        char bookName[MAX_BOOKNAME_LENGTH];
        char author[MAX_AUTHORNAME_LENGTH];
        int year;
        bool status; // indicates if the book is currently loaned out or not 
        //loaned = 1, notloaned = 0
        char customerName[MAX_CUSTOMER_NAME_LENGTH];
        int borrowCount;
        time_t dueDate; // the date when the book is due to be returned

   
    };

    //Structure template for one node
    struct LinearNode {
        struct Book* element;
        struct LinearNode* next;
    };

    // Global Variables

    struct LinearNode* front = NULL; //pointer to the front of the list
    struct LinearNode* last = NULL; //pointer to last node in the list

    // Function prototypes
    void addBook();// adds new book to library
    void borrowBook();// "borrows" a book from the library by increasing the borrow count and setting the book status to "unavailible"
    void returnBook();//"returns" a book back to the libraru by setting the status to availible and removing the borrowers name from the borrowed to
    void deleteBook();//deletes a book from the book list
    void viewAllBooks();//displays all books in the list in a user friendly format
    void viewBook();// searches the list and displays a specific book based identifier inputted
    void viewPopularBooks();// displays the book with the highest borrower count 
    void viewLeastPopularBooks();// displays the book with the lowest borrower count
    void overdueBooks();//displays books that are overdue
    bool isEmpty();// checks the list to see if it empty by checking the book at the front of the list


    //Main Function
    int main() {

        // Check if book.dat exists
      FILE* fp;
errno_t err = fopen_s(&fp, "book.dat", "wb");


// Load books into linked list
if (fp == NULL) {
    printf("Database of books doesn't exist. Please add books to the system using the addBook method.\n");
    Sleep(2000);
} else {
    printf("System has been populated with books from the data file.\n");
    Sleep(2000);
    struct Book book; // create a temporary book struct to store data read from file
    while (fread(&book, sizeof(struct Book), 1, fp)) { // read a book from the file
        struct LinearNode* newNode = (struct LinearNode*)malloc(sizeof(struct LinearNode)); // create a new node
        newNode->element = (struct Book*)malloc(sizeof(struct Book)); // allocate memory for the book in the node
        memcpy(newNode->element, &book, sizeof(struct Book)); // copy the book data to the node
        newNode->next = NULL; // set the next pointer of the node to NULL
        if (front == NULL) { // if the list is empty
            front = newNode; // set the front pointer to the new node
            last = newNode; // set the last pointer to the new node
        }
        else { // if the list is not empty
            last->next = newNode; // set the next pointer of the last node to the new node
            last = newNode; // set the last pointer to the new node
        }
    }
    fclose(fp);
}   
        int choice;//varaible to store users inputed number

        system("cls");//clears console
        do {
            printf("========================================\n");
            printf("           ADE LIBRARY STYSTEM         \n");
            printf("========================================\n\n");
            printf("1. Add a new book to the library\n");
            printf("2. Borrow Book \n");
            printf("3. Return a book\n");
            printf("4. Remove a book from the library\n");
            printf("5. View all books in library\n");
            printf("6. View a specific book\n");
            printf("7. View details of most popular and least popular books\n");
            printf("8. List overdue borrowed books\n");
            printf("9. Exit the system\n");
            printf("\nPlease enter your choice (1-9): ");



            scanf_s("%d", &choice);// takes user input

            //
            switch (choice) {
            case 1:
                addBook();
                break;
            case 2:
                borrowBook();
                break;
            case 3:
                returnBook();
                break;
            case 4:
                deleteBook();
                break;
            case 5:
                viewAllBooks();
                break;
            case 6:
                viewBook();
                break;
            case 7:
                viewPopularBooks();
                viewLeastPopularBooks();
                break;
            case 8:
                overdueBooks();
                break;
            case 9:
                printf("Thank you for visiting ADE LIBRARY!\n");// exits the library
                break;
            default:
                printf("Invalid choice, please try again");
                break;  
            }
        } while (choice != 9);
        // Copy all books back to database before exiting
       
        struct LinearNode* current = front;
        while (current != NULL) {
            fwrite(current->element, sizeof(struct Book), 1, fp);
            current = current->next;
        }
        fclose(fp);

        return 0;
    }

   
 

    void addBook() {
        system("cls");//clears the console
        // Check if maximum number of books have already been added
        int count = 0; // initialize variable count used to count the number of elements in the linked list
        struct LinearNode* current = front;// Define the current node in the list aka the node at the front of the list
        while (current != NULL) { //if the linkedlist is not null
            count++; // count how many elements are in the list
            current = current->next; // and set the current element to next 
        }
        if (count == MAX_BOOKS) {
            printf("Error: Maximum number of books reached.\n");// display message if maximum amount of books has been reached
            return;
        }
        //message to let users know where they are 
        printf("========================================\n");
        printf("        ADD NEW BOOK TO ADE LIBRARY      \n");
        printf("========================================\n\n");

        // Get details of new book from user
        struct Book* newBook = (struct Book*)malloc(sizeof(struct Book));

        bool validId = false;// creates boolean value
        while (!validId) {
            printf("Enter book identifier:");
            scanf_s("%8s", newBook->identifier, sizeof(newBook->identifier)); // limit input to 8 characters
            if (strlen(newBook->identifier) != 8 || newBook->identifier[4] != '-') { // check format of id
                printf("Error: Invalid book identifier format. \nNOTE: ID must be formatted as shown xxxx-xxxx, and must only include letters\n\n");
            }
            else {
                // Check if book identifier is already in use
                current = front;
                bool idInUse = false;
                while (current != NULL) {
                    if (strcmp(current->element->identifier, newBook->identifier) == 0) {
                        printf("Error: Book identifier already in use.\n");
                        idInUse = true;
                        break;
                    }
                    current = current->next;
                }
                if (!idInUse) {
                    validId = true;
                }
            }

        }


        bool validYear = false;
        while (!validYear) {

            printf("\nEnter year of publication of new book:");
            getchar();
            scanf_s("%d", &newBook->year);
            if (newBook->year < 2017) {
                printf("ERROR!: Publication year cannot be older than 2017.\n");
                continue; // Add this line to skip the current iteration and ask the user for input again
            }
            if (newBook->year > 2023) {
                printf("ERROR!: Publication year cannot be more recent than 2023.\n");
                continue;
            }
            else {
                validYear = true;
            }

            printf("Enter name of new book:");
            scanf_s(" %[^\n]s", newBook->bookName, MAX_BOOKNAME_LENGTH);

            printf("Enter author of new book:");
            scanf_s(" %[^\n]s", newBook->author, MAX_AUTHORNAME_LENGTH);

            // Initialise status of new book
            newBook->status = false;
            strcpy_s(newBook->customerName, MAX_CUSTOMER_NAME_LENGTH, "");
            newBook->borrowCount = 0;
            newBook->dueDate = 0; // set dueDate to a null value

            FILE* fp;
            errno_t err = fopen_s(&fp, "book.dat", "ab");
            if (err != 0) {
                printf("Error opening book.dat file.\n");
                return;
            }

            // Write new book to file and close file
            fwrite(newBook, sizeof(struct Book), 1, fp);
            fclose(fp);


            // Create and insert new node into linked list
            struct LinearNode* newNode = (struct LinearNode*)malloc(sizeof(struct LinearNode));
            newNode->element = newBook;
            newNode->next = NULL;
            if (front == NULL) {
                front = newNode;
            }
            else {
                current = front;
                while (current->next != NULL) {
                    current = current->next;
                }
                current->next = newNode;
            }

          

            printf("\n\nBook successfully added!\n");
            printf("\nReturning to main menu......");
            getchar(); // Wait for user input

            Sleep(2000);
            system("cls"); // Clear screen

            return; // Return to main menu

        }
    }

    

    void borrowBook() {
        system("cls");
        printf("========================================\n");
        printf("      BORROW A BOOK FROM ADE LIBRARY    \n");
        printf("========================================\n\n");

        // Check if there are any books in the system
        if (front == NULL) {
            printf("There are no books in the system.\n");
            printf("\nReturning to main menu......");
            getchar(); // Wait for user input
            Sleep(2000);
            system("cls"); // Clear screen
            return; // Return to main menu
        }

        // Get customer name from user
        char customerName[MAX_CUSTOMER_NAME_LENGTH];
        printf("Enter your name to borrow a book: ");
        scanf_s("%s", customerName);

        // Get book identifier from user
        char identifier[9];
        printf("Enter book identifier of the book you would like to borrow:");
        scanf_s("%8s", identifier);

        // Search for book with matching identifier in linked list
        struct LinearNode* current = front;
        while (current != NULL) {
            if (strcmp(current->element->identifier, identifier) == 0) {
                // Book found, check if it is available for borrowing
                if (current->element->status == 1) {
                    printf("Unfortunately, this book is currently borrowed to %s.\n", current->element->customerName);// displays the name of th current owner of the book
                    printf("\nReturning to main menu......");
                    getchar(); // Wait for user input

                    Sleep(2000);
                    system("cls"); // Clear screen

                    return; // Return to main menu
                }
                else {
                    // Book available, update status and customer name
                    current->element->status = 1;
                    strcpy_s(current->element->customerName, customerName);
                    current->element->borrowCount++;

                    time_t rawtime;
                    time(&rawtime);
                    current->element->dueDate = rawtime + MAX_BORROW_DURATION;
                    time_t current_time = time(NULL);
                    double timeremaining = difftime(current->element->dueDate, current_time);
                    double daysremaining = timeremaining / 86400;
               
                    if (daysremaining > 0) {
                        printf("\nBook borrowed successfully! Please return it within %.0f days.\n", daysremaining);
                    }

                    printf("\nReturning to main menu......");
                    getchar(); // Wait for user input

                    Sleep(2000);
                    system("cls"); // Clears console

                    return; // Return to main menu
                }
            }
            current = current->next;
        }

        // If book not found in linked list
        printf("ERROR! Invalid book identifier.\n");

        printf("\nReturning to main menu......");
        getchar(); // Wait for user input
        Sleep(2000);
        system("cls"); // Clears console
        return;
    }

bool isEmpty() {
    return front == NULL;
}
void returnBook() {
    system("cls");
    printf("========================================\n");
    printf("       RETURN A BOOK TO ADE LIBRARY     \n");
    printf("========================================\n\n");

    // Ask for the book identifier
    char identifier[9];
    printf("Enter book identifier:");
    scanf_s("%8s", identifier);

    // clear input buffer
    while (getchar() != '\n');

    // Search for the book in the list
    struct LinearNode* current = front;
    struct Book* book = NULL;
    while (current != NULL) {
        if (strcmp(current->element->identifier, identifier) == 0) {
            book = current->element;
            break;
        }
        current = current->next;
    }

    // If the book is not found, display an error message and return
    if (book == NULL) {
        printf("Error: Book not found.\n");
        printf("\nReturning to main menu......");
        Sleep(2000);
        system("cls"); // Clear screen
        return;
    }

    // If the book is not loaned out, display an error message and return
    if (!book->status) {
        printf("\nError: This book has not been borrowed.\n");
        printf("\nReturning to main menu......");
        Sleep(2000);
        system("cls"); // Clear screen
        return;
    }

    // Ask for the customer name
    char customerName[50];
    printf("Enter customer name:");
    scanf_s("%49s", customerName, 50);

    // Compare the customer name entered by the user with the name stored in the book's data structure
    if (strcmp(book->customerName, customerName) == 0) {
        // If the names match, update the book status, borrow count, and display a success message
        book->status = false;
        book->borrowCount++;
        current->element->dueDate = 0;

        // check if book was returned overdue
        time_t rawtime;
        time(&rawtime);
        if (rawtime > current->element->dueDate) {
            printf("\nWARNING: Book was returned overdue!\n");
        }

        strcpy_s(book->customerName, ""); // set customerName to an empty string
        printf("Book successfully returned.\n");
    }
    else {
        // If the names do not match error message is displayed
        printf("Error: The customer name entered does not match the name of the borrower of this book.\n");
    }

    printf("\nReturning to main menu......");
    Sleep(2000);
    system("cls"); // Clears console
}

void insertBook(struct Book* book) {
    struct LinearNode* temp;
    temp = (struct LinearNode*)malloc(sizeof(struct LinearNode));
    temp->element = book;
    temp->next = NULL;
    if (front == NULL) {
        front = temp;
        last = temp;
    }
    else {
        last->next = temp;
        last = temp;
    }
}

// Function to delete a specific book
void deleteBook() {
    system("cls");
    printf("========================================\n");
    printf("      DELETE BOOK FROM ADE LIBRARY         \n");
    printf("========================================\n\n");

    if (isEmpty()) { //check if the library is empty
        printf("The library is empty!\n");
        return;// returns the user to the main menu
    }

    char id[9];
    printf("Enter book identifier:");
    scanf_s("%8s", id, sizeof(id));

    // Traverses the linked list using the book identifier
    struct LinearNode* current = front;
    struct LinearNode* previous = NULL;
    bool bookFound = false;

    while (current != NULL) {//starts a loop that iterates through each node in the list until the end
        if (strcmp(current->element->identifier, id) == 0) { //if id match found
            bookFound = true;// sets boolean to true if book that matches inputted id is found
            break;
        }
        previous = current;
        current = current->next;
    }

    if (!bookFound) {
        printf("Error: Book with identifier %s not found\n", id);
        return;
    }

    // Prompt the user for confirmation before deleting the book
    printf("Are you sure you want to delete the following book?\n\n");
    printf("Identifier: %s\n", current->element->identifier);
    printf("Name: %s\n", current->element->bookName);
    printf("Author: %s\n", current->element->author);
    printf("Year: %d\n", current->element->year);
    if (current->element->status == 0) {
        printf("Status: Available\n");
    }
    else {
        printf("Status: Unavailable\n");
    }
    printf("Name of customer currently borrowed to: %s\n", current->element->customerName);
    printf("Number of times borrowed: %d\n", current->element->borrowCount);
    printf("\nEnter Y to confirm deletion, or any other key to cancel:");
    char confirm;
    while (getchar() != '\n');
    scanf_s("%c", &confirm, sizeof(confirm));
    while (getchar() != '\n'); // clear the input buffer

    if (confirm == 'Y' || confirm == 'y') {
        // Delete the book from the linked list
        if (current == front) {
            front = current->next;
        }
        else {
            previous->next = current->next;// sets the previous node in the list to the current node and the current node to the next
        }
        free(current->element);
        free(current);
        printf("Book with identifier %s has been deleted from the library system.\n", id);//display deletion confirmation message
    }
    else {
        printf("Book deletion cancelled.\n");
    }
    printf("\nReturning to main menu......");
    Sleep(2000); //waits before returning user to menu
    system("cls"); // Clear screen
}

void viewAllBooks() {
    system("cls");

    
    printf("========================================\n");
    printf("         ALL BOOKS IN ADE LIBRARY        \n");
    printf("========================================\n\n");

    if (isEmpty()) {
        printf("No books found in ADE LIBRARY.\n");
        printf("\nReturning to main menu......");
        Sleep(2000); //waits 2 seconds before returning user to menu
        system("cls"); // Clear screen
        return;
    }

    struct LinearNode* current = front;
    printf("%-15s%-30s%-30s%-15s%-15s%-30s\n", "Identifier", "Book Name", "Author", "Year", "Status", "Loaned to");
    printf("-----------------------------------------------------------------------------------------------------------------------\n");
    while (current != NULL) {
        printf("%-15s%-30s%-30s%-15d%-15s%-30s\n", current->element->identifier, current->element->bookName, current->element->author, current->element->year, (current->element->status == 1 ? "Loaned" : "Not loaned"), current->element->customerName);
        current = current->next;
    }

    printf("Press Y to return to main menu: ");
    char choice = getchar();
    scanf_s("%c", &choice, sizeof(choice));

    if (choice == 'Y' || choice == 'y') {
        system("cls"); // Clears console
        return;
    }
}

void viewBook() {
    system("cls");//clears screen
    //check if the library system is empty
    if (isEmpty()) {
        printf("No books found in the library.\n");
        return;
    }
    
    printf("========================================\n");
    printf("          VIEW A BOOK IN ADE LIBRARY    \n");
    printf("========================================\n\n");


    char identifier[9];//this declares array of character "identifier" 
    printf("Enter book identifier:");
    scanf_s("%8s", identifier, sizeof(identifier));//this reads the book identifier inputted by the user and stores it in the identifier array, with a maximum size of 8 characters to prevent buffer overflow.

    struct LinearNode* current = front;//
    bool bookFound = false;// initialises boolean that is set to false
    while (current != NULL) { //starts a loop that iterates through each node in the list until the end
        if (strcmp(current->element->identifier, identifier) == 0) {
            bookFound = true;// sets boolean to true if book that matches inputted id is found
            printf("Book Name: %s\n", current->element->bookName);
            printf("Author: %s\n", current->element->author);
            printf("Year: %d\n", current->element->year);
            printf("Status: %s\n", current->element->status ? "Loaned" : "Not Loaned");// prints the status of the book stored
            if (current->element->status) {
                printf("Currently borrowed by : %s\n", current->element->customerName);//prints the name of the customer who is currently being borrowed the book
            }
            printf("Borrow Count: %d\n", current->element->borrowCount);// prints the number of times a book has been borrowed

            printf("\nEnter Y to return to the main menu\n");
            while (getchar() != '\n');
            char choice;
            scanf_s(" %c", &choice, sizeof(choice));
            if (choice == 'Y' || choice == 'y') {
                system("cls");
                return;
            }
        }
        current = current->next;
    }
    if (!bookFound) {//if the book is not found message is displayed and user is returned to main menu
        printf("Book not found in the library.\n");
        printf("\nReturning to main menu......");
        Sleep(2000);
        system("cls"); // Clear screen
    }


}


//printf("\nReturning to main menu......");
//Sleep(2000);
//system("cls"); // Clear screen

// prints book with the highest borrow count number 
void viewPopularBooks() { 
    system("cls"); //clears console
    printf("========================================\n");
    printf("    MOST POPULAR BOOK IN ADE LIBRARY         \n");
    printf("========================================\n\n");

    //check if the library system is empty
    if (isEmpty()) { 
        printf("The library is empty!\n");
        printf("\nReturning to main menu......");
        Sleep(2000); //waits before returning user to menu
        system("cls"); // Clear screen
        return;//returns user to menu 
    }

    int maxCount = 0;//initializes variable to store the max number of times a book has been borrowed
    struct LinearNode* current = front;//initialises pointer to the first node in the linkedl ist
    struct LinearNode* mostPopular = current;//initialises pointer to the to the most popular book which is the front book

    while (current != NULL) { //Loop that iterates through the linked list
        if (current->element->borrowCount > maxCount) { //checks if the current book has been borrowed more than the previous most popular
            maxCount = current->element->borrowCount; //updates the maximum number of times a book has been borrowed
            mostPopular = current;//sets the most popular book to the current book
        }
        current = current->next;// oves the current pointer to the next node in the linked list
    }

    //prints the most popular books details
    printf("The most popular book in the library is:\n\n");
    printf("Book Identifier: %s\n", mostPopular->element->identifier);
    printf("Book Name: %s\n", mostPopular->element->bookName);
    printf("Author: %s\n", mostPopular->element->author);
    printf("Year: %d\n", mostPopular->element->year);
    printf("Number of times borrowed: %d\n", mostPopular->element->borrowCount);

}

void viewLeastPopularBooks() {
    printf("\n\n");
    printf("========================================\n");//
    printf("    LEAST POPULAR BOOK IN ADE LIBRARY       \n");
    printf("========================================\n");

    if (isEmpty()) { //checks if the library is empty
        printf("The library is empty!\n");//displays message
        printf("\nReturning to main menu......");
        Sleep(2000);//waits 2 seconds before returning users to menu
        system("cls"); // Clears console
        return;
    }

    int minCount = INT_MAX; //initializes a variable minCount to the maximum possible integer value
    struct LinearNode* current = front;// initializes a pointer the current to the first element
    struct LinearNode* leastPopular = current;

    while (current != NULL) {//loop that iterates through each element in the linked list until it reaches the end
        if (current->element->borrowCount < minCount) { //checks if the current element's borrow count is less than minCount
            minCount = current->element->borrowCount;// checks if the current element is less than minCount
            leastPopular = current;// sets the leastpopular book to the current current element
        }
        current = current->next;
    }

    //displays the details of the least popular book in the linked list
    printf("The least popular book in the library is:\n\n");
    printf("Identifier: %s\n", leastPopular->element->identifier);
    printf("Name: %s\n", leastPopular->element->bookName);
    printf("Author: %s\n", leastPopular->element->author);
    printf("Year: %d\n", leastPopular->element->year);
    printf("Number of times borrowed: %d\n", leastPopular->element->borrowCount);
    printf("\n\nEnter Y to return to the main menu\n");
    while (getchar() != '\n');

    char choice;
    scanf_s(" %c", &choice, sizeof(choice));
    if (choice == 'Y' || choice == 'y') {
        system("cls");
        return;
    }


}

void overdueBooks() {
    system("cls");
    printf("========================================\n");
    printf("        OVERDUE BOOKS IN ADE LIBRARY     \n");
    printf("========================================\n\n");

    // Check if there are any books in the system
    if (front == NULL) {
        printf("There are no books in the system.\n");
        printf("\nReturning to main menu......");
        getchar(); // Wait for user input
        Sleep(2000);
        system("cls"); // Clears console
        return; // Return to main menu
    }

    // Get current time
    time_t rawtime; //delcares rawtime of type time_t
    time(&rawtime);//assigns the current system time to rawtime

    // iterates through linked list of books and check if due date has passed
    struct LinearNode* current = front;
    while (current != NULL) { 
        char dueDateString[26];
        if (current->element->status == 1 && current->element->dueDate < rawtime) {//checks loaned books to see if they have passed the due date
            // prints all books that are overdue
            printf("Book Identifier: %s\n", current->element->identifier);
            printf("Book name: %s\n", current->element->bookName);
            printf("Author: %s\n", current->element->author);
            printf("Currently borrowed by : %s\n", current->element->customerName);
            ctime_s(dueDateString, sizeof(dueDateString), &current->element->dueDate);
            printf("Book was due to be returned: %s", dueDateString);
            printf("\n");
        }
        current = current->next;
    }

    // Check if there are any overdue books
    int count = 0;
    current = front;
    while (current != NULL) {
        if (current->element->status == 1 && current->element->dueDate < rawtime) {
            count++;
        }
        current = current->next;
    }

    if (count == 0) {
        printf("There are no overdue books in the system.\n");
    }

    printf("\n\nEnter Y to return to the main menu\n");
    while (getchar() != '\n');
    char choice;
    scanf_s(" %c", &choice, sizeof(choice));
    if (choice == 'Y' || choice == 'y') {
        system("cls");
        return;
    }

}