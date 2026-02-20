#include "resultwidget.h"
#include <QHeaderView>
#include <QFileDialog>
#include <QTextStream>
#include <QDebug>
#include <QScrollBar>
#include <QDateTime>
#include <QTextDocument>
#include <QPrinter>
#include <QDesktopServices>
#include <QFile>

ResultWidget::ResultWidget(QWidget *parent)
    : QWidget(parent)
    , db(nullptr)
{
    setupUI();
    setupConnections();
}

void ResultWidget::setupUI()
{
    // 创建主水平布局
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    // ========== 左侧三个表格区域 ==========
    QHBoxLayout* tablesLayout = new QHBoxLayout();
    tablesLayout->setSpacing(10);

    // 1. 未识别文本表格
    QWidget* notWordsContainer = new QWidget();
    QVBoxLayout* notWordsLayout = new QVBoxLayout(notWordsContainer);
    notWordsLayout->setContentsMargins(0, 0, 0, 0);

    QLabel* notWordsLabel = new QLabel("未识别文本");
    notWordsLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #666; padding: 5px 0;");

    notWordsTable = new QTableWidget();
    notWordsTable->setColumnCount(1);
    notWordsTable->setHorizontalHeaderLabels(QStringList() << "未识别单词");
    notWordsTable->horizontalHeader()->setStretchLastSection(true);
    notWordsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    notWordsTable->setAlternatingRowColors(true);
    notWordsTable->verticalHeader()->setVisible(false);
    notWordsTable->setWordWrap(true);
    notWordsTable->setTextElideMode(Qt::ElideNone);

    notWordsLayout->addWidget(notWordsLabel);
    notWordsLayout->addWidget(notWordsTable);

    // 2. 生词表格（3列：单词、音标、释义）
    QWidget* wordsContainer = new QWidget();
    QVBoxLayout* wordsLayout = new QVBoxLayout(wordsContainer);
    wordsLayout->setContentsMargins(0, 0, 0, 0);

    QLabel* wordsLabel = new QLabel("生词");
    wordsLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #c45a65; padding: 5px 0;");

    wordsTable = new QTableWidget();
    wordsTable->setColumnCount(3);
    wordsTable->setHorizontalHeaderLabels(QStringList() << "单词" << "音标" << "释义");
    wordsTable->horizontalHeader()->setStretchLastSection(true);
    wordsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    wordsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    wordsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    wordsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    wordsTable->setAlternatingRowColors(true);
    wordsTable->verticalHeader()->setVisible(false);
    wordsTable->setWordWrap(true);
    wordsTable->setTextElideMode(Qt::ElideNone);
    wordsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    wordsTable->setSelectionMode(QAbstractItemView::MultiSelection);

    wordsLayout->addWidget(wordsLabel);
    wordsLayout->addWidget(wordsTable);

    // 3. 熟词表格
    QWidget* yzWordsContainer = new QWidget();
    QVBoxLayout* yzWordsLayout = new QVBoxLayout(yzWordsContainer);
    yzWordsLayout->setContentsMargins(0, 0, 0, 0);

    QLabel* yzWordsLabel = new QLabel("已在熟词库");
    yzWordsLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: #2e8b57; padding: 5px 0;");

    yzWordsTable = new QTableWidget();
    yzWordsTable->setColumnCount(1);
    yzWordsTable->setHorizontalHeaderLabels(QStringList() << "已知单词");
    yzWordsTable->horizontalHeader()->setStretchLastSection(true);
    yzWordsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    yzWordsTable->setAlternatingRowColors(true);
    yzWordsTable->verticalHeader()->setVisible(false);
    yzWordsTable->setWordWrap(true);

    yzWordsLayout->addWidget(yzWordsLabel);
    yzWordsLayout->addWidget(yzWordsTable);

    // 设置表格最小宽度
    notWordsContainer->setMinimumWidth(200);
    wordsContainer->setMinimumWidth(350);
    yzWordsContainer->setMinimumWidth(200);

    tablesLayout->addWidget(notWordsContainer);
    tablesLayout->addWidget(wordsContainer);
    tablesLayout->addWidget(yzWordsContainer);

    // ========== 右侧操作面板 ==========
    rightPanel = new QWidget();
    rightPanel->setFixedWidth(250);
    rightPanel->setStyleSheet("background-color: #f8f9fa; border-left: 2px solid #dee2e6;");

    QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(15, 20, 15, 20);
    rightLayout->setSpacing(12);

    // 操作面板标题
    operationLabel = new QLabel("操作面板");
    operationLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #495057;");
    operationLabel->setAlignment(Qt::AlignCenter);

    // 水平分割线
    horizontalLine = new QFrame();
    horizontalLine->setFrameShape(QFrame::HLine);
    horizontalLine->setFrameShadow(QFrame::Sunken);
    horizontalLine->setStyleSheet("background-color: #dee2e6; max-height: 2px; margin: 5px 0;");

    // ===== 基本操作组 =====
    QLabel* basicGroupLabel = new QLabel("基本操作");
    basicGroupLabel->setStyleSheet("font-weight: bold; color: #495057; padding-top: 5px;");

    // 继续识别按钮
    continueBtn = new QPushButton("▶ 继续识别");
    continueBtn->setMinimumHeight(40);
    continueBtn->setStyleSheet(
        "QPushButton {"
        "   background-color: #007bff;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 5px;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   text-align: left;"
        "   padding-left: 15px;"
        "}"
        "QPushButton:hover { background-color: #0056b3; }"
        "QPushButton:pressed { background-color: #004085; }"
        );

    // 录入熟词库按钮
    addToKnownBtn = new QPushButton("➕ 录入熟词库");
    addToKnownBtn->setMinimumHeight(40);
    addToKnownBtn->setStyleSheet(
        "QPushButton {"
        "   background-color: #28a745;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 5px;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   text-align: left;"
        "   padding-left: 15px;"
        "}"
        "QPushButton:hover { background-color: #218838; }"
        "QPushButton:pressed { background-color: #1e7e34; }"
        );

    // ===== 导出操作组 =====
    QLabel* exportGroupLabel = new QLabel("导出操作");
    exportGroupLabel->setStyleSheet("font-weight: bold; color: #495057; padding-top: 15px;");

    // 导出CSV按钮
    exportCsvBtn = new QPushButton("📄 导出为CSV");
    exportCsvBtn->setMinimumHeight(40);
    exportCsvBtn->setStyleSheet(
        "QPushButton {"
        "   background-color: #6c757d;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 5px;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   text-align: left;"
        "   padding-left: 15px;"
        "}"
        "QPushButton:hover { background-color: #5a6268; }"
        "QPushButton:pressed { background-color: #545b62; }"
        );

    // 导出PDF按钮
    exportPdfBtn = new QPushButton("📑 导出为PDF");
    exportPdfBtn->setMinimumHeight(40);
    exportPdfBtn->setStyleSheet(
        "QPushButton {"
        "   background-color: #fd7e14;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 5px;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   text-align: left;"
        "   padding-left: 15px;"
        "}"
        "QPushButton:hover { background-color: #dc6a0d; }"
        "QPushButton:pressed { background-color: #c85e0b; }"
        );

    // 导出DOC按钮
    exportDocBtn = new QPushButton("📝 导出为DOC");
    exportDocBtn->setMinimumHeight(40);
    exportDocBtn->setStyleSheet(
        "QPushButton {"
        "   background-color: #17a2b8;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 5px;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   text-align: left;"
        "   padding-left: 15px;"
        "}"
        "QPushButton:hover { background-color: #138496; }"
        "QPushButton:pressed { background-color: #0f6674; }"
        );

    // 组装右侧面板
    rightLayout->addWidget(operationLabel);
    rightLayout->addWidget(horizontalLine);
    rightLayout->addWidget(basicGroupLabel);
    rightLayout->addWidget(continueBtn);
    rightLayout->addWidget(addToKnownBtn);
    rightLayout->addWidget(exportGroupLabel);
    rightLayout->addWidget(exportCsvBtn);
    rightLayout->addWidget(exportPdfBtn);
    rightLayout->addWidget(exportDocBtn);
    rightLayout->addStretch();

    // ========== 组装主界面 ==========
    mainLayout->addLayout(tablesLayout, 3);
    mainLayout->addWidget(rightPanel, 1);

    // 设置窗口属性
    setWindowTitle("识别结果");
    setMinimumSize(1000, 600);
}

