#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <ctime>
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

struct Notification {
    int studentId;
    string message;
    string dueDate;
};

struct AuditEntry {
    string action;
    int studentId;
    string officer;
    double amount;
    string date;
};

class FeeManager {
private:
    vector<Student> students;
    vector<FeeChallan> challans;
    vector<Payment> payments;
    vector<Notification> notifications;
    vector<AuditEntry> auditLog;
    double semesterFee;
    int notificationLeadDays;

    int dateToSerial(const string& date) const {
        int year, month, day;
        char first, second;

        if (date.size() != 10)
            return -1;

        stringstream stream(date);
        stream >> year >> first >> month >> second >> day;

        if (!stream || first != '-' || second != '-')
            return -1;

        tm value = {};
        value.tm_year = year - 1900;
        value.tm_mon = month - 1;
        value.tm_mday = day;
        value.tm_hour = 12;

        time_t timestamp = mktime(&value);
        if (timestamp == -1)
            return -1;

        return static_cast<int>(timestamp / 86400);
    }

    bool isValidDate(const string& date) const {
        return dateToSerial(date) != -1;
    }

    int daysUntil(const string& fromDate, const string& dueDate) const {
        int from = dateToSerial(fromDate);
        int due = dateToSerial(dueDate);

        if (from == -1 || due == -1)
            return -1;

        return due - from;
    }

    string nextPaymentReference(int studentId) const {
        int number = 1;

        for (const auto& payment : payments) {
            if (payment.reference.rfind(
                    to_string(studentId) + "-P", 0) == 0) {
                number++;
            }
        }

        return to_string(studentId) + "-P" + to_string(number);
    }

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
    FeeManager() : semesterFee(85000.0), notificationLeadDays(7) {
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

        string reference = nextPaymentReference(studentId);

        payments.push_back({date, amount, reference});

        cout << "Payment recorded successfully.\n";
        cout << "Remaining balance: " << fixed << setprecision(2)
             << challan->totalFee - challan->paidAmount << "\n";
    }

    void markFeePaid(int studentId,
                     const string& semester,
                     const string& paymentDate,
                     const string& officer) {
        Student* student = findStudent(studentId);
        FeeChallan* challan = findChallan(studentId, semester);

        if (student == nullptr) {
            cout << "Student not found.\n";
            return;
        }

        if (challan == nullptr) {
            cout << "Challan not found.\n";
            return;
        }

        double outstanding =
            max(0.0, challan->totalFee - challan->paidAmount);

        if (outstanding == 0) {
            cout << "Fee is already fully paid.\n";
            return;
        }

        if (!isValidDate(paymentDate)) {
            cout << "Invalid payment date. Use YYYY-MM-DD.\n";
            return;
        }

        if (officer.empty()) {
            cout << "Officer name cannot be empty.\n";
            return;
        }

        challan->paidAmount = challan->totalFee;
        challan->paid = true;

        payments.push_back({
            paymentDate,
            outstanding,
            nextPaymentReference(studentId)
        });

        auditLog.push_back({
            "FEE_MARKED_PAID",
            studentId,
            officer,
            outstanding,
            paymentDate
        });

        cout << "Fee marked as paid successfully.\n";
        cout << "Student: " << student->name << "\n";
        cout << "Amount recorded: " << fixed << setprecision(2)
             << outstanding << "\n";
        cout << "Outstanding balance: 0.00\n";
        cout << "Verified by: " << officer << "\n";
    }

    void generateDueNotifications(const string& currentDate) {
        if (!isValidDate(currentDate)) {
            cout << "Invalid current date. Use YYYY-MM-DD.\n";
            return;
        }

        int created = 0;

        for (const auto& challan : challans) {
            if (challan.paid)
                continue;

            int daysRemaining = daysUntil(currentDate, challan.dueDate);

            if (daysRemaining < 0 ||
                daysRemaining > notificationLeadDays) {
                continue;
            }

            bool alreadySent = false;

            for (const auto& notification : notifications) {
                if (notification.studentId == challan.studentId &&
                    notification.dueDate == challan.dueDate) {
                    alreadySent = true;
                    break;
                }
            }

            if (alreadySent)
                continue;

            Student* student = findStudent(challan.studentId);
            if (student == nullptr)
                continue;

            notifications.push_back({
                challan.studentId,
                "Fee of " +
                to_string(static_cast<int>(
                    challan.totalFee - challan.paidAmount)) +
                " is due on " + challan.dueDate,
                challan.dueDate
            });

            created++;
        }

        cout << "\nDue-date notification check completed.\n";
        cout << "Notifications created: " << created << "\n";
        cout << "Reminder window: " << notificationLeadDays
             << " day(s)\n";
    }

    void configureNotificationDays() {
        int days;

        cout << "Enter number of days before due date: ";
        cin >> days;

        if (days < 0) {
            cout << "Number of days cannot be negative.\n";
            return;
        }

        notificationLeadDays = days;
        cout << "Notification window set to "
             << notificationLeadDays << " day(s).\n";
    }

    void viewNotifications(int studentId) const {
        Student* student = const_cast<FeeManager*>(this)->findStudent(studentId);

        if (student == nullptr) {
            cout << "Student not found.\n";
            return;
        }

        cout << "\n====== NOTIFICATIONS ======\n";
        cout << "Student: " << student->name << "\n";

        bool found = false;

        for (const auto& notification : notifications) {
            if (notification.studentId != studentId)
                continue;

            cout << "- " << notification.message << "\n";
            found = true;
        }

        if (!found)
            cout << "No notifications found.\n";

        cout << "============================\n";
    }

    void viewAuditLog() const {
        cout << "\n========== AUDIT LOG ==========\n";

        if (auditLog.empty()) {
            cout << "No audit entries found.\n";
            cout << "===============================\n";
            return;
        }

        for (const auto& entry : auditLog) {
            cout << "Action: " << entry.action
                 << " | Student: " << entry.studentId
                 << " | Officer: " << entry.officer
                 << " | Amount: " << fixed << setprecision(2)
                 << entry.amount
                 << " | Date: " << entry.date << "\n";
        }

        cout << "===============================\n";
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
            cout << "7. Mark Fee as Paid\n";
            cout << "8. Configure Notification Window\n";
            cout << "9. Generate Due-Date Notifications\n";
            cout << "10. View Notifications\n";
            cout << "11. View Audit Log\n";
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
            else if (choice == 7) {
                int id;
                string semester;
                string paymentDate;
                string officer;

                cout << "Enter student ID: ";
                cin >> id;

                cout << "Enter semester: ";
                cin.ignore();
                getline(cin, semester);

                cout << "Enter payment date (YYYY-MM-DD): ";
                getline(cin, paymentDate);

                cout << "Enter verifying officer: ";
                getline(cin, officer);

                markFeePaid(id, semester, paymentDate, officer);
            }
            else if (choice == 8) {
                configureNotificationDays();
            }
            else if (choice == 9) {
                string currentDate;

                cout << "Enter current date (YYYY-MM-DD): ";
                cin >> currentDate;

                generateDueNotifications(currentDate);
            }
            else if (choice == 10) {
                int id;

                cout << "Enter student ID: ";
                cin >> id;

                viewNotifications(id);
            }
            else if (choice == 11) {
                viewAuditLog();
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
