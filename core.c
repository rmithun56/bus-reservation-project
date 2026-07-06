#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <conio.h> // Required for getch()

#define ROWS 10
#define COLS 4
#define MAX_ROUTES 10

// --- STRUCTURES ---
typedef struct
{
    int isBooked; // 0 = Free, 1 = Booked
    char passengerName[50];
    int age;
    char pnr[15];
} Seat;

typedef struct
{
    int routeID;
    char source[50];
    char destination[50];
    char departureTime[10]; // e.g., "22:30"
    char busType[20];       // e.g., "AC Sleeper" or "Non-AC Seater"
    double price;           // e.g., 750.00
    Seat seats[ROWS][COLS];
} Route;

// --- GLOBAL VARIABLES ---
Route busRoutes[MAX_ROUTES];
int totalRoutes = 0;
const char *FILENAME = "bus_data.dat";

// --- FUNCTION DECLARATIONS (Prototypes) ---
void loadFromFile();
void saveToFile();
void adminMenu();
void userMenu();
void addRoute();
void displayRoutes();
void displaySeatMap(int routeIndex);
void bookTicket();
void cancelTicket();
void recommendSeat(int routeIndex, int preference);
void generatePNR(char *pnrBuffer);
void getMaskedPassword(char *passwordBuffer, int maxLength);

// --- MAIN FUNCTION ---
int main()
{
    srand(time(NULL)); // Seed for random PNR generation
    loadFromFile();    // Load data instantly when program starts

    int choice;
    while (1)
    {
        printf("\n=================================");
        printf("\n   BUS TICKET RESERVATION SYSTEM ");
        printf("\n=================================");
        printf("\n1. User Mode");
        printf("\n2. Admin Mode");
        printf("\n3. Exit & Save");
        printf("\nEnter your choice: ");

        // Input validation placeholder
        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input! Entering integers only.\n");
            while (getchar() != '\n')
                ; // Clear buffer
            continue;
        }

        switch (choice)
        {
        case 1:
            userMenu();
            break;
        case 2:
            adminMenu();
            break;
        case 3:
            saveToFile();
            printf("Thank you for using our system. Goodbye!\n");
            exit(0);
        default:
            printf("Invalid choice! Try again.\n");
        }
    }
    return 0;
}

void adminMenu()
{
    char correctUsername[] = "admin";
    char correctPassword[] = "1234"; // Default admin PIN

    char enteredUsername[30];
    char enteredPassword[30];
    int attempts = 3;
    int authenticated = 0;

    printf("\n=================================");
    printf("\n🔒      ADMIN SECURE LOGIN      ");
    printf("\n=================================");

    while (attempts > 0)
    {
        printf("\nEnter Username: ");
        scanf("%s", enteredUsername);

        printf("Enter Password: ");
        getMaskedPassword(enteredPassword, 30); // Use our new secure masker

        // Verify credentials
        if (strcmp(enteredUsername, correctUsername) == 0 &&
            strcmp(enteredPassword, correctPassword) == 0)
        {
            authenticated = 1;
            printf("\n✅ Access Granted! Welcome back, System Admin.\n");
            break;
        }
        else
        {
            attempts--;
            if (attempts > 0)
            {
                printf("❌ Invalid credentials! %d attempts remaining. Try again.\n", attempts);
            }
            else
            {
                printf("🛑 Access Denied! Too many failed attempts. Returning to Main Menu.\n");
                return; // Boot them back to the user menu
            }
        }
    }

    // --- PROTECTED ADMIN SECTOR ---
    // If authenticated, the administrative console loop executes safely
    int choice;
    while (1)
    {
        printf("\n--- ADMIN PANEL ---");
        printf("\n1. Add New Bus Route");
        printf("\n2. View All Active Routes & Financials");
        printf("\n3. Log Out & Exit Admin Mode");
        printf("\nEnter choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            addRoute();
            saveToFile();
            break;
        case 2:
            displayRoutes(); // Shows the upgraded table we built with pricing/capacity
            break;
        case 3:
            printf("\n🔒 Logging out securely...\n");
            return; // Safely goes back to main menu
        default:
            printf("Invalid choice.\n");
        }
    }
}

