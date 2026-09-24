#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
using namespace std;

struct Payment {
    string date;
    double amount;
    string reference;
};

struct FeeChallan {
    int studentId;
    string semester;
    double totalFee;
    double paidAmount;
    string dueDate;
    bool paid;
};

struct Student {
    int id;
    string name;
    string section;
};

class FeeManager {
private:
    vector<Student> students;
    vector<FeeChallan> challans;
    vector<Payment> payments;
    double semesterFee;

    Student* findStudent(int id) {
        for (auto& student : students) {
            if (student.id == id)
                return &student;
        }
        return nullptr;
    }

    FeeChallan* findChallan(int id, const string& semester) {
        for (auto& challan : challans) {
            if (challan.studentId == id && challan.semester == semester)
                return &challan;
        }
        return nullptr;
    }

    vector<Payment> getStudentPayments(int id) const {
        vector<Payment> result;

        for (const auto& payment : payments) {
            if (payment.reference.rfind(to_string(id) + "-", 0) == 0)
                result.push_back(payment);
        }

        sort(result.begin(), result.end(),
             [](const Payment& a, const Payment& b) {
                 return a.date > b.date;
             });

        return result;
    }

public:
    FeeManager() : semesterFee(85000.0) {
        students.push_back({101, "Ali", "BSE-2B"});
        students.push_back({102, "Ahmed", "BSE-2B"});
        students.push_back({103, "Hamza", "BSE-2B"});
        students.push_back({104, "Usman", "BSE-2B"});

        payments.push_back({"2026-09-15", 30000, "101-P001"});
        payments.push_back({"2026-09-10", 20000, "101-P002"});
        payments.push_back({"2026-09-18", 85000, "102-P001"});

        challans.push_back({101, "Fall 2026", semesterFee, 50000, "2026-10-10", false});
        challans.push_back({102, "Fall 2026", semesterFee, 85000, "2026-10-10", true});
        challans.push_back({103, "Fall 2026", semesterFee, 0, "2026-10-10", false});
        challans.push_back({104, "Fall 2026", semesterFee, 0, "2026-10-10", false});
    }

    void generateChallans(const string& semester, const string& dueDate) {
        int created = 0;
        int skipped = 0;

        for (const auto& student : students) {
            if (findChallan(student.id, semester) != nullptr) {
                skipped++;
                continue;
            }

            challans.push_back({
                student.id, semester, semesterFee, 0, dueDate, false
            });
            created++;
        }

        cout << "\nChallan generation completed.\n";
        cout << "New challans created: " << created << "\n";
        cout << "Duplicates skipped: " << skipped << "\n";
    }

    void viewChallan(int studentId, const string& semester) {
        Student* student = findStudent(studentId);
        FeeChallan* challan = findChallan(studentId, semester);

        if (student == nullptr) {
            cout << "Student not found.\n";
            return;
        }

        if (challan == nullptr) {
            cout << "No challan found for this semester.\n";
            return;
        }

        double outstanding =
            max(0.0, challan->totalFee - challan->paidAmount);

        string status = outstanding == 0 ? "PAID" : "UNPAID";

        cout << "\n========== FEE CHALLAN ==========\n";
        cout << "Student ID : " << student->id << "\n";
        cout << "Name       : " << student->name << "\n";
        cout << "Section    : " << student->section << "\n";
        cout << "Semester   : " << challan->semester << "\n";
        cout << "Total Fee  : " << fixed << setprecision(2)
             << challan->totalFee << "\n";
        cout << "Paid       : " << challan->paidAmount << "\n";
        cout << "Outstanding: " << outstanding << "\n";
        cout << "Due Date   : " << challan->dueDate << "\n";
        cout << "Status     : " << status << "\n";
        cout << "=================================\n";
    }

    void viewPaymentHistory(int studentId) {
        Student* student = findStudent(studentId);

        if (student == nullptr) {
            cout << "Student not found.\n";
            return;
        }

        vector<Payment> history = getStudentPayments(studentId);

        cout << "\n====== PAYMENT HISTORY ======\n";
        cout << "Student: " << student->name
             << " (" << student->id << ")\n";

        if (history.empty()) {
            cout << "No payment records found.\n";
            cout << "=============================\n";
            return;
        }

        for (const auto& payment : history) {
            cout << "Date: " << payment.date
                 << " | Amount: " << fixed << setprecision(2)
                 << payment.amount
                 << " | Reference: " << payment.reference << "\n";
        }

        cout << "=============================\n";
    }

