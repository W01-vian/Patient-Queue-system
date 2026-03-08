#include <iostream>
#include <string>
#include <queue>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>

using namespace std;

// ==================== DATA STRUCTURES ====================

// Patient structure
struct Patient {
    int id;
    string name;
    int age;
    string condition;
    int priority; // 1=Critical, 2=Urgent, 3=Normal
    string arrivalTime;
    bool treated;
    
    Patient(int i, string n, int a, string c, int p) 
        : id(i), name(n), age(a), condition(c), priority(p), treated(false) {
        time_t now = time(0);
        tm* ltm = localtime(&now);
        stringstream ss;
        ss << setfill('0') << setw(2) << ltm->tm_hour << ":" 
           << setw(2) << ltm->tm_min;
        arrivalTime = ss.str();
    }
};

// Comparator for priority queue (Min-Heap based on priority)
struct ComparePriority {
    bool operator()(const Patient& p1, const Patient& p2) {
        return p1.priority > p2.priority; // Lower priority value = higher priority
    }
};

// ==================== HOSPITAL QUEUE SYSTEM CLASS ====================

class HospitalQueueSystem {
private:
    // Data Structure 1: Priority Queue (Heap) for emergency patients
    priority_queue<Patient, vector<Patient>, ComparePriority> emergencyQueue;
    
    // Data Structure 2: Regular Queue (FIFO) for normal patients
    queue<Patient> normalQueue;
    
    // Data Structure 3: Dynamic Array (Vector) to store treated patients
    vector<Patient> treatedPatients;
    
    int patientCounter;
    
public:
    HospitalQueueSystem() : patientCounter(1000) {}
    
    // Add patient to appropriate queue
    void admitPatient(string name, int age, string condition, int priority) {
        Patient p(++patientCounter, name, age, condition, priority);
        
        if (priority == 1 || priority == 2) {
            // Critical or Urgent -> Priority Queue
            emergencyQueue.push(p);
            cout << "\n[EMERGENCY] Patient " << p.name << " (ID: " << p.id 
                 << ") admitted with priority " << priority << endl;
        } else {
            // Normal -> Regular Queue
            normalQueue.push(p);
            cout << "\n[NORMAL] Patient " << p.name << " (ID: " << p.id 
                 << ") admitted to normal queue" << endl;
        }
    }
    
    // Treat next patient (emergency first, then normal)
    void treatNextPatient() {
        Patient currentPatient(0, "", 0, "", 0);
        bool found = false;
        
        if (!emergencyQueue.empty()) {
            currentPatient = emergencyQueue.top();
            emergencyQueue.pop();
            found = true;
            cout << "\n[TREATING EMERGENCY PATIENT]" << endl;
        } else if (!normalQueue.empty()) {
            currentPatient = normalQueue.front();
            normalQueue.pop();
            found = true;
            cout << "\n[TREATING NORMAL PATIENT]" << endl;
        }
        
        if (found) {
            currentPatient.treated = true;
            treatedPatients.push_back(currentPatient);
            
            cout << "Patient ID: " << currentPatient.id << endl;
            cout << "Name: " << currentPatient.name << endl;
            cout << "Age: " << currentPatient.age << endl;
            cout << "Condition: " << currentPatient.condition << endl;
            cout << "Priority: " << getPriorityString(currentPatient.priority) << endl;
            cout << "Arrival Time: " << currentPatient.arrivalTime << endl;
        } else {
            cout << "\nNo patients in queue!" << endl;
        }
    }
    
    // Algorithm 1: Bubble Sort - Sort treated patients by ID
    void sortTreatedPatientsById() {
        int n = treatedPatients.size();
        for (int i = 0; i < n - 1; i++) {
            for (int j = 0; j < n - i - 1; j++) {
                if (treatedPatients[j].id > treatedPatients[j + 1].id) {
                    swap(treatedPatients[j], treatedPatients[j + 1]);
                }
            }
        }
        cout << "\nTreated patients sorted by ID (Bubble Sort)" << endl;
    }
    
