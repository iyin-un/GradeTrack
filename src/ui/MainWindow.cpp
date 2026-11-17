
#include "MainWindow.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QListWidget>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QSpinBox>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QSaveFile>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("GradeTrack - CGPA Manager");
    resize(1000, 700);

    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(30, 25, 30, 25);
    mainLayout->setSpacing(18);

    // Header
    QLabel *title = new QLabel("GradeTrack");

    title->setStyleSheet(
        "font-size: 30px;"
        "font-weight: bold;"
        "color: #2563EB;"
    );

    QLabel *subtitle =
        new QLabel("Your academic performance dashboard");

    subtitle->setStyleSheet(
        "font-size: 14px; color: #64748B;"
    );

    mainLayout->addWidget(title);
    mainLayout->addWidget(subtitle);

    // Statistics
    QHBoxLayout *statsLayout = new QHBoxLayout();

    auto createCard = [](const QString &heading,
                         QLabel **valueLabel)
    {
        QFrame *card = new QFrame();

        card->setStyleSheet(
            "QFrame {"
            "background-color: #F1F5F9;"
            "border-radius: 12px;"
            "}"
        );

        QVBoxLayout *layout = new QVBoxLayout(card);

        QLabel *headingLabel = new QLabel(heading);

        headingLabel->setStyleSheet(
            "font-size: 13px;"
            "color: #64748B;"
            "border: none;"
        );

        *valueLabel = new QLabel("0");

        (*valueLabel)->setStyleSheet(
            "font-size: 25px;"
            "font-weight: bold;"
            "color: #0F172A;"
            "border: none;"
        );

        layout->addWidget(headingLabel);
        layout->addWidget(*valueLabel);

        return card;
    };

    statsLayout->addWidget(
        createCard("Current CGPA", &cgpaValue)
    );

    statsLayout->addWidget(
        createCard("Total Credits", &creditsValue)
    );

    statsLayout->addWidget(
        createCard("Semesters", &semesterCountLabel)
    );

    mainLayout->addLayout(statsLayout);

    // Semester selection
    QLabel *semesterHeading =
        new QLabel("Select Semester");

    semesterHeading->setStyleSheet(
        "font-size: 18px;"
        "font-weight: bold;"
    );

    semesterSelector = new QComboBox();

    semesterSelector->setMinimumHeight(38);

    semesterSelector->setStyleSheet(
        "QComboBox {"
        "padding: 6px;"
        "border: 1px solid #CBD5E1;"
        "border-radius: 6px;"
        "}"
    );

    mainLayout->addWidget(semesterHeading);
    mainLayout->addWidget(semesterSelector);

    // Semester GPA
    semesterGpaLabel = new QLabel("Semester GPA: 0.00");

    semesterGpaLabel->setStyleSheet(
        "font-size: 15px;"
        "font-weight: bold;"
        "color: #2563EB;"
    );

    mainLayout->addWidget(semesterGpaLabel);

    // Course list
    QLabel *courseHeading = new QLabel("Registered Courses");

    courseHeading->setStyleSheet(
        "font-size: 18px;"
        "font-weight: bold;"
    );

    courseList = new QListWidget();

    courseList->setStyleSheet(
        "QListWidget {"
        "background-color: #F8FAFC;"
        "border: 1px solid #E2E8F0;"
        "border-radius: 10px;"
        "padding: 8px;"
        "font-size: 14px;"
        "}"
        "QListWidget::item {"
        "padding: 12px;"
        "border-bottom: 1px solid #E2E8F0;"
        "}"
    );

    mainLayout->addWidget(courseHeading);
    mainLayout->addWidget(courseList, 1);

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    QPushButton *addSemesterButton =
        new QPushButton("+ Add Semester");

    QPushButton *addCourseButton =
        new QPushButton("+ Add Course");

    QPushButton *editCourseButton =
        new QPushButton("Edit Selected");

    QPushButton *deleteCourseButton =
        new QPushButton("Delete Selected");

    QString buttonStyle =
        "QPushButton {"
        "background-color: #2563EB;"
        "color: white;"
        "font-size: 14px;"
        "font-weight: bold;"
        "padding: 12px 20px;"
        "border-radius: 8px;"
        "}"
        "QPushButton:hover {"
        "background-color: #1D4ED8;"
        "}";

    addSemesterButton->setStyleSheet(buttonStyle);
    addCourseButton->setStyleSheet(buttonStyle);
    editCourseButton->setStyleSheet(buttonStyle);
    deleteCourseButton->setStyleSheet(
        buttonStyle + "QPushButton { background-color: #DC2626; }"
    );

    buttonLayout->addWidget(addSemesterButton);
    buttonLayout->addWidget(addCourseButton);
    buttonLayout->addWidget(editCourseButton);
    buttonLayout->addWidget(deleteCourseButton);

    mainLayout->addLayout(buttonLayout);

    // Connections
    connect(addSemesterButton, &QPushButton::clicked,
            this, [this]() {
                addSemester();
            });

    connect(addCourseButton, &QPushButton::clicked,
            this, [this]() {
                addCourse();
            });

    connect(editCourseButton, &QPushButton::clicked,
            this, [this]() { editCourse(); });

    connect(deleteCourseButton, &QPushButton::clicked,
            this, [this]() { deleteCourse(); });

    connect(semesterSelector,
            &QComboBox::currentIndexChanged,
            this, [this](int) {
                refreshCourses();
            });

    loadData();
    refreshDashboard();
}

