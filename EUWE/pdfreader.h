#ifndef PDFREADER_H
#define PDFREADER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QPdfDocument>
#include <QScopedPointer>

class PdfReader : public QObject
{
    Q_OBJECT
public:
    explicit PdfReader(QObject *parent = nullptr);
    ~PdfReader();

    // 主要功能：从PDF文件读取所有文本
    QString readAllText(const QString &filePath);

    // 按页读取文本
    QStringList readPages(const QString &filePath);

    // 读取指定页的文本（页码从0开始）
    QString readPage(const QString &filePath, int pageIndex);

    // 获取PDF页数
    int pageCount() const;

    // 获取最后错误信息
    QString lastError() const { return m_lastError; }

    // 检查PDF是否成功加载
    bool isLoaded() const { return m_isLoaded; }

    // 获取当前PDF文档信息
    QString documentInfo() const;

private:
    void clearError();
    void setError(const QString &error);

    QScopedPointer<QPdfDocument> m_document;
    QString m_lastError;
    bool m_isLoaded;
    QString m_currentFilePath;
};

#endif // PDFREADER_H
