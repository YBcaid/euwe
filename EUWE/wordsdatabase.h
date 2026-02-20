#ifndef WORDSDATABASE_H
#define WORDSDATABASE_H

// Qt Core 模块
#include <QObject>
#include <QDebug>
#include <QApplication>

#include <QTimer>
#include <QShortcut>

// Qt GUI 模块 - 基础控件
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QFrame>
#include <QGroupBox>
#include <QComboBox>
#include <QCheckBox>
#include <QTabWidget>
#include <QToolBar>
#include <QStatusBar>
#include <QTabWidget>

// Qt GUI 模块 - 布局
#include <QHBoxLayout>
#include <QVBoxLayout>

// Qt GUI 模块 - 视图
#include <QTableView>
#include <QHeaderView>

// Qt SQL 模块
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlTableModel>

// Qt 文件模块
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QDir>

// Qt 对话框
#include <QMessageBox>
#include "Word.h"
//词汇数据库操作
class WordsDatabase : public QObject
{
    Q_OBJECT
public:
    explicit WordsDatabase(QObject *parent = nullptr);
    void add(QString word,QString description,QString ps);//对于批量添加单词效率过慢
    int addWords(const QList<Word>& words);   // 返回成功插入的单词数,批量添加单词效率快
    void show();
    bool find(QString word, QList<int> &rowNumbers,
                             QList<QString> &matchWords,
                             QList<QString> &matchTranslation,
                             QList<QString> &matchYB);//查找词典里是否存在该词汇
    bool findWithRowNumbers(QString word, QList<int> &rowNumbers, QList<QString> &matchWords);//查找熟词库里是否存在该词汇
    QString keepOnlyEnglishLetters(const QString &text);
    QPushButton* createStyledButton(
        const QString& text,
        const QString& bgColor,
        const QString& hoverColor,
        const QString& pressedColor);
    void migrateIfNeeded();
private slots:
    void onDataChanged(const QModelIndex &topLeft,
                       const QModelIndex &bottomRight,
                       const QVector<int> &roles){}
    void onSaveChanges();
    void updateCountLabel(QLabel *label);
    // 导入导出功能的槽函数
    void importFromCsv();
    void exportToCsv();
    void exportToJson();
private:
    QSqlDatabase db;
    QSqlQuery query;
    QSqlTableModel *model = new QSqlTableModel(this);
    QTableView *view = new QTableView;

signals:
};

#endif // WORDSDATABASE_H
