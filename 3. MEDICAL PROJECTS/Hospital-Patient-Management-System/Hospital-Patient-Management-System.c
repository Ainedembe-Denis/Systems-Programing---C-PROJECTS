#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DOCTOR_FILE "doctors.dat"
#define PATIENT_FILE "patients.dat"
#define APPOINTMENT_FILE "appointments.dat"
#define BILL_FILE "bills.dat"

// Structures ------------------------------------------------------------------
struct Doctor {
    int doctorID;
    char name[50];
    char specialization[50];
};

struct Patient {
    int patientID;
    char name[50];
    int age;
};

struct Appointment {
    int appointmentID;
    int doctorID;
    int patientID;
    char complaint[100];
    char appointmentDate[20];
    char appointmentTime[10];
    char diagnosis[100];
    char status; // 'P' = pending, 'C' = complete
};

struct BillItem {
    int appointmentID;
    int billItemNo;
    char itemName[50]; 
    float amount;      
};

// Utility Functions -----------------------------------------------------------

// Generate auto-increment Appointment ID
int generateAppointmentID() {
    FILE *fp = fopen(APPOINTMENT_FILE, "rb");
    int lastID = 1000;
    struct Appointment a;
    if (!fp) return lastID + 1;
    while (fread(&a, sizeof(a), 1, fp)) {
        if (a.appointmentID > lastID) lastID = a.appointmentID;
    }
    fclose(fp);
    return lastID + 1;
}

// Find patient by ID
int findPatientByID(int patientID, struct Patient *p) {
    FILE *fp = fopen(PATIENT_FILE, "rb");
    if (!fp) return 0;
    while(fread(p, sizeof(*p), 1, fp)) {
        if(p->patientID == patientID) { fclose(fp); return 1; }
    }
    fclose(fp);
    return 0;
}

// Find doctor by ID
int findDoctorByID(int doctorID, struct Doctor *d) {
    FILE *fp = fopen(DOCTOR_FILE, "rb");
    if (!fp) return 0;
    while(fread(d, sizeof(*d), 1, fp)) {
        if(d->doctorID == doctorID) { fclose(fp); return 1; }
    }
    fclose(fp);
    return 0;
}

// Compute total bill for an appointment
float computeTotalBill(int appointmentID) {
    FILE *fp = fopen(BILL_FILE, "rb");
    struct BillItem b;
    float total = 0;
    if (!fp) return 0;
    while(fread(&b, sizeof(b), 1, fp)) {
        if(b.appointmentID == appointmentID) total += b.amount;
    }
    fclose(fp);
    return total;
}

// Doctor Module ---------------------------------------------------------------
void addDoctor() {
    FILE *fp = fopen(DOCTOR_FILE, "ab");
    struct Doctor d;
    if(!fp){ printf("File error!\n"); return; }

    printf("Enter Doctor ID: "); 
	scanf("%d", &d.doctorID); 
	getchar();
	
    printf("Enter Name: "); 
	fgets(d.name, sizeof(d.name), stdin); 
	d.name[strcspn(d.name,"\n")] = 0;
	
    printf("Enter Specialization: "); 
	fgets(d.specialization, sizeof(d.specialization), stdin); 
	d.specialization[strcspn(d.specialization,"\n")] = 0;

    fwrite(&d, sizeof(d), 1, fp);
    fclose(fp);
    printf("Doctor added successfully!\n");
}

// Patient Module --------------------------------------------------------------
void addPatient() {
    FILE *fp = fopen(PATIENT_FILE, "ab");
    struct Patient p;
    if(!fp){ 
		printf("File error!\n"); 
		return; 
	}

    printf("Enter Patient ID: "); 
	scanf("%d", &p.patientID); 
	getchar();
	
    printf("Enter Name: "); 
	fgets(p.name, sizeof(p.name), stdin); 
	p.name[strcspn(p.name,"\n")] = 0;
	
    printf("Enter Age: "); 
	scanf("%d", &p.age); 
	getchar();

    fwrite(&p, sizeof(p), 1, fp);
    fclose(fp);
    printf("Patient added successfully!\n");
}