void addRoute()
{
    if (totalRoutes >= MAX_ROUTES)
    {
        printf("Error: Maximum route capacity reached!\n");
        return;
    }

    Route newRoute;
    printf("\nEnter Route ID (Integer): ");
    scanf("%d", &newRoute.routeID);

    printf("Enter Source City: ");
    scanf("%s", newRoute.source);

    printf("Enter Destination City: ");
    scanf("%s", newRoute.destination);

    printf("Enter Departure Time (e.g., 08:15AM or 21:45): ");
    scanf("%s", newRoute.departureTime);

    printf("Enter Bus Type (1 for AC_Sleeper, 2 for Non-AC_Seater): ");
    int typeChoice;
    scanf("%d", &typeChoice);
    if (typeChoice == 1)
        strcpy(newRoute.busType, "AC Sleeper");
    else
        strcpy(newRoute.busType, "Non-AC Seater");

    printf("Enter Ticket Price (INR): ");
    scanf("%lf", &newRoute.price);

    // Initialize all seats as empty
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            newRoute.seats[i][j].isBooked = 0;
            strcpy(newRoute.seats[i][j].passengerName, "");
            newRoute.seats[i][j].age = 0;
            strcpy(newRoute.seats[i][j].pnr, "");
        }
    }

    busRoutes[totalRoutes] = newRoute;
    totalRoutes++;
    printf("\n✨ Route %d successfully posted to the live dashboard!\n", newRoute.routeID);
}

// Helper function to count empty seats on a bus
int getAvailableSeatsCount(int routeIndex)
{
    int count = 0;
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            if (busRoutes[routeIndex].seats[i][j].isBooked == 0)
            {
                count++;
            }
        }
    }
    return count;
}

void displayRoutes()
{
    if (totalRoutes == 0)
    {
        printf("\n🛑 No active buses available right now.\n");
        return;
    }

    printf("\n==========================================================================================");
    printf("\n                           🚍 AVAILABLE BUSES SEARCH RESULTS                            ");
    printf("\n==========================================================================================");
    printf("\n%-10s %-15s %-15s %-12s %-15s %-10s %-10s",
           "ID", "From", "To", "Departure", "Type", "Price", "Available");
    printf("\n------------------------------------------------------------------------------------------");

    for (int i = 0; i < totalRoutes; i++)
    {
        int availableSeats = getAvailableSeatsCount(i);
        printf("\n%-10d %-15s %-15s %-12s %-15s ₹%-9.2f %d/%d Seats",
               busRoutes[i].routeID,
               busRoutes[i].source,
               busRoutes[i].destination,
               busRoutes[i].departureTime,
               busRoutes[i].busType,
               busRoutes[i].price,
               availableSeats,
               ROWS * COLS);
    }
    printf("\n==========================================================================================\n");
}

void saveToFile()
{
    FILE *file = fopen(FILENAME, "wb");
    if (file == NULL)
        return;
    fwrite(&totalRoutes, sizeof(int), 1, file);
    if (totalRoutes > 0)
    {
        fwrite(busRoutes, sizeof(Route), totalRoutes, file);
    }
    fclose(file);
}

void loadFromFile()
{
    FILE *file = fopen(FILENAME, "rb");
    if (file == NULL)
        return; // Fresh start if file doesn't exist
    fread(&totalRoutes, sizeof(int), 1, file);
    if (totalRoutes > 0)
    {
        fread(busRoutes, sizeof(Route), totalRoutes, file);
    }
    fclose(file);
}