    double calculateOutstandingBalance(int studentId,
                                       const string& semester) {
        FeeChallan* challan = findChallan(studentId, semester);

        if (challan == nullptr)
            return -1;

        return max(0.0, challan->totalFee - challan->paidAmount);
    }

    void showOutstandingBalance(int studentId,
                                const string& semester) {
        Student* student = findStudent(studentId);
        double balance =
            calculateOutstandingBalance(studentId, semester);

        if (student == nullptr) {
            cout << "Student not found.\n";
            return;
        }

        if (balance < 0) {
            cout << "No challan found for this semester.\n";
            return;
        }

        cout << "\n====== OUTSTANDING BALANCE ======\n";
        cout << "Student: " << student->name << "\n";
        cout << "Balance: " << fixed << setprecision(2)
             << balance << "\n";
        cout << "No Dues: " << (balance == 0 ? "YES" : "NO") << "\n";
        cout << "=================================\n";
    }

    void recordPartialPayment(int studentId,
                              const string& semester,
                              double amount,
                              const string& date) {
        FeeChallan* challan = findChallan(studentId, semester);

        if (challan == nullptr) {
            cout << "Challan not found.\n";
            return;
        }

        double outstanding =
            challan->totalFee - challan->paidAmount;

        if (amount <= 0) {
            cout << "Payment must be greater than zero.\n";
            return;
        }

        if (amount > outstanding) {
            cout << "Payment exceeds outstanding balance.\n";
            return;
        }

        challan->paidAmount += amount;
        challan->paid =
            (challan->paidAmount >= challan->totalFee);

        int paymentNumber = 1;

        for (const auto& payment : payments) {
            if (payment.reference.rfind(
                    to_string(studentId) + "-P", 0) == 0) {
                paymentNumber++;
            }
        }

        string reference =
            to_string(studentId) + "-P" + to_string(paymentNumber);

        payments.push_back({date, amount, reference});

        cout << "Payment recorded successfully.\n";
        cout << "Remaining balance: " << fixed << setprecision(2)
             << challan->totalFee - challan->paidAmount << "\n";
    }

    void listStudents() const {
        cout << "\n===== STUDENTS =====\n";

        for (const auto& student : students) {
            cout << student.id << " - "
                 << student.name << " - "
                 << student.section << "\n";
        }
    }

    void run() {
        int choice;

        do {
            cout << "\n========== UMS FEE MANAGEMENT ==========\n";
            cout << "1. List Students\n";
            cout << "2. Generate Semester Challans\n";
            cout << "3. View Fee Challan\n";
            cout << "4. View Payment History\n";
            cout << "5. View Outstanding Balance\n";
            cout << "6. Record Partial Payment\n";
            cout << "0. Exit\n";
            cout << "Choose an option: ";
            cin >> choice;

            if (choice == 1) {
                listStudents();
            }
            else if (choice == 2) {
                string semester;
                string dueDate;

                cout << "Enter semester: ";
                cin.ignore();
                getline(cin, semester);

                cout << "Enter due date (YYYY-MM-DD): ";
                getline(cin, dueDate);

                generateChallans(semester, dueDate);
            }
            else if (choice == 3) {
                int id;
                string semester;

                cout << "Enter student ID: ";
                cin >> id;

                cout << "Enter semester: ";
                cin.ignore();
                getline(cin, semester);

                viewChallan(id, semester);
            }
            else if (choice == 4) {
                int id;

                cout << "Enter student ID: ";
                cin >> id;

                viewPaymentHistory(id);
            }
            else if (choice == 5) {
                int id;
                string semester;

                cout << "Enter student ID: ";
                cin >> id;

                cout << "Enter semester: ";
                cin.ignore();
                getline(cin, semester);

                showOutstandingBalance(id, semester);
            }
            else if (choice == 6) {
                int id;
                string semester;
                double amount;
                string date;

                cout << "Enter student ID: ";
                cin >> id;

                cout << "Enter semester: ";
                cin.ignore();
                getline(cin, semester);

                cout << "Enter payment amount: ";
                cin >> amount;

                cout << "Enter payment date (YYYY-MM-DD): ";
                cin >> date;

                recordPartialPayment(id, semester, amount, date);
            }
            else if (choice != 0) {
                cout << "Invalid option.\n";
            }

        } while (choice != 0);
    }
};

int main() {
    FeeManager manager;
    manager.run();
    return 0;
}