QString MainWindow::dataFilePath() const
{
    const QString directory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    return QDir(directory).filePath("gradetrack-data.json");
}

void MainWindow::loadData()
{
    const QString path = dataFilePath();
    QFile file(path);
    if (!file.exists())
        return;
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, "Could Not Load Records",
                             "GradeTrack could not open its saved records:\n" + file.errorString());
        return;
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject())
    {
        QMessageBox::warning(this, "Could Not Load Records",
                             "The saved GradeTrack data is invalid. Your existing file was left unchanged.");
        return;
    }

    try
    {
        calculator = CGPAEngine::fromJson(document.object());
        const auto &semesters = calculator.getSemesters();
        for (std::size_t i = 0; i < semesters.size(); ++i)
        {
            const Semester &semester = semesters[i];
            semesterSelector->addItem(
                QString::fromStdString(semester.getSemesterName()) + " - " +
                QString::fromStdString(semester.getAcademicSession()),
                static_cast<int>(i));
        }
        if (!semesters.empty())
            semesterSelector->setCurrentIndex(0);
    }
    catch (const std::exception &error)
    {
        QMessageBox::warning(this, "Could Not Load Records",
                             QString("The saved GradeTrack data is invalid: %1").arg(error.what()));
    }
}

bool MainWindow::saveData()
{
    const QString path = dataFilePath();
    const QFileInfo info(path);
    if (!QDir().mkpath(info.absolutePath()))
    {
        QMessageBox::warning(this, "Could Not Save Records",
                             "GradeTrack could not create its data folder.");
        return false;
    }

    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(this, "Could Not Save Records",
                             "GradeTrack could not save your records:\n" + file.errorString());
        return false;
    }
    const QByteArray json = QJsonDocument(calculator.toJson()).toJson(QJsonDocument::Indented);
    if (file.write(json) != json.size() || !file.commit())
    {
        QMessageBox::warning(this, "Could Not Save Records",
                             "GradeTrack could not finish saving your records:\n" + file.errorString());
        return false;
    }
    return true;
}


// ADD SEMESTER
void MainWindow::addSemester()
{
    QDialog dialog(this);

    dialog.setWindowTitle("Add Semester");

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QFormLayout *form = new QFormLayout();

    QLineEdit *nameInput = new QLineEdit();
    QLineEdit *sessionInput = new QLineEdit();

    nameInput->setPlaceholderText("First Semester");
    sessionInput->setPlaceholderText("2026/2027");

    form->addRow("Semester Name:", nameInput);
    form->addRow("Academic Session:", sessionInput);

    layout->addLayout(form);

    QDialogButtonBox *buttons =
        new QDialogButtonBox(
            QDialogButtonBox::Ok |
            QDialogButtonBox::Cancel
        );

    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted,
            &dialog, &QDialog::accept);

    connect(buttons, &QDialogButtonBox::rejected,
            &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted)
        return;

    QString name = nameInput->text().trimmed();
    QString session = sessionInput->text().trimmed();

    if (name.isEmpty() || session.isEmpty())
    {
        QMessageBox::warning(
            this,
            "Missing Information",
            "Enter both semester name and academic session."
        );

        return;
    }

    try
    {
        Semester semester(
            name.toStdString(),
            session.toStdString()
        );

        calculator.addSemester(semester);
        saveData();

        int index = static_cast<int>(
            calculator.getSemesters().size() - 1
        );

        semesterSelector->addItem(
            name + " - " + session,
            index
        );

        semesterSelector->setCurrentIndex(index);

        refreshDashboard();

        QMessageBox::information(
            this,
            "Success",
            "Semester added successfully."
        );
    }
    catch (const std::exception &error)
    {
        QMessageBox::critical(
            this,
            "Error",
            error.what()
        );
    }
}


