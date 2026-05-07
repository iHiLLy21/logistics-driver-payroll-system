#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <limits>
#include <cctype>

using namespace std;

//==============================================================================================================
// Program information
//==============================================================================================================

// Name: Sam Hill
// Assignment: Logistics Driver Payroll System 

// Program overview:
// This program reads driver details from a file, processes monthly pay files,
// calculates gross pay, tax, and net pay, and stores payroll history for each driver.
// The system also allows the user to view individual driver records, view all payroll
// records, and create monthly output files while logging invalid or incomplete records.

//==============================================================================================================
// Forward declaration
//==============================================================================================================

class Driver;

//==============================================================================================================
// Function prototypes
//==============================================================================================================

double CalculateMonthlyTax(double grossMonthly);
void LogError(const string& payFilename, const string& message);

void ClearScreen();
void Pause();
void DrawLine();
void DrawGap(int gap);

void ShowSplashScreen();
int ShowMenu();

bool LoadDrivers(vector<Driver>& drivers, const string& filename);
bool ProcessPayFile(vector<Driver>& drivers, const string& payFilename, const string& monthLabel);
bool WriteMonthlyOutput(const vector<Driver>& drivers, const string& monthLabel, const string& outputFilename);

void DisplayDrivers(const vector<Driver>& drivers);
void DisplayDriverRecords(const vector<Driver>& drivers);
void DisplayAllPayrollRecords(const vector<Driver>& drivers);
void ProcessPayFileScreen(vector<Driver>& drivers);

string MonthLabelFromFilename(string filename);
string EnsureTxtExtension(string filename);
int FindDriverIndexById(const vector<Driver>& drivers, const string& id);

//==============================================================================================================
// Constants
//==============================================================================================================

const double TAX_RATE = 0.20;
const double PERSONAL_ALLOWANCE = 12570.0;
const int MONTHS_IN_YEAR = 12;
const int LINE_LENGTH = 55;

//==============================================================================================================
// Payroll record structure
//==============================================================================================================

// Stores payroll information for one driver for one processed month.

struct PayRecord
{
    string month = "";
    double miles = 0.0;
    double gross = 0.0;
    double tax = 0.0;
    double net = 0.0;
};

//==============================================================================================================
// Driver class
//==============================================================================================================

// Represents one driver and stores their personal details, pay rate, and payroll history.

class Driver
{
private:
    string id = "";
    string name = "";
    double ratePerMile = 0.0;
    vector<PayRecord> history;

public:
    Driver() = default;

    Driver(string driverId, string driverName, double driverRate)
    {
        id = driverId;
        name = driverName;
        ratePerMile = driverRate;
    }

    string GetId() const { return id; }
    string GetName() const { return name; }
    double GetRate() const { return ratePerMile; }

    void AddRecord(const PayRecord& rec)
    {
        history.push_back(rec);
    }

    double TotalMiles() const
    {
        double total = 0;
        for (int i = 0; i < history.size(); i++)
            total += history[i].miles;
        return total;
    }

    double TotalNetPay() const
    {
        double total = 0;
        for (int i = 0; i < history.size(); i++)
            total += history[i].net;
        return total;
    }

    bool GetRecordForMonth(const string& monthLabel, PayRecord& outRec) const
    {
        for (int i = 0; i < history.size(); i++)
        {
            if (history[i].month == monthLabel)
            {
                outRec = history[i];
                return true;
            }
        }

        return false;
    }

    bool HasRecordForMonth(const string& monthLabel) const
    {
        for (int i = 0; i < history.size(); i++)
        {
            if (history[i].month == monthLabel)
            {
                return true;
            }
        }
        return false;
    }

    const vector<PayRecord>& GetHistory() const
    {
        return history;
    }

    double TotalGrossPay() const
    {
        double total = 0;
        for (int i = 0; i < history.size(); i++)
            total += history[i].gross;
        return total;
    }

    double TotalTax() const
    {
        double total = 0;
        for (int i = 0; i < history.size(); i++)
            total += history[i].tax;
        return total;
    }
};

