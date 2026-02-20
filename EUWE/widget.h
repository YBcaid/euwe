#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QAxObject>
#include <QClipboard>
#include <QTextDocument>
#include <QTextBlock>
#include <QAxObject>
#include <QDebug>
#include <QPdfDocument>
#include <QPdfPageRenderer>
#include <QPdfSearchModel>
#include <QPdfDocument>
#include <QPdfPageNavigator>
#include <QPdfSelection>
#include <QRegularExpression>
#include <QProcess>
#include "englishextracter.h"
#include "wordsdatabase.h"
#include "resultwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void analyze(QString str);

    // 读取Word文档所有文本的函数
    QString readTxt(const QString &filePath);
    QString readWordDocument(const QString &filePath);
    QString readPdf(const QString &filePath);
private:
    void updateStats();  // 新增：更新统计信息

    WordsDatabase* wdb;
    Ui::Widget *ui;
};

#endif // WIDGET_H
