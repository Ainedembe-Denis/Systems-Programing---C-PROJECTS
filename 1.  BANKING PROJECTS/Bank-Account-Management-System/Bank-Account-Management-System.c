#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TELLER_USER "Teller1"
#define TELLER_PASS "Teller123"
#define ADMIN_USER  "Sysadmin"
#define ADMIN_PASS  "admin123"
#define LOG_PASSWORD "logfile123"
#define MIN_UGX 500.0
#define MIN_USD 10.0

char CURRENT_USER[20] = "";
char CURRENT_ROLE[10] = ""; // TELLER or ADMIN


struct Account {
    long long accountNumber;
    char name[50];
    char currency[4];   // "UGX" or "USD"
    float balance;
};

// Function prototypes
void createAccount();
void deposit();
void withdraw();
void checkBalance();
void logAction(const char *action, long long accNo, float amount);
void viewLogs();
int accountExists(long long accNo);
const char* currencySymbol(const char *currency);

const char *filename = "accounts.dat";

// Helper: Currency symbol
const char* currencySymbol(const char *currency) {
    if (strcmp(currency, "UGX") == 0) return "Shs";
    if (strcmp(currency, "USD") == 0) return "$";
    return "";
}

int login() {
    char username[20], password[20];

    printf("\nLOGIN - FIN-BANK ACCOUNT MANAGEMENT\n");
    printf("Username: ");
    scanf("%19s", username);

    printf("Password: ");
    scanf("%19s", password);

    // Teller login
    if (strcmp(username, TELLER_USER) == 0 &&
        strcmp(password, TELLER_PASS) == 0) {

        strcpy(CURRENT_USER, username);
        strcpy(CURRENT_ROLE, "TELLER");

        printf("? Teller login successful.\n");
        logAction("LOGIN_SUCCESS", 0, 0);
        return 1;
    }

    // Admin login
    if (strcmp(username, ADMIN_USER) == 0 &&
        strcmp(password, ADMIN_PASS) == 0) {

        strcpy(CURRENT_USER, username);
        strcpy(CURRENT_ROLE, "ADMIN");

        printf("? Admin login successful.\n");
        logAction("LOGIN_SUCCESS", 0, 0);
        return 1;
    }

    // Failed login (log attempted username)
    strcpy(CURRENT_USER, username);
    logAction("LOGIN_FAILED", 0, 0);

    printf("? Invalid credentials!\n");
    return 0;
}

// Main Menu -----------------------------
int main() {
    int choice;

    if (!login()) return 0;

    do {
        printf("\nFIN-BANK ACCOUNT MANAGEMENT SYSTEM (%s) \n", CURRENT_ROLE);

        if (strcmp(CURRENT_ROLE, "TELLER") == 0) {
            printf("1. Create New Account\n");
            printf("2. Deposit Money\n");
            printf("3. Withdraw Money\n");
            printf("4. Check Balance / View Account\n");
            printf("6. Exit\n");
        } else {
            printf("4. Check Balance / View Account\n");
            printf("5. View Audit Logs (Protected)\n");
            printf("6. Exit\n");
        }

        printf("Enter your choice: ");
        scanf("%d", &choice);

        if (strcmp(CURRENT_ROLE, "TELLER") == 0) {
            switch(choice) {
                case 1: createAccount(); break;
                case 2: deposit(); break;
                case 3: withdraw(); break;
                case 4: checkBalance(); break;
                case 6:
				    logAction("LOGOUT", 0, 0);
				    printf("Exiting...\n");
				    break;
                default: printf("Invalid choice!\n");
            }
        } else {
            switch(choice) {
                case 4: checkBalance(); break;
                case 5: viewLogs(); break;
                case 6:
				    logAction("LOGOUT", 0, 0);
				    printf("Exiting...\n");
				    break;
                default: printf("Invalid choice!\n");
            }
        }

    } while(choice != 6);

    return 0;
}

