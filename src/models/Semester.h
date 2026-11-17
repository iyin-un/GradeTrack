
#ifndef SEMESTER_H
#define SEMESTER_H

#include <string>
#include <vector>
#include <stdexcept>
#include "Course.h"

class Semester {
private:
    std::string semesterName;
    std::string academicSession;
    std::vector<Course> courses;

public:
    // Constructor
    Semester(const std::string& name, const std::string& session)
        : semesterName(name), academicSession(session) {

        if (name.empty()) {
            throw std::invalid_argument("Semester name cannot be empty.");
        }

        if (session.empty()) {
            throw std::invalid_argument("Academic session cannot be empty.");
        }
    }

    // Add a course
    void addCourse(const Course& course) {
        courses.push_back(course);
    }

    void updateCourse(std::size_t courseIndex, const Course& course) {
        if (courseIndex >= courses.size()) {
            throw std::out_of_range("Invalid course selected.");
        }
        courses[courseIndex] = course;
    }

    void removeCourse(std::size_t courseIndex) {
        if (courseIndex >= courses.size()) {
            throw std::out_of_range("Invalid course selected.");
        }
        courses.erase(courses.begin() + static_cast<std::ptrdiff_t>(courseIndex));
    }

    // Get semester name
    std::string getSemesterName() const {
        return semesterName;
    }

    // Get academic session
    std::string getAcademicSession() const {
        return academicSession;
    }

    // Get all courses
    const std::vector<Course>& getCourses() const {
        return courses;
    }

    // Calculate total credit units
    int getTotalCreditUnits() const {
        int total = 0;

        for (const Course& course : courses) {
            total += course.getCreditUnits();
        }

        return total;
    }

    // Calculate semester GPA
    double calculateGPA() const {
        if (courses.empty()) {
            throw std::runtime_error("No courses available for GPA calculation.");
        }

        int totalGradePoints = 0;
        int totalUnits = 0;

        for (const Course& course : courses) {
            totalGradePoints +=
                course.getCreditUnits() * course.getGradePoint();

            totalUnits += course.getCreditUnits();
        }

        return static_cast<double>(totalGradePoints) / totalUnits;
    }
};

#endif