// Secure helper function to read a password while printing asterisks (****)
void getMaskedPassword(char *passwordBuffer, int maxLength)
{
    int i = 0;
    char ch;

    while (1)
    {
        ch = getch(); // Read character without echoing it to the console

        if (ch == 13 || ch == 10)
        { // Carriage return or Enter key
            passwordBuffer[i] = '\0';
            break;
        }
        else if (ch == 8)
        { // Backspace key
            if (i > 0)
            {
                i--;
                printf("\b \b"); // Erase the character visually from screen
            }
        }
        else
        { // Normal character
            if (i < maxLength - 1)
            {
                passwordBuffer[i] = ch;
                i++;
                printf("*"); // Mask character on screen
            }
        }
    }
    printf("\n");
}

// --- HELPER FUNCTION: FIND ROUTE INDEX BY ID ---
// Because users type a Route ID (like 101), we need to find where it sits in our array (0, 1, 2...)
int findRouteIndex(int id)
{
    for (int i = 0; i < totalRoutes; i++)
    {
        if (busRoutes[i].routeID == id)
        {
            return i;
        }
    }
    return -1; // Not found
}

// --- GENERATE UNIQUE PNR ---
void generatePNR(char *pnrBuffer)
{
    // Generates a PNR format like: BUS-XXXX (where X is a random digit)
    sprintf(pnrBuffer, "BUS-%04d", rand() % 10000);
}

// --- USER MENU ---
void userMenu()
{
    if (totalRoutes == 0)
    {
        printf("\nNo active buses running at the moment. Check back later!\n");
        return;
    }

    int choice;
    while (1)
    {
        printf("\n--- USER PANEL ---");
        printf("\n1. View Available Buses & Seat Maps");
        printf("\n2. Book a Ticket");
        printf("\n3. Cancel a Ticket");
        printf("\n4. Return to Main Menu");
        printf("\nEnter choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
        {
            displayRoutes();
            int id;
            printf("\nEnter Route ID to view its seat map: ");
            scanf("%d", &id);
            int idx = findRouteIndex(id);
            if (idx != -1)
                displaySeatMap(idx);
            else
                printf("Route ID not found.\n");
            break;
        }
        case 2:
            bookTicket();
            saveToFile();
            break;
        case 3:
            cancelTicket();
            saveToFile();
            break;
        case 4:
            return;
        default:
            printf("Invalid choice.\n");
        }
    }
}

// --- DISPLAY ASCII SEAT MAP ---
void displaySeatMap(int routeIndex)
{
    printf("\n==== SEAT MAP FOR ROUTE %d ====\n", busRoutes[routeIndex].routeID);
    printf("   [Window]  Aisle  [Window]\n");
    printf("     Col 0   Col 1   Col 2   Col 3\n");
    printf("-----------------------------------\n");

    for (int i = 0; i < ROWS; i++)
    {
        printf("Row %d: ", i);
        for (int j = 0; j < COLS; j++)
        {
            if (j == 2)
                printf("   "); // Print a visual aisle space down the middle

            if (busRoutes[routeIndex].seats[i][j].isBooked == 1)
            {
                printf("[ X ] "); // X means Booked
            }
            else
            {
                printf("[%d,%d] ", i, j); // Shows coordinates for easy selection
            }
        }
        printf("\n");
    }
    printf("-----------------------------------\n");
}

