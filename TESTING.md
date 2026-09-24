## Sprint 1 Verification

- [x] C++17 compilation
- [x] Student lookup
- [x] Fee challan display
- [x] Semester challan generation
- [x] Duplicate challan prevention
- [x] Payment history retrieval
- [x] Newest-first payment sorting
- [x] Outstanding balance calculation
- [x] Partial payment validation
- [x] Partial payment balance update
- [x] No-dues indicator
- [x] Paid/unpaid status

## Sprint 2 Verification Checklist

### UMS-15 — Examination datesheet
- [x] Add exam entry with course, date, time and venue
- [x] Save entries as draft before publishing
- [x] Publish datesheet
- [x] View published datesheet
- [x] Edit published exam entry
- [x] Unpublish datesheet

### UMS-12 — Exam notifications
- [x] Publish event creates notifications for affected students
- [x] Edit event creates notifications for students affected by that exam
- [x] Notification identifies that the datesheet changed
- [x] Notifications are stored in the student's notification history

### UMS-13 — Section announcements
- [x] Select a batch/section
- [x] Deliver only to students in that section
- [x] Display recipient count
- [x] Store announcement in recipients' notification history

### UMS-14 — Notification history
- [x] Show fee/exam/announcement notifications in one list
- [x] Sort newest first
- [x] Show read/unread state
- [x] Paginate long notification histories
- [x] Mark a student's notifications as read

Build command:
```bash
g++ -std=c++17 sprint2.cpp -o ums_sprint2
```
