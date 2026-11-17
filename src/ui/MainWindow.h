
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include "../calculations/CGPAEngine.h"

class QLabel;
class QListWidget;
class QComboBox;

class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    CGPAEngine calculator;

    QLabel *cgpaValue;
    QLabel *creditsValue;
    QLabel *semesterCountLabel;
    QLabel *semesterGpaLabel;

    QComboBox *semesterSelector;
    QListWidget *courseList;

    void addSemester();
    void addCourse();
    void editCourse();
    void deleteCourse();
    void refreshDashboard();
    void refreshCourses();
    void loadData();
    bool saveData();
    QString dataFilePath() const;
};

#endif
