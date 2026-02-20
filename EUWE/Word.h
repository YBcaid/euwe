#ifndef WORD_H
#define WORD_H

#include <QString>
#include <QList>

typedef struct Word{
public:
    Word(QString _word,QString _translation,QString _phonetic){
        word = _word;
        translation =_translation;
        phonetic = _phonetic;
    }
    QString word;
    QString translation;
    QString phonetic;
}Word;

#endif // WORD_H
