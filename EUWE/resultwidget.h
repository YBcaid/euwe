#ifndef RESULTWIDGET_H
#define RESULTWIDGET_H

#include <QObject>
#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QHeaderView>
#include <QMessageBox>
#include <QDesktopServices>
#include <QTextDocumentWriter>
#include <QPrinter>
#include <QFileDialog>
#include <QTextStream>
#include <QDateTime>
#include "wordsdatabase.h"
#include "englishextracter.h"

//结果显示窗口
class ResultWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ResultWidget(QWidget *parent = nullptr);

    // 设置数据库指针
    void setDatabase(WordsDatabase* database) { db = database; }

    // 公开方法用于更新数据
    void setNotWords(const std::vector<QString>& words);
    void setYzWords(const std::vector<QString>& words);
    void setWords(const std::vector<Word>& wordList);

private slots:
    void onContinueClicked();           // 继续识别
    void onAddToKnownClicked();         // 录入熟词库
    void onExportCsvClicked();          // 导出CSV
    void onExportPdfClicked();          // 导出PDF
    void onExportDocClicked();          // 导出DOC（HTML格式）

private:
    void setupUI();                 // 设置UI
    void setupConnections();        // 设置信号连接
    void adjustTableRowHeights();   // 调整表格行高
    QString generateHtmlContent();  // 生成HTML内容

    // UI组件 - 三个表格
    QTableWidget* notWordsTable;    // 未识别文本表格
    QTableWidget* yzWordsTable;     // 已知单词表格
    QTableWidget* wordsTable;       // 生词表格（3列：单词、音标、释义）

    // UI组件 - 右侧操作面板
    QWidget* rightPanel;
    QLabel* operationLabel;
    QFrame* horizontalLine;
    QPushButton* continueBtn;       // 继续识别按钮
    QPushButton* addToKnownBtn;     // 录入熟词库按钮
    QPushButton* exportCsvBtn;      // 导出CSV按钮
    QPushButton* exportPdfBtn;      // 导出PDF按钮
    QPushButton* exportDocBtn;      // 导出DOC按钮

    // 数据成员
    WordsDatabase* db;               // 数据库指针
    std::vector<QString> not_words;  // 未识别的单词
    std::vector<QString> yz_words;   // 已知的单词
    std::vector<Word> words;         // 生词

signals:
    void continueRequested();        // 继续识别信号
};

#endif // RESULTWIDGET_H
