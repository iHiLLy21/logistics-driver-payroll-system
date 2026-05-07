# Logistics Driver Payroll System

A C++ console-based payroll management system created for a university programming assignment.

This application reads driver and mileage data from text files, processes monthly payroll information, calculates tax and net pay, stores payroll history, and generates payroll output reports while logging invalid records.

---

## Features

- Load driver information from external text files
- Process monthly payroll files
- Calculate:
  - Gross pay
  - Monthly tax
  - Net pay
- Store payroll history for each driver
- Display:
  - All drivers
  - Individual driver payroll records
  - All payroll records
- Generate monthly payroll output files
- Automatic error logging to `errors.txt`
- Input validation and duplicate record checking
- Case-insensitive driver ID matching
- Console menu navigation system
- Custom console UI with ASCII splash screens and menus

---

## Technologies Used

- C++
- Object-Oriented Programming (OOP)
- Classes and Structures
- Vectors
- File Handling (`ifstream` / `ofstream`)
- Functions and Modular Programming
- Input Validation
- Console Formatting (`iomanip`)

---

## Project Structure

### Main Components

- `Driver` class  
  Stores driver details, pay rate, and payroll history.

- `PayRecord` structure  
  Stores monthly payroll information for a driver.

- Payroll Processing Functions  
  Handle payroll calculations, validation, and output generation.

- File Processing System  
  Reads and writes payroll data using external text files.

---

## Files Used

### Input Files

- `drivers.txt`
  - Stores driver IDs, names, and pay rates.

- Monthly pay files (example: `jan26.txt`)
  - Stores mileage records for drivers.

### Output Files

- `jan26_output.txt`
  - Generated payroll report for the processed month.

- `errors.txt`
  - Stores invalid or incomplete payroll record errors.

---

## Validation Features

The system checks for:

- Invalid driver IDs
- Missing mileage values
- Negative mileage values
- Duplicate payroll records
- File loading errors
- Invalid menu input

---

## Example Functions Included

- Driver payroll calculations
- Monthly tax calculations
- Payroll history tracking
- File generation
- Error logging
- Menu systems
- Payroll reporting

---

## What I Learned

Through this project I developed experience with:

- Object-oriented programming in C++
- File handling and data processing
- Building larger structured programs
- Creating reusable functions
- Input validation and error handling
- Managing vectors and records
- Console application design
- Organising projects using GitHub

---

## How to Run

1. Open the `.sln` file in Visual Studio
2. Build the solution
3. Run the application
4. Ensure required text files are in the correct directory

---

## Notes

Some Visual Studio generated folders and temporary build files were removed before uploading to GitHub to keep the repository clean and organised.
