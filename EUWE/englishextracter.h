#ifndef ENGLISHEXTRACTER_H
#define ENGLISHEXTRACTER_H

#include <QDebug>
#include <QObject>
#include <QString>
#include <iostream>
#include "Word.h"
#include <unordered_set>
using namespace std;

//提取英语字母的组合，但是不辨别是否为英语单词
class EnglishExtracter : public QObject
{
    Q_OBJECT
public:
    explicit EnglishExtracter(QString str = "",QObject *parent = nullptr);

    void extract(QString text);
    unordered_set<QString> get();
private:
    unordered_set<QString> en_words = {};
};

#endif // ENGLISHEXTRACTER_H
