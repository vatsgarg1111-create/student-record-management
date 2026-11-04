/*
 * Student Record Management System
 * --------------------------------
 * A menu-driven C program to add, view, search, update, delete and sort
 * student records, with grades, statistics and permanent file storage.
 *
 * Concepts used: structures, arrays, functions, strings, pointers,
 *                file handling (binary), sorting with qsort().
 *
 * Compile: gcc student_record.c -o student_record
 * Run:     ./student_record        (Windows: student_record.exe)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_STUDENTS 100
#define SUBJECTS     5
#define NAME_LEN     50
#define DATA_FILE    "students.dat"
#define PASSWORD     "admin123"

typedef struct {
    int   roll;
    char  name[NAME_LEN];
    float marks[SUBJECTS];
    float total;
    float percentage;
    char  grade;
} Student;

static Student students[MAX_STUDENTS];
static int count = 0;

static const char *subjectNames[SUBJECTS] = {
    "Maths", "Physics", "Chemistry", "English", "C Programming"
};

/* ---------- Input helpers ---------- */

int readInt(const char *prompt, int min, int max)
{
    char buf[64], extra;
    int val;

    while (1) {
        printf("%s", prompt);
        if (!fgets(buf, sizeof buf, stdin))
            exit(0);
        if (sscanf(buf, "%d %c", &val, &extra) == 1 && val >= min && val <= max)
            return val;
        printf("  Invalid input. Enter a whole number between %d and %d.\n", min, max);
    }
}

float readFloat(const char *prompt, float min, float max)
{
    char buf[64], extra;
    float val;

    while (1) {
        printf("%s", prompt);
        if (!fgets(buf, sizeof buf, stdin))
            exit(0);
        if (sscanf(buf, "%f %c", &val, &extra) == 1 && val >= min && val <= max)
            return val;
        printf("  Invalid input. Enter a number between %.0f and %.0f.\n", min, max);
    }
}

void readString(const char *prompt, char *dest, int size)
{
    while (1) {
        printf("%s", prompt);
        if (!fgets(dest, size, stdin))
            exit(0);
        dest[strcspn(dest, "\n")] = '\0';
        if (dest[0] != '\0')
            return;
        printf("  Input cannot be empty.\n");
    }
}

int confirm(const char *prompt)
{
    char buf[16];
    printf("%s (y/n): ", prompt);
    if (!fgets(buf, sizeof buf, stdin))
        exit(0);
    return tolower((unsigned char)buf[0]) == 'y';
}

/* ---------- Core helpers ---------- */

void calculateResult(Student *s)
{
    int i;
    s->total = 0;
    for (i = 0; i < SUBJECTS; i++)
        s->total += s->marks[i];
    s->percentage = s->total / SUBJECTS;   /* each subject is out of 100 */

    if      (s->percentage >= 80) s->grade = 'A';
    else if (s->percentage >= 70) s->grade = 'B';
    else if (s->percentage >= 60) s->grade = 'C';
    else if (s->percentage >= 50) s->grade = 'D';
    else if (s->percentage >= 40) s->grade = 'E';
    else                          s->grade = 'F';
}

int findByRoll(int roll)
{
    int i;
    for (i = 0; i < count; i++)
        if (students[i].roll == roll)
            return i;
    return -1;
}

/* Case-insensitive "does text contain pattern?" */
int containsIgnoreCase(const char *text, const char *pattern)
{
    char t[NAME_LEN], p[NAME_LEN];
    int i;

    for (i = 0; text[i] && i < NAME_LEN - 1; i++)
        t[i] = (char)tolower((unsigned char)text[i]);
    t[i] = '\0';
    for (i = 0; pattern[i] && i < NAME_LEN - 1; i++)
        p[i] = (char)tolower((unsigned char)pattern[i]);
    p[i] = '\0';

    return strstr(t, p) != NULL;
}

void printLine(void)
{
    printf("------------------------------------------------------------\n");
}

void printTableHeader(void)
{
    printLine();
    printf("%-6s %-20s %8s %9s  %s\n", "Roll", "Name", "Total", "Percent", "Grade");
    printLine();
}

void printTableRow(const Student *s)
{
    printf("%-6d %-20.20s %8.2f %8.2f%%  %c\n",
           s->roll, s->name, s->total, s->percentage, s->grade);
}

