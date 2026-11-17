
#ifndef COURSE_H
#define COURSE_H

#include <string>
#include <stdexcept>
#include <cctype>

enum class Grade {
    A, B, C, D, E, F
};

class Course {
private:
    std::string courseCode;
    int creditUnits;
    Grade grade;

public:
    // Constructor
    Course(const std::string& code, int units, Grade courseGrade)
        : courseCode(code), creditUnits(units), grade(courseGrade) {

        if (code.empty()) {
            throw std::invalid_argument("Course code cannot be empty.");
        }

        if (units <= 0) {
            throw std::invalid_argument("Credit units must be greater than zero.");
        }
    }

    // Getters
    std::string getCourseCode() const {
        return courseCode;
    }

    int getCreditUnits() const {
        return creditUnits;
    }

    Grade getGrade() const {
        return grade;
    }

    // Update grade
    void updateGrade(Grade newGrade) {
        grade = newGrade;
    }

    // Update one registered attempt while preserving value validation.
    void updateDetails(const std::string& code, int units, Grade newGrade) {
        if (code.empty()) {
            throw std::invalid_argument("Course code cannot be empty.");
        }
        if (units <= 0) {
            throw std::invalid_argument("Credit units must be greater than zero.");
        }

        courseCode = code;
        creditUnits = units;
        grade = newGrade;
    }

    // Convert grade to grade points
    int getGradePoint() const {
        switch (grade) {
            case Grade::A: return 5;
            case Grade::B: return 4;
            case Grade::C: return 3;
            case Grade::D: return 2;
            case Grade::E: return 1;
            case Grade::F: return 0;
        }

        throw std::logic_error("Invalid grade.");
    }
};

#endif
