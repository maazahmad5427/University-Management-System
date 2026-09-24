# University Management System — Sprint 1

C++ console implementation for the Fee Management work assigned to Maaz Ahmad in Jira project UMS.

## Jira Work Implemented

- UMS-6 — View fee challan
- UMS-7 — View payment history
- UMS-8 — Generate semester fee challans
- UMS-10 — View outstanding balance
- UMS-25 — Prevent duplicate challans
- UMS-38 — Design payment history view
- UMS-39 — Fetch payment history by student
- UMS-40 — Sort payments newest first
- UMS-41 — Calculate outstanding balance
- UMS-42 — Handle partial payment updates
- UMS-43 — Display no-dues indicator

## Functionality

- Student lookup
- Fee challan display
- Semester challan generation
- Duplicate challan prevention
- Payment history retrieval
- Newest-first payment sorting
- Outstanding balance calculation
- Partial payment validation and update
- Paid/unpaid status
- No-dues indicator
- Console menu for all Sprint 1 features

## Build

```bash
g++ -std=c++17 main.cpp -o ums
./ums
```

On Windows with MinGW:

```bash
g++ -std=c++17 main.cpp -o ums.exe
ums.exe
```

## Test Scenario

Student 101 starts with a semester fee of 85000 and paid amount of 50000. Recording a valid 35000 payment changes the paid amount to 85000, outstanding balance to 0, and no-dues status to YES.

The application keeps data in memory for this Sprint 1 academic implementation.