void ResultWidget::setupConnections()
{
    connect(continueBtn, &QPushButton::clicked, this, &ResultWidget::onContinueClicked);
    connect(addToKnownBtn, &QPushButton::clicked, this, &ResultWidget::onAddToKnownClicked);
    connect(exportCsvBtn, &QPushButton::clicked, this, &ResultWidget::onExportCsvClicked);
    connect(exportPdfBtn, &QPushButton::clicked, this, &ResultWidget::onExportPdfClicked);
    connect(exportDocBtn, &QPushButton::clicked, this, &ResultWidget::onExportDocClicked);
}

// 继续识别 - 关闭窗口
void ResultWidget::onContinueClicked()
{
    emit continueRequested();
    close();
}

// 录入熟词库
void ResultWidget::onAddToKnownClicked()
{
    if (!db) {
        QMessageBox::warning(this, "错误", "数据库未初始化");
        return;
    }

    // 获取选中的行（去重行号）
    QList<QTableWidgetItem*> selectedItems = wordsTable->selectedItems();
    if (selectedItems.isEmpty()) {
        QMessageBox::information(this, "提示", "请先选中要录入的生词");
        return;
    }

    QSet<int> selectedRows;
    for (QTableWidgetItem* item : selectedItems) {
        selectedRows.insert(item->row());
    }

    // 确认对话框
    int result = QMessageBox::question(this, "确认录入",
                                       QString("确定要将选中的 %1 个单词录入熟词库吗？").arg(selectedRows.size()),
                                       QMessageBox::Yes | QMessageBox::No);
    if (result != QMessageBox::Yes) return;

    // 收集选中的 Word 对象
    QList<Word> wordsToAdd;
    for (int row : selectedRows) {
        if (row >= 0 && row < (int)words.size()) {
            wordsToAdd.append(words[row]);
        }
    }

    // 调用批量插入
    int successCount = db->addWords(wordsToAdd);

    // 显示结果
    if (successCount == wordsToAdd.size()) {
        QMessageBox::information(this, "成功",
                                 QString("成功录入 %1 个单词到熟词库").arg(successCount));
    } else {
        int failCount = wordsToAdd.size() - successCount;
        QMessageBox::warning(this, "部分成功",
                             QString("成功录入 %1 个单词，%2 个已存在或失败")
                                 .arg(successCount).arg(failCount));
    }
}

