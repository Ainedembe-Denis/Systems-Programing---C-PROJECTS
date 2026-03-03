#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_VEHICLES 100
#define DATA_FILE "vehicles.dat"

// Structure to store vehicle details
struct Vehicle {
    int vehicleID;
    char type[30];
    float mileage;
    float fuelUsed;
};

struct Vehicle vehicles[MAX_VEHICLES];
int vehicleCount = 0;

// Load vehicles from file
void loadVehicles() {
    FILE *file = fopen(DATA_FILE, "rb");
    if(file != NULL) {
        fread(&vehicleCount, sizeof(int), 1, file);
        fread(vehicles, sizeof(struct Vehicle), vehicleCount, file);
        fclose(file);
    }
}

// Save vehicles to file
void saveVehicles() {
    FILE *file = fopen(DATA_FILE, "wb");
    if(file != NULL) {
        fwrite(&vehicleCount, sizeof(int), 1, file);
        fwrite(vehicles, sizeof(struct Vehicle), vehicleCount, file);
        fclose(file);
    }
}

// Add a new vehicle
void addVehicle() {
    if(vehicleCount >= MAX_VEHICLES){
        printf("Fleet is full.\n");
        return;
    }
    struct Vehicle v;
    printf("Enter Vehicle ID: ");
    scanf("%d", &v.vehicleID);
    printf("Enter Vehicle Type: ");
    scanf(" %[^\n]s", v.type);
    printf("Enter Mileage (km): ");
    scanf("%f", &v.mileage);
    printf("Enter Fuel Used (litres): ");
    scanf("%f", &v.fuelUsed);

    vehicles[vehicleCount++] = v;
    saveVehicles();
    printf("Vehicle added successfully!\n");
}

// Record maintenance (update mileage)
void recordMaintenance() {
    int id, found = 0;
    printf("Enter Vehicle ID for maintenance: ");
    scanf("%d", &id);
	int i;
    for(i = 0; i < vehicleCount; i++){
        if(vehicles[i].vehicleID == id){
            float additionalMileage;
            printf("Enter additional mileage since last maintenance: ");
            scanf("%f", &additionalMileage);
            vehicles[i].mileage += additionalMileage;
            saveVehicles();
            printf("Maintenance recorded for Vehicle ID %d\n", id);
            found = 1;
            break;
        }
    }
    if(!found)
        printf("Vehicle not found.\n");
}

// Record fuel usage
void recordFuelUsage() {
    int id, found = 0;
    printf("Enter Vehicle ID: ");
    scanf("%d", &id);
	int i;
    for(i = 0; i < vehicleCount; i++){
        if(vehicles[i].vehicleID == id){
            float fuel;
            printf("Enter fuel used (litres): ");
            scanf("%f", &fuel);
            vehicles[i].fuelUsed += fuel;
            saveVehicles();
            printf("Fuel usage updated for Vehicle ID %d\n", id);
            found = 1;
            break;
        }
    }
    if(!found)
        printf("Vehicle not found.\n");
}

// Generate maintenance alerts based on mileage threshold
void maintenanceAlert() {
    float threshold;
    printf("Enter mileage threshold for maintenance alert: ");
    scanf("%f", &threshold);

    printf("\nVehicles needing maintenance:\n");
    int i;
    for(i = 0; i < vehicleCount; i++){
        if(vehicles[i].mileage >= threshold){
            printf("Vehicle ID: %d | Type: %s | Mileage: %.2f km\n",
                   vehicles[i].vehicleID, vehicles[i].type, vehicles[i].mileage);
        }
    }
}

// Display all vehicles
void displayVehicles() {
    if(vehicleCount == 0) {
        printf("No vehicles in fleet.\n");
        return;
    }
    printf("\nVehicle Fleet -------------------------------\n");
    int i;
    for(i = 0; i < vehicleCount; i++){
        printf("ID: %d | Type: %s | Mileage: %.2f km | Fuel Used: %.2f L\n",
               vehicles[i].vehicleID, vehicles[i].type, vehicles[i].mileage, vehicles[i].fuelUsed);
    }
}

int main() {
    loadVehicles();
    int choice;
    do {
        printf("\nVEHICLE FLEET MANAGEMENT\n");
        printf("1. Add Vehicle\n");
        printf("2. Record Maintenance\n");
        printf("3. Record Fuel Usage\n");
        printf("4. Generate Maintenance Alert\n");
        printf("5. Display All Vehicles\n");
        printf("6. Exit\n");
        printf("\nEnter choice: ");
        scanf("%d", &choice);

        switch(choice) {
            case 1: addVehicle(); break;
            case 2: recordMaintenance(); break;
            case 3: recordFuelUsage(); break;
            case 4: maintenanceAlert(); break;
            case 5: displayVehicles(); break;
            case 6: printf("Exiting...\n"); break;
            default: printf("Invalid choice!\n");
        }
    } while(choice != 6);

    return 0;
}