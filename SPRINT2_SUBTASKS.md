# Sprint 2 Subtask Implementation

## UMS-29 — Build datesheet entry form
Implemented in `sprint2.cpp`.

The console form collects:
- Course
- Exam date
- Exam time
- Venue
- Affected student IDs

The entry is saved to the draft datesheet before publishing.

## UMS-30 — Add publish/unpublish toggle
Implemented in `ExaminationSystem`.

- Draft datesheet remains unpublished until the publish action.
- Publish copies the draft into the published datesheet.
- Unpublish clears the published datesheet.
- Published and draft views are separate.

## UMS-31 — Trigger notification on publish
Implemented in `ExaminationSystem::publish()`.

When publishing:
- Affected students are identified.
- Duplicate student IDs are removed from the recipient list.
- A notification is created for each affected student.
- The notification is available in notification history.

Build:
```bash
g++ -std=c++17 sprint2.cpp -o ums_sprint2
```