void printDetails(const Student *s)
{
    int i;
    printLine();
    printf("Roll No    : %d\n", s->roll);
    printf("Name       : %s\n", s->name);
    for (i = 0; i < SUBJECTS; i++)
        printf("%-14s: %.2f\n", subjectNames[i], s->marks[i]);
    printf("Total      : %.2f / %d\n", s->total, SUBJECTS * 100);
    printf("Percentage : %.2f%%\n", s->percentage);
    printf("Grade      : %c  (%s)\n", s->grade, s->grade == 'F' ? "FAIL" : "PASS");
    printLine();
}

/* ---------- File handling ---------- */

void saveToFile(void)
{
    FILE *fp = fopen(DATA_FILE, "wb");
    if (!fp) {
        printf("  Error: could not save data to %s\n", DATA_FILE);
        return;
    }
    fwrite(&count, sizeof count, 1, fp);
    fwrite(students, sizeof(Student), (size_t)count, fp);
    fclose(fp);
}

void loadFromFile(void)
{
    FILE *fp = fopen(DATA_FILE, "rb");
    int n;

    if (!fp)
        return;     /* first run: no file yet */

    if (fread(&n, sizeof n, 1, fp) == 1 && n >= 0 && n <= MAX_STUDENTS) {
        count = (int)fread(students, sizeof(Student), (size_t)n, fp);
    }
    fclose(fp);
}

/* ---------- Features ---------- */

void addStudent(void)
{
    Student s;
    char prompt[64];
    int i;

    if (count >= MAX_STUDENTS) {
        printf("  Record limit (%d) reached.\n", MAX_STUDENTS);
        return;
    }

    s.roll = readInt("Enter roll number (1-99999): ", 1, 99999);
    if (findByRoll(s.roll) != -1) {
        printf("  A student with roll number %d already exists.\n", s.roll);
        return;
    }

    readString("Enter name: ", s.name, NAME_LEN);

    for (i = 0; i < SUBJECTS; i++) {
        snprintf(prompt, sizeof prompt, "Marks in %s (0-100): ", subjectNames[i]);
        s.marks[i] = readFloat(prompt, 0, 100);
    }

    calculateResult(&s);
    students[count++] = s;
    saveToFile();
    printf("  Student added successfully.\n");
}

void displayAll(void)
{
    int i;
    if (count == 0) {
        printf("  No records found.\n");
        return;
    }
    printTableHeader();
    for (i = 0; i < count; i++)
        printTableRow(&students[i]);
    printLine();
    printf("Total students: %d\n", count);
}

void searchStudent(void)
{
    int choice, i, found = 0;
    char name[NAME_LEN];

    if (count == 0) {
        printf("  No records to search.\n");
        return;
    }

    printf("\n1. Search by roll number\n2. Search by name\n");
    choice = readInt("Choice: ", 1, 2);

    if (choice == 1) {
        int roll = readInt("Enter roll number: ", 1, 99999);
        int idx = findByRoll(roll);
        if (idx == -1)
            printf("  No student found with roll number %d.\n", roll);
        else
            printDetails(&students[idx]);
    } else {
        readString("Enter name (or part of it): ", name, NAME_LEN);
        for (i = 0; i < count; i++) {
            if (containsIgnoreCase(students[i].name, name)) {
                if (!found)
                    printTableHeader();
                printTableRow(&students[i]);
                found++;
            }
        }
        if (found)
            printLine();
        else
            printf("  No student found matching \"%s\".\n", name);
    }
}

void updateStudent(void)
{
    int roll, idx, choice, sub, i;
    char prompt[64];
    Student *s;

    if (count == 0) {
        printf("  No records to update.\n");
        return;
    }

    roll = readInt("Enter roll number to update: ", 1, 99999);
    idx = findByRoll(roll);
    if (idx == -1) {
        printf("  No student found with roll number %d.\n", roll);
        return;
    }
    s = &students[idx];

    printf("\n1. Update name\n2. Update marks of one subject\n3. Update all marks\n");
    choice = readInt("Choice: ", 1, 3);

    if (choice == 1) {
        readString("Enter new name: ", s->name, NAME_LEN);
    } else if (choice == 2) {
        for (i = 0; i < SUBJECTS; i++)
            printf("  %d. %s\n", i + 1, subjectNames[i]);
        sub = readInt("Which subject? ", 1, SUBJECTS) - 1;
        snprintf(prompt, sizeof prompt, "New marks in %s (0-100): ", subjectNames[sub]);
        s->marks[sub] = readFloat(prompt, 0, 100);
    } else {
        for (i = 0; i < SUBJECTS; i++) {
            snprintf(prompt, sizeof prompt, "Marks in %s (0-100): ", subjectNames[i]);
            s->marks[i] = readFloat(prompt, 0, 100);
        }
    }

    calculateResult(s);
    saveToFile();
    printf("  Record updated.\n");
}

