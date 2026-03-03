#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100
#define SUBJECTS 5

// Subject names
char subjectNames[SUBJECTS][10] = {"MTC", "ENG", "CHE", "BIO", "PHY"};

// Structure definition
struct Student {
    int id;
    char name[50];
    char gender[10];
    float marks[SUBJECTS];
    float average;
    char grade;
};

struct Student students[MAX];
int count = 0;

// Function prototypes
void addStudent();
void calculateAverage(struct Student *std);
void assignGrade(struct Student *std);
void displayReportMenu();
void displayAllStudents();
void displaySingleStudent();
void sortByAverage();
void saveToFile();
void loadFromFile();
int findStudentById(int id);

int main() {
    int choice;

    loadFromFile();

    do {
        printf("\nStudent Record Management System\n");
        printf("1. Add Student\n");
        printf("2. Display Report\n");
        printf("3. Save to File\n");
        printf("4. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1: addStudent(); break;
            case 2: displayReportMenu(); break;
            case 3: saveToFile(); break;
            case 4: saveToFile(); printf("Exiting...\n"); break;
            default: printf("Invalid choice!\n");
        }

    } while(choice != 4);

    return 0;
}

// ADD STUDENT
void addStudent() {
    if (count >= MAX) {
        printf("Student limit reached!\n");
        return;
    }

    struct Student std;

    printf("\nStudent ID: ");
    scanf("%d", &std.id);

    printf("Student Name: ");
    scanf(" %[^\n]", std.name);

    printf("Gender: ");
    scanf("%1s", std.gender);

    printf("Enter marks for %d subjects:\n", SUBJECTS);

    int i;
    for (i = 0; i < SUBJECTS; i++) {
        printf("%s: ", subjectNames[i]);
        scanf("%f", &std.marks[i]);
    }

    calculateAverage(&std);
    assignGrade(&std);

    students[count++] = std;

    printf("Student added successfully!\n");
}

// CALCULATE AVERAGE
void calculateAverage(struct Student *std) {
    float sum = 0;
    int i;
    for (i = 0; i < SUBJECTS; i++) {
        sum += std->marks[i];
    }
    std->average = sum / SUBJECTS;
}

// ASSIGN GRADE 
void assignGrade(struct Student *std) {
    if (std->average >= 80) std->grade = 'A';
    else if (std->average >= 70) std->grade = 'B';
    else if (std->average >= 60) std->grade = 'C';
    else if (std->average >= 50) std->grade = 'D';
    else std->grade = 'F';
}

// DISPLAY MENU
void displayReportMenu() {
    char option[10];

    printf("\nDisplay Report Options:\n");
    printf("Type 'All' to view all students\n");
    printf("Type 'Std' to view one student\n");
    printf("Enter option: ");
    scanf("%s", option);

    if (strcmp(option, "All") == 0 || strcmp(option, "all") == 0) {
        displayAllStudents();
    } else if (strcmp(option, "Std") == 0 || strcmp(option, "std") == 0) {
        displaySingleStudent();
    } else {
        printf("Invalid option!\n");
    }
}

//DISPLAY ALL
void displayAllStudents() {
    if (count == 0) {
        printf("No student records available.\n");
        return;
    }

    sortByAverage();

    printf("\nALL STUDENTS REPORT-------------------------------------------------\n");
    printf("ID   Name            Gender  MTC  ENG  CHE  BIO  PHY  Avg   Grade\n");
    printf("---------------------------------------------------------------------\n");
    int i;
    for (i = 0; i < count; i++) {
        printf("%-4d %-15s %-7s",
               students[i].id,
               students[i].name,
               students[i].gender);
        int j;
        for (j = 0; j < SUBJECTS; j++) {
            printf("%-5.0f", students[i].marks[j]);
        }

        printf("%-6.2f %-2c\n",
               students[i].average,
               students[i].grade);
    }
}

// DISPLAY SINGLE REPORT
void displaySingleStudent() {
    int id;
    printf("Enter Student ID: ");
    scanf("%d", &id);

    int index = findStudentById(id);

    if (index == -1) {
        printf("Student not found!\n");
        return;
    }

    struct Student std = students[index];

    printf("\nSTUDENT REPORT -----------------\n");
    printf("Student ID: %d\n", std.id);
    printf("Name      : %s\n", std.name);
    printf("Gender    : %s\n", std.gender);

    printf("\nSubject Scores:\n");
    int i;
    for (i = 0; i < SUBJECTS; i++) {
        printf("%s: %.2f\n", subjectNames[i], std.marks[i]);
    }

    printf("\nAverage: %.2f\n", std.average);
    printf("Grade  : %c\n", std.grade);
}

// FIND STUDENT
int findStudentById(int id) {
	int i;
    for (i = 0; i < count; i++) {
        if (students[i].id == id)
            return i;
    }
    return -1;
}

// SORT
void sortByAverage() {
    struct Student temp;
	int i, j;
    for (i = 0; i < count - 1; i++) {
        for (j = i + 1; j < count; j++) {
            if (students[i].average < students[j].average) {
                temp = students[i];
                students[i] = students[j];
                students[j] = temp;
            }
        }
    }
}

// SAVE
void saveToFile() {
    FILE *fp = fopen("students.dat", "wb");
    if (!fp) return;

    fwrite(&count, sizeof(int), 1, fp);
    fwrite(students, sizeof(struct Student), count, fp);
    fclose(fp);
}

// LOAD 
void loadFromFile() {
    FILE *fp = fopen("students.dat", "rb");
    if (!fp) return;

    fread(&count, sizeof(int), 1, fp);
    fread(students, sizeof(struct Student), count, fp);
    fclose(fp);
}