void viewPatients() {
    FILE *fpP = fopen(PATIENT_FILE, "rb");
    FILE *fpA = fopen(APPOINTMENT_FILE, "rb");
    struct Patient p;
    struct Appointment a;

    if(!fpP){ 
		printf("No patients found!\n"); 
		return;
	}

    printf("\nPatients -------------------------\n");
    while(fread(&p, sizeof(p), 1, fpP)){
        struct Appointment lastAppt;
        int found = 0;
        if(fpA) rewind(fpA);
        while(fpA && fread(&a, sizeof(a), 1, fpA)){
            if(a.patientID == p.patientID && a.status=='C'){
                lastAppt = a;
                found = 1;
            }
        }
        if(found){
            float totalBill = computeTotalBill(lastAppt.appointmentID);
            printf("ID: %d | Name: %s | Age: %d | Last Complaint: %s | Last Diagnosis: %s | TL Bill: %.2f\n",
                p.patientID, p.name, p.age, lastAppt.complaint, lastAppt.diagnosis, totalBill);
        } else {
            printf("ID: %d | Name: %s | Age: %d | No completed visit yet.\n", p.patientID, p.name, p.age);
        }
    }
    if(fpA) fclose(fpA);
    fclose(fpP);
}

// Appointment Module ----------------------------------------------------------
void createAppointment() {
    struct Appointment a;
    FILE *fp = fopen(APPOINTMENT_FILE, "ab");
    if(!fp){ printf("File error!\n"); return; }

    a.appointmentID = generateAppointmentID();

    printf("Enter Doctor ID: "); 
	scanf("%d",&a.doctorID); 
	getchar();
	
    struct Doctor d; 
	
	if(!findDoctorByID(a.doctorID,&d)){ 
		printf("Doctor not found!\n"); 
		fclose(fp); 
		return; 
	}

    printf("Enter Patient ID: "); 
	scanf("%d",&a.patientID); 
	getchar();
	
    struct Patient p; 
    
	if(!findPatientByID(a.patientID,&p)){ 
		printf("Patient not found!\n"); 
		fclose(fp); 
		return; 
	}

    printf("Enter Complaint: "); 
	fgets(a.complaint,sizeof(a.complaint),stdin); 
	a.complaint[strcspn(a.complaint,"\n")] = 0;
	
    printf("Enter Appointment Date (DD/MM/YYYY): "); 
	fgets(a.appointmentDate,sizeof(a.appointmentDate),stdin); 
	a.appointmentDate[strcspn(a.appointmentDate,"\n")] = 0;
	
    printf("Enter Appointment Time (HH:MM): "); 
	fgets(a.appointmentTime,sizeof(a.appointmentTime),stdin); 
	a.appointmentTime[strcspn(a.appointmentTime,"\n")] = 0;

    strcpy(a.diagnosis,""); a.status='P';

    fwrite(&a,sizeof(a),1,fp);
    fclose(fp);
    printf("Appointment created successfully! Appointment ID: %d\n", a.appointmentID);
}

// View appointments (general)
void viewAllAppointments() {
    FILE *fpA = fopen(APPOINTMENT_FILE,"rb");
    struct Appointment a; struct Patient p; struct Doctor d;
    if(!fpA){ printf("No appointments found!\n"); return; }

    printf("\nAll Appointments --------------------------\n");
    while(fread(&a,sizeof(a),1,fpA)){
        if(!findPatientByID(a.patientID,&p)) continue;
        if(!findDoctorByID(a.doctorID,&d)) continue;
        float totalBill = computeTotalBill(a.appointmentID);
        printf("Appt. ID: %d | Patient: %s | Dr: %s | Date: %s | Time: %s | Status: %c | Diagnosis: %s | Total Bill: %.2f\n",
            a.appointmentID,p.name,d.name,a.appointmentDate,a.appointmentTime,a.status,
            strlen(a.diagnosis)?a.diagnosis:"N/A",totalBill);
    }
    fclose(fpA);
}

