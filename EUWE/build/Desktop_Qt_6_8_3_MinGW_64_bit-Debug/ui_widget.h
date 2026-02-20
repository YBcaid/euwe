/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QWidget *titleBar;
    QLabel *appIcon;
    QLabel *appTitle;
    QLabel *appSubtitle;
    QPushButton *openDb;
    QLabel *label;
    QTextEdit *textEdit;
    QGroupBox *functionGroup;
    QPushButton *leftextract;
    QLabel *sepLabel;
    QPushButton *input;
    QPushButton *ocr;
    QPushButton *clearBtn;
    QPushButton *copyBtn;
    QWidget *bottomBar;
    QPushButton *help;
    QPushButton *about;
    QLabel *statusLabel;
    QLabel *statsLabel;
    QFrame *decorLine;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName("Widget");
        Widget->resize(850, 550);
        Widget->setStyleSheet(QString::fromUtf8("/* \345\205\250\345\261\200\346\240\267\345\274\217 */\n"
"QWidget {\n"
"    background-color: #f8fafc;\n"
"    font-family: 'Microsoft YaHei', 'Segoe UI', sans-serif;\n"
"}\n"
"\n"
"/* \346\240\207\351\242\230\346\240\267\345\274\217 */\n"
"QLineEdit, QTextEdit {\n"
"    background-color: white;\n"
"    border: 2px solid #e2e8f0;\n"
"    border-radius: 8px;\n"
"    padding: 10px;\n"
"    font-size: 13px;\n"
"    selection-background-color: #4299e1;\n"
"    selection-color: white;\n"
"}\n"
"\n"
"QTextEdit:focus, QLineEdit:focus {\n"
"    border-color: #4299e1;\n"
"    box-shadow: 0 0 0 3px rgba(66, 153, 225, 0.15);\n"
"}\n"
"\n"
"/* \346\214\211\351\222\256\345\237\272\347\241\200\346\240\267\345\274\217 */\n"
"QPushButton {\n"
"    border: none;\n"
"    border-radius: 8px;\n"
"    font-weight: bold;\n"
"    font-size: 13px;\n"
"    color: white;\n"
"    transition: all 0.2s ease-in-out;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    transform: translateY(-2px);\n"
"    box-shadow: 0 4px 12px rgba(0, 0, 0, 0.15);\n"
""
                        "}\n"
"\n"
"QPushButton:pressed {\n"
"    transform: translateY(0px);\n"
"    box-shadow: 0 2px 6px rgba(0, 0, 0, 0.1);\n"
"}\n"
"\n"
"/* \346\240\207\347\255\276\346\240\267\345\274\217 */\n"
"QLabel {\n"
"    color: #2d3748;\n"
"    font-size: 13px;\n"
"    font-weight: 500;\n"
"}\n"
"\n"
"/* \345\210\206\347\273\204\346\241\206\346\240\267\345\274\217 */\n"
"QGroupBox {\n"
"    font-weight: bold;\n"
"    font-size: 14px;\n"
"    color: #2d3748;\n"
"    border: 2px solid #e2e8f0;\n"
"    border-radius: 10px;\n"
"    margin-top: 10px;\n"
"    padding-top: 10px;\n"
"}\n"
"\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 10px;\n"
"    padding: 0 10px 0 10px;\n"
"}"));
        titleBar = new QWidget(Widget);
        titleBar->setObjectName("titleBar");
        titleBar->setGeometry(QRect(0, 0, 850, 70));
        titleBar->setStyleSheet(QString::fromUtf8("background-color: white;\n"
"border-bottom: 1px solid #e2e8f0;"));
        appIcon = new QLabel(titleBar);
        appIcon->setObjectName("appIcon");
        appIcon->setGeometry(QRect(20, 15, 40, 40));
        appIcon->setStyleSheet(QString::fromUtf8("font-size: 30px;\n"
"background-color: transparent;"));
        appIcon->setAlignment(Qt::AlignmentFlag::AlignCenter);
        appTitle = new QLabel(titleBar);
        appTitle->setObjectName("appTitle");
        appTitle->setGeometry(QRect(70, 15, 200, 25));
        appTitle->setStyleSheet(QString::fromUtf8("font-size: 18px;\n"
"font-weight: bold;\n"
"color: #2d3748;\n"
"background-color: transparent;"));
        appSubtitle = new QLabel(titleBar);
        appSubtitle->setObjectName("appSubtitle");
        appSubtitle->setGeometry(QRect(70, 40, 200, 20));
        appSubtitle->setStyleSheet(QString::fromUtf8("font-size: 12px;\n"
"color: #718096;\n"
"background-color: transparent;"));
        openDb = new QPushButton(titleBar);
        openDb->setObjectName("openDb");
        openDb->setGeometry(QRect(700, 15, 130, 40));
        openDb->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 #4299e1, stop:1 #667eea);\n"
