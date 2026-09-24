#include <cassert>
#include <iostream>
#include <string>
using namespace std;

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

int main() {
    assert(examsOverlap("2026-10-10", "09:00", "2026-10-10", "09:30"));
    assert(examsOverlap("2026-10-10", "09:30", "2026-10-10", "09:00"));
    assert(!examsOverlap("2026-10-10", "09:00", "2026-10-10", "10:00"));
    assert(!examsOverlap("2026-10-10", "09:00", "2026-10-11", "09:30"));
    assert(!examsOverlap("2026-10-10", "25:00", "2026-10-10", "09:30"));

    cout << "All exam overlap tests passed.\n";
    return 0;
}
