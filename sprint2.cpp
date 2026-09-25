#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

struct Student {
    int id;
    string name;
    string section;
};

struct Notification {
    int studentId;
    string date;
    string title;
    string message;
    bool read;
};

struct Exam {
    string course;
    string date;
    string time;
    string venue;
    vector<int> studentIds;
};

class NotificationSystem {
    vector<Notification> items;

    bool alreadySent(int studentId, const string& date,
                     const string& title,
                     const string& message) const {
        for (const auto& item : items) {
            if (item.studentId == studentId &&
                item.date == date &&
                item.title == title &&
                item.message == message) {
                return true;
            }
        }
        return false;
    }

public:
    void add(int id, const string& date, const string& title,
             const string& message) {
        if (!alreadySent(id, date, title, message))
            items.push_back({id, date, title, message, false});
    }

    int sendToSection(const vector<Student>& students, const string& section,
                      const string& date, const string& title,
                      const string& message) {
        int count = 0;

        for (const auto& student : students) {
            if (student.section == section) {
                add(student.id, date, title, message);
                count++;
            }
        }

        return count;
    }

    int sendToStudents(const vector<int>& ids, const string& date,
                       const string& title, const string& message) {
        int count = 0;

        for (int id : ids) {
            bool duplicate = false;

            for (const auto& item : items) {
                if (item.studentId == id &&
                    item.date == date &&
                    item.title == title &&
                    item.message == message) {
                    duplicate = true;
                    break;
                }
            }

            if (!duplicate) {
                add(id, date, title, message);
                count++;
            }
        }

        return count;
    }

    void history(int studentId, int page) const {
        vector<Notification> list;

        for (const auto& notification : items) {
            if (notification.studentId == studentId)
                list.push_back(notification);
        }

        sort(list.begin(), list.end(),
             [](const Notification& a, const Notification& b) {
                 if (a.date != b.date)
                     return a.date > b.date;
                 return a.title > b.title;
             });

        const int pageSize = 5;
        int pages = max(
            1,
            (static_cast<int>(list.size()) + pageSize - 1) / pageSize
        );

        page = max(1, min(page, pages));

        int start = (page - 1) * pageSize;
        int finish = min(
            start + pageSize,
            static_cast<int>(list.size())
        );

        cout << "\n===== NOTIFICATION HISTORY =====\n";

        for (int i = start; i < finish; i++) {
            cout << (list[i].read ? "[READ] " : "[UNREAD] ")
                 << list[i].date << " | "
                 << list[i].title << "\n";
            cout << list[i].message << "\n";
        }

        if (list.empty())
            cout << "No notifications found.\n";

        cout << "Page " << page << " of " << pages << "\n";
    }

    void markRead(int studentId) {
        for (auto& notification : items) {
            if (notification.studentId == studentId)
                notification.read = true;
        }
    }
};

class ExaminationSystem {
    vector<Exam> draft;
    vector<Exam> published;
    NotificationSystem& notifications;

    bool validDate(const string& value) const {
        if (value.size() != 10 ||
            value[4] != '-' ||
            value[7] != '-') {
            return false;
        }

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

        int days[] = {
            31, 28, 31, 30, 31, 30,
            31, 31, 30, 31, 30, 31
        };

        bool leap =
            (year % 400 == 0) ||
            (year % 4 == 0 && year % 100 != 0);

        if (leap)
            days[1] = 29;

        return day <= days[month - 1];
    }

    bool validTime(const string& value) const {
        if (value.size() != 5 || value[2] != ':')
            return false;

        for (int i = 0; i < 5; i++) {
            if (i == 2)
                continue;

            if (value[i] < '0' || value[i] > '9')
                return false;
        }

        int hour = stoi(value.substr(0, 2));
        int minute = stoi(value.substr(3, 2));

        return hour >= 0 && hour <= 23 &&
               minute >= 0 && minute <= 59;
    }