    // Algorithm 2: Binary Search - Search treated patient by ID
    int searchTreatedPatient(int id) {
        // First sort the array
        sort(treatedPatients.begin(), treatedPatients.end(), 
             [](const Patient& a, const Patient& b) { return a.id < b.id; });
        
        int left = 0, right = treatedPatients.size() - 1;
        
        while (left <= right) {
            int mid = left + (right - left) / 2;
            
            if (treatedPatients[mid].id == id) {
                return mid;
            }
            
            if (treatedPatients[mid].id < id) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }
        return -1;
    }
    
    // Display all waiting patients
    void displayWaitingPatients() {
        cout << "\n========== WAITING PATIENTS ==========" << endl;
        
        // Display emergency queue
        if (!emergencyQueue.empty()) {
            cout << "\n--- EMERGENCY QUEUE ---" << endl;
            priority_queue<Patient, vector<Patient>, ComparePriority> tempQueue = emergencyQueue;
            int pos = 1;
            while (!tempQueue.empty()) {
                Patient p = tempQueue.top();
                tempQueue.pop();
                cout << pos++ << ". " << p.name << " (ID: " << p.id 
                     << ") - " << getPriorityString(p.priority) 
                     << " - " << p.condition << endl;
            }
        }
        
        // Display normal queue
        if (!normalQueue.empty()) {
            cout << "\n--- NORMAL QUEUE ---" << endl;
            queue<Patient> tempQueue = normalQueue;
            int pos = 1;
            while (!tempQueue.empty()) {
                Patient p = tempQueue.front();
                tempQueue.pop();
                cout << pos++ << ". " << p.name << " (ID: " << p.id 
                     << ") - " << p.condition << endl;
            }
        }
        
        if (emergencyQueue.empty() && normalQueue.empty()) {
            cout << "No patients waiting" << endl;
        }
    }
    
    // Display treated patients
    void displayTreatedPatients() {
        cout << "\n========== TREATED PATIENTS ==========" << endl;
        if (treatedPatients.empty()) {
            cout << "No patients treated yet" << endl;
            return;
        }
        
        for (size_t i = 0; i < treatedPatients.size(); i++) {
            Patient p = treatedPatients[i];
            cout << i + 1 << ". " << p.name << " (ID: " << p.id 
                 << ") - " << getPriorityString(p.priority) 
                 << " - " << p.condition 
                 << " - Arrival: " << p.arrivalTime << endl;
        }
    }
    
    // Export data to file
    void exportToFile(string filename) {
        ofstream file(filename);
        if (!file) {
            cout << "Error creating file!" << endl;
            return;
        }
        
        file << "HOSPITAL PATIENT QUEUE SYSTEM - REPORT\n";
        file << "======================================\n\n";
        
        file << "WAITING PATIENTS:\n";
        file << "-----------------\n";
        
        // Emergency queue
        priority_queue<Patient, vector<Patient>, ComparePriority> tempEQ = emergencyQueue;
        while (!tempEQ.empty()) {
            Patient p = tempEQ.top();
            tempEQ.pop();
            file << "ID: " << p.id << " | " << p.name << " | Priority: " 
                 << getPriorityString(p.priority) << " | " << p.condition << "\n";
        }
        
        // Normal queue
        queue<Patient> tempNQ = normalQueue;
        while (!tempNQ.empty()) {
            Patient p = tempNQ.front();
            tempNQ.pop();
            file << "ID: " << p.id << " | " << p.name << " | Priority: Normal | " 
                 << p.condition << "\n";
        }
        
        file << "\nTREATED PATIENTS:\n";
        file << "-----------------\n";
        for (const Patient& p : treatedPatients) {
            file << "ID: " << p.id << " | " << p.name << " | " 
                 << getPriorityString(p.priority) << " | " << p.condition 
                 << " | Arrival: " << p.arrivalTime << "\n";
        }
        
        file.close();
        cout << "\nData exported to " << filename << " successfully!" << endl;
    }
    