// View appointments by patient ------------------------------------------------
void viewAppointmentsByPatient() {
    int patientID;
    printf("Enter Patient ID: "); 
	scanf("%d",&patientID); 
	getchar();
    FILE *fpA = fopen(APPOINTMENT_FILE,"rb");
    struct Appointment a; 
	struct Patient p; 
	struct Doctor d;
    if(!findPatientByID(patientID,&p)){ 
		printf("Patient not found!\n"); 
		return; 
	}
    if(!fpA){ 
		printf("No appointments found!\n"); 
		return; 
	}

    printf("\nAppointments for %s ------------------\n",p.name);
    while(fread(&a,sizeof(a),1,fpA)){
        if(a.patientID != patientID) continue;
        if(!findDoctorByID(a.doctorID,&d)) continue;
        float totalBill = computeTotalBill(a.appointmentID);
        printf("Appt. ID: %d | Dr: %s | Date: %s | Time: %s | Status: %c | Diagnosis: %s | TL Bill: %.2f\n",
            a.appointmentID,d.name,a.appointmentDate,a.appointmentTime,a.status,
            strlen(a.diagnosis)?a.diagnosis:"N/A",totalBill);
    }
    fclose(fpA);
}

// View appointments by doctor
void viewAppointmentsByDoctor() {
    int doctorID;
    printf("Enter Doctor ID: "); 
	scanf("%d",&doctorID); 
	getchar();
    FILE *fpA = fopen(APPOINTMENT_FILE,"rb");
    struct Appointment a; 
	struct Patient p; 
	struct Doctor d;
    if(!findDoctorByID(doctorID,&d)){ 
		printf("Doctor not found!\n"); 
		return; 
	}
    if(!fpA){ 
		printf("No appointments found!\n"); 
		return; 
	}

    printf("\nAppointments for Dr %s ------------------\n",d.name);
    while(fread(&a,sizeof(a),1,fpA)){
        if(a.doctorID != doctorID) continue;
        if(!findPatientByID(a.patientID,&p)) continue;
        float totalBill = computeTotalBill(a.appointmentID);
        printf("Appt. ID: %d | Patient: %s | Date: %s | Time: %s | Status: %c | Diagnosis: %s | TL Bill: %.2f\n",
            a.appointmentID,p.name,a.appointmentDate,a.appointmentTime,a.status,
            strlen(a.diagnosis)?a.diagnosis:"N/A",totalBill);
    }
    fclose(fpA);
}

// Diagnose patient & add bill items -------------------------------------------
void diagnosePatient() {
    int apptID, numBills;
    struct Appointment a;
    FILE *fp = fopen(APPOINTMENT_FILE,"rb+");
    if(!fp){ 
		printf("No appointments found!\n"); 
		return; 
	}

    printf("Enter Appointment ID to diagnose: "); 
	scanf("%d",&apptID); 
	getchar();
    int found=0;
    while(fread(&a,sizeof(a),1,fp)){
        if(a.appointmentID == apptID){
            printf("Enter Diagnosis: "); 
			fgets(a.diagnosis,sizeof(a.diagnosis),stdin); 
			a.diagnosis[strcspn(a.diagnosis,"\n")]=0;
            a.status='C';
            fseek(fp,-sizeof(a),SEEK_CUR);
            fwrite(&a,sizeof(a),1,fp);

            printf("Enter number of bill items for this appointment: "); 
			scanf("%d",&numBills); 
			getchar();
            FILE *fpB = fopen(BILL_FILE,"ab");
            int i;
            for(i=1;i<=numBills;i++){
                struct BillItem b;
                b.appointmentID = a.appointmentID;
                b.billItemNo = i;
                printf("Enter Bill Item Name %d: ",i); 
				fgets(b.itemName,sizeof(b.itemName),stdin); 
				b.itemName[strcspn(b.itemName,"\n")]=0;
                printf("Enter Amount for '%s': ", b.itemName); 
				scanf("%f",&b.amount); 
				getchar();
                fwrite(&b,sizeof(b),1,fpB);
            }
            fclose(fpB);
            printf("Diagnosis and billing recorded!\n");
            found=1; break;
        }
    }
    if(!found) printf("Appointment ID not found!\n");
    fclose(fp);
}