"    font-size: 14px;\n"
"    border-radius: 20px;\n"
"}\n"
"QPushButton:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 #3182ce, stop:1 #5a67d8);\n"
"}"));
        label = new QLabel(Widget);
        label->setObjectName("label");
        label->setGeometry(QRect(20, 90, 120, 25));
        label->setStyleSheet(QString::fromUtf8("font-size: 14px;\n"
"font-weight: bold;\n"
"color: #2d3748;"));
        textEdit = new QTextEdit(Widget);
        textEdit->setObjectName("textEdit");
        textEdit->setGeometry(QRect(20, 120, 550, 280));
        functionGroup = new QGroupBox(Widget);
        functionGroup->setObjectName("functionGroup");
        functionGroup->setGeometry(QRect(590, 120, 240, 280));
        leftextract = new QPushButton(functionGroup);
        leftextract->setObjectName("leftextract");
        leftextract->setGeometry(QRect(15, 30, 210, 50));
        leftextract->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: #ed8936;\n"
"    text-align: left;\n"
"    padding-left: 20px;\n"
"    font-size: 14px;\n"
"}\n"
"QPushButton:hover {\n"
"    background-color: #dd6b20;\n"
"}"));
        sepLabel = new QLabel(functionGroup);
        sepLabel->setObjectName("sepLabel");
        sepLabel->setGeometry(QRect(15, 90, 210, 20));
        sepLabel->setStyleSheet(QString::fromUtf8("color: #a0aec0;\n"
"font-size: 11px;\n"
"text-align: center;"));
        sepLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);
        input = new QPushButton(functionGroup);
        input->setObjectName("input");
        input->setGeometry(QRect(15, 115, 210, 45));
        input->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: #48bb78;\n"
"    text-align: left;\n"
"    padding-left: 20px;\n"
"}\n"
"QPushButton:hover {\n"
"    background-color: #38a169;\n"
"}"));
        ocr = new QPushButton(functionGroup);
        ocr->setObjectName("ocr");
        ocr->setGeometry(QRect(15, 170, 210, 45));
        ocr->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: #9f7aea;\n"
"    text-align: left;\n"
"    padding-left: 20px;\n"
"}\n"
"QPushButton:hover {\n"
"    background-color: #805ad5;\n"
"}"));
        clearBtn = new QPushButton(functionGroup);
        clearBtn->setObjectName("clearBtn");
        clearBtn->setGeometry(QRect(15, 225, 100, 35));
        clearBtn->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: #cbd5e0;\n"
"    color: #2d3748;\n"
"    font-weight: normal;\n"
"}\n"
"QPushButton:hover {\n"
"    background-color: #a0aec0;\n"
"}"));
        copyBtn = new QPushButton(functionGroup);
        copyBtn->setObjectName("copyBtn");
        copyBtn->setGeometry(QRect(125, 225, 100, 35));
        copyBtn->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: #cbd5e0;\n"
"    color: #2d3748;\n"
"    font-weight: normal;\n"
"}\n"
"QPushButton:hover {\n"
"    background-color: #a0aec0;\n"
"}"));
        bottomBar = new QWidget(Widget);
        bottomBar->setObjectName("bottomBar");
        bottomBar->setGeometry(QRect(0, 420, 850, 130));
        bottomBar->setStyleSheet(QString::fromUtf8("background-color: white;\n"
"border-top: 1px solid #e2e8f0;"));
        help = new QPushButton(bottomBar);
        help->setObjectName("help");
        help->setGeometry(QRect(20, 20, 100, 40));
        help->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: #4299e1;\n"
"    border-radius: 20px;\n"
"    font-size: 13px;\n"
"}\n"
"QPushButton:hover {\n"
"    background-color: #3182ce;\n"
"}"));
        about = new QPushButton(bottomBar);
        about->setObjectName("about");
        about->setGeometry(QRect(130, 20, 100, 40));
        about->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    background-color: #667eea;\n"