    // Get statistics
    void displayStatistics() {
        int emergency = 0, normal = 0;
        priority_queue<Patient, vector<Patient>, ComparePriority> tempEQ = emergencyQueue;
        while (!tempEQ.empty()) {
            emergency++;
            tempEQ.pop();
        }
        
        queue<Patient> tempNQ = normalQueue;
        while (!tempNQ.empty()) {
            normal++;
            tempNQ.pop();
        }
        
        cout << "\n========== STATISTICS ==========" << endl;
        cout << "Emergency Queue: " << emergency << " patients" << endl;
        cout << "Normal Queue: " << normal << " patients" << endl;
        cout << "Total Waiting: " << (emergency + normal) << " patients" << endl;
        cout << "Treated Patients: " << treatedPatients.size() << " patients" << endl;
        cout << "================================" << endl;
    }
    
private:
    string getPriorityString(int priority) {
        switch(priority) {
            case 1: return "Critical";
            case 2: return "Urgent";
            case 3: return "Normal";
            default: return "Unknown";
        }
    }
};

// ==================== MAIN MENU ====================

void displayMenu() {
    cout << "================================" << endl;
    cout << "   HOSPITAL PATIENT QUEUE SYSTEM        " << endl;      
	cout << "================================" << endl;
    cout << "1. Admit New Patient" << endl;
    cout << "2. Treat Next Patient" << endl;
    cout << "3. Display Waiting Patients" << endl;
    cout << "4. Display Treated Patients" << endl;
    cout << "5. Sort Treated Patients by ID" << endl;
    cout << "6. Search Treated Patient by ID" << endl;
    cout << "7. Display Statistics" << endl;
    cout << "8. Export Data to File" << endl;
    cout << "9. Exit" << endl;
    cout << "----------------------------------------" << endl;
    cout << "Enter your choice: ";
}

int main() {
    HospitalQueueSystem hospital;
    int choice;
    
    // Demo data
    cout << "Initializing system with sample patients...\n" << endl;
    hospital.admitPatient("John Doe", 45, "Chest Pain", 1);
    hospital.admitPatient("Jane Smith", 32, "Flu Symptoms", 3);
    hospital.admitPatient("Robert Brown", 67, "Severe Bleeding", 1);
    hospital.admitPatient("Emily Davis", 28, "Minor Cut", 3);
    hospital.admitPatient("Michael Wilson", 55, "Difficulty Breathing", 2);
    
    while (true) {
        displayMenu();
        cin >> choice;
        cin.ignore();
        
        switch (choice) {
            case 1: {
                string name, condition;
                int age, priority;
                
                cout << "\nEnter patient name: ";
                getline(cin, name);
                cout << "Enter age: ";
                cin >> age;
                cin.ignore();
                cout << "Enter condition: ";
                getline(cin, condition);
                cout << "Enter priority (1=Critical, 2=Urgent, 3=Normal): ";
                cin >> priority;
                cin.ignore();
                
                if (priority < 1 || priority > 3) {
                    cout << "Invalid priority! Using Normal (3)" << endl;
                    priority = 3;
                }
                
                hospital.admitPatient(name, age, condition, priority);
                break;
            }
            case 2:
                hospital.treatNextPatient();
                break;
            case 3:
                hospital.displayWaitingPatients();
                break;
            case 4:
                hospital.displayTreatedPatients();
                break;
            case 5:
                hospital.sortTreatedPatientsById();
                hospital.displayTreatedPatients();
                break;
            case 6: {
                int id;
                cout << "\nEnter patient ID to search: ";
                cin >> id;
                cin.ignore();
                
                int index = hospital.searchTreatedPatient(id);
                if (index != -1) {
                    cout << "Patient found at position " << (index + 1) << endl;
                } else {
                    cout << "Patient not found in treated records" << endl;
                }
                break;
            }
            case 7:
                hospital.displayStatistics();
                break;
            case 8:
                hospital.exportToFile("hospital_report.txt");
                break;
            case 9:
                cout << "\nThank you for using Hospital Queue System!" << endl;
                return 0;
            default:
                cout << "\nInvalid choice! Please try again." << endl;
        }
    }
    
    return 0;
}