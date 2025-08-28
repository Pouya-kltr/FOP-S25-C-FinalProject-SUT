#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

// -----------------------------
// Data Structures
// -----------------------------
typedef struct {
    int id;
    char type[30];
    double price;
    int isBooked;
} Room;

typedef struct {
    int bookingId;
    char customerName[50];
    char phone[20];
    int roomId;
    char date[20];
} Booking;

// -----------------------------
// Function Prototypes
// -----------------------------
void customerPanel(char name[], char phone[]);
void adminPanel();
void saveRooms();

// -----------------------------
// Global Variables
// -----------------------------
Room rooms[100];
int roomCount = 0;

Booking bookings[100];
int bookingCount = 0;

// -----------------------------
// File Functions
// -----------------------------
void loadRooms() {
    FILE *fp = fopen("prices.txt", "r");
    if (fp == NULL) {
        // Create default prices.txt if it doesn't exist
        fp = fopen("prices.txt", "w");
        fprintf(fp, "Single 50.00 3\n");
        fprintf(fp, "Double 80.00 6\n");
        fprintf(fp, "Suite 120.00 8\n");
        fprintf(fp, "Deluxe 100.00 5\n");
        fprintf(fp, "Family 150.00 10\n");
        fclose(fp);
        fp = fopen("prices.txt", "r");
        if (fp == NULL) {
            printf("Failed to create prices.txt!\n");
            return;
        }
    }

    char type[30];
    double price;
    int count;
    while (fscanf(fp, "%s %lf %d", type, &price, &count) == 3) {
        for (int i = 0; i < count; i++) {
            if (roomCount >= 100) {
                printf("Maximum rooms reached!\n");
                fclose(fp);
                return;
            }
            int id, unique;
            do {
                id = rand() % 900 + 100; // Generate 3-digit ID
                unique = 1;
                for (int j = 0; j < roomCount; j++) {
                    if (rooms[j].id == id) {
                        unique = 0;
                        break;
                    }
                }
            } while (!unique);

            rooms[roomCount].id = id;
            strcpy(rooms[roomCount].type, type);
            rooms[roomCount].price = price;
            rooms[roomCount].isBooked = 0;
            roomCount++;
        }
    }
    fclose(fp);
    saveRooms();
}

void saveRooms() {
    FILE *fp = fopen("rooms.txt", "w");
    for (int i = 0; i < roomCount; i++) {
        fprintf(fp, "%d %s %.2lf %d\n", rooms[i].id, rooms[i].type, rooms[i].price, rooms[i].isBooked);
    }
    fclose(fp);
}

void loadBookings() {
    FILE *fp = fopen("bookings.txt", "r");
    if (fp == NULL) return;
    while (fscanf(fp, "%d %s %s %d %s",
                  &bookings[bookingCount].bookingId,
                  bookings[bookingCount].customerName,
                  bookings[bookingCount].phone,
                  &bookings[bookingCount].roomId,
                  bookings[bookingCount].date) == 5) {
        bookingCount++;
    }
    fclose(fp);
}

void saveBookings() {
    FILE *fp = fopen("bookings.txt", "w");
    for (int i = 0; i < bookingCount; i++) {
        fprintf(fp, "%d %s %s %d %s\n",
                bookings[i].bookingId,
                bookings[i].customerName,
                bookings[i].phone,
                bookings[i].roomId,
                bookings[i].date);
    }
    fclose(fp);
}

// -----------------------------
// Admin Login
// -----------------------------
int checkAdminPassword() {
    FILE *fp = fopen("admin_pass.txt", "r");
    char storedPass[50], enteredPass[50];
    if (fp == NULL) {
        fp = fopen("admin_pass.txt", "w");
        fprintf(fp, "admin123\n");
        fclose(fp);
        strcpy(storedPass, "admin123");
    } else {
        fscanf(fp, "%s", storedPass);
        fclose(fp);
    }

    printf("Admin Password: ");
    scanf("%s", enteredPass);

    if (strcmp(storedPass, enteredPass) == 0) return 1;
    else {
        printf("Incorrect Password!\n");
        return 0;
    }
}

// -----------------------------
// Validation Functions
// -----------------------------
int isValidName(const char *name) {
    if (strlen(name) == 0) return 0;
    for (int i = 0; name[i] != '\0'; i++) {
        if (!isalpha(name[i])) {
            return 0;
        }
    }
    return 1;
}

int isValidPhone(const char *phone) {
    if (strlen(phone) != 10) return 0;
    for (int i = 0; phone[i] != '\0'; i++) {
        if (!isdigit(phone[i])) {
            return 0;
        }
    }
    return 1;
}

