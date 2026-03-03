#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FILE_NAME "pharmacy.dat"
#define LOW_STOCK 5
#define MAX_CART 100

// STRUCTURES 
struct Medicine {
    int id;
    char name[50];
    int quantity;
    float price;
    char expiryDate[15];   // YYYY-MM-DD
};

struct CartItem {
    int id;
    int quantity;
    float subtotal;
};

float totalSales = 0;


// ADD MEDICINE ----------------------------------------------------------------
void addMedicine() {
    FILE *fp = fopen(FILE_NAME, "ab");
    struct Medicine m;

    if (!fp) {
        printf("File error!\n");
        return;
    }

    printf("\n                 ADD NEW MEDICINE\n");
    printf("------------------------------------------------------------n");

    printf("Enter ID: ");
    scanf("%d", &m.id);

    printf("Enter Name: ");
    scanf(" %[^\n]", m.name);

    printf("Enter Quantity: ");
    scanf("%d", &m.quantity);

    printf("Enter Price: ");
    scanf("%f", &m.price);

    printf("Enter Expiry Date (YYYY-MM-DD): ");
    scanf("%s", m.expiryDate);

    fwrite(&m, sizeof(m), 1, fp);
    fclose(fp);

    printf("Medicine Added Successfully!\n");
}

// STOCK REFILL ----------------------------------------------------------------
void refillStock() {
    FILE *fp = fopen(FILE_NAME, "rb+");
    struct Medicine m;
    int id, addQty;
    int found = 0;

    if (!fp) {
        printf("File not found!\n");
        return;
    }

    printf("                 STOCK REFILL\n");
    printf("------------------------------------------------------------\n");

    printf("Enter Medicine ID: ");
    scanf("%d", &id);

    while (fread(&m, sizeof(m), 1, fp)) {
        if (m.id == id) {
            found = 1;

            printf("Current Quantity: %d\n", m.quantity);
            printf("Enter Quantity to Add: ");
            scanf("%d", &addQty);

            if (addQty > 0) {
                m.quantity += addQty;

                fseek(fp, -sizeof(m), SEEK_CUR);
                fwrite(&m, sizeof(m), 1, fp);

                printf("Stock Updated Successfully!\n");
            } else {
                printf("Invalid quantity!\n");
            }
            break;
        }
    }

    if (!found)
        printf("Medicine Not Found!\n");

    fclose(fp);
}

// DISPLAY INVENTORY -----------------------------------------------------------
void displayInventory() {
    FILE *fp = fopen(FILE_NAME, "rb");
    struct Medicine m;

    if (!fp) {
        printf("No records found!\n");
        return;
    }

    printf("                    INVENTORY LIST\n");
    printf("------------------------------------------------------------\n");

    printf("%-5s %-20s %-8s %-8s %-12s\n",
           "ID", "NAME", "QTY", "PRICE", "EXPIRY");

    printf("------------------------------------------------------------\n");

    while (fread(&m, sizeof(m), 1, fp)) {
        printf("%-5d %-20s %-8d %-8.2f %-12s\n",
               m.id, m.name, m.quantity, m.price, m.expiryDate);
    }
	
	printf("\n");
	
    fclose(fp);
}


// DATE COMPARISON 
int isExpired(char expiry[], char today[]) {
    return strcmp(expiry, today) < 0;
}

// CHECK EXPIRY ----------------------------------------------------------------
void checkExpiry() {
    FILE *fp = fopen(FILE_NAME, "rb");
    struct Medicine m;
    char today[15];
    int found = 0;

    if (!fp) {
        printf("No records found!\n");
        return;
    }

    printf("Enter today's date (YYYY-MM-DD): ");
    scanf("%s", today);

    printf("                EXPIRED MEDICINES\n");
    printf("------------------------------------------------------------\n");

    while (fread(&m, sizeof(m), 1, fp)) {
        if (isExpired(m.expiryDate, today)) {
            printf("%-20s | Expiry: %s\n", m.name, m.expiryDate);
            found = 1;
        }
    }

    if (!found)
        printf("No expired medicines found.\n");

    fclose(fp);
}