    bool validStudentIds(const vector<Student>& students,
                         const vector<int>& ids) const {
        for (int id : ids) {
            bool found = false;

            for (const auto& student : students) {
                if (student.id == id) {
                    found = true;
                    break;
                }
            }

            if (!found)
                return false;
        }

        return true;
    }

    vector<int> uniqueIds(const vector<int>& ids) const {
        vector<int> result;

        for (int id : ids) {
            if (find(result.begin(), result.end(), id) == result.end())
                result.push_back(id);
        }

        return result;
    }

    void print(const vector<Exam>& list) const {
        cout << "\n===== EXAM DATESHEET =====\n";

        if (list.empty()) {
            cout << "No entries.\n";
            return;
        }

        for (int i = 0; i < static_cast<int>(list.size()); i++) {
            cout << i << ". "
                 << list[i].course << " | "
                 << list[i].date << " | "
                 << list[i].time << " | "
                 << list[i].venue << "\n";
        }
    }

public:
    ExaminationSystem(NotificationSystem& n)
        : notifications(n) {}

    // UMS-29: Build and save a datesheet entry from the exam form.
    void addExam(const string& course, const string& date,
                 const string& time, const string& venue,
                 const vector<int>& ids,
                 const vector<Student>& students) {
        if (course.empty() ||
            venue.empty() ||
            !validDate(date) ||
            !validTime(time)) {
            cout << "Invalid course, venue, date, or time. "
                    "Exam was not saved.\n";
            return;
        }

        if (!validStudentIds(students, ids)) {
            cout << "One or more student IDs are invalid. "
                    "Exam was not saved.\n";
            return;
        }

        vector<int> cleanIds = uniqueIds(ids);

        draft.push_back({
            course, date, time, venue, cleanIds
        });

        cout << "Exam saved as draft.\n";
    }

    void showDraft() const {
        print(draft);
    }

    void showPublished() const {
        print(published);
    }

    // UMS-30: Publish the draft datesheet.
    void publish(const string& notificationDate) {
        if (draft.empty()) {
            cout << "Draft is empty.\n";
            return;
        }

        if (!validDate(notificationDate)) {
            cout << "Invalid notification date. "
                    "Use YYYY-MM-DD.\n";
            return;
        }

        published = draft;

        vector<int> affected;

        for (const auto& exam : published) {
            for (int id : exam.studentIds) {
                if (find(affected.begin(), affected.end(), id) ==
                    affected.end()) {
                    affected.push_back(id);
                }
            }
        }

        int notified = notifications.sendToStudents(
            affected,
            notificationDate,
            "Exam Datesheet Published",
            "Your exam datesheet has been published."
        );

        cout << "Datesheet published. "
             << "Affected students notified: "
             << notified << "\n";
    }

    void editPublished(int index, const string& course,
                       const string& date, const string& time,
                       const string& venue,
                       const string& notificationDate) {
        if (index < 0 ||
            index >= static_cast<int>(published.size())) {
            cout << "Invalid exam index.\n";
            return;
        }

        if (course.empty() ||
            venue.empty() ||
            !validDate(date) ||
            !validTime(time)) {
            cout << "Invalid course, venue, date, or time. "
                    "Exam was not updated.\n";
            return;
        }

        if (!validDate(notificationDate)) {
            cout << "Invalid notification date. "
                    "Use YYYY-MM-DD.\n";
            return;
        }

        vector<int> affectedStudents = published[index].studentIds;

        published[index].course = course;
        published[index].date = date;
        published[index].time = time;
        published[index].venue = venue;

        if (index < static_cast<int>(draft.size()))
            draft[index] = published[index];

        string message =
            "Exam " + course + " was changed to " +
            date + " at " + time + " in " + venue +
            ". Check the updated schedule.";

        int notified = notifications.sendToStudents(
            affectedStudents,
            notificationDate,
            "Exam Datesheet Changed",
            message
        );

        cout << "Exam updated. Affected students notified: "
             << notified << "\n";
    }

    // UMS-30: Return the datesheet to an unpublished state.
    void unpublish() {
        published.clear();
        cout << "Datesheet unpublished.\n";
    }
};