// --- BOOK TICKET (WITH AI SUGGESTER INTEGRATION) ---
void bookTicket()
{
    displayRoutes();
    int id;
    printf("\nEnter Route ID you want to book: ");
    scanf("%d", &id);

    int idx = findRouteIndex(id);
    if (idx == -1)
    {
        printf("Invalid Route ID!\n");
        return;
    }

    displaySeatMap(idx);

    int useAI;
    int row = -1, col = -1;

    printf("\nWould you like our Smart Seat Suggester to find you a seat? (1 for Yes, 0 to manual select): ");
    scanf("%d", &useAI);

    if (useAI == 1)
    {
        int pref;
        printf("Enter your preference (1 = Window, 2 = Aisle, 3 = Front Row): ");
        scanf("%d", &pref);

        // Scan the matrix to find a seat matching preference
        // Window seats are columns 0 and 3. Aisle seats are columns 1 and 2.
        int found = 0;
        for (int i = 0; i < ROWS && !found; i++)
        {
            for (int j = 0; j < COLS; j++)
            {
                if (busRoutes[idx].seats[i][j].isBooked == 0)
                {
                    if ((pref == 1 && (j == 0 || j == 3)) || // Window
                        (pref == 2 && (j == 1 || j == 2)) || // Aisle
                        (pref == 3 && i < 3))
                    { // Front Row (rows 0-2)

                        row = i;
                        col = j;
                        found = 1;
                        printf("\n💡 AI Suggestion: Seat [%d,%d] matches your preference!\n", row, col);
                        break;
                    }
                }
            }
        }
        if (!found)
        {
            printf("\nCould not find a perfect match for your preference. Please select manually.\n");
            useAI = 0; // Fallback to manual selection
        }
    }

    if (useAI == 0)
    {
        printf("\nEnter Row number (0-%d): ", ROWS - 1);
        scanf("%d", &row);
        printf("Enter Column number (0-%d): ", COLS - 1);
        scanf("%d", &col);

        if (row < 0 || row >= ROWS || col < 0 || col >= COLS)
        {
            printf("Invalid seat coordinates!\n");
            return;
        }
        if (busRoutes[idx].seats[row][col].isBooked == 1)
        {
            printf("This seat is already booked! Please choose another.\n");
            return;
        }
    }

    // Capture Passenger Info
    Seat *s = &busRoutes[idx].seats[row][col];
    s->isBooked = 1;
    printf("Enter Passenger Name (No spaces): ");
    scanf("%s", s->passengerName);
    printf("Enter Passenger Age: ");
    scanf("%d", &s->age);

    generatePNR(s->pnr);

    // Calculate simulated taxes like a real website
    double baseFare = busRoutes[idx].price;
    double gst = baseFare * 0.05; // 5% GST
    double totalFare = baseFare + gst;

    // Print Formatted Modern Invoice Summary
    printf("\n=================================================");
    printf("\n📦               BOOKING CONFIRMED              ");
    printf("\n=================================================");
    printf("\n  PNR NUMBER      : %s", s->pnr);
    printf("\n  Passenger Name  : %s (Age: %d)", s->passengerName, s->age);
    printf("\n  Journey         : %s ➔ %s", busRoutes[idx].source, busRoutes[idx].destination);
    printf("\n  Departure Time  : %s", busRoutes[idx].departureTime);
    printf("\n  Bus Category    : %s", busRoutes[idx].busType);
    printf("\n  Assigned Seat   : Row %d, Column %d", row, col);
    printf("\n-------------------------------------------------");
    printf("\n  Base Fare       : ₹%.2f", baseFare);
    printf("\n  Internet GST(5%): ₹%.2f", gst);
    printf("\n  TOTAL PAID      : ₹%.2f", totalFare);
    printf("\n=================================================");
    printf("\n       🎟️  THANK YOU FOR BOOKING WITH US!       ");
    printf("\n=================================================\n");
}

// --- CANCEL TICKET ---
void cancelTicket()
{
    displayRoutes();
    int id;
    printf("\nEnter Route ID: ");
    scanf("%d", &id);

    int idx = findRouteIndex(id);
    if (idx == -1)
    {
        printf("Route not found.\n");
        return;
    }

    char targetPNR[15];
    printf("Enter your PNR number to cancel reservation: ");
    scanf("%s", targetPNR);

    
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            if (busRoutes[idx].seats[i][j].isBooked == 1 && strcmp(busRoutes[idx].seats[i][j].pnr, targetPNR) == 0)
            {
                
                busRoutes[idx].seats[i][j].isBooked = 0;
                strcpy(busRoutes[idx].seats[i][j].passengerName, "");
                busRoutes[idx].seats[i][j].age = 0;
                strcpy(busRoutes[idx].seats[i][j].pnr, "");

                printf("\nSuccess: Reservation under PNR %s has been canceled.\n", targetPNR);
                return;
            }
        }
    }
    printf("\nError: PNR not found on this route.\n");
}


void recommendSeat(int routeIndex, int preference)
{
    
}
