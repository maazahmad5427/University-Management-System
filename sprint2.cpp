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
    vector<int> students;
};

class NotificationSystem {
    vector<Notification> notifications;

public:
    void notify(int studentId, string date, string title, string message) {
        notifications.push_back({studentId, date, title, message, false});
    }

    int notifySection(const vector<Student>& students, string section,
                      string date, string title, string message) {
        int count = 0;
        for (const auto& student : students) {
            if (student.section == section) {
                notify(student.id, date, title, message);
                count++;
            }
        }
        return count;
    }

    void notifyStudents(const vector<int>& ids, string date,
                        string title, string message) {
        for (int id : ids)
            notify(id, date, title, message);
    }

    void show(int studentId, int page) {
        vector<Notification> list;

        for (const auto& n : notifications)
            if (n.studentId == studentId)
                list.push_back(n);

        sort(list.begin(), list.end(),
             [](const Notification& a, const Notification& b) {
                 return a.date > b.date;
             });

        const int pageSize = 5;
        int pages = max(1, (static_cast<int>(list.size()) + pageSize - 1) / pageSize);

        if (page < 1) page = 1;
        if (page > pages) page = pages;

        int start = (page - 1) * pageSize;
        int end = min(start + pageSize, static_cast<int>(list.size()));

        cout << "\n===== NOTIFICATION HISTORY =====\n";

        if (list.empty()) {
            cout << "No notifications.\n";
        }

        for (int i = start; i < end; i++) {
            cout << (list[i].read ? "[READ] " : "[UNREAD] ")
                 << list[i].date << " | " << list[i].title << "\n";
            cout << list[i].message << "\n";
        }

        cout << "Page " << page << " of " << pages << "\n";
    }

    void markRead(int studentId) {
        for (auto& n : notifications)
            if (n.studentId == studentId)
                n.read = true;
    }
};

class ExaminationSystem {
    vector<Exam> draft;
    vector<Exam> published;
    NotificationSystem& notifications;

public:
    ExaminationSystem(NotificationSystem& system) : notifications(system) {}

    void addExam(string course, string date, string time, string venue,
                 vector<int> studentIds) {
        draft.push_back({course, date, time, venue, studentIds});
        cout << "Exam saved as draft.\n";
    }

    void show(const vector<Exam>& list) {
        cout << "\n===== EXAM DATESHEET =====\n";

        if (list.empty()) {
            cout << "No exam entries.\n";
            return;
        }

        for (int i = 0; i < static_cast<int>(list.size()); i++) {
            cout << i << ". " << list[i].course
                 << " | " << list[i].date
                 << " | " << list[i].time
                 << " | " << list[i].venue << "\n";
        }
    }

    void publish(string notificationDate) {
        if (draft.empty()) {
            cout << "Draft is empty.\n";
            return;
        }

        published = draft;
        vector<int> affected;

        for (const auto& exam : published) {
            for (int id : exam.students) {
                if (find(affected.begin(), affected.end(), id) == affected.end())
                    affected.push_back(id);
            }
        }

        notifications.notifyStudents(
            affected, notificationDate,
            "Exam Datesheet Published",
            "Your exam datesheet is now published."
        );

        cout << "Datesheet published. Students notified: "
             << affected.size() << "\n";
    }

    void editPublished(int index, string course, string date,
                       string time, string venue, string notificationDate) {
        if (index < 0 || index >= static_cast<int>(published.size())) {
            cout << "Invalid exam index.\n";
            return;
        }

        published[index].course = course;
        published[index].date = date;
        published[index].time = time;
        published[index].venue = venue;

        notifications.notifyStudents(
            published[index].students, notificationDate,
            "Exam Datesheet Changed",
            "An exam in your datesheet was changed. Check the updated schedule."
        );

        cout << "Exam updated and affected students notified.\n";
    }

    void unpublish() {
        published.clear();
        cout << "Datesheet unpublished.\n";
    }

    void checkClashes() {
        bool found = false;

        for (int i = 0; i < static_cast<int>(published.size()); i++) {
            for (int j = i + 1; j < static_cast<int>(published.size()); j++) {
                if (published[i].date == published[j].date &&
                    published[i].time == published[j].time) {

                    for (int id : published[i].students) {
                        if (find(published[j].students.begin(),
                                 published[j].students.end(), id)
                            != published[j].students.end()) {

                            cout << "CLASH: " << published[i].course
                                 << " and " << published[j].course
                                 << " for student " << id << "\n";
                            found = true;
                        }
                    }
                }
            }
        }

        if (!found)
            cout << "No exam clashes detected.\n";
    }
};

class AnnouncementSystem {
    NotificationSystem& notifications;

public:
    AnnouncementSystem(NotificationSystem& system)
        : notifications(system) {}

    void send(const vector<Student>& students, string section,
              string date, string title, string message) {
        int count = notifications.notifySection(
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
        cout << "1. Add Exam Draft\n";
        cout << "2. View Draft Datesheet\n";
        cout << "3. Publish Datesheet\n";
        cout << "4. View Published Datesheet\n";
        cout << "5. Edit Published Exam\n";
        cout << "6. Unpublish Datesheet\n";
        cout << "7. Detect Exam Clashes\n";
        cout << "8. Send Section Announcement\n";
        cout << "9. View Notification History\n";
        cout << "10. Mark Notifications Read\n";
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
            cout << "Number of students: "; cin >> count;

            vector<int> ids;
            for (int i = 0; i < count; i++) {
                int id;
                cout << "Student ID: ";
                cin >> id;
                ids.push_back(id);
            }

            exams.addExam(course, date, time, venue, ids);
        }
        else if (choice == 2) {
            exams.show(vector<Exam>());
        }
        else if (choice == 3) {
            string date;
            cout << "Notification date: ";
            cin >> date;
            exams.publish(date);
        }
        else if (choice == 4) {
            exams.show(vector<Exam>());
            cout << "Use option 3 to publish the current draft.\n";
        }
        else if (choice == 5) {
            int index;
            string course, date, time, venue, notificationDate;

            cout << "Exam index: "; cin >> index;
            cout << "New course: "; cin >> course;
            cout << "New date: "; cin >> date;
            cout << "New time: "; cin >> time;
            cout << "New venue: "; cin >> venue;
            cout << "Notification date: "; cin >> notificationDate;

            exams.editPublished(index, course, date, time, venue, notificationDate);
        }
        else if (choice == 6) {
            exams.unpublish();
        }
        else if (choice == 7) {
            exams.checkClashes();
        }
        else if (choice == 8) {
            string section, date, title, message;

            cout << "Batch/section: "; cin >> section;
            cout << "Date: "; cin >> date;
            cout << "Title: "; cin >> title;
            cout << "Message: "; cin >> message;

            announcements.send(students, section, date, title, message);
        }
        else if (choice == 9) {
            int id, page;
            cout << "Student ID: "; cin >> id;
            cout << "Page: "; cin >> page;
            notifications.show(id, page);
        }
        else if (choice == 10) {
            int id;
            cout << "Student ID: "; cin >> id;
            notifications.markRead(id);
            cout << "Notifications marked as read.\n";
        }

    } while (choice != 0);

    return 0;
}
