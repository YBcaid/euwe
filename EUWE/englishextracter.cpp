#include "englishextracter.h"


bool isLetter(QChar c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

EnglishExtracter::EnglishExtracter(QString str,QObject *parent)
    : QObject{parent}
{
    this->extract(str);
}

void EnglishExtracter::extract(QString text){
    unordered_set<QString>().swap(this->en_words); // clear vector
    QString current_word = "";
    int status = 0;
    for (QChar c : text + " ") {
        if (isLetter(c)) {
            if (status == 2) {
                current_word += '-';
            }
            current_word += c;
            status = 1;
            continue;
        }
        if (status == 1) {
            if (c == '-') {
                status = 2;
                continue;
            }
            en_words.insert(current_word);
            current_word = "";
            status = 0;
        }
        else if (status == 2) {
            en_words.insert(current_word);
            current_word = "";
            status = 0;
        }
    }
}

unordered_set<QString> EnglishExtracter::get(){
    for (QString str: this->en_words) {
        qDebug() << str << "  ";
    }
    return  std::move(this->en_words);
}
