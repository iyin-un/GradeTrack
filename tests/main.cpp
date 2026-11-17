
#include <cassert>
#include <stdexcept>
#include "../src/calculations/CGPAEngine.h"

int main() {
    CGPAEngine calculator;
    Semester semester("First Semester", "2026/2027");
    semester.addCourse(Course("CSC 211", 3, Grade::A));
    semester.addCourse(Course("CSC 211", 2, Grade::C)); // repeated attempt
    semester.addCourse(Course("MTH 213", 2, Grade::B));
    calculator.addSemester(semester);

    // Editing changes only the selected entry, retaining the repeated attempt.
    calculator.updateCourseInSemester(0, 1, Course("CSC 211", 4, Grade::A));
    const auto &coursesAfterEdit = calculator.getSemesters()[0].getCourses();
    assert(coursesAfterEdit.size() == 3);
    assert(coursesAfterEdit[0].getCreditUnits() == 3);
    assert(coursesAfterEdit[0].getGrade() == Grade::A);
    assert(coursesAfterEdit[1].getCreditUnits() == 4);
    assert(coursesAfterEdit[1].getGrade() == Grade::A);

    // Deleting that entry leaves both other entries untouched.
    calculator.removeCourseFromSemester(0, 1);
    const auto &coursesAfterDelete = calculator.getSemesters()[0].getCourses();
    assert(coursesAfterDelete.size() == 2);
    assert(coursesAfterDelete[0].getCourseCode() == "CSC 211");
    assert(coursesAfterDelete[0].getCreditUnits() == 3);
    assert(coursesAfterDelete[1].getCourseCode() == "MTH 213");

    bool invalidIndexRejected = false;
    try { calculator.removeCourseFromSemester(0, 9); }
    catch (const std::out_of_range &) { invalidIndexRejected = true; }
    assert(invalidIndexRejected);
    return 0;
}