// ADD COURSE
void MainWindow::addCourse()
{
    if (semesterSelector->currentIndex() < 0)
    {
        QMessageBox::warning(
            this,
            "No Semester",
            "Add a semester before registering courses."
        );

        return;
    }

    QDialog dialog(this);

    dialog.setWindowTitle("Add Course");

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QFormLayout *form = new QFormLayout();

    QLineEdit *codeInput = new QLineEdit();

    codeInput->setPlaceholderText("e.g. CSC 211");

    QSpinBox *unitsInput = new QSpinBox();

    unitsInput->setRange(1, 30);
    unitsInput->setValue(3);

    QComboBox *gradeInput = new QComboBox();

    gradeInput->addItem("A", static_cast<int>(Grade::A));
    gradeInput->addItem("B", static_cast<int>(Grade::B));
    gradeInput->addItem("C", static_cast<int>(Grade::C));
    gradeInput->addItem("D", static_cast<int>(Grade::D));
    gradeInput->addItem("E", static_cast<int>(Grade::E));
    gradeInput->addItem("F", static_cast<int>(Grade::F));

    form->addRow("Course Code:", codeInput);
    form->addRow("Credit Units:", unitsInput);
    form->addRow("Grade:", gradeInput);

    layout->addLayout(form);

    QDialogButtonBox *buttons =
        new QDialogButtonBox(
            QDialogButtonBox::Ok |
            QDialogButtonBox::Cancel
        );

    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted,
            &dialog, &QDialog::accept);

    connect(buttons, &QDialogButtonBox::rejected,
            &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted)
        return;

    QString code = codeInput->text().trimmed();

    if (code.isEmpty())
    {
        QMessageBox::warning(
            this,
            "Missing Information",
            "Enter a course code."
        );

        return;
    }

    try
    {
        int selectedGrade = gradeInput->currentData().toInt();

        Course course(
            code.toStdString(),
            unitsInput->value(),
            static_cast<Grade>(selectedGrade)
        );

        int semesterIndex =
            semesterSelector->currentData().toInt();

        calculator.addCourseToSemester(
            static_cast<std::size_t>(semesterIndex),
            course
        );

        saveData();

        refreshDashboard();
        refreshCourses();

        QMessageBox::information(
            this,
            "Success",
            "Course registered successfully."
        );
    }
    catch (const std::exception &error)
    {
        QMessageBox::critical(
            this,
            "Error",
            error.what()
        );
    }
}


// EDIT ONLY THE SELECTED COURSE ATTEMPT
void MainWindow::editCourse()
{
    const int semesterIndex = semesterSelector->currentData().toInt();
    const int courseIndex = courseList->currentRow();
    const auto &semesters = calculator.getSemesters();

    if (semesterIndex < 0 || semesterIndex >= static_cast<int>(semesters.size()) ||
        courseIndex < 0 || courseIndex >= static_cast<int>(semesters[semesterIndex].getCourses().size()))
    {
        QMessageBox::warning(this, "Select a Course", "Select a registered course to edit.");
        return;
    }

    const Course &existing = semesters[semesterIndex].getCourses()[courseIndex];
    QDialog dialog(this);
    dialog.setWindowTitle("Edit Course");
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    QFormLayout *form = new QFormLayout();
    QLineEdit *codeInput = new QLineEdit(QString::fromStdString(existing.getCourseCode()));
    QSpinBox *unitsInput = new QSpinBox();
    unitsInput->setRange(1, 30);
    unitsInput->setValue(existing.getCreditUnits());
    QComboBox *gradeInput = new QComboBox();
    gradeInput->addItem("A", static_cast<int>(Grade::A));
    gradeInput->addItem("B", static_cast<int>(Grade::B));
    gradeInput->addItem("C", static_cast<int>(Grade::C));
    gradeInput->addItem("D", static_cast<int>(Grade::D));
    gradeInput->addItem("E", static_cast<int>(Grade::E));
    gradeInput->addItem("F", static_cast<int>(Grade::F));
    gradeInput->setCurrentIndex(gradeInput->findData(static_cast<int>(existing.getGrade())));
    form->addRow("Course Code:", codeInput);
    form->addRow("Credit Units:", unitsInput);
    form->addRow("Grade:", gradeInput);
    layout->addLayout(form);
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
    layout->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted)
        return;

    const QString code = codeInput->text().trimmed();
    if (code.isEmpty()) {
        QMessageBox::warning(this, "Missing Information", "Enter a course code.");
        return;
    }

    try {
        Course updated(code.toStdString(), unitsInput->value(),
                       static_cast<Grade>(gradeInput->currentData().toInt()));
        calculator.updateCourseInSemester(static_cast<std::size_t>(semesterIndex),
                                          static_cast<std::size_t>(courseIndex), updated);
        saveData();
        refreshDashboard();
        QMessageBox::information(this, "Updated", "Course updated successfully.");
    } catch (const std::exception &error) {
        QMessageBox::critical(this, "Error", error.what());
    }
}


