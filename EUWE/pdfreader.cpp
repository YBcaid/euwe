#include "PdfReader.h"
#include <QDebug>

PdfReader::PdfReader(QObject *parent)
    : QObject(parent)
    , m_document(new QPdfDocument(this))
    , m_isLoaded(false)
{
}

PdfReader::~PdfReader()
{
    if (m_isLoaded) {
        m_document->close();
    }
}

void PdfReader::clearError()
{
    m_lastError.clear();
}

void PdfReader::setError(const QString &error)
{
    m_lastError = error;
    qWarning() << "PdfReader Error:" << error;
}

QString PdfReader::readAllText(const QString &filePath)
{
    clearError();
    QString result;

    // 如果已经加载了其他文件，先关闭
    if (m_isLoaded) {
        m_document->close();
        m_isLoaded = false;
    }

    // 加载PDF文档
    QPdfDocument::Error error = m_document->load(filePath);
    if (error != QPdfDocument::Error::None) {
        setError(QString("无法加载PDF文件: %1, 错误代码: %2")
                     .arg(filePath)
                     .arg(static_cast<int>(error)));
        return result;
    }

    m_isLoaded = true;
    m_currentFilePath = filePath;

    // 获取页数
    int pages = m_document->pageCount();
    if (pages == 0) {
        setError("PDF文件没有页面");
        return result;
    }

    // 遍历所有页面提取文本
    for (int i = 0; i < pages; ++i) {
        QString pageText = m_document->pageText(i);
        if (!pageText.isEmpty()) {
            if (!result.isEmpty()) {
                result += "\n";  // 添加换行分隔页面
            }
            result += pageText;
        }
    }

    // 如果没有提取到任何文本
    if (result.isEmpty()) {
        setError("PDF文件中没有可提取的文本");
    }

    return result;
}

QStringList PdfReader::readPages(const QString &filePath)
{
    clearError();
    QStringList result;

    // 如果已经加载了其他文件，先关闭
    if (m_isLoaded) {
        m_document->close();
        m_isLoaded = false;
    }

    // 加载PDF文档
    QPdfDocument::Error error = m_document->load(filePath);
    if (error != QPdfDocument::Error::None) {
        setError(QString("无法加载PDF文件: %1, 错误代码: %2")
                     .arg(filePath)
                     .arg(static_cast<int>(error)));
        return result;
    }

    m_isLoaded = true;
    m_currentFilePath = filePath;

    // 获取页数
    int pages = m_document->pageCount();
    if (pages == 0) {
        setError("PDF文件没有页面");
        return result;
    }

    // 提取每一页的文本
    for (int i = 0; i < pages; ++i) {
        QString pageText = m_document->pageText(i);
        result.append(pageText);
    }

    return result;
}

QString PdfReader::readPage(const QString &filePath, int pageIndex)
{
    clearError();
    QString result;

    // 如果文件路径变化或未加载，重新加载
    if (m_currentFilePath != filePath || !m_isLoaded) {
        if (m_isLoaded) {
            m_document->close();
        }

        QPdfDocument::Error error = m_document->load(filePath);
        if (error != QPdfDocument::Error::None) {
            setError(QString("无法加载PDF文件: %1, 错误代码: %2")
                         .arg(filePath)
                         .arg(static_cast<int>(error)));
            return result;
        }

        m_isLoaded = true;
        m_currentFilePath = filePath;
    }

    // 检查页码有效性
    if (pageIndex < 0 || pageIndex >= m_document->pageCount()) {
        setError(QString("无效的页码: %1, 总页数: %2")
                     .arg(pageIndex)
                     .arg(m_document->pageCount()));
        return result;
    }

    // 提取指定页的文本
    result = m_document->pageText(pageIndex);

    return result;
}

int PdfReader::pageCount() const
{
    if (!m_isLoaded) {
        return 0;
    }
    return m_document->pageCount();
}

QString PdfReader::documentInfo() const
{
    if (!m_isLoaded) {
        return "没有加载PDF文件";
    }

    QString info;
    info += QString("文件: %1\n").arg(m_currentFilePath);
    info += QString("页数: %1\n").arg(m_document->pageCount());
    info += QString("PDF版本: %1\n").arg(m_document->metaData(QPdfDocument::MetaDataField::PdfVersion).toString());
    info += QString("标题: %1\n").arg(m_document->metaData(QPdfDocument::MetaDataField::Title).toString());
    info += QString("作者: %1\n").arg(m_document->metaData(QPdfDocument::MetaDataField::Author).toString());
    info += QString("主题: %1\n").arg(m_document->metaData(QPdfDocument::MetaDataField::Subject).toString());
    info += QString("关键词: %1\n").arg(m_document->metaData(QPdfDocument::MetaDataField::Keywords).toString());
    info += QString("创建者: %1\n").arg(m_document->metaData(QPdfDocument::MetaDataField::Creator).toString());
    info += QString("生产者: %1\n").arg(m_document->metaData(QPdfDocument::MetaDataField::Producer).toString());
    info += QString("创建时间: %1\n").arg(m_document->metaData(QPdfDocument::MetaDataField::CreationDate).toDateTime().toString());
    info += QString("修改时间: %1\n").arg(m_document->metaData(QPdfDocument::MetaDataField::ModificationDate).toDateTime().toString());

    return info;
}