//==============================================================================================================
// Utility functions
//==============================================================================================================

// Displays a horizontal divider line to make the console layout easier to read.

void DrawLine()
{

    for (int i = 0; i < LINE_LENGTH; i++)
    {
        cout << "-";
    }

}

// Prints blank lines to create spacing between sections of the console interface.

void DrawGap(int gap)
{
    for (int i = 0; i < gap; i++)
    {
        cout << endl;
    }
}

//==============================================================================================================
// Screen control functions
//==============================================================================================================

// Clears the console window so each menu or report screen is displayed cleanly.

void ClearScreen()
{
    system("cls"); 
}

// Pauses the program until the user presses ENTER so they can read the current screen.

void Pause()
{
    cout << "\nPress ENTER to continue...";
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

//==============================================================================================================
// Tax Calculation functions
//==============================================================================================================

// Calculates monthly tax by converting gross monthly pay to annual pay,
// applying the personal allowance and tax rate, then converting the result back to monthly tax.

double CalculateMonthlyTax(double grossMonthly)
{
    double annual = grossMonthly * MONTHS_IN_YEAR;
    double taxable = annual - PERSONAL_ALLOWANCE;

    if (taxable <= 0)
        return 0.0;

    double annualTax = taxable * TAX_RATE;
    return annualTax / MONTHS_IN_YEAR;
}

//==============================================================================================================
// Filename helper functions
//==============================================================================================================

// Removes the .txt extension from a filename so it can be used as a month label.

string MonthLabelFromFilename(string filename)
{
    if (filename.size() >= 4 && filename.substr(filename.size() - 4) == ".txt")
        filename = filename.substr(0, filename.size() - 4);

    return filename; // e.g., "jan26"
}

// Adds the .txt extension to a filename if the user did not type it.

string EnsureTxtExtension(string filename)
{
    if (filename.size() < 4 || filename.substr(filename.size() - 4) != ".txt")
        filename += ".txt";

    return filename;
}

int FindDriverIndexById(const vector<Driver>& drivers, const string& id)
{
    for (int i = 0; i < drivers.size(); i++)
    {
        if (drivers[i].GetId() == id)
        {
            return i;
        }
    }

    return -1;
}

//==============================================================================================================
// File processing functions
//==============================================================================================================

// Reads driver details from the driver file and stores them in a vector of Driver objects.

bool LoadDrivers(vector<Driver>& drivers, const string& filename)
{
    ifstream file(filename);

    if (!file.is_open())
    {
        cout << "Error: Could not open " << filename << endl;
        return false;
    }

    string id, name;
    double rate;

    while (file >> id >> name >> rate)
    {
        // Converts the ID to uppercase so matching is case-insensitive.
        for (char& c : id)
            c = toupper(c);

        drivers.push_back(Driver(id, name, rate));
    }

    file.close();
    return true;
}

//==============================================================================================================

// Reads a monthly pay file, validates each line, calculates payroll values,
// and stores a payroll record for each valid driver.

bool ProcessPayFile(vector<Driver>& drivers,
    const string& payFilename,
    const string& monthLabel)
{
    ifstream file(payFilename);

    if (!file.is_open())
    {
        cout << "Error: Could not open pay file " << payFilename << endl;
        return false;
    }

    string id;
    double miles;

    while (file >> id)
    {
        // Converts the ID to uppercase so file matching is case-insensitive.
        for (char& c : id) c = toupper(c);

        // Logs and skips incomplete records where the mileage value is missing.
        if (!(file >> miles))
        {
            LogError(payFilename, "Pay entry for " + id + " is incomplete.");
            file.clear();
            file.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        // Checks whether the driver ID exists in the loaded driver data.
        int driverIndex = FindDriverIndexById(drivers, id);
        if (driverIndex == -1)
        {
            LogError(payFilename, id + " is not a valid driver ID.");
            continue;
        }

        // Prevents invalid negative mileage values from being processed.
        if (miles < 0)
        {
            LogError(payFilename, "Miles cannot be negative for " + id + ".");
            continue;
        }

        // Prevents the same month from being processed twice for one driver.
        if (drivers[driverIndex].HasRecordForMonth(monthLabel))
        {
            LogError(payFilename, "Duplicate record for " + id + " in month " + monthLabel + ".");
            continue;
        }

        double gross = miles * drivers[driverIndex].GetRate();
        double tax = CalculateMonthlyTax(gross);
        double net = gross - tax;

        // Creates a payroll record for the processed month and stores it in the driver's history.
        PayRecord rec{ monthLabel, miles, gross, tax, net };
        drivers[driverIndex].AddRecord(rec);
    }

    file.close();
    return true;
}

//==============================================================================================================

// Writes the processed payroll records for one selected month to a new output text file.

bool WriteMonthlyOutput(const vector<Driver>& drivers,
    const string& monthLabel,
    const string& outputFilename)
{
    ofstream out(outputFilename);

    if (!out.is_open())
    {
        cout << "Error: Could not write to " << outputFilename << endl;
        return false;
    }

    out << left << setw(8) << "ID"
        << setw(10) << "Miles"
        << setw(15) << "Gross"
        << setw(15) << "Tax"
        << setw(15) << "Net" << endl;

    out << string(63, '-') << endl;

    for (int i = 0; i < drivers.size(); i++)
    {
        PayRecord rec;

        if (drivers[i].GetRecordForMonth(monthLabel, rec))
        {
            out << left << setw(8) << drivers[i].GetId()
                << setw(10) << fixed << setprecision(2) << rec.miles
                << setw(15) << rec.gross
                << setw(15) << rec.tax
                << setw(15) << rec.net
                << endl;
        }
    }

    out.close();
    return true;
}

//==============================================================================================================

// Writes invalid or incomplete record details to errors.txt so problems can be reviewed later.

void LogError(const string& payFilename, const string& message)
{
    ofstream err("errors.txt", ios::app); // append

    if (err.is_open())
    {
        err << payFilename << " - " << message << endl;
    }
}

//==============================================================================================================
// Splash functions
//==============================================================================================================

// Displays the program splash screen when the system first starts.

void ShowSplashScreen()
{
    ClearScreen();

    cout << R"(
  _     ___   ____ ___ ____ _____ ___ ____ ____                                
 | |   / _ \ / ___|_ _/ ___|_   _|_ _/ ___/ ___|                               
 | |  | | | | |  _ | |\___ \ | |  | | |   \___ \                               
 | |__| |_| | |_| || | ___) || |  | | |___ ___) |                              
 |_____\___/ \____|___|____/_|_|_|___\____|____/                               
 |  _ \|  _ \|_ _\ \   / / ____|  _ \                                          
 | | | | |_) || | \ \ / /|  _| | |_) |                                         
 | |_| |  _ < | |  \ V / | |___|  _ <                                          
 |____/|_| \_\___|__\_/  |_____|_| \_\      ______   ______ _____ _____ __  __ 
 |  _ \ / \\ \ / /  _ \ / _ \| |   | |     / ___\ \ / / ___|_   _| ____|  \/  |
 | |_) / _ \\ V /| |_) | | | | |   | |     \___ \\ V /\___ \ | | |  _| | |\/| |
 |  __/ ___ \| | |  _ <| |_| | |___| |___   ___) || |  ___) || | | |___| |  | |
 |_| /_/   \_\_| |_| \_\\___/|_____|_____| |____/ |_| |____/ |_| |_____|_|  |_|

    )" << endl;

    cout << "Enter System\n";
    Pause();

}