// DELETE ONLY THE SELECTED COURSE ATTEMPT
void MainWindow::deleteCourse()
{
    const int semesterIndex = semesterSelector->currentData().toInt();
    const int courseIndex = courseList->currentRow();
    const auto &semesters = calculator.getSemesters();

    if (semesterIndex < 0 || semesterIndex >= static_cast<int>(semesters.size()) ||
        courseIndex < 0 || courseIndex >= static_cast<int>(semesters[semesterIndex].getCourses().size()))
    {
        QMessageBox::warning(this, "Select a Course", "Select a registered course to delete.");
        return;
    }

    const Course &selected = semesters[semesterIndex].getCourses()[courseIndex];
    const QString label = QString::fromStdString(selected.getCourseCode());
    if (QMessageBox::question(this, "Delete Course",
            "Delete this course attempt (" + label + ")?") != QMessageBox::Yes)
        return;

    try {
        calculator.removeCourseFromSemester(static_cast<std::size_t>(semesterIndex),
                                            static_cast<std::size_t>(courseIndex));
        saveData();
        refreshDashboard();
    } catch (const std::exception &error) {
        QMessageBox::critical(this, "Error", error.what());
    }
}


// UPDATE DASHBOARD
void MainWindow::refreshDashboard()
{
    semesterCountLabel->setText(
        QString::number(
            calculator.getSemesters().size()
        )
    );

    creditsValue->setText(
        QString::number(
            calculator.getTotalCreditUnits()
        )
    );

    if (calculator.getTotalCreditUnits() == 0)
    {
        cgpaValue->setText("0.00");
    }
    else
    {
        cgpaValue->setText(
            QString::number(
                calculator.calculateCGPA(),
                'f', 2
            )
        );
    }

    refreshCourses();
}


// UPDATE COURSE LIST
void MainWindow::refreshCourses()
{
    courseList->clear();

    int index = semesterSelector->currentIndex();

    if (index < 0)
    {
        semesterGpaLabel->setText("Semester GPA: 0.00");
        return;
    }

    const auto &semesters = calculator.getSemesters();

    int semesterIndex =
        semesterSelector->currentData().toInt();

    if (semesterIndex < 0 ||
        semesterIndex >= static_cast<int>(semesters.size()))
        return;

    const Semester &semester = semesters[semesterIndex];

    for (const Course &course : semester.getCourses())
    {
        QString details =
            QString::fromStdString(course.getCourseCode())
            + "   |   "
            + QString::number(course.getCreditUnits())
            + " Units   |   ";

        switch (course.getGrade())
        {
            case Grade::A: details += "A"; break;
            case Grade::B: details += "B"; break;
            case Grade::C: details += "C"; break;
            case Grade::D: details += "D"; break;
            case Grade::E: details += "E"; break;
            case Grade::F: details += "F"; break;
        }

        details += "   |   "
            + QString::number(course.getGradePoint())
            + " Points";

        courseList->addItem(details);
    }

    if (semester.getCourses().empty())
    {
        courseList->addItem("No courses registered yet.");
        semesterGpaLabel->setText("Semester GPA: 0.00");
    }
    else
    {
        semesterGpaLabel->setText(
            "Semester GPA: "
            + QString::number(semester.calculateGPA(), 'f', 2)
        );
    }
}