int isValidDate(const char *date) {
    int year, month, day;
    if (sscanf(date, "%d/%d/%d", &year, &month, &day) != 3) {
        return 0;
    }
    if (year < 2025 || month < 1 || month > 12 || day < 1 || day > 31) {
        return 0;
    }
    int daysInMonth[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (day > daysInMonth[month]) {
        return 0;
    }
    // Simple leap year check for February
    if (month == 2 && day == 29) {
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
            return 1;
        } else {
            return 0;
        }
    }
    return 1;
}

// -----------------------------
// Customer Functions
// -----------------------------
void viewRooms() {
    printf("\nAvailable Room Types:\n");
    struct RoomTypeCount {
        char type[30];
        double price;
        int available;
    } roomTypes[] = {
        {"Single", 50.00, 0},
        {"Double", 80.00, 0},
        {"Suite", 120.00, 0},
        {"Deluxe", 100.00, 0},
        {"Family", 150.00, 0}
    };

    // Count available rooms for each type
    for (int i = 0; i < roomCount; i++) {
        for (int j = 0; j < 5; j++) {
            if (strcmp(rooms[i].type, roomTypes[j].type) == 0 && !rooms[i].isBooked) {
                roomTypes[j].available++;
                roomTypes[j].price = rooms[i].price; // Update price from rooms
            }
        }
    }

    // Display available counts and prices
    for (int j = 0; j < 5; j++) {
        printf("%d. %s: %d rooms available, Price: $%.2lf\n", j + 1, roomTypes[j].type, roomTypes[j].available, roomTypes[j].price);
    }
}

void makeBooking(char name[], char phone[]) {
    int roomTypeChoice;
    char date[20];
    printf("Room Type (1-5): ");
    scanf("%d", &roomTypeChoice);
    if (roomTypeChoice < 1 || roomTypeChoice > 5) {
        printf("Invalid room type! Please choose a number between 1 and 5.\n");
        return;
    }
    printf("Date (e.g., 2025/01/10): ");
    scanf("%s", date);

    if (!isValidDate(date)) {
        printf("Invalid date! Please enter a valid date (YYYY/MM/DD, month 1-12, day 1-31 depending on month).\n");
        return;
    }

    // Map room type choice to room type
    char *roomTypes[] = {"Single", "Double", "Suite", "Deluxe", "Family"};
    char *selectedType = roomTypes[roomTypeChoice - 1];

    // Find an available room of the selected type
    int selectedRoomId = -1;
    for (int i = 0; i < roomCount; i++) {
        if (strcmp(rooms[i].type, selectedType) == 0 && !rooms[i].isBooked) {
            selectedRoomId = rooms[i].id;
            rooms[i].isBooked = 1;
            break;
        }
    }

    if (selectedRoomId == -1) {
        printf("No rooms available for %s!\n", selectedType);
        return;
    }

    // Generate unique 6-digit booking ID
    int bookingId, unique;
    do {
        bookingId = rand() % 900000 + 100000; // Generate 6-digit Booking ID
        unique = 1;
        for (int j = 0; j < bookingCount; j++) {
            if (bookings[j].bookingId == bookingId) {
                unique = 0;
                break;
            }
        }
    } while (!unique);

    bookings[bookingCount].bookingId = bookingId;
    strcpy(bookings[bookingCount].customerName, name);
    strcpy(bookings[bookingCount].phone, phone);
    bookings[bookingCount].roomId = selectedRoomId;
    strcpy(bookings[bookingCount].date, date);
    bookingCount++;
    printf("Booking Successful! Booking ID: %d, Room Number: %d\n", bookingId, selectedRoomId);
    saveRooms();
    saveBookings();
}

void viewMyBookings(char name[], char phone[]) {
    printf("\nYour Bookings:\n");
    for (int i = 0; i < bookingCount; i++) {
        if (strcmp(bookings[i].customerName, name) == 0 &&
            strcmp(bookings[i].phone, phone) == 0) {
            printf("Booking %d | Room: %d | Date: %s\n",
                   bookings[i].bookingId, bookings[i].roomId, bookings[i].date);
        }
    }
}