// 导出CSV
// 导出CSV（只导出生词）
void ResultWidget::onExportCsvClicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "导出CSV文件",
                                                    QDir::homePath() + "/生词表.csv",
                                                    "CSV文件 (*.csv)");

    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法创建文件");
        return;
    }

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);

    // 写入表头
    stream << "单词,音标,释义\n";

    // 只写入生词
    for (const Word& w : words) {
        QString word = w.word;
        QString phonetic = w.phonetic.isEmpty() ? "" : w.phonetic;
        QString translation = w.translation;

        auto needsQuotes = [](const QString &field) {
            return field.contains(',') || field.contains('"') ||
                   field.contains('\n') || field.contains('\r');
        };
        auto escapeField = [](QString field) -> QString {
            if (field.contains('"')) field.replace("\"", "\"\"");
            return field;
        };

        if (needsQuotes(word)) word = "\"" + escapeField(word) + "\"";
        if (needsQuotes(phonetic)) phonetic = "\"" + escapeField(phonetic) + "\"";
        if (needsQuotes(translation)) translation = "\"" + escapeField(translation) + "\"";

        stream << word << "," << phonetic << "," << translation << "\n";
    }

    file.close();

    QMessageBox::information(this, "导出成功",
                             QString("已导出 %1 个生词到:\n%2")
                                 .arg(words.size())
                                 .arg(fileName));
}
// 调整表格行高以适应内容
void ResultWidget::adjustTableRowHeights()
{
    // 生词表格自动调整行高
    for (int row = 0; row < wordsTable->rowCount(); ++row) {
        int height = wordsTable->rowHeight(row);

        // 获取每行内容的高度
        for (int col = 0; col < 3; ++col) {
            QTableWidgetItem* item = wordsTable->item(row, col);
            if (item) {
                QRect rect = wordsTable->visualItemRect(item);
                height = qMax(height, rect.height());
            }
        }

        // 设置行高（加上一些边距）
        wordsTable->setRowHeight(row, height + 10);
    }

    // 未识别表格自动调整行高
    for (int row = 0; row < notWordsTable->rowCount(); ++row) {
        QTableWidgetItem* item = notWordsTable->item(row, 0);
        if (item) {
            QRect rect = notWordsTable->visualItemRect(item);
            notWordsTable->setRowHeight(row, rect.height() + 10);
        }
    }

    // 熟词表格自动调整行高
    for (int row = 0; row < yzWordsTable->rowCount(); ++row) {
        QTableWidgetItem* item = yzWordsTable->item(row, 0);
        if (item) {
            QRect rect = yzWordsTable->visualItemRect(item);
            yzWordsTable->setRowHeight(row, rect.height() + 10);
        }
    }
}