void deleteStudent(void)
{
    int roll, idx, i;

    if (count == 0) {
        printf("  No records to delete.\n");
        return;
    }

    roll = readInt("Enter roll number to delete: ", 1, 99999);
    idx = findByRoll(roll);
    if (idx == -1) {
        printf("  No student found with roll number %d.\n", roll);
        return;
    }

    printDetails(&students[idx]);
    if (!confirm("Delete this record?")) {
        printf("  Deletion cancelled.\n");
        return;
    }

    for (i = idx; i < count - 1; i++)
        students[i] = students[i + 1];
    count--;
    saveToFile();
    printf("  Record deleted.\n");
}

int cmpRoll(const void *a, const void *b)
{
    return ((const Student *)a)->roll - ((const Student *)b)->roll;
}

int cmpMarksDesc(const void *a, const void *b)
{
    float x = ((const Student *)a)->total;
    float y = ((const Student *)b)->total;
    return (y > x) - (y < x);
}

void sortStudents(void)
{
    int choice;

    if (count < 2) {
        printf("  Need at least 2 records to sort.\n");
        return;
    }

    printf("\n1. Sort by roll number (ascending)\n2. Sort by marks (highest first)\n");
    choice = readInt("Choice: ", 1, 2);

    qsort(students, (size_t)count, sizeof(Student), choice == 1 ? cmpRoll : cmpMarksDesc);
    saveToFile();
    printf("  Records sorted.\n");
    displayAll();
}

void showStatistics(void)
{
    int i, top = 0, low = 0, passed = 0;
    float sum = 0;

    if (count == 0) {
        printf("  No records available.\n");
        return;
    }

    for (i = 0; i < count; i++) {
        sum += students[i].percentage;
        if (students[i].total > students[top].total) top = i;
        if (students[i].total < students[low].total) low = i;
        if (students[i].grade != 'F') passed++;
    }

    printLine();
    printf("Total students   : %d\n", count);
    printf("Class average    : %.2f%%\n", sum / count);
    printf("Passed / Failed  : %d / %d\n", passed, count - passed);
    printf("\nTopper:\n");
    printTableHeader();
    printTableRow(&students[top]);
    printf("\nLowest scorer:\n");
    printTableHeader();
    printTableRow(&students[low]);
    printLine();
}

/* ---------- Login & main menu ---------- */

int login(void)
{
    char pass[64];
    int attempts;

    for (attempts = 1; attempts <= 3; attempts++) {
        printf("Enter password: ");
        if (!fgets(pass, sizeof pass, stdin))
            exit(0);
        pass[strcspn(pass, "\n")] = '\0';
        if (strcmp(pass, PASSWORD) == 0)
            return 1;
        printf("  Wrong password (%d/3).\n", attempts);
    }
    return 0;
}

void showMenu(void)
{
    printf("\n========== STUDENT RECORD MANAGEMENT ==========\n");
    printf(" 1. Add student\n");
    printf(" 2. Display all students\n");
    printf(" 3. Search student\n");
    printf(" 4. Update student\n");
    printf(" 5. Delete student\n");
    printf(" 6. Sort records\n");
    printf(" 7. Statistics (topper, average, pass/fail)\n");
    printf(" 0. Exit\n");
    printf("===============================================\n");
}

int main(void)
{
    int choice;

    printf("=== Student Record Management System ===\n");
    if (!login()) {
        printf("Too many wrong attempts. Exiting.\n");
        return 1;
    }

    loadFromFile();
    printf("Loaded %d record(s).\n", count);

    do {
        showMenu();
        choice = readInt("Enter your choice: ", 0, 7);
        printf("\n");

        switch (choice) {
            case 1: addStudent();      break;
            case 2: displayAll();      break;
            case 3: searchStudent();   break;
            case 4: updateStudent();   break;
            case 5: deleteStudent();   break;
            case 6: sortStudents();    break;
            case 7: showStatistics();  break;
            case 0: printf("Data saved. Goodbye!\n"); break;
        }
    } while (choice != 0);

    return 0;
}