// Check if account exists -----------------------------
int accountExists(long long accNo) {
    struct Account acc;
    FILE *fp = fopen(filename, "rb");
    if (!fp) return 0;

    while (fread(&acc, sizeof(acc), 1, fp)) {
        if (acc.accountNumber == accNo) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

// Logging Function (append-only) -----------------------------
void logAction(const char *action, long long accNo, float amount) {
    FILE *log = fopen("audit_log.txt", "a");
    if (!log) return;

    time_t now = time(NULL);
    struct tm *t = localtime(&now);

	fprintf(log,
	    "%04d-%02d-%02d %02d:%02d:%02d | USER:%s | %s | Acc:%lld | Amount:%.2f\n",
	    t->tm_year + 1900,
	    t->tm_mon + 1,
	    t->tm_mday,
	    t->tm_hour,
	    t->tm_min,
	    t->tm_sec,
	    CURRENT_USER,
	    action,
	    accNo,
	    amount
	);

    fclose(log);
}

// Create Account -----------------------------
void createAccount() {
    struct Account acc;
    FILE *fp = fopen(filename, "ab");

    if (!fp) {
        printf("File error!\n");
        return;
    }

    printf("\nEnter Account Number: ");
    scanf("%lld", &acc.accountNumber);

    // ?? Prevent duplicates
    if (accountExists(acc.accountNumber)) {
        printf("? Account already exists!\n");
        fclose(fp);
        return;
    }

    // ? Currency selection
    printf("Select Currency (UGX/USD): ");
    scanf("%3s", acc.currency);

    if (strcmp(acc.currency, "UGX") != 0 &&
        strcmp(acc.currency, "USD") != 0) {
        printf("? Invalid currency!\n");
        fclose(fp);
        return;
    }

    printf("Enter Name/Title: ");
    scanf(" %[^\n]", acc.name);

    printf("Enter Initial Deposit: ");
    scanf("%f", &acc.balance);

	// ? Minimum deposit enforcement
	if ((strcmp(acc.currency, "UGX") == 0 && acc.balance < MIN_UGX) ||
	    (strcmp(acc.currency, "USD") == 0 && acc.balance < MIN_USD)) {
	
	    if (strcmp(acc.currency, "UGX") == 0)
	        printf("? Minimum deposit for this account is Shs %.2f\n", MIN_UGX);
	    else
	        printf("? Minimum deposit for this account is $ %.2f\n", MIN_USD);
	
	    fclose(fp);
	    return;
	}

    fwrite(&acc, sizeof(acc), 1, fp);
    fclose(fp);

    logAction("ACCOUNT_CREATED", acc.accountNumber, acc.balance);

    printf("? Account created successfully!\n");
}

// Deposit Money -----------------------------
void deposit() {
    long long accNo;
    int found = 0;
    float amount;
    char confirm;
    struct Account acc;

    FILE *fp = fopen(filename, "rb+");
    if (!fp) {
        printf("File not found!\n");
        return;
    }

    printf("\nEnter Account Number: ");
    scanf("%lld", &accNo);

	printf("Enter amount to deposit: ");
    scanf("%f", &amount);
 
	// ? Minimum deposit enforcement
	if ((strcmp(acc.currency, "UGX") == 0 && acc.balance < MIN_UGX) ||
	    (strcmp(acc.currency, "USD") == 0 && acc.balance < MIN_USD)) {
	
	    if (strcmp(acc.currency, "UGX") == 0)
	        printf("? Minimum deposit for this account is Shs %.2f\n", MIN_UGX);
	    else
	        printf("? Minimum deposit for this account is $ %.2f\n", MIN_USD);
	
	    fclose(fp);
	    return;
	}
	
    while (fread(&acc, sizeof(acc), 1, fp)) {
        if (acc.accountNumber == accNo) {

            printf("\n>>> A/c: %lld (%s)\n",
                   acc.accountNumber, acc.name);

            printf("Proceed with deposit? (y/n): ");
            scanf(" %c", &confirm);

            if (confirm != 'Y' && confirm != 'y') {
                printf("? Deposit cancelled.\n");
                fclose(fp);
                return;
            }

            acc.balance += amount;

            fseek(fp, -sizeof(acc), SEEK_CUR);
            fwrite(&acc, sizeof(acc), 1, fp);

            logAction("DEPOSIT", acc.accountNumber, amount);

            printf("? Deposit successful! New Balance: %s %.2f\n",
                   currencySymbol(acc.currency), acc.balance);

            found = 1;
            break;
        }
    }

    if (!found)
        printf("? Account not found!\n");

    fclose(fp);
}

// Withdraw Money -----------------------------
void withdraw() {
    long long accNo;
    int found = 0;
    float amount;
    struct Account acc;

    FILE *fp = fopen(filename, "rb+");
    if (!fp) {
        printf("File not found!\n");
        return;
    }

    printf("\nEnter Account Number: ");
    scanf("%lld", &accNo);

    while (fread(&acc, sizeof(acc), 1, fp)) {
        if (acc.accountNumber == accNo) {

            printf("Enter amount to withdraw: ");
            scanf("%f", &amount);

            if (amount > acc.balance) {
                printf("? Insufficient balance!\n");
            } else {
                acc.balance -= amount;

                fseek(fp, -sizeof(acc), SEEK_CUR);
                fwrite(&acc, sizeof(acc), 1, fp);

                logAction("WITHDRAW", acc.accountNumber, amount);

                printf("? Withdrawal successful! Remaining Balance: %s %.2f\n",
                       currencySymbol(acc.currency), acc.balance);
            }

            found = 1;
            break;
        }
    }

    if (!found)
        printf("? Account not found!\n");

    fclose(fp);
}

// Check Balance -----------------------------
void checkBalance() {
    long long accNo;
    int found = 0;
    struct Account acc;

    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        printf("File not found!\n");
        return;
    }

    printf("\nEnter Account Number: ");
    scanf("%lld", &accNo);

    while (fread(&acc, sizeof(acc), 1, fp)) {
        if (acc.accountNumber == accNo) {

            printf("\nACCOUNT DETAILS:\n");
            printf("A/c. Number: %lld\n", acc.accountNumber);
            printf("A/c. Title: %s\n", acc.name);
            printf("Currency: %s\n", acc.currency);
            printf("Balance: %s %.2f\n", 
			          currencySymbol(acc.currency), acc.balance);

            logAction("BALANCE_CHECK", acc.accountNumber, acc.balance);

            found = 1;
            break;
        }
    }

    if (!found)
        printf("? Account not found!\n");

    fclose(fp);
}

// View Logs (Password Protected) -----------------------------
void viewLogs() {
    char password[20];
    char ch;

    FILE *log = fopen("audit_log.txt", "r");
    if (!log) {
        printf("No logs available.\n");
        return;
    }

    printf("Enter log password: ");
    scanf("%19s", password);

    if (strcmp(password, LOG_PASSWORD) != 0) {
        printf("? Incorrect password!\n");
        fclose(log);
        return;
    }

    printf("\nAUDIT LOGS________________\n");

    while ((ch = fgetc(log)) != EOF) {
        putchar(ch);
    }

    fclose(log);
}