class AnnouncementSystem {
    NotificationSystem& notifications;

public:
    AnnouncementSystem(NotificationSystem& n)
        : notifications(n) {}

    void send(const vector<Student>& students,
              const string& section,
              const string& date,
              const string& title,
              const string& message) {
        if (section.empty() ||
            title.empty() ||
            message.empty()) {
            cout << "Section, title, and message cannot be empty.\n";
            return;
        }

        int count = notifications.sendToSection(
            students,
            section,
            date,
            title,
            message
        );

        cout << "Announcement delivered to "
             << count << " students in "
             << section << ".\n";
    }
};

int main() {
    vector<Student> students = {
        {101, "Ali", "BSE-2B"},
        {102, "Ahmed", "BSE-2B"},
        {103, "Hamza", "BSE-2B"},
        {104, "Usman", "BSE-2B"}
    };

    NotificationSystem notifications;
    ExaminationSystem exams(notifications);
    AnnouncementSystem announcements(notifications);

    int choice;

    do {
        cout << "\n========== UMS SPRINT 2 ==========\n";
        cout << "1. Add Exam To Draft\n";
        cout << "2. View Draft Datesheet\n";
        cout << "3. Publish Datesheet\n";
        cout << "4. View Published Datesheet\n";
        cout << "5. Edit Published Exam\n";
        cout << "6. Unpublish Datesheet\n";
        cout << "7. Send Section Announcement\n";
        cout << "8. View Notification History\n";
        cout << "9. Mark Notifications Read\n";
        cout << "0. Exit\n";
        cout << "Choose: ";
        cin >> choice;

        if (choice == 1) {
            string course, date, time, venue;
            int count;

            cout << "Course: ";
            cin >> course;

            cout << "Date: ";
            cin >> date;

            cout << "Time: ";
            cin >> time;

            cout << "Venue: ";
            cin >> venue;

            cout << "Number of affected students: ";
            cin >> count;

            if (count < 0) {
                cout << "Number of students cannot be negative.\n";
                continue;
            }

            vector<int> ids;

            for (int i = 0; i < count; i++) {
                int id;

                cout << "Student ID: ";
                cin >> id;

                ids.push_back(id);
            }

            exams.addExam(
                course,
                date,
                time,
                venue,
                ids,
                students
            );
        }
        else if (choice == 2) {
            exams.showDraft();
        }
        else if (choice == 3) {
            string date;

            cout << "Notification date: ";
            cin >> date;

            exams.publish(date);
        }
        else if (choice == 4) {
            exams.showPublished();
        }
        else if (choice == 5) {
            int index;
            string course, date, time, venue, notificationDate;

            exams.showPublished();

            cout << "Exam index: ";
            cin >> index;

            cout << "New course: ";
            cin >> course;

            cout << "New date: ";
            cin >> date;

            cout << "New time: ";
            cin >> time;

            cout << "New venue: ";
            cin >> venue;

            cout << "Notification date: ";
            cin >> notificationDate;

            exams.editPublished(
                index,
                course,
                date,
                time,
                venue,
                notificationDate
            );
        }
        else if (choice == 6) {
            exams.unpublish();
        }
        else if (choice == 7) {
            string section, date, title, message;

            cout << "Batch/section: ";
            cin >> section;

            cout << "Date: ";
            cin >> date;

            cin.ignore();

            cout << "Title: ";
            getline(cin, title);

            cout << "Message: ";
            getline(cin, message);

            announcements.send(
                students,
                section,
                date,
                title,
                message
            );
        }
        else if (choice == 8) {
            int id, page;

            cout << "Student ID: ";
            cin >> id;

            cout << "Page: ";
            cin >> page;

            notifications.history(id, page);
        }
        else if (choice == 9) {
            int id;

            cout << "Student ID: ";
            cin >> id;

            notifications.markRead(id);

            cout << "Notifications marked as read.\n";
        }
        else if (choice != 0) {
            cout << "Invalid option.\n";
        }

    } while (choice != 0);

    return 0;
}