"    border-radius: 20px;\n"
"    font-size: 13px;\n"
"}\n"
"QPushButton:hover {\n"
"    background-color: #5a67d8;\n"
"}"));
        statusLabel = new QLabel(bottomBar);
        statusLabel->setObjectName("statusLabel");
        statusLabel->setGeometry(QRect(20, 80, 300, 30));
        statusLabel->setStyleSheet(QString::fromUtf8("color: #718096;\n"
"font-size: 12px;\n"
"padding-left: 5px;"));
        statsLabel = new QLabel(bottomBar);
        statsLabel->setObjectName("statsLabel");
        statsLabel->setGeometry(QRect(550, 70, 280, 40));
        statsLabel->setStyleSheet(QString::fromUtf8("color: #4a5568;\n"
"font-size: 12px;\n"
"font-weight: bold;\n"
"background-color: #f7fafc;\n"
"border-radius: 8px;\n"
"padding: 8px;"));
        statsLabel->setAlignment(Qt::AlignmentFlag::AlignCenter);
        decorLine = new QFrame(Widget);
        decorLine->setObjectName("decorLine");
        decorLine->setGeometry(QRect(0, 70, 850, 3));
        decorLine->setStyleSheet(QString::fromUtf8("background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"                                stop:0 #4299e1, stop:0.5 #9f7aea, stop:1 #ed8936);\n"
"border: none;"));
        decorLine->setFrameShape(QFrame::Shape::HLine);

        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::translate("Widget", "\346\231\272\350\203\275\346\226\207\346\234\254\346\217\220\345\217\226\345\267\245\345\205\267", nullptr));
        appIcon->setText(QCoreApplication::translate("Widget", "\360\237\223\235", nullptr));
        appTitle->setText(QCoreApplication::translate("Widget", "\347\224\237\350\257\215\346\217\220\345\217\226\345\267\245\345\205\267 EUWE", nullptr));
        appSubtitle->setText(QCoreApplication::translate("Widget", "Made By Bcaid", nullptr));
        openDb->setText(QCoreApplication::translate("Widget", "\360\237\223\232 \346\211\223\345\274\200\347\206\237\350\257\215\345\272\223", nullptr));
        label->setText(QCoreApplication::translate("Widget", "\360\237\223\204 \345\276\205\346\217\220\345\217\226\346\226\207\346\234\254", nullptr));
        textEdit->setPlaceholderText(QCoreApplication::translate("Widget", "\345\234\250\346\255\244\350\276\223\345\205\245\346\226\207\346\234\254\357\274\214\346\210\226\347\202\271\345\207\273\344\270\213\346\226\271\346\214\211\351\222\256\345\257\274\345\205\245\346\226\207\344\273\266/\345\233\276\347\211\207...", nullptr));
        functionGroup->setTitle(QCoreApplication::translate("Widget", "\342\232\241 \345\277\253\346\215\267\346\223\215\344\275\234", nullptr));
        leftextract->setText(QCoreApplication::translate("Widget", "\360\237\223\213 \345\267\246\344\276\247\346\226\207\346\234\254\346\217\220\345\217\226", nullptr));
        sepLabel->setText(QCoreApplication::translate("Widget", "\342\200\224\342\200\224\342\200\224\342\200\224 \345\257\274\345\205\245\346\226\271\345\274\217 \342\200\224\342\200\224\342\200\224\342\200\224", nullptr));
        input->setText(QCoreApplication::translate("Widget", "\360\237\223\201 \346\226\207\344\273\266\345\257\274\345\205\245 (PDF/DOC/TXT)", nullptr));
        ocr->setText(QCoreApplication::translate("Widget", "\360\237\226\274\357\270\217 \345\233\276\347\211\207OCR\350\257\206\345\210\253", nullptr));
        clearBtn->setText(QCoreApplication::translate("Widget", "\360\237\227\221\357\270\217 \346\270\205\347\251\272", nullptr));
        copyBtn->setText(QCoreApplication::translate("Widget", "\360\237\223\213 \345\244\215\345\210\266", nullptr));
        help->setText(QCoreApplication::translate("Widget", "\342\235\223 \345\270\256\345\212\251", nullptr));
        about->setText(QCoreApplication::translate("Widget", "\342\204\271\357\270\217 \345\205\263\344\272\216", nullptr));
        statusLabel->setText(QCoreApplication::translate("Widget", "\342\234\250 \345\260\261\347\273\252 - \345\217\257\344\273\245\345\274\200\345\247\213\346\217\220\345\217\226\346\226\207\346\234\254", nullptr));
        statsLabel->setText(QCoreApplication::translate("Widget", "\360\237\223\212 \345\255\227\347\254\246\346\225\260: 0 | \345\215\225\350\257\215\346\225\260: 0 | \350\241\214\346\225\260: 0", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