//==============================================================================================================
// Menu functions
//==============================================================================================================

// Displays the main menu and returns a validated menu choice entered by the user.

int ShowMenu()
{
    ClearScreen();
    cout << R"(
  __  __       _         __  __                   
 |  \/  | __ _(_)_ __   |  \/  | ___ _ __  _   _  
 | |\/| |/ _` | | '_ \  | |\/| |/ _ \ '_ \| | | | 
 | |  | | (_| | | | | | | |  | |  __/ | | | |_| | 
 |_|  |_|\__,_|_|_| |_| |_|  |_|\___|_| |_|\__,_| 
    
    )" << endl;
    DrawLine();
    cout << "\n\n1. Display all drivers\n";
    cout << "2. Process a pay file (e.g., jan26)\n";
    cout << "3. Show payroll records for driver (e.g., D001)\n";
    cout << "4. Show all payroll records\n";
    cout << "5. Exit\n\n";
    DrawLine();
    cout << "\n\nEnter choice: ";

    int choice;
    cin >> choice;

    // Repeats until the user enters a valid menu option.
    while (cin.fail() || choice < 1 || choice > 5)
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid choice. Enter 1-5: ";
        cin >> choice;
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clears the leftover newline so later input works correctly.
    return choice;
}

//==============================================================================================================
// Display Driver List functions
//==============================================================================================================

// Displays the full list of drivers currently loaded from the driver file.

void DisplayDrivers(const vector<Driver>& drivers)
{
    cout << R"(
      ____       _                  _     _     _   
     |  _ \ _ __(_)_   _____ _ __  | |   (_)___| |_ 
     | | | | '__| \ \ / / _ \ '__| | |   | / __| __|
     | |_| | |  | |\ V /  __/ |    | |___| \__ \ |_ 
     |____/|_|  |_| \_/ \___|_|    |_____|_|___/\__|
    )" << endl;
    DrawLine();
    
    cout << "\n\nDrivers loaded: " << drivers.size() << "\n\n";

    cout << left << setw(8) << "ID"
        << setw(15) << "Name"
        << setw(10) << "\x9c Rate" << endl;

    DrawLine();
    DrawGap(2);

    for (int i = 0; i < drivers.size(); i++)
    {
        cout << left << setw(8) << drivers[i].GetId()
            << setw(15) << drivers[i].GetName()
            << fixed << setprecision(2)
            << setw(10) << drivers[i].GetRate()
            << endl;
    }
}

//==============================================================================================================
// Payroll screen functions
//==============================================================================================================

// Allows the user to process multiple pay files until they type quit to return to the main menu.

void ProcessPayFileScreen(vector<Driver>& drivers)
{
    string payFile;

    while (true)
    {
        ClearScreen();

        cout << R"(
      ____  ____   ___   ____ _____ ____ ____    ____   _ __   __  _____ ___ _     _____ 
     |  _ \|  _ \ / _ \ / ___| ____/ ___/ ___|  |  _ \ / \\ \ / / |  ___|_ _| |   | ____|
     | |_) | |_) | | | | |   |  _| \___ \___ \  | |_) / _ \\ V /  | |_   | || |   |  _|  
     |  __/|  _ <| |_| | |___| |___ ___) |__) | |  __/ ___ \| |   |  _|  | || |___| |___ 
     |_|   |_| \_\\___/ \____|_____|____/____/  |_| /_/   \_\_|   |_|   |___|_____|_____|
   
        )" << endl;
        
        DrawLine();
        cout << "\n\nProcess Pay File\n\n";
        cout << "Enter a pay file name to process.\n";
        cout << "Example: jan26 or jan26.txt\n";
        cout << "Type quit to return to the main menu.\n";
        DrawGap(1);
        DrawLine();
        cout << "\n\nEnter pay file name: ";

        cin >> payFile;

        for (char& c : payFile)
        {
            c = tolower(c);
        }

        if (payFile == "quit")
        {
            break;
        }

        payFile = EnsureTxtExtension(payFile);
        string monthLabel = MonthLabelFromFilename(payFile);

        if (ProcessPayFile(drivers, payFile, monthLabel))
        {
            string outFile = monthLabel + "_output.txt";

            if (WriteMonthlyOutput(drivers, monthLabel, outFile))
            {
                cout << "\nCreated file: " << outFile << endl;
            }
        }

        cout << "\nCheck errors.txt for logged issues.\n";
        Pause();
    }
}

//==============================================================================================================
// Driver display functions
//==============================================================================================================

// Displays payroll records for one selected driver and allows repeated lookups until the user types quit.

void DisplayDriverRecords(const vector<Driver>& drivers)
{
    string id;

    while (true)
    {
        ClearScreen();

        cout << R"(
      ____  ____  _____     _______ ____    ____   _ __   ______   ___  _     _     
     |  _ \|  _ \|_ _\ \   / / ____|  _ \  |  _ \ / \\ \ / /  _ \ / _ \| |   | |    
     | | | | |_) || | \ \ / /|  _| | |_) | | |_) / _ \\ V /| |_) | | | | |   | |    
     | |_| |  _ < | |  \ V / | |___|  _ <  |  __/ ___ \| | |  _ <| |_| | |___| |___ 
     |____/|_|_\_\___| _\_/ _|_____|_| \_\_|_| /_/   \_\_| |_| \_\\___/|_____|_____|
     |  _ \| ____/ ___/ _ \|  _ \|  _ \/ ___|                                       
     | |_) |  _|| |  | | | | |_) | | | \___ \                                       
     |  _ <| |__| |__| |_| |  _ <| |_| |___) |                                      
     |_| \_\_____\____\___/|_| \_\____/|____/ 
        
        )" << endl;

        DrawLine();
        cout << "\n\nAvailable Drivers:\n\n";

        cout << left << setw(8) << "ID"
            << setw(15) << "Name"
            << setw(10) << "\x9c Rate" << endl;

        DrawLine();
        cout << "\n";

        for (int i = 0; i < drivers.size(); i++)
        {
            cout << left << setw(8) << drivers[i].GetId()
                << setw(15) << drivers[i].GetName()
                << fixed << setprecision(2)
                << setw(10) << drivers[i].GetRate()
                << endl;
        }

        DrawLine();
        cout << "\n\nEnter Driver ID (e.g., D001) or type quit: ";
        cin >> id;

        for (char& c : id)
        {
            c = toupper(c);
        }

        if (id == "QUIT")
        {
            break;
        }

        int driverIndex = FindDriverIndexById(drivers, id);
        if (driverIndex == -1)
        {
            cout << "\nDriver not found.\n";
            Pause();
            continue;
        }

        const Driver& driver = drivers[driverIndex];
        const vector<PayRecord>& history = driver.GetHistory();

        cout << fixed << setprecision(2);

        cout << "\n";
        DrawLine();
        cout << "\n";
        cout << "Driver: " << driver.GetId() << " - " << driver.GetName() << "\n";
        cout << "\x9c Rate per mile: " << driver.GetRate() << "\n";

        if (history.empty())
        {
            cout << "\nNo payroll records processed yet for this driver.\n";
            Pause();
            continue;
        }

        DrawLine();
        cout << "\n";

        cout << left << setw(10) << "Month"
            << setw(12) << "Miles"
            << setw(12) << "Gross"
            << setw(12) << "Tax"
            << setw(12) << "Net" << endl;

        DrawLine();
        cout << "\n";

        for (int i = 0; i < history.size(); i++)
        {
            cout << left << setw(10) << history[i].month
                << setw(12) << history[i].miles
                << setw(12) << history[i].gross
                << setw(12) << history[i].tax
                << setw(12) << history[i].net
                << endl;
        }

        DrawLine();
        cout << "\n";
        cout << "TOTALS\n";
        cout << "Miles: " << driver.TotalMiles() << "\n";
        cout << "Gross: " << driver.TotalGrossPay() << "\n";
        cout << "Tax:   " << driver.TotalTax() << "\n";
        cout << "Net:   " << driver.TotalNetPay() << "\n";

        Pause();
    }

}

//==============================================================================================================
// All payroll reporting functions
//==============================================================================================================

// Displays payroll records and totals for all drivers who currently have processed payroll history.

void DisplayAllPayrollRecords(const vector<Driver>& drivers)
{
    string input;

    while (true)
    {
        ClearScreen();

        cout << R"(
         _    _     _       ____   _ __   ______   ___  _     _     
        / \  | |   | |     |  _ \ / \\ \ / /  _ \ / _ \| |   | |    
       / _ \ | |   | |     | |_) / _ \\ V /| |_) | | | | |   | |    
      / ___ \| |___| |___  |  __/ ___ \| | |  _ <| |_| | |___| |___ 
     /_/__ \_\_____|_____| |_|_/_/___\_\_|_|_| \_\\___/|_____|_____|
     |  _ \| ____/ ___/ _ \|  _ \|  _ \/ ___|                       
     | |_) |  _|| |  | | | | |_) | | | \___ \                       
     |  _ <| |__| |__| |_| |  _ <| |_| |___) |                      
     |_| \_\_____\____\___/|_| \_\____/|____/  

        )" << endl;

        DrawLine();
        cout << "\n\nAll Payroll Records\n";
        cout << "\nType quit to return to the main menu.\n";
        DrawGap(1);
        DrawLine();
        cout << "\n";

        // Used to check whether any driver currently has payroll records to display.
        bool anyRecordsFound = false;

        for (int i = 0; i < drivers.size(); i++)
        {
            const Driver& driver = drivers[i];
            const vector<PayRecord>& history = driver.GetHistory();

            if (history.empty())
            {
                continue;
            }

            anyRecordsFound = true;

            cout << "\nDriver: " << driver.GetId() << " - " << driver.GetName() << "\n";
            cout << "\x9c Rate per mile: " << fixed << setprecision(2) << driver.GetRate() << "\n";

            DrawLine();
            DrawGap(1);

            cout << left << setw(10) << "Month"
                << setw(12) << "Miles"
                << setw(12) << "Gross"
                << setw(12) << "Tax"
                << setw(12) << "Net" << endl;

            DrawLine();
            DrawGap(1);

            for (int j = 0; j < history.size(); j++)
            {
                cout << left << setw(10) << history[j].month
                    << setw(12) << history[j].miles
                    << setw(12) << history[j].gross
                    << setw(12) << history[j].tax
                    << setw(12) << history[j].net
                    << endl;
            }

            DrawLine();
            DrawGap(1);
            cout << "TOTALS\n";
            cout << "Miles: " << driver.TotalMiles() << "\n";
            cout << "Gross: " << driver.TotalGrossPay() << "\n";
            cout << "Tax:   " << driver.TotalTax() << "\n";
            cout << "Net:   " << driver.TotalNetPay() << "\n";

            DrawLine();
            cout << "\n";
        }

        if (!anyRecordsFound)
        {
            cout << "\nNo payroll records have been processed yet.\n";
        }

        cout << "\nEnter quit to return: ";
        cin >> input;

        for (char& c : input)
        {
            c = tolower(c);
        }

        if (input == "quit")
        {
            break;
        }
    }
}

//==============================================================================================================
// Main program
//==============================================================================================================

// Program entry point.
// Loads driver data, clears the old error log, displays the menu, and controls the overall system flow.

int main()
{
    vector<Driver> drivers;

    // Clears the old error log at the start of each program run.
    ofstream clearErrors("errors.txt");
    clearErrors.close();

    ShowSplashScreen();

    if (!LoadDrivers(drivers, "drivers.txt"))
    {
        cout << "Fatal error: drivers.txt could not be loaded.\n";
        Pause();
        return 1;
    }

    bool running = true;

    while (running)
    {
        int choice = ShowMenu();

        switch (choice)
        {
        case 1:
            ClearScreen();
            DisplayDrivers(drivers);
            Pause();
            break;

        case 2:
            ProcessPayFileScreen(drivers);
            break;

        case 3:
            DisplayDriverRecords(drivers);
            break;

        case 4:
            DisplayAllPayrollRecords(drivers);
            break;

        case 5:
            running = false;
            break;
        }
    }

    ClearScreen();
    cout << "Thank you for using the Payroll System.\n";
    return 0;
}