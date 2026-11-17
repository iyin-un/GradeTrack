
#ifndef CGPA_ENGINE_H
#define CGPA_ENGINE_H

#include <vector>
#include <stdexcept>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include "../models/Semester.h"

class CGPAEngine
{
private:
    std::vector<Semester> semesters;

public:
    QJsonObject toJson() const
    {
        QJsonArray semesterArray;
        for (const Semester &semester : semesters)
        {
            QJsonArray courseArray;
            for (const Course &course : semester.getCourses())
            {
                QJsonObject courseObject;
                courseObject.insert("code", QString::fromStdString(course.getCourseCode()));
                courseObject.insert("units", course.getCreditUnits());
                courseObject.insert("grade", static_cast<int>(course.getGrade()));
                courseArray.append(courseObject);
            }

            QJsonObject semesterObject;
            semesterObject.insert("name", QString::fromStdString(semester.getSemesterName()));
            semesterObject.insert("session", QString::fromStdString(semester.getAcademicSession()));
            semesterObject.insert("courses", courseArray);
            semesterArray.append(semesterObject);
        }

        QJsonObject root;
        root.insert("version", 1);
        root.insert("semesters", semesterArray);
        return root;
    }

    static CGPAEngine fromJson(const QJsonObject &root)
    {
        if (root.value("version").toInt(-1) != 1 || !root.value("semesters").isArray())
            throw std::runtime_error("Unsupported or invalid GradeTrack data file.");

        CGPAEngine loaded;
        const QJsonArray semesterArray = root.value("semesters").toArray();
        for (const QJsonValue &semesterValue : semesterArray)
        {
            if (!semesterValue.isObject())
                throw std::runtime_error("Invalid semester data in GradeTrack file.");
            const QJsonObject semesterObject = semesterValue.toObject();
            if (!semesterObject.value("name").isString() ||
                !semesterObject.value("session").isString() ||
                !semesterObject.value("courses").isArray())
                throw std::runtime_error("Invalid semester data in GradeTrack file.");

            Semester semester(semesterObject.value("name").toString().toStdString(),
                              semesterObject.value("session").toString().toStdString());
            const QJsonArray courses = semesterObject.value("courses").toArray();
            for (const QJsonValue &courseValue : courses)
            {
                if (!courseValue.isObject())
                    throw std::runtime_error("Invalid course data in GradeTrack file.");
                const QJsonObject courseObject = courseValue.toObject();
                const int grade = courseObject.value("grade").toInt(-1);
                const int units = courseObject.value("units").toInt(0);
                if (!courseObject.value("code").isString() || units <= 0 || grade < 0 || grade > 5)
                    throw std::runtime_error("Invalid course data in GradeTrack file.");
                semester.addCourse(Course(courseObject.value("code").toString().toStdString(),
                                           units, static_cast<Grade>(grade)));
            }
            loaded.addSemester(semester);
        }
        return loaded;
    }

    void addSemester(const Semester& semester)
    {
        semesters.push_back(semester);
    }

    void addCourseToSemester(
        std::size_t semesterIndex,
        const Course& course)
    {
        if (semesterIndex >= semesters.size())
            throw std::out_of_range("Invalid semester selected.");

        semesters[semesterIndex].addCourse(course);
    }

    void updateCourseInSemester(
        std::size_t semesterIndex,
        std::size_t courseIndex,
        const Course& course)
    {
        if (semesterIndex >= semesters.size())
            throw std::out_of_range("Invalid semester selected.");
        semesters[semesterIndex].updateCourse(courseIndex, course);
    }

    void removeCourseFromSemester(
        std::size_t semesterIndex,
        std::size_t courseIndex)
    {
        if (semesterIndex >= semesters.size())
            throw std::out_of_range("Invalid semester selected.");
        semesters[semesterIndex].removeCourse(courseIndex);
    }

    const std::vector<Semester>& getSemesters() const
    {
        return semesters;
    }

    int getTotalCreditUnits() const
    {
        int total = 0;

        for (const Semester& semester : semesters)
            total += semester.getTotalCreditUnits();

        return total;
    }

    int getTotalGradePoints() const
    {
        int total = 0;

        for (const Semester& semester : semesters)
        {
            for (const Course& course : semester.getCourses())
            {
                total += course.getCreditUnits()
                       * course.getGradePoint();
            }
        }

        return total;
    }

    double calculateCGPA() const
    {
        int totalUnits = getTotalCreditUnits();

        if (totalUnits == 0)
            throw std::runtime_error(
                "Cannot calculate CGPA without registered courses."
            );

        return static_cast<double>(getTotalGradePoints())
               / totalUnits;
    }
};

#endif
