#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

struct Student { int id; string name; string section; };
struct Notification {
    int studentId; string date, title, message; bool read;
};
struct Exam {
    string course, date, time, venue;
    vector<int> studentIds;
};

class NotificationSystem {
    vector<Notification> items;

public:
    void add(int id, const string& date, const string& title,
             const string& message) {
        items.push_back({id, date, title, message, false});
    }

    int sendToSection(const vector<Student>& students, const string& section,
                      const string& date, const string& title,
                      const string& message) {
        int count = 0;
        for (const auto& s : students) {
            if (s.section == section) {
                add(s.id, date, title, message);
                count++;
            }
        }
        return count;
    }

    void sendToStudents(const vector<int>& ids, const string& date,
                        const string& title, const string& message) {
        for (int id : ids)
            add(id, date, title, message);
    }

    void history(int studentId, int page) const {
        vector<Notification> list;
        for (const auto& n : items)
            if (n.studentId == studentId)
                list.push_back(n);

        sort(list.begin(), list.end(),
             [](const Notification& a, const Notification& b) {
                 return a.date > b.date;
             });

        const int pageSize = 5;
        int pages = max(1, (static_cast<int>(list.size()) + pageSize - 1) / pageSize);
        page = max(1, min(page, pages));

        int start = (page - 1) * pageSize;
        int finish = min(start + pageSize, static_cast<int>(list.size()));

        cout << "\n===== NOTIFICATION HISTORY =====\n";
        for (int i = start; i < finish; i++) {
            cout << (list[i].read ? "[READ] " : "[UNREAD] ")
                 << list[i].date << " | " << list[i].title << "\n";
            cout << list[i].message << "\n";
        }
        if (list.empty())
            cout << "No notifications found.\n";
        cout << "Page " << page << " of " << pages << "\n";
    }

    void markRead(int studentId) {
        for (auto& n : items)
            if (n.studentId == studentId)
                n.read = true;
    }
};

class ExaminationSystem {
    vector<Exam> draft, published;
    NotificationSystem& notifications;

    void print(const vector<Exam>& list) const {
        cout << "\n===== EXAM DATESHEET =====\n";
        if (list.empty()) {
            cout << "No entries.\n";
            return;
        }
        for (int i = 0; i < static_cast<int>(list.size()); i++) {
            cout << i << ". " << list[i].course
                 << " | " << list[i].date
                 << " | " << list[i].time
                 << " | " << list[i].venue << "\n";
        }
    }

public:
    ExaminationSystem(NotificationSystem& n) : notifications(n) {}

    void addExam(const string& course, const string& date,
                 const string& time, const string& venue,
                 const vector<int>& ids) {
        draft.push_back({course, date, time, venue, ids});
        cout << "Exam saved as draft.\n";
    }

    void showDraft() const { print(draft); }
    void showPublished() const { print(published); }

    void publish(const string& notificationDate) {
        if (draft.empty()) {
            cout << "Draft is empty.\n";
            return;
        }

        published = draft;
        vector<int> affected;

        for (const auto& exam : published) {
            for (int id : exam.studentIds) {
                if (find(affected.begin(), affected.end(), id) == affected.end())
                    affected.push_back(id);
            }
        }

        notifications.sendToStudents(
            affected, notificationDate,
            "Exam Datesheet Published",
            "Your exam datesheet has been published."
        );

        cout << "Datesheet published. Affected students notified: "
             << affected.size() << "\n";
    }

    void editPublished(int index, const string& course,
                       const string& date, const string& time,
                       const string& venue, const string& notificationDate) {
        if (index < 0 || index >= static_cast<int>(published.size())) {
            cout << "Invalid exam index.\n";
            return;
        }

        published[index].course = course;
        published[index].date = date;
        published[index].time = time;
        published[index].venue = venue;

        notifications.sendToStudents(
            published[index].studentIds, notificationDate,
            "Exam Datesheet Changed",
            "An exam entry was changed. Check the updated schedule."
        );

        cout << "Exam updated and affected students notified.\n";
    }

    void unpublish() {
        published.clear();
        cout << "Datesheet unpublished.\n";
    }
};

class AnnouncementSystem {
    NotificationSystem& notifications;

public:
    AnnouncementSystem(NotificationSystem& n) : notifications(n) {}

    void send(const vector<Student>& students, const string& section,
              const string& date, const string& title,
              const string& message) {
        int count = notifications.sendToSection(
            students, section, date, title, message
        );

        cout << "Announcement delivered to " << count
             << " students in " << section << ".\n";
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
            cout << "Course: "; cin >> course;
            cout << "Date: "; cin >> date;
            cout << "Time: "; cin >> time;
            cout << "Venue: "; cin >> venue;
            cout << "Number of affected students: "; cin >> count;

            vector<int> ids;
            for (int i = 0; i < count; i++) {
                int id;
                cout << "Student ID: "; cin >> id;
                ids.push_back(id);
            }
            exams.addExam(course, date, time, venue, ids);
        }
        else if (choice == 2) {
            exams.showDraft();
        }
        else if (choice == 3) {
            string date;
            cout << "Notification date: "; cin >> date;
            exams.publish(date);
        }
        else if (choice == 4) {
            exams.showPublished();
        }
        else if (choice == 5) {
            int index;
            string course, date, time, venue, notificationDate;

            exams.showPublished();
            cout << "Exam index: "; cin >> index;
            cout << "New course: "; cin >> course;
            cout << "New date: "; cin >> date;
            cout << "New time: "; cin >> time;
            cout << "New venue: "; cin >> venue;
            cout << "Notification date: "; cin >> notificationDate;

            exams.editPublished(index, course, date, time,
                                venue, notificationDate);
        }
        else if (choice == 6) {
            exams.unpublish();
        }
        else if (choice == 7) {
            string section, date, title, message;
            cout << "Batch/section: "; cin >> section;
            cout << "Date: "; cin >> date;
            cout << "Title: "; cin >> title;
            cout << "Message: "; cin >> message;

            announcements.send(
                students, section, date, title, message
            );
        }
        else if (choice == 8) {
            int id, page;
            cout << "Student ID: "; cin >> id;
            cout << "Page: "; cin >> page;
            notifications.history(id, page);
        }
        else if (choice == 9) {
            int id;
            cout << "Student ID: "; cin >> id;
            notifications.markRead(id);
            cout << "Notifications marked as read.\n";
        }
        else if (choice != 0) {
            cout << "Invalid option.\n";
        }

    } while (choice != 0);

    return 0;
}