// 更新未识别单词表格
void ResultWidget::setNotWords(const std::vector<QString>& words)
{
    not_words = words;
    notWordsTable->setRowCount(not_words.size());

    for (int i = 0; i < not_words.size(); ++i) {
        QTableWidgetItem* item = new QTableWidgetItem(not_words[i]);
        item->setForeground(QBrush(QColor("#dc3545")));
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        notWordsTable->setItem(i, 0, item);
    }

    notWordsTable->resizeColumnsToContents();
    notWordsTable->resizeRowsToContents();
}

// 更新已知单词表格
void ResultWidget::setYzWords(const std::vector<QString>& words)
{
    yz_words = words;
    yzWordsTable->setRowCount(yz_words.size());

    for (int i = 0; i < yz_words.size(); ++i) {
        QTableWidgetItem* item = new QTableWidgetItem(yz_words[i]);
        item->setForeground(QBrush(QColor("#28a745")));
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        yzWordsTable->setItem(i, 0, item);
    }

    yzWordsTable->resizeColumnsToContents();
    yzWordsTable->resizeRowsToContents();
}

// 更新生词表格
void ResultWidget::setWords(const std::vector<Word>& wordList)
{
    words = wordList;
    wordsTable->setRowCount(words.size());

    for (int i = 0; i < words.size(); ++i) {
        // 单词列
        QTableWidgetItem* wordItem = new QTableWidgetItem(words[i].word);
        wordItem->setForeground(QBrush(QColor("#dc3545")));
        wordItem->setFlags(wordItem->flags() & ~Qt::ItemIsEditable);

        // 音标列
        QTableWidgetItem* phoneticItem = new QTableWidgetItem(words[i].phonetic);
        phoneticItem->setForeground(QBrush(QColor("#9c27b0")));
        phoneticItem->setFont(QFont("Arial", 11));
        phoneticItem->setFlags(phoneticItem->flags() & ~Qt::ItemIsEditable);

        // 释义列
        QTableWidgetItem* transItem = new QTableWidgetItem(words[i].translation);
        transItem->setForeground(QBrush(QColor("#6c757d")));
        transItem->setFlags(transItem->flags() & ~Qt::ItemIsEditable);

        wordsTable->setItem(i, 0, wordItem);
        wordsTable->setItem(i, 1, phoneticItem);
        wordsTable->setItem(i, 2, transItem);
    }

    // 调整列宽和行高
    wordsTable->resizeColumnsToContents();
    wordsTable->resizeRowsToContents();

    // 确保释义列填满剩余空间
    wordsTable->horizontalHeader()->setStretchLastSection(true);
}

// 生成HTML内容（超紧凑版 - 只导出生词，六号字体）
QString ResultWidget::generateHtmlContent()
{
    QString html;

    // 构建 HTML 内容 - 超紧凑布局，六号字体（约7.5pt）
    html += "<html><head><meta charset='UTF-8'><style>";
    html += "body { font-family: 'Microsoft YaHei', 'SimHei', 'Arial', sans-serif; margin: 0.3cm; }";  // 边距进一步减小
    html += "h1 { color: #2c3e50; border-bottom: 1px solid #3498db; padding-bottom: 2px; margin: 2px 0; font-size: 9pt; font-weight: bold; }";  // 六号字体约7.5pt，标题稍大为9pt
    html += "table { width:100%; border-collapse: collapse; font-size: 7.5pt; }";  // 六号字体
    html += "th { background-color: #e9ecef; padding: 2px; text-align: left; font-weight: bold; border-bottom: 1px solid #aaa; }";
    html += "td { padding: 1px 2px; border-bottom: 1px dotted #ccc; }";
    html += ".word { color: #e74c3c; font-weight: bold; }";
    html += ".phonetic { color: #9b59b6; font-style: italic; }";
    html += ".translation { color: #2c3e50; }";
    html += ".count { color: #7f8c8d; font-size: 7pt; margin-left: 5px; font-weight: normal; }";
    html += ".footer { text-align: right; color: #95a5a6; margin-top: 5px; font-size: 6pt; }";  // 脚注更小
    html += "</style></head><body>";

    // 标题和统计 - 只显示生词数量
    html += QString("<h1>生词本 <span class='count'>(共 %1 个生词)</span></h1>")
                .arg(words.size());

    // 生词部分 - 使用表格，只导出生词
    if (!words.empty()) {
        html += "<table>";
        html += "<tr><th>单词</th><th>音标</th><th>释义</th></tr>";

        for (const Word& w : words) {
            QString phonetic = w.phonetic.isEmpty() ? "—" : w.phonetic;  // 无音标用破折号代替
            html += QString("<tr>"
                            "<td class='word'>%1</td>"
                            "<td class='phonetic'>%2</td>"
                            "<td class='translation'>%3</td>"
                            "</tr>")
                        .arg(w.word)
                        .arg(phonetic)
                        .arg(w.translation);
        }
        html += "</table>";
    } else {
        html += "<p style='font-size:7.5pt; color:#999;'>没有生词记录</p>";
    }

    // 生成时间
    //html += QString("<p class='footer'>生成时间: %1</p>")
    //            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));

    html += "</body></html>";

    return html;
}