void cancelBooking(char name[], char phone[]) {
    int bId;
    printf("Booking ID to Cancel: ");
    scanf("%d", &bId);
    for (int i = 0; i < bookingCount; i++) {
        if (bookings[i].bookingId == bId &&
            strcmp(bookings[i].customerName, name) == 0 &&
            strcmp(bookings[i].phone, phone) == 0) {
            for (int j = 0; j < roomCount; j++) {
                if (rooms[j].id == bookings[i].roomId) {
                    rooms[j].isBooked = 0;
                }
            }
            for (int k = i; k < bookingCount - 1; k++) {
                bookings[k] = bookings[k + 1];
            }
            bookingCount--;
            printf("Booking Cancelled.\n");
            saveRooms();
            saveBookings();
            return;
        }
    }
    printf("Booking Not Found.\n");
}

// -----------------------------
// Admin Functions
// -----------------------------
void viewAllBookings() {
    printf("\nAll Bookings:\n");
    for (int i = 0; i < bookingCount; i++) {
        printf("Booking %d | %s | %s | Room %d | %s\n",
               bookings[i].bookingId,
               bookings[i].customerName,
               bookings[i].phone,
               bookings[i].roomId,
               bookings[i].date);
    }
}

void revenueReport() {
    double total = 0;
    for (int i = 0; i < bookingCount; i++) {
        for (int j = 0; j < roomCount; j++) {
            if (rooms[j].id == bookings[i].roomId) {
                total += rooms[j].price;
            }
        }
    }
    printf("Total Revenue: %.2lf\n", total);
}

void changeAdminPassword() {
    char newPass[50];
    printf("New Password: ");
    scanf("%s", newPass);
    FILE *fp = fopen("admin_pass.txt", "w");
    fprintf(fp, "%s\n", newPass);
    fclose(fp);
    printf("Password Changed.\n");
}

void addRoom() {
    if (roomCount >= 100) {
        printf("Maximum rooms reached!\n");
        return;
    }
    char type[30];
    double price;
    printf("Room Type: ");
    scanf("%s", type);
    printf("Room Price: ");
    scanf("%lf", &price);

    int id;
    int unique;
    do {
        id = rand() % 900 + 100; // Generate 3-digit ID (100 to 999)
        unique = 1;
        for (int i = 0; i < roomCount; i++) {
            if (rooms[i].id == id) {
                unique = 0;
                break;
            }
        }
    } while (!unique);

    rooms[roomCount].id = id;
    strcpy(rooms[roomCount].type, type);
    rooms[roomCount].price = price;
    rooms[roomCount].isBooked = 0;
    roomCount++;
    saveRooms();
    printf("Room added with ID: %d\n", id);
}

// -----------------------------
// Menus
// -----------------------------
void customerPanel(char name[], char phone[]) {
    int c;
    do {
        printf("\n===== Customer Menu =====\n");
        printf("1. View Rooms\n");
        printf("2. Book a Room\n");
        printf("3. View My Bookings\n");
        printf("4. Cancel Booking\n");
        printf("0. Exit\nChoice: ");
        scanf("%d", &c);
        switch(c) {
            case 1: viewRooms(); break;
            case 2: makeBooking(name, phone); break;
            case 3: viewMyBookings(name, phone); break;
            case 4: cancelBooking(name, phone); break;
        }
    } while(c != 0);
}

void adminPanel() {
    int c;
    do {
        printf("\n===== Admin Menu =====\n");
        printf("1. View All Bookings\n");
        printf("2. Revenue Report\n");
        printf("3. Change Password\n");
        printf("4. Add Room\n");
        printf("0. Exit\nChoice: ");
        scanf("%d", &c);
        switch(c) {
            case 1: viewAllBookings(); break;
            case 2: revenueReport(); break;
            case 3: changeAdminPassword(); break;
            case 4: addRoom(); break;
        }
    } while(c != 0);
}

// -----------------------------
// main
// -----------------------------
int main() {
    srand(time(NULL));
    loadRooms();
    loadBookings();

    int choice;
    do {
        printf("\n===== Hotel Reservation System =====\n");
        printf("1. Customer Login\n");
        printf("2. Admin Login\n");
        printf("0. Exit\nChoice: ");
        scanf("%d", &choice);

        if (choice == 1) {
            char name[50], phone[20];
            printf("Name: ");
            scanf("%s", name);
            if (!isValidName(name)) {
                printf("Invalid name! Name must contain only letters.\n");
                continue;
            }
            printf("Phone Number: ");
            scanf("%s", phone);
            if (!isValidPhone(phone)) {
                printf("Invalid phone number! Must be exactly 10 digits.\n");
                continue;
            }
            customerPanel(name, phone);
        } else if (choice == 2) {
            if (checkAdminPassword()) {
                printf("Login Successful!\n");
                adminPanel();
            }
        }
    } while(choice != 0);

    saveRooms();
    saveBookings();

    printf("Goodbye!\n");
    return 0;
}