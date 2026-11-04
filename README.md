# Student Record Management System (C)

A menu-driven console application written in C to manage student records. Data is stored in a binary file, so it stays saved after the program closes.

## Features
- Password-protected login (3 attempts)
- Add, display, search, update and delete student records
- Search by roll number or by name (partial, case-insensitive)
- Automatic total, percentage and grade calculation
- Sort by roll number or by marks
- Statistics: class average, topper, lowest scorer, pass/fail count
- Input validation and duplicate roll number check
- Persistent storage using file handling

## Concepts Used
Structures, arrays, functions, strings, pointers, file handling, `qsort()`, input validation.

## How to Compile and Run

```
gcc student_record.c -o student_record
./student_record
```

On Windows, run `student_record.exe` instead.

Default password: `admin123` (change the `PASSWORD` macro in the source).

## Grading Scheme
| Percentage | Grade |
|-----------|-------|
| 80 and above | A |
| 70 - 79 | B |
| 60 - 69 | C |
| 50 - 59 | D |
| 40 - 49 | E |
| Below 40 | F (fail) |

## Project Structure
```
student_record.c   # source code
README.md          # this file
.gitignore         # ignores compiled files and saved data
```

## Possible Improvements
- Store records in a linked list using dynamic memory
- Export records to a CSV file
- Add attendance tracking
