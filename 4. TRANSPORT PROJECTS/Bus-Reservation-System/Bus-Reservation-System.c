#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROWS 10
#define COLS 4
#define FILE_NAME "bus_reservation.dat"

struct Booking {
    char name[50];
    int row;
    int col;
};

int seats[ROWS][COLS] = {0};

// Display the Seat Layout -----------------------------------------------------
void displaySeats() {
    printf("\nSEAT LAYOUT ------------------\n");
    printf("0 = Available | 1 = Booked\n\n");
	int i, j;
    for (i = 0; i < ROWS; i++) {
        printf("Row %2d: ", i + 1);
        for (j = 0; j < COLS; j++) {
            printf("%d ", seats[i][j]);
        }
        printf("\n");
    }
}

// Load the Bookings from FIle -------------------------------------------------
void loadBookings() {
    FILE *fp = fopen(FILE_NAME, "rb");
    struct Booking b;

    if (!fp)
        return;

    while (fread(&b, sizeof(b), 1, fp)) {
        seats[b.row][b.col] = 1;
    }

    fclose(fp);
}

// Book Seat -------------------------------------------------------------------
void bookSeat() {
    int row, col;
    struct Booking b;
    FILE *fp = fopen(FILE_NAME, "ab");

    if (!fp) {
        printf("File error!\n");
        return;
    }

    displaySeats();

    printf("\nEnter Row (1-10): ");
    scanf("%d", &row);

    printf("Enter Seat (1-4): ");
    scanf("%d", &col);

    row--; col--;

    if (row < 0 || row >= ROWS || col < 0 || col >= COLS) {
        printf("Invalid seat number!\n");
        fclose(fp);
        return;
    }

    if (seats[row][col] == 1) {
        printf("Seat already booked!\n");
        fclose(fp);
        return;
    }

    printf("Enter Passenger Name: ");
    scanf(" %[^\n]", b.name);

    b.row = row;
    b.col = col;

    seats[row][col] = 1;

    fwrite(&b, sizeof(b), 1, fp);
    fclose(fp);

    printf("Seat booked successfully!\n");
}

// Cancel Booking --------------------------------------------------------------
void cancelBooking() {
    int row, col;
    struct Booking b;
    FILE *fp = fopen(FILE_NAME, "rb");
    FILE *temp = fopen("temp.dat", "wb");

    if (!fp || !temp) {
        printf("File error!\n");
        return;
    }

    displaySeats();

    printf("\nEnter Row to Cancel (1-10): ");
    scanf("%d", &row);

    printf("Enter Seat to Cancel (1-4): ");
    scanf("%d", &col);

    row--; col--;

    int found = 0;

    while (fread(&b, sizeof(b), 1, fp)) {
        if (b.row == row && b.col == col) {
            seats[row][col] = 0;
            found = 1;
            continue;
        }
        fwrite(&b, sizeof(b), 1, temp);
    }

    fclose(fp);
    fclose(temp);

    remove(FILE_NAME);
    rename("temp.dat", FILE_NAME);

    if (found)
        printf("Booking cancelled successfully!\n");
    else
        printf("Seat was not booked!\n");
}

// Display Passengers ----------------------------------------------------------

void displayPassengers() {
    FILE *fp = fopen(FILE_NAME, "rb");
    struct Booking b;

    if (!fp) {
        printf("No bookings found.\n");
        return;
    }

    printf("\nPASSENGER LIST ---------------------\n");

    while (fread(&b, sizeof(b), 1, fp)) {
        printf("Seat: Row %d Seat %d | Name: %s\n",
               b.row + 1, b.col + 1, b.name);
    }

    fclose(fp);
}

// Main Method or Main Menu ----------------------------------------------------

int main() {

    int choice;

    loadBookings();

    while (1) {
        printf("\nBUS RESERVATION SYSTEM\n");
        printf("1. View Seat Layout\n");
        printf("2. Book Seat\n");
        printf("3. Cancel Booking\n");
        printf("4. Display Passenger List\n");
        printf("5. Exit\n");
        printf("\nSelect Option: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: displaySeats(); break;
            case 2: bookSeat(); break;
            case 3: cancelBooking(); break;
            case 4: displayPassengers(); break;
            case 5: exit(0);
            default: printf("Invalid choice!\n");
        }
    }

    return 0;
}