// Update the BIll Item --------------------------------------------------------
void updateBillItem() {
    int apptID, itemNo;
    char choice;
    struct BillItem b;
    FILE *fp = fopen(BILL_FILE,"rb+");
    
    if(!fp){ 
		printf("No bills found!\n"); 
		return; 
	}

    printf("Enter Appointment ID: "); 
	scanf("%d",&apptID); 
	getchar();

    // List all bill items for this appointment
    printf("\nBill Items for Appointment %d ------------------\n", apptID);
    int foundItems=0;
    while(fread(&b,sizeof(b),1,fp)){
        if(b.appointmentID == apptID){
            printf("%d. %s, %.2f\n", b.billItemNo, b.itemName, b.amount);
            foundItems = 1;
        }
    }
    if(!foundItems){ 
		printf("No bill items found for this appointment!\n"); 
		fclose(fp); 
		return; 
	}

    // Ask which item to update
    printf("Enter Bill Item Number to update: "); 
	scanf("%d",&itemNo); 
	getchar();
    rewind(fp);

    int updated=0;
    while(fread(&b,sizeof(b),1,fp)){
        if(b.appointmentID==apptID && b.billItemNo==itemNo){
            printf("Update Name? (y/n): "); 
			scanf("%c",&choice); 
			getchar();
            
            if(choice=='y' || choice=='Y'){
                printf("Enter new name: ");
                fgets(b.itemName,sizeof(b.itemName),stdin);
                b.itemName[strcspn(b.itemName,"\n")] = 0;
            }
            printf("Update Amount? (y/n): "); 
			scanf("%c",&choice); 
			getchar();
            
            if(choice=='y' || choice=='Y'){
                printf("Enter new amount: "); 
				scanf("%f",&b.amount); 
				getchar();
            }

            fseek(fp,-sizeof(b),SEEK_CUR);
            fwrite(&b,sizeof(b),1,fp);
            printf("Bill item updated successfully!\n");
            updated=1;
            break;
        }
    }

    if(!updated) printf("Bill item not found!\n");
    fclose(fp);
}


// View full patient history ---------------------------------------------------
void viewPatientHistory() {
    int patientID;
    printf("Enter Patient ID: "); 
	scanf("%d",&patientID); 
	getchar();
	
    FILE *fpA = fopen(APPOINTMENT_FILE,"rb");
    FILE *fpB = fopen(BILL_FILE,"rb");
    
    struct Appointment a; 
	struct Patient p; 
	struct Doctor d; 
	struct BillItem b;
	
    if(!findPatientByID(patientID,&p)){ 
		printf("Patient not found!\n"); 
		return; 
	}
	
    if(!fpA){ 
		printf("No appointments found!\n"); 
		return; 
	}

    printf("\nPatient History for %s -------------------\n",p.name);
    while(fread(&a,sizeof(a),1,fpA)){
        if(a.patientID != patientID) continue;
        if(!findDoctorByID(a.doctorID,&d)) continue;
        float totalBill = computeTotalBill(a.appointmentID);
        printf("\nAppointmentID: %d | Doctor: %s | Date: %s | Time: %s | Status: %c | Diagnosis: %s | Total Bill: %.2f\n",
            a.appointmentID,d.name,a.appointmentDate,a.appointmentTime,a.status,
            strlen(a.diagnosis)?a.diagnosis:"N/A",totalBill);

        if(fpB) rewind(fpB);
        printf("Bill Items:\n");
        while(fread(&b,sizeof(b),1,fpB)){
            if(b.appointmentID == a.appointmentID){
                printf("  %d. %s, %.2f\n", b.billItemNo, b.itemName, b.amount);
            }
        }
    }
    if(fpA) fclose(fpA); 
	if(fpB) fclose(fpB);
}

// Main Menu -------------------------------------------------------------------
int main() {
    int choice;
    while(1){
        printf("\nHOSPITAL PATIENT MANAGEMENT SYSTEM\n");
        printf("1. Add Doctor\n");
        printf("2. Add Patient\n");
        printf("3. View Patients\n");
        printf("4. Create Appointment\n");
        printf("5. View All Appointments\n");
        printf("6. View Appointments by Patient\n");
        printf("7. View Appointments by Doctor\n");
        printf("8. Diagnose Patient & Add Bills\n");
        printf("9. View Patient History\n");
        printf("10. Update Bill Items\n");
        printf("11. Exit\n");
        printf("Enter your choice: "); 
        scanf("%d",&choice); getchar();

        switch(choice){
            case 1: addDoctor(); break;
            case 2: addPatient(); break;
            case 3: viewPatients(); break;
            case 4: createAppointment(); break;
            case 5: viewAllAppointments(); break;
            case 6: viewAppointmentsByPatient(); break;
            case 7: viewAppointmentsByDoctor(); break;
            case 8: diagnosePatient(); break;
            case 9: viewPatientHistory(); break;
            case 10: updateBillItem(); break;
            case 11: exit(0);
            default: printf("Invalid choice!\n");
        }
    }
    return 0;
}