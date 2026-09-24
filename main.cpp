#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <ctime>
#include <fstream>
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
        if (date.size() != 10 || date[4] != '-' || date[7] != '-')
            return -1;

        for (int i = 0; i < 10; i++) {
            if (i == 4 || i == 7)
                continue;

            if (date[i] < '0' || date[i] > '9')
                return -1;
        }

        int year = stoi(date.substr(0, 4));
        int month = stoi(date.substr(5, 2));
        int day = stoi(date.substr(8, 2));

        if (year < 1 || month < 1 || month > 12 || day < 1)
            return -1;

        int daysInMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31};
        bool leapYear = (year % 400 == 0) ||
                        (year % 4 == 0 && year % 100 != 0);

        if (leapYear)
            daysInMonth[1] = 29;

        if (day > daysInMonth[month - 1])
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

        string numberText = to_string(number);
        if (numberText.size() < 3)
            numberText = string(3 - numberText.size(), '0') + numberText;

        return to_string(studentId) + "-P" + numberText;
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
        if (semester.empty()) {
            cout << "Semester cannot be empty.\n";
            return;
        }

        if (!isValidDate(dueDate)) {
            cout << "Invalid due date. Use YYYY-MM-DD.\n";
            return;
        }

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

        if (!isValidDate(date)) {
            cout << "Invalid payment date. Use YYYY-MM-DD.\n";
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


struct ExamEntry {
    string course;
    string date;
    string time;
    string venue;
    bool published;
};

struct ExamNotification {
    int studentId;
    string message;
};

struct ExamResult {
    int studentId;
    string course;
    double marks;
    double maximumMarks;
    bool finalized;
};

bool examsOverlap(const string& firstDate, const string& firstTime,
                  const string& secondDate, const string& secondTime) {
    if (firstDate != secondDate)
        return false;

    auto parseTimeValue = [](const string& value) {
        if (value.size() != 5 || value[2] != ':')
            return -1;
        int hour = (value[0] - '0') * 10 + (value[1] - '0');
        int minute = (value[3] - '0') * 10 + (value[4] - '0');
        if (hour < 0 || hour > 23 || minute < 0 || minute > 59)
            return -1;
        return hour * 60 + minute;
    };

    int firstMinutes = parseTimeValue(firstTime);
    int secondMinutes = parseTimeValue(secondTime);

    if (firstMinutes == -1 || secondMinutes == -1)
        return false;

    return firstMinutes < secondMinutes + 60 &&
           secondMinutes < firstMinutes + 60;
}

class ExamManager {
private:
    vector<Student> students;
    vector<ExamEntry> exams;
    vector<ExamNotification> notifications;
    vector<ExamResult> results;

    vector<string> getCourses(int studentId) const {
        if (studentId == 101)
            return {"OOP", "DS", "COAL"};
        if (studentId == 102)
            return {"OOP", "DS", "COAL"};
        if (studentId == 103)
            return {"OOP", "DS", "COAL"};
        if (studentId == 104)
            return {"OOP", "DS", "COAL"};
        return {};
    }

    bool isEnrolled(int studentId, const string& course) const {
        vector<string> courses = getCourses(studentId);

        for (const auto& item : courses) {
            if (item == course)
                return true;
        }

        return false;
    }

    Student* findStudent(int id) {
        for (auto& student : students) {
            if (student.id == id)
                return &student;
        }

        return nullptr;
    }

    int timeToMinutes(const string& value) const {
        if (value.size() != 5 || value[2] != '-')
            return -1;

        return -1;
    }

    int parseTime(const string& value) const {
        if (value.size() != 5 || value[2] != ':')
            return -1;

        int hour = (value[0] - '0') * 10 + (value[1] - '0');
        int minute = (value[3] - '0') * 10 + (value[4] - '0');

        if (hour < 0 || hour > 23 || minute < 0 || minute > 59)
            return -1;

        return hour * 60 + minute;
    }

    bool validDate(const string& value) const {
        if (value.size() != 10 || value[4] != '-' || value[7] != '-')
            return false;

        for (int i = 0; i < 10; i++) {
            if (i == 4 || i == 7)
                continue;

            if (value[i] < '0' || value[i] > '9')
                return false;
        }

        int year = stoi(value.substr(0, 4));
        int month = stoi(value.substr(5, 2));
        int day = stoi(value.substr(8, 2));

        if (year < 1 || month < 1 || month > 12 || day < 1)
            return false;

        int daysInMonth[] = {31, 28, 31, 30, 31, 30,
                             31, 31, 30, 31, 30, 31};

        bool leapYear = (year % 400 == 0) ||
                        (year % 4 == 0 && year % 100 != 0);

        if (leapYear)
            daysInMonth[1] = 29;

        return day <= daysInMonth[month - 1];
    }

    bool examsClash(const ExamEntry& first,
                    const ExamEntry& second) const {
        return examsOverlap(first.date, first.time,
                            second.date, second.time);
    }

    void notifyStudentsAboutExam(const ExamEntry& exam,
                                 bool updateNotice) {
        for (const auto& student : students) {
            if (!isEnrolled(student.id, exam.course))
                continue;

            string message = updateNotice
                ? "Exam update: " + exam.course + " is on " +
                  exam.date + " at " + exam.time +
                  " in " + exam.venue + "."
                : "Exam published: " + exam.course + " is on " +
                  exam.date + " at " + exam.time +
                  " in " + exam.venue + ".";

            notifications.push_back({student.id, message});
        }
    }

    void notifyControllerAboutClashes() {
        for (const auto& student : students) {
            vector<int> studentExams;

            for (int i = 0; i < static_cast<int>(exams.size()); i++) {
                if (!exams[i].published ||
                    !isEnrolled(student.id, exams[i].course))
                    continue;

                studentExams.push_back(i);
            }

            for (int i = 0; i < static_cast<int>(studentExams.size()); i++) {
                for (int j = i + 1;
                     j < static_cast<int>(studentExams.size()); j++) {

                    const ExamEntry& first = exams[studentExams[i]];
                    const ExamEntry& second = exams[studentExams[j]];

                    if (examsClash(first, second)) {
                        notifications.push_back({
                            0,
                            "CONTROLLER ALERT: Student " +
                            to_string(student.id) +
                            " has a clash between " +
                            first.course + " and " + second.course + "."
                        });
                    }
                }
            }
        }
    }

public:
    ExamManager() {
        students.push_back({101, "Ali", "BSE-2B"});
        students.push_back({102, "Ahmed", "BSE-2B"});
        students.push_back({103, "Hamza", "BSE-2B"});
        students.push_back({104, "Usman", "BSE-2B"});
    }

    void createDatesheet() {
        int count;

        cout << "\nHow many exam entries do you want to add? ";
        cin >> count;

        if (count <= 0) {
            cout << "Number of entries must be greater than zero.\n";
            return;
        }

        for (int i = 0; i < count; i++) {
            ExamEntry exam;

            cout << "\nExam " << i + 1 << "\n";
            cout << "Course: ";
            cin >> exam.course;

            cout << "Date (YYYY-MM-DD): ";
            cin >> exam.date;

            cout << "Time (HH:MM): ";
            cin >> exam.time;

            cout << "Venue: ";
            cin.ignore();
            getline(cin, exam.venue);

            if (!validDate(exam.date) || parseTime(exam.time) == -1) {
                cout << "Invalid date or time. Entry was not added.\n";
                i--;
                continue;
            }

            exam.published = false;
            exams.push_back(exam);
        }

        cout << "\nDatesheet saved as DRAFT.\n";
        cout << "It is not visible to students until published.\n";
    }

    void viewDatesheet(bool includeDrafts) const {
        cout << "\n========== EXAM DATESHEET ==========\n";

        bool found = false;

        for (const auto& exam : exams) {
            if (!includeDrafts && !exam.published)
                continue;

            cout << "Course: " << exam.course
                 << " | Date: " << exam.date
                 << " | Time: " << exam.time
                 << " | Venue: " << exam.venue
                 << " | Status: "
                 << (exam.published ? "PUBLISHED" : "DRAFT")
                 << "\n";

            found = true;
        }

        if (!found)
            cout << "No exam entries found.\n";

        cout << "=====================================\n";
    }

    void publishDatesheet() {
        bool publishedSomething = false;

        for (auto& exam : exams) {
            if (exam.published)
                continue;

            exam.published = true;
            publishedSomething = true;
            notifyStudentsAboutExam(exam, false);
        }

        if (!publishedSomething) {
            cout << "There are no draft exams to publish.\n";
            return;
        }

        notifyControllerAboutClashes();

        cout << "\nDatesheet published successfully.\n";
        cout << "Affected students were notified.\n";
        cout << "The system also checked for exam clashes.\n";
    }

    void unpublishDatesheet() {
        bool changed = false;

        for (auto& exam : exams) {
            if (!exam.published)
                continue;

            exam.published = false;
            changed = true;
        }

        if (!changed) {
            cout << "No published exams found.\n";
            return;
        }

        cout << "Datesheet unpublished. Controller can now edit the draft.\n";
    }

    void editExam() {
        string course;
        cout << "Enter course to edit: ";
        cin >> course;

        for (auto& exam : exams) {
            if (exam.course != course)
                continue;

            bool wasPublished = exam.published;

            cout << "New date (YYYY-MM-DD): ";
            cin >> exam.date;

            cout << "New time (HH:MM): ";
            cin >> exam.time;

            cout << "New venue: ";
            cin.ignore();
            getline(cin, exam.venue);

            if (!validDate(exam.date) || parseTime(exam.time) == -1) {
                cout << "Invalid date or time. Edit cancelled.\n";
                return;
            }

            exam.published = wasPublished;

            if (wasPublished) {
                notifyStudentsAboutExam(exam, true);
                notifyControllerAboutClashes();
                cout << "Published exam updated successfully.\n";
                cout << "Affected students were notified.\n";
            }
            else {
                cout << "Exam draft updated successfully.\n";
            }

            return;
        }

        cout << "Course not found in datesheet.\n";
    }

    void viewStudentSchedule(int studentId) {
        Student* student = findStudent(studentId);

        if (student == nullptr) {
            cout << "Student not found.\n";
            return;
        }

        vector<ExamEntry> schedule;

        for (const auto& exam : exams) {
            if (exam.published && isEnrolled(studentId, exam.course))
                schedule.push_back(exam);
        }

        sort(schedule.begin(), schedule.end(),
             [](const ExamEntry& a, const ExamEntry& b) {
                 if (a.date != b.date)
                     return a.date < b.date;
                 return a.time < b.time;
             });

        cout << "\n====== STUDENT EXAM SCHEDULE ======\n";
        cout << "Student: " << student->name
             << " (" << student->id << ")\n";

        if (schedule.empty()) {
            cout << "No published exams for this student.\n";
            cout << "===================================\n";
            return;
        }

        for (const auto& exam : schedule) {
            cout << exam.course
                 << " | " << exam.date
                 << " | " << exam.time
                 << " | " << exam.venue << "\n";
        }

        cout << "===================================\n";
    }

    void detectStudentClashes(int studentId) {
        Student* student = findStudent(studentId);

        if (student == nullptr) {
            cout << "Student not found.\n";
            return;
        }

        vector<ExamEntry> schedule;

        for (const auto& exam : exams) {
            if (exam.published && isEnrolled(studentId, exam.course))
                schedule.push_back(exam);
        }

        bool found = false;

        cout << "\n========== CLASH CHECK ==========\n";

        for (int i = 0; i < static_cast<int>(schedule.size()); i++) {
            for (int j = i + 1; j < static_cast<int>(schedule.size()); j++) {
                if (examsClash(schedule[i], schedule[j])) {
                    cout << "CLASH: " << schedule[i].course
                         << " and " << schedule[j].course
                         << " on " << schedule[i].date
                         << " around " << schedule[i].time << ".\n";
                    found = true;
                }
            }
        }

        if (!found)
            cout << "No exam clashes detected.\n";

        cout << "================================\n";
    }

    void enterResult() {
        int studentId;
        double marks;
        double maximumMarks;
        string course;

        cout << "Student ID: ";
        cin >> studentId;

        if (findStudent(studentId) == nullptr) {
            cout << "Student not found.\n";
            return;
        }

        cout << "Course: ";
        cin >> course;

        if (!isEnrolled(studentId, course)) {
            cout << "Student is not enrolled in this course.\n";
            return;
        }

        cout << "Maximum marks: ";
        cin >> maximumMarks;

        cout << "Obtained marks: ";
        cin >> marks;

        if (maximumMarks <= 0 || marks < 0 || marks > maximumMarks) {
            cout << "Invalid marks. Obtained marks must be between 0 and maximum marks.\n";
            return;
        }

        bool updated = false;

        for (auto& result : results) {
            if (result.studentId == studentId &&
                result.course == course &&
                !result.finalized) {

                result.marks = marks;
                result.maximumMarks = maximumMarks;
                updated = true;
                break;
            }
        }

        if (!updated) {
            results.push_back({
                studentId, course, marks, maximumMarks, false
            });
        }

        cout << "Result entered successfully as a draft.\n";
    }

    void uploadResultsFile() {
        string fileName;
        cout << "Enter results file name: ";
        cin >> fileName;

        ifstream file(fileName);

        if (!file) {
            cout << "Could not open results file.\n";
            cout << "Expected format: studentId,course,marks,maxMarks\n";
            return;
        }

        string line;
        int imported = 0;
        int rejected = 0;

        while (getline(file, line)) {
            if (line.empty())
                continue;

            stringstream stream(line);
            string studentText;
            string course;
            string marksText;
            string maximumText;

            getline(stream, studentText, ',');
            getline(stream, course, ',');
            getline(stream, marksText, ',');
            getline(stream, maximumText, ',');

            try {
                int studentId = stoi(studentText);
                double marks = stod(marksText);
                double maximumMarks = stod(maximumText);

                if (findStudent(studentId) == nullptr ||
                    !isEnrolled(studentId, course) ||
                    maximumMarks <= 0 ||
                    marks < 0 ||
                    marks > maximumMarks) {
                    rejected++;
                    continue;
                }

                bool updated = false;

                for (auto& result : results) {
                    if (result.studentId == studentId &&
                        result.course == course &&
                        !result.finalized) {
                        result.marks = marks;
                        result.maximumMarks = maximumMarks;
                        updated = true;
                        break;
                    }
                }

                if (!updated) {
                    results.push_back({
                        studentId, course, marks, maximumMarks, false
                    });
                }

                imported++;
            }
            catch (...) {
                rejected++;
            }
        }

        cout << "Results imported: " << imported << "\n";
        cout << "Invalid rows rejected: " << rejected << "\n";
    }

    void finalizeResults() {
        int finalizedCount = 0;

        for (auto& result : results) {
            if (!result.finalized) {
                result.finalized = true;
                finalizedCount++;
            }
        }

        cout << "Results finalized: " << finalizedCount << "\n";
        cout << "Finalized results are now visible to students.\n";
    }

    void correctResult() {
        int studentId;
        string course;

        cout << "Student ID: ";
        cin >> studentId;

        cout << "Course: ";
        cin >> course;

        for (auto& result : results) {
            if (result.studentId == studentId &&
                result.course == course) {

                if (result.finalized) {
                    cout << "Finalized result cannot be corrected through this draft option.\n";
                    return;
                }

                double newMarks;
                cout << "New marks: ";
                cin >> newMarks;

                if (newMarks < 0 ||
                    newMarks > result.maximumMarks) {
                    cout << "Invalid marks.\n";
                    return;
                }

                result.marks = newMarks;
                cout << "Result corrected successfully.\n";
                return;
            }
        }

        cout << "Draft result not found.\n";
    }

    void viewStudentResults(int studentId) const {
        const Student* student = nullptr;

        for (const auto& item : students) {
            if (item.id == studentId) {
                student = &item;
                break;
            }
        }

        if (student == nullptr) {
            cout << "Student not found.\n";
            return;
        }

        cout << "\n========== EXAM RESULTS ==========\n";
        cout << "Student: " << student->name
             << " (" << student->id << ")\n";

        bool found = false;

        for (const auto& result : results) {
            if (result.studentId != studentId || !result.finalized)
                continue;

            cout << result.course
                 << " | Marks: " << fixed << setprecision(2)
                 << result.marks << "/" << result.maximumMarks
                 << "\n";
            found = true;
        }

        if (!found)
            cout << "No finalized results are available.\n";

        cout << "==================================\n";
    }

    void viewNotifications(int studentId) const {
        cout << "\n====== EXAM NOTIFICATIONS ======\n";

        bool found = false;

        for (const auto& notification : notifications) {
            if (notification.studentId != studentId)
                continue;

            cout << "- " << notification.message << "\n";
            found = true;
        }

        if (!found)
            cout << "No notifications found.\n";

        cout << "================================\n";
    }

    void run() {
        int choice;

        do {
            cout << "\n========== UMS EXAM MANAGEMENT ==========\n";
            cout << "1. Create / Save Datesheet as Draft\n";
            cout << "2. View Draft / Published Datesheet\n";
            cout << "3. Publish Datesheet\n";
            cout << "4. Unpublish Datesheet\n";
            cout << "5. Edit Exam Entry\n";
            cout << "6. View Student Exam Schedule\n";
            cout << "7. Detect Student Exam Clashes\n";
            cout << "8. Enter Result Manually\n";
            cout << "9. Upload Results File\n";
            cout << "10. Correct Draft Result\n";
            cout << "11. Finalize Results\n";
            cout << "12. View Student Results\n";
            cout << "13. View Exam Notifications\n";
            cout << "0. Back\n";
            cout << "Choose an option: ";
            cin >> choice;

            if (choice == 1) {
                createDatesheet();
            }
            else if (choice == 2) {
                viewDatesheet(true);
            }
            else if (choice == 3) {
                publishDatesheet();
            }
            else if (choice == 4) {
                unpublishDatesheet();
            }
            else if (choice == 5) {
                editExam();
            }
            else if (choice == 6) {
                int id;
                cout << "Student ID: ";
                cin >> id;
                viewStudentSchedule(id);
            }
            else if (choice == 7) {
                int id;
                cout << "Student ID: ";
                cin >> id;
                detectStudentClashes(id);
            }
            else if (choice == 8) {
                enterResult();
            }
            else if (choice == 9) {
                uploadResultsFile();
            }
            else if (choice == 10) {
                correctResult();
            }
            else if (choice == 11) {
                finalizeResults();
            }
            else if (choice == 12) {
                int id;
                cout << "Student ID: ";
                cin >> id;
                viewStudentResults(id);
            }
            else if (choice == 13) {
                int id;
                cout << "Student ID: ";
                cin >> id;
                viewNotifications(id);
            }
            else if (choice != 0) {
                cout << "Invalid option.\n";
            }

        } while (choice != 0);
    }
};


int main() {
    FeeManager feeManager;
    ExamManager examManager;
    int choice;

    do {
        cout << "\n========== UNIVERSITY MANAGEMENT SYSTEM ==========\n";
        cout << "1. Fee Management\n";
        cout << "2. Exam Management\n";
        cout << "0. Exit\n";
        cout << "Choose an option: ";
        cin >> choice;

        if (choice == 1)
            feeManager.run();
        else if (choice == 2)
            examManager.run();
        else if (choice != 0)
            cout << "Invalid option.\n";

    } while (choice != 0);

    cout << "University Management System closed.\n";
    return 0;
}