// SELL MEDECINE - POS ---------------------------------------------------------
void sellMedicinePOS() {
    FILE *fp = fopen(FILE_NAME, "rb+");
    if (fp == NULL) {
        printf("No inventory found!\n");
        return;
    }

    struct Medicine m;
    struct Medicine cart[100];
    int cartQty[100];
    int cartCount = 0;
    float grandTotal = 0;
    int id, qty;
    char choice;

    printf("\n                    PHARMACY POS\n");
    printf("------------------------------------------------------------\n");

    while (1) {
        printf("\nEnter Medicine ID: ");
        scanf("%d", &id);

        rewind(fp);
        int found = 0;

        while (fread(&m, sizeof(m), 1, fp)) {
            if (m.id == id) {
                found = 1;

                printf("\n%-20s | Av.Qty: %-5d | Unit: %-8.2f",
                       m.name, m.quantity, m.price);

                if (m.quantity <= 5)
                    printf(" ***LOW STOCK***");

                printf("\nEnter Quantity: ");
                scanf("%d", &qty);

                if (qty > m.quantity) {
                    printf("Not enough stock available!\n");
                    break;
                }

                float subtotal = qty * m.price;
                grandTotal += subtotal;

                cart[cartCount] = m;
                cartQty[cartCount] = qty;
                cartCount++;

                printf("Subtotal: %.2f\n", subtotal);
                printf("Running Total: %.2f\n", grandTotal);

                break;
            }
        }

        if (!found) {
            printf("Medicine not found!\n");
        }

        // Show Cart Summary After Each Item
        printf("\nCart Summary:\n");
        printf("------------------------------------------------------------\n");
        printf("ID   NAME                 QTY    PRICE     SUBTOTAL\n");
        printf("------------------------------------------------------------\n");
		int i;
        for (i = 0; i < cartCount; i++) {
            printf("%-4d %-20s %-6d %-8.2f %-8.2f\n",
                   cart[i].id,
                   cart[i].name,
                   cartQty[i],
                   cart[i].price,
                   cartQty[i] * cart[i].price);
        }

        //printf("------------------------------------------------------------\n");
        printf("GRAND TOTAL: %.2f\n", grandTotal);

        printf("\nConfirm Sale? (1=Yes / 0=Cancel / C=Continue): ");
        scanf(" %c", &choice);

        if (choice == 'C' || choice == 'c') {
            continue;
        } else if (choice == '0') {
            printf("Transaction Cancelled!\n");
            fclose(fp);
            return;
        } else if (choice == '1') {
            break;
        }
    }

    // Finalise the Sale
    rewind(fp);
	int i;
    for (i = 0; i < cartCount; i++) {
        while (fread(&m, sizeof(m), 1, fp)) {
            if (m.id == cart[i].id) {
                m.quantity -= cartQty[i];
                fseek(fp, -sizeof(m), SEEK_CUR);
                fwrite(&m, sizeof(m), 1, fp);
                break;
            }
        }
        rewind(fp);
    }

    fclose(fp);

    // Save Total Sales
    FILE *sales = fopen("sales.dat", "ab");
    fwrite(&grandTotal, sizeof(grandTotal), 1, sales);
    fclose(sales);

    // Print Receipt
    time_t t;
    time(&t);

    printf("\n____________________________________________________________\n");
    printf("                    PHARMACY RECEIPT\n");
    //printf("____________________________________________________________\n");
    printf("            Date: %s", ctime(&t));
    printf("------------------------------------------------------------\n");
	int j;
    for (j = 0; j < cartCount; j++) {
        printf("%-15s x%d = %.2f\n",
               cart[j].name,
               cartQty[j],
               cartQty[j] * cart[j].price);
    }

    printf("------------------------------------------------------------\n");
    printf("TOTAL AMOUNT: %.2f\n", grandTotal);
    //printf("____________________________________________________________\n");
    printf("Thank you for your Purchase!\n");
    printf("____________________________________________________________\n\n");
}

// SHOW TOTAL SALES ------------------------------------------------------------
void showTotalSales() {
    FILE *fp = fopen("sales.dat", "rb");
    float amount;
    float total = 0;

    if (!fp) {
        printf("No sales records found.\n");
        return;
    }

    while (fread(&amount, sizeof(amount), 1, fp)) {
        total += amount;
    }

    fclose(fp);

    printf("\nTotal Sales So Far: %.2f\n", total);
    printf("------------------------------------------------------------\n");
}

// MAIN METHOD -----------------------------------------------------------------

int main() {

    int choice;

    while (1) {

        printf("PHARMACY INVENTORY SYSTEM\n");
        printf("1. Add Medicine\n");
        printf("2. Stock Refill\n");
        printf("3. Sell Medicine (POS)\n");
        printf("4. Display Inventory\n");
        printf("5. Check Expiry\n");
        printf("6. Show Total Sales\n");
        printf("7. Exit\n");
        printf("\n");
        printf("Select Option: ");
        scanf("%d", &choice);

        switch (choice) {

            case 1: addMedicine(); break;
            case 2: refillStock(); break;
            case 3: sellMedicinePOS(); break;
            case 4: displayInventory(); break;
            case 5: checkExpiry(); break;
            case 6: showTotalSales(); break;
            case 7: exit(0);
            default: printf("Invalid Choice!\n");
        }
    }

    return 0;
}