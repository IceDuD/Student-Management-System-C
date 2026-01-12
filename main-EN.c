/*
  Project Name: Student Management System (SMS)
  Environment: Windows 10 / Dev C++
  Author: ICEDODO
  Description: 
  1. Persistence: Field-level binary storage (data.txt) ensuring high precision.
  2. Excel Integration: Support for CSV Export/Import with scientific notation fix.
  3. Core Management: Linked-list based CRUD operations (Insert, Modify, Delete).
  4. Search Engine: Supports exact ID matching and fuzzy Name search.
  5. Analytics: Descending score sort, entry time sort, and statistical reports.
*/

#include <stdio.h>
#include <stdlib.h>     // Memory management
#include <string.h>     // String operations
#include <time.h>       // Time formatting and timestamps
#include <conio.h>      // Non-buffered input: getch()
#include <windows.h>    // Windows console configuration

// ================= Macros & Structures =================
#define FILENAME "data.txt"                                                         
#define FORMAT "%-20s%-15s%-8.1lf%-8.1lf%-8.1lf%-8.1lf%-20s\n"                      
#define DATA p->num, p->name, p->chinese, p->math, p->english, p->sum, p->timestr   

/**
 * Struct: student (Alias: Student)
 * Purpose: Stores a complete student profile in a singly linked list node.
 * Features: Long Student ID support and entry order tracking via timestamps.
 */
struct student {
    char num[30];           // Student ID
    char name[15];          // Name
    double chinese;         // Chinese Score
    double math;            // Math Score 
    double english;         // English Score 
    double sum;             // Total Score
    char timestr[35];       // Formatted time string
    long timeStamp;         // Unix timestamp for sorting
    struct student *next;   // Pointer to the next student node
};
typedef struct student Student;

// Global Head Pointer
Student *head = NULL;

// ================= Function Prototypes =================
void menu();                                     
void readData();                                 
void saveData();                                 
void wait_for_enter();                           
void get_current_time(char *tstr, long *tstamp); 
void swap_data(Student *a, Student *b);          

// Core Logic
void input();                                    
void display();                                  
void delete_stu();                               
void insert();                                   
void modify();                                   
void statistics();                               

// Search Module
void search();                                   
void search_by_id();                             
void search_fuzzy();                             

// Sort Module
void sort();                                     
void sort_by_sum();                              
void sort_by_time();                             

// Extension Features
void exportToCSV();                              
void importFromCSV();                            

/**
 * Function: main
 * Description: Program entry point. Handles initialization, data loading, 
 * and the main control loop via a switch-case menu system.
 */
int main() {
    system("color 0B");     // Aesthetic: Cyan text on black background

    readData();             // Load data from file on startup

    int choice;             
    while (1) {
        menu();             
        printf("\nPlease select an option (0-10): ");
        if (scanf("%d", &choice) != 1) {    
            while(getchar() != '\n');       // Clear input buffer
            continue;                       
        }

        switch (choice) {
            case 1: input(); break;
            case 2: search(); break;
            case 3: delete_stu(); break;
            case 4: modify(); break;
            case 5: insert(); break;
            case 6: sort(); break;
            case 7: statistics(); break;
            case 8: display(); break;
            case 9: exportToCSV(); wait_for_enter(); break;
            case 10: importFromCSV(); wait_for_enter(); break;
            case 0: 
                saveData(); 
                printf("\n[Info] Data saved successfully. Exiting system.\n");
                return 0;
            default: 
                printf("\n[Error] Invalid option! Please try again.\n");
                wait_for_enter();
        }
    }
    return 0;
}

// ================= UI Module =================

void menu() {
    system("cls");
    printf("\n\n\t\t------------------- Student Management System -------------------\n");
    printf("\t\t\t\t1. Input Student Information\n");
    printf("\t\t\t\t2. Search Information (Sub-menu)\n");
    printf("\t\t\t\t3. Delete Student Record\n");
    printf("\t\t\t\t4. Modify Student Record\n");
    printf("\t\t\t\t5. Insert Student Record\n");
    printf("\t\t\t\t6. Sort Records (Sub-menu)\n");
    printf("\t\t\t\t7. Statistical Analysis\n");
    printf("\t\t\t\t8. Display All Records\n");
    printf("\t\t\t\t9. Export to Excel (CSV)\n");
    printf("\t\t\t\t10. Import from Excel (CSV)\n");
    printf("\t\t\t\t0. Save and Exit\n");
    printf("\t\t-----------------------------------------------------------------\n");
}

void wait_for_enter() {
    printf("\nPress any key to continue..."); 
    getch(); 
}

// ================= File Operations =================

void saveData() {
    FILE *fp = fopen(FILENAME, "wb");           
    if (!fp) return;                            
    
    Student *p = head;                          
    while (p) {                                 
        fwrite(p->num, 30, 1, fp);              
        fwrite(p->name, 15, 1, fp);             
        fwrite(&p->chinese, sizeof(double), 1, fp); 
        fwrite(&p->math, sizeof(double), 1, fp);    
        fwrite(&p->english, sizeof(double), 1, fp); 
        fwrite(&p->sum, sizeof(double), 1, fp);     
        fwrite(p->timestr, 35, 1, fp);              
        fwrite(&p->timeStamp, sizeof(long), 1, fp); 
        
        p = p->next;                            
    }
    fclose(fp);                                 
}

void readData() {
    FILE *fp = fopen(FILENAME, "rb");
    if (!fp) return; 

    while (1) { 
        Student *n = (Student *)malloc(sizeof(Student));
        
        if (fread(n->num, 30, 1, fp) != 1) { 
            free(n); 
            break; 
        }

        fread(n->name, 15, 1, fp);
        fread(&n->chinese, sizeof(double), 1, fp);
        fread(&n->math, sizeof(double), 1, fp);
        fread(&n->english, sizeof(double), 1, fp);
        fread(&n->sum, sizeof(double), 1, fp);
        fread(n->timestr, 35, 1, fp);
        fread(&n->timeStamp, sizeof(long), 1, fp);
        
        n->next = NULL; 

        if (!head) {
            head = n; 
        } else {
            Student *p = head;
            while (p->next) p = p->next; 
            p->next = n; 
        }
    }
    fclose(fp);
}

// ================= Core Logic =================

void input() {
    char ch[2];
    do {
        Student *n = (Student *)malloc(sizeof(Student));
        printf("\nEnter Student ID: "); scanf("%s", n->num);
        
        Student *p = head; int exist = 0;
        while(p) {
            if(strcmp(p->num, n->num) == 0) { exist = 1; break; }
            p = p->next;
        }

        if(exist) { 
            printf("[Error] Student ID already exists!\n"); 
            free(n);
        }
        else {
            printf("Enter Name: "); scanf("%s", n->name);
            
            do {
                printf("Chinese (0-150): "); scanf("%lf", &n->chinese);
                if (n->chinese < 0 || n->chinese > 150) 
                    printf("Invalid input! Score must be between 0 and 150.\n");
            } while (n->chinese < 0 || n->chinese > 150);

            do {
                printf("Math (0-150): "); scanf("%lf", &n->math);
                if (n->math < 0 || n->math > 150) 
                    printf("Invalid input! Score must be between 0 and 150.\n");
            } while (n->math < 0 || n->math > 150);

            do {
                printf("English (0-150): "); scanf("%lf", &n->english);
                if (n->english < 0 || n->english > 150) 
                    printf("Invalid input! Score must be between 0 and 150.\n");
            } while (n->english < 0 || n->english > 150);

            n->sum = n->chinese + n->math + n->english;
            get_current_time(n->timestr, &n->timeStamp);
            n->next = NULL; 

            if (head == NULL) {
                head = n;
            } else {
                Student *temp = head;
                while (temp->next != NULL) temp = temp->next;
                temp->next = n;
            }

            printf("Entry Successful!\n");
            saveData();
        }
        printf("Continue to add more? (y/n): "); scanf("%s", ch);
    } while (strcmp(ch, "y") == 0 || strcmp(ch, "Y") == 0);
}

void display() {
    system("cls");
    if (!head) { printf("\nNo student records found.\n"); wait_for_enter(); return; }
    
    printf("\n%-20s%-15s%-8s%-8s%-8s%-8s%-20s\n", "ID", "Name", "Chinese", "Math", "English", "Total", "Last Modified");
    printf("------------------------------------------------------------------------------------------\n");
    Student *p = head;
    while (p) { printf(FORMAT, DATA); p = p->next; } 
    wait_for_enter();
}

// ================= Search Module =================

void search() {
    int sel;
    system("cls");
    printf("\n======= Search Sub-menu =======\n");
    printf("1. Exact ID Match\n");
    printf("2. Fuzzy Name Search\n");
    printf("0. Return to Main Menu\n");
    printf("===============================\nSelect: ");
    scanf("%d", &sel);
    if (sel == 1) search_by_id();
    else if (sel == 2) search_fuzzy();
    else return;
    wait_for_enter();
}

void search_by_id() {
    char id[30];
    printf("Enter ID to search: ");
    scanf("%s", id);
    Student *p = head;
    while(p) {
        if(strcmp(p->num, id) == 0) {
            printf("\nMatch Found:\n%-20s%-15s%-8s%-8s%-8s%-8s\n", "ID","Name","CH","MA","EN","Total");
            printf("%-20s%-15s%-8.1f%-8.1f%-8.1f%-8.1f\n", p->num, p->name, p->chinese, p->math, p->english, p->sum);
            return;
        }
        p = p->next;
    }
    printf("No record found with ID: %s\n", id);
}

void search_fuzzy() {
    char key[15]; int count = 0;
    printf("Enter Name keyword: ");
    scanf("%s", key);
    printf("\nFuzzy Search Results:\n");
    Student *p = head;
    while(p) {
        if(strstr(p->name, key)) {
            printf(FORMAT, DATA);
            count++;
        }
        p = p->next;
    }
    if(count == 0) printf("No student found containing \"%s\".\n", key);
    else printf("Found %d record(s).\n", count);
}

// ================= Modify & Delete =================

void modify() {
    char id[30];
    printf("\nEnter Student ID to modify: "); scanf("%s", id);
    Student *p = head;
    while(p) {
        if(strcmp(p->num, id) == 0) {
            printf("Found Record: %s. Enter new details:\n", p->name);
            printf("New Name: "); scanf("%s", p->name);
            printf("New Chinese: "); scanf("%lf", &p->chinese);
            printf("New Math: "); scanf("%lf", &p->math);
            printf("New English: "); scanf("%lf", &p->english);
            p->sum = p->chinese + p->math + p->english;
            get_current_time(p->timestr, &p->timeStamp); 
            printf("Modification Successful!\n");
            saveData();
            wait_for_enter();
            return;
        }
        p = p->next;
    }
    printf("Student not found.\n");
    wait_for_enter();
}

void delete_stu() {
    char id[30];
    printf("\nEnter Student ID to delete: "); scanf("%s", id);
    Student *p = head, *pre = NULL;
    while(p) {
        if(strcmp(p->num, id) == 0) {
            printf("Are you sure you want to delete %s? (y/n): ", p->name);
            char cf[2]; scanf("%s", cf);
            if(strcmp(cf,"y")==0 || strcmp(cf,"Y")==0) { 
                if(!pre) head = p->next; 
                else pre->next = p->next;
                free(p);
                printf("Deletion Successful!\n");
                saveData(); 
            }
            wait_for_enter();
            return;
        }
        pre = p; p = p->next;
    }
    printf("Student ID not found.\n");
    wait_for_enter();
}

// ================= Sort Module =================

void sort() {
    int sel;
    system("cls");
    printf("\n======= Sort Sub-menu =======\n");
    printf("1. Sort by Total Score (Descending)\n");
    printf("2. Sort by Entry Time (Newest First)\n");
    printf("0. Return to Main Menu\n");
    printf("=============================\nSelect: ");
    scanf("%d", &sel);
    if(sel == 1) sort_by_sum();
    else if(sel == 2) sort_by_time();
    else return;
    display();
}

void sort_by_sum() {
    if(!head) return;
    Student *p, *q;
    for(p=head; p; p=p->next)
        for(q=p->next; q; q=q->next)
            if(p->sum < q->sum) swap_data(p, q);
    printf("Sort by total score completed!\n");
}

void sort_by_time() {
    if(!head) return;
    Student *p, *q;
    for(p=head; p; p=p->next)
        for(q=p->next; q; q=q->next)
            if(p->timeStamp < q->timeStamp)
                swap_data(p, q);
    printf("Sort by time completed!\n");
}

// ================= Stats & Utils =================

void statistics() {
    system("cls");
    if(!head) { printf("No data available for statistics.\n"); wait_for_enter(); return; }
    int count = 0; double total = 0, max_s = -1, min_s = 999;
    Student *p = head, *max_p = head, *min_p = head;
    while(p) {
        count++;
        total += p->sum;
        if(p->sum > max_s) { max_s = p->sum; max_p = p; }
        if(p->sum < min_s) { min_s = p->sum; min_p = p; }
        p = p->next;
    }
    printf("\n---------- Statistical Analysis Report ----------\n");
    printf("Total Students:   %d\n", count);
    printf("Average Score:    %.2lf\n", total / count);
    printf("Highest Score:    %.2lf (%s)\n", max_s, max_p->name);
    printf("Lowest Score:     %.2lf (%s)\n", min_s, min_p->name);
    printf("------------------------------------------------\n");
    wait_for_enter();
}

void swap_data(Student *a, Student *b) {
    Student temp;
    memcpy(&temp, a, sizeof(Student) - sizeof(Student*));
    memcpy(a, b, sizeof(Student) - sizeof(Student*));
    memcpy(b, &temp, sizeof(Student) - sizeof(Student*));
}

void get_current_time(char *tstr, long *tstamp) {
    time_t t; struct tm *lt; time(&t); lt = localtime(&t);
    *tstamp = (long)t; strftime(tstr, 35, "%Y-%m-%d %H:%M:%S", lt);
}

void insert() {
    if (head == NULL) {
        printf("\n[Info] No existing data. Please use Input function first.\n");
        wait_for_enter();
        return;
    }

    char targetID[30];
    printf("\nInsert after which Student ID: ");
    scanf("%s", targetID);

    Student *p = head;
    while (p != NULL && strcmp(p->num, targetID) != 0) {
        p = p->next;
    }

    if (p == NULL) {
        printf("[Error] ID %s not found. Insertion aborted.\n", targetID);
        wait_for_enter();
        return;
    }

    Student *newNode = (Student *)malloc(sizeof(Student));
    printf("\nTarget found. Enter new student details:\n");
    
    printf("New ID: "); scanf("%s", newNode->num);
    
    Student *check = head;
    while(check) {
        if(strcmp(check->num, newNode->num) == 0) {
            printf("[Error] ID %s already exists. Insertion aborted!\n", newNode->num);
            free(newNode);
            wait_for_enter();
            return;
        }
        check = check->next;
    }

    printf("Name: "); scanf("%s", newNode->name);
    printf("Chinese: "); scanf("%lf", &newNode->chinese);
    printf("Math: "); scanf("%lf", &newNode->math);
    printf("English: "); scanf("%lf", &newNode->english);
    
    newNode->sum = newNode->chinese + newNode->math + newNode->english;
    get_current_time(newNode->timestr, &newNode->timeStamp);

    newNode->next = p->next;
    p->next = newNode;

    printf("\n[Success] Student %s inserted after ID %s.\n", newNode->name, targetID);
    saveData(); 
    wait_for_enter();
}

// ================= CSV Data Exchange =================

void exportToCSV() {
    FILE *fp = fopen("students.csv", "w");
    if (fp == NULL) {
        printf("Export failed. File might be occupied by Excel.\n");
        return;
    }
    
    fprintf(fp, "sep=,\n"); 
    fprintf(fp, "ID,Name,Chinese,Math,English,Total,Timestamp\n");
    
    Student *p = head;
    while (p) {
        fprintf(fp, "=\"%s\",%s,%.1f,%.1f,%.1f,%.1f,%s\n",
                p->num, p->name, p->chinese, p->math, p->english, p->sum, p->timestr);
        p = p->next;
    }
    fclose(fp);
    printf("\n[Success] CSV exported. You can open 'students.csv' in Excel.\n");
}

void importFromCSV() {
    FILE *fp = fopen("students.csv", "r");
    if (fp == NULL) {
        printf("File 'students.csv' not found.\n");
        return;
    }

    printf("Importing will overwrite current data. Continue? (y/n): ");
    char cf[2]; scanf("%s", cf);
    if(strcmp(cf, "y") != 0 && strcmp(cf, "Y") != 0) { fclose(fp); return; }

    while(head) { Student *tmp = head; head = head->next; free(tmp); }

    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        if (strstr(line, "ID")) break;
    }

    int count = 0;
    while (fgets(line, sizeof(line), fp)) {
        if (strlen(line) < 5) continue;

        Student *n = (Student *)malloc(sizeof(Student));
        
        if (sscanf(line, "%*[^\"]\"%[^\"]\",%[^,],%lf,%lf,%lf,%lf,%[^,\n]", n->num, n->name, &n->chinese, &n->math, &n->english, &n->sum, n->timestr) >= 6) {
            n->timeStamp = (long)time(NULL); 
            n->next = head;
            head = n;
            count++;
        } else {
            if (sscanf(line, "%[^,],%[^,],%lf,%lf,%lf,%lf", n->num, n->name, &n->chinese, &n->math, &n->english, &n->sum) >= 6) {
                get_current_time(n->timestr, &n->timeStamp);
                n->next = head;
                head = n;
                count++;
            } else {
                free(n);
            }
        }
    }

    fclose(fp);
    saveData(); 
    printf("\n[Success] Imported %d records from CSV!\n", count);
}