// 导出PDF（超紧凑版）
// 导出PDF（修正字体大小版）
void ResultWidget::onExportPdfClicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "导出PDF文件",
                                                    QDir::homePath() + "/生词表.pdf",
                                                    "PDF文件 (*.pdf)");

    if (fileName.isEmpty()) return;

    QTextDocument doc;

    // 生成HTML内容
    QString html = generateHtmlContent();
    doc.setHtml(html);

    // 关键：设置文档的默认字体为六号（7.5pt）
    QFont defaultFont("Microsoft YaHei");
    defaultFont.setPointSizeF(7.5);  // 设置为7.5磅（六号字体）
    doc.setDefaultFont(defaultFont);

    // 创建PDF打印机
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    printer.setPageSize(QPageSize(QPageSize::A4));
    printer.setPageMargins(QMarginsF(5, 5, 5, 5), QPageLayout::Millimeter);  // 5mm边距

    // 设置页面方向
    printer.setPageOrientation(QPageLayout::Portrait);

    // 打印到PDF
    doc.print(&printer);

    QMessageBox::information(this, "导出成功",
                             QString("PDF文件已导出到:\n%1").arg(fileName));

    // 询问是否打开文件
    if (QMessageBox::question(this, "打开文件", "是否打开PDF文件？",
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
    }
}

// 导出DOC（超紧凑版 - 修复图标缺失问题）
void ResultWidget::onExportDocClicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "导出DOC文件",
                                                    QDir::homePath() + "/生词表.doc",
                                                    "Word文档 (*.doc)");

    if (fileName.isEmpty()) return;

    QString html = generateHtmlContent();

    // 为Word添加完整的HTML头部，修复图标缺失问题
    html = "<html xmlns:v='urn:schemas-microsoft-com:vml' "
           "xmlns:o='urn:schemas-microsoft-com:office:office' "
           "xmlns:w='urn:schemas-microsoft-com:office:word' "
           "xmlns:m='http://schemas.microsoft.com/office/2004/12/omml' "
           "xmlns='http://www.w3.org/TR/REC-html40'>"
           "<head>"
           "<meta charset='UTF-8'>"
           "<meta http-equiv='Content-Type' content='text/html; charset=UTF-8'>"
           "<!--[if gte mso 9]><xml>"
           "<w:WordDocument>"
           "<w:View>Print</w:View>"
           "<w:Zoom>100</w:Zoom>"
           "<w:HyphenationZone>21</w:HyphenationZone>"
           "<w:DoNotOptimizeForBrowser/>"
           "</w:WordDocument>"
           "</xml><![endif]-->"
           "<style>"
           "/* 移除所有图标相关样式，只保留纯文本样式 */"
           "body { margin: 0.3cm; font-family: 'Microsoft YaHei', 'SimHei', 'Arial', sans-serif; }"
           "h1 { font-size: 9pt; margin: 2px 0; border-bottom: 1px solid #3498db; }"
           "table { border-collapse: collapse; width: 100%; font-size: 7.5pt; }"
           "th { background-color: #e9ecef; padding: 2px; border-bottom: 1px solid #aaa; }"
           "td { padding: 1px 2px; border-bottom: 1px dotted #ccc; }"
           ".word { color: #e74c3c; font-weight: bold; }"
           ".phonetic { color: #9b59b6; font-style: italic; }"
           ".translation { color: #2c3e50; }"
           ".count { color: #7f8c8d; font-size: 7pt; }"
           ".footer { text-align: right; color: #95a5a6; font-size: 6pt; }"
           "</style>"
           "</head>"
           + html.mid(html.indexOf("<body>"));

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法创建文件");
        return;
    }

    file.write(html.toUtf8());
    file.close();

    QMessageBox::information(this, "导出成功",
                             QString("DOC文件已导出到:\n%1").arg(fileName));

    // 询问是否打开文件
    if (QMessageBox::question(this, "打开文件", "是否打开文件？",
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
    }
}
