# University Management System

C++ console implementation for the UMS Jira project.

## Sprint 1

Implemented in `main.cpp`:

- UMS-6 — View fee challan
- UMS-7 — View payment history
- UMS-8 — Generate semester fee challans
- UMS-10 — View outstanding balance
- UMS-25 — Prevent duplicate challans
- UMS-38 — Payment history view
- UMS-39 — Fetch payment history by student
- UMS-40 — Sort payments newest first
- UMS-41 — Calculate outstanding balance
- UMS-42 — Handle partial payment updates
- UMS-43 — Display no-dues indicator

## Sprint 2

Implemented in `sprint2.cpp`:

- UMS-12 — Exam datesheet publish/change notifications
- UMS-13 — Batch/section announcements with recipient count
- UMS-14 — Student notification history, newest-first, read/unread, pagination
- UMS-15 — Exam datesheet draft, publish, view, edit and unpublish

### Sprint 2 functionality

- Create exam datesheet entries as drafts
- Publish a datesheet
- Notify affected students automatically when published
- Edit a published exam entry
- Notify only students affected by the edited exam
- Unpublish a datesheet
- Send announcements to a selected batch/section
- Show announcement recipient count
- Store announcements in each recipient's notification history
- View notifications newest first
- Display read/unread status
- Paginate notification history

## Build

Sprint 1:
```bash
g++ -std=c++17 main.cpp -o ums
./ums
```

Sprint 2:
```bash
g++ -std=c++17 sprint2.cpp -o ums_sprint2
./ums_sprint2
```

Windows/MinGW:
```bash
g++ -std=c++17 sprint2.cpp -o ums_sprint2.exe
ums_sprint2.exe
```

The academic implementation uses in-memory data structures; no external database is required for these sprint tasks.
