#include "widget.h"
#include "ui_widget.h"
#define VERSION "V1.0"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    setWindowIcon(QIcon(":/logo.ico"));
    ui->setupUi(this);
    this->setWindowTitle("English Unfamiliar Words Extractor（EUWE）\t"+ QString(VERSION));
    this->setFixedSize(this->width(),this->height());
    wdb = new WordsDatabase(this);

    // 连接信号槽
    connect(ui->openDb,&QPushButton::clicked,[=](){
        wdb->show();
    });

    connect(ui->help,&QPushButton::clicked,[=](){
        QMessageBox::about(nullptr,"帮助","联系Bcaid\nQQ:75011847");
    });

    connect(ui->about,&QPushButton::clicked,[=](){
        QMessageBox::about(nullptr,"关于","软件版本:" + QString(VERSION)+"\n 由Bcaid编写");
    });

    connect(ui->leftextract,&QPushButton::clicked,[=](){
        analyze(ui->textEdit->toPlainText());
    });

    // ===== 新增按钮功能 =====

    // 清空按钮
    connect(ui->clearBtn, &QPushButton::clicked, [=]() {
        ui->textEdit->clear();
        ui->statusLabel->setText("✨ 文本已清空");
        updateStats(); // 更新统计信息
    });

    // 复制按钮
    connect(ui->copyBtn, &QPushButton::clicked, [=]() {
        QString text = ui->textEdit->toPlainText();
        if (text.isEmpty()) {
            ui->statusLabel->setText("⚠️ 没有文本可复制");
            return;
        }

        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(text);
        ui->statusLabel->setText("✅ 已复制到剪贴板");
    });

    // 文本内容变化时更新统计
    connect(ui->textEdit, &QTextEdit::textChanged, [=]() {
        updateStats();
    });

    // 文件导入按钮 - 支持 txt, pdf, doc/docx
    connect(ui->input, &QPushButton::clicked, [=]() {
        // 弹出文件选择对话框，过滤器：所有支持的文件
        QString filter = "所有文件 (*.*);;文本文档 (*.txt);;PDF 文件 (*.pdf);;Word 文档 (*.doc *.docx)";
        QString filePath = QFileDialog::getOpenFileName(this, "选择文件", QString(), filter);
        if (filePath.isEmpty()) {
            return; // 用户取消
        }

        // 更新状态
        ui->statusLabel->setText("⏳ 正在读取文件...");
        QApplication::processEvents(); // 让界面及时刷新

        QString content;
        QFileInfo fileInfo(filePath);
        QString suffix = fileInfo.suffix().toLower();

        // 根据后缀选择读取函数
        if (suffix == "txt") {
            content = readTxt(filePath);
        } else if (suffix == "pdf") {
            content = readPdf(filePath);
        } else if (suffix == "doc" || suffix == "docx") {
            content = readWordDocument(filePath);
        } else {
            // 尝试作为纯文本打开（用户可能选了所有文件）
            content = readTxt(filePath);
        }

        // 检查是否读取成功
        if (content.isEmpty()) {
            QMessageBox::warning(this, "读取失败", "无法读取文件内容，请检查文件格式或权限。");
            ui->statusLabel->setText("❌ 读取失败");
        } else {
            ui->textEdit->setPlainText(content);
            ui->statusLabel->setText("✅ 文件导入成功：" + fileInfo.fileName());
            updateStats(); // 更新字符/单词统计
        }
    });

    // OCR按钮功能
    connect(ui->ocr, &QPushButton::clicked, [=]() {
        // 1. 弹出文件对话框选择图片
        QString filter = "图片文件 (*.png *.jpg *.jpeg *.bmp *.tiff *.gif);;所有文件 (*.*)";
        QString imagePath = QFileDialog::getOpenFileName(this, "选择要识别的图片", QString(), filter);
        if (imagePath.isEmpty()) {
            return; // 用户取消
        }

        // 更新状态
        ui->statusLabel->setText("⏳ 正在识别图片...");
        QApplication::processEvents();

        // 2. 创建 QProcess 对象
        QProcess *ocrProcess = new QProcess(this);

        // 3. 连接信号：读取输出
        connect(ocrProcess, &QProcess::readyReadStandardOutput, this, [=]() {
            QByteArray output = ocrProcess->readAllStandardOutput();
            QString result = QString::fromUtf8(output).trimmed();
            if (!result.isEmpty()) {
                // 将识别结果追加到文本编辑区（也可选择替换，这里采用追加并换行）
                QString currentText = ui->textEdit->toPlainText();
                if (!currentText.isEmpty() && !currentText.endsWith('\n')) {
                    ui->textEdit->append(""); // 换行
                }
                ui->textEdit->append("【OCR识别结果】");
                ui->textEdit->append(result);
                ui->textEdit->append(""); // 空行分隔
            }
        });

        // 4. 连接信号：读取错误输出
        connect(ocrProcess, &QProcess::readyReadStandardError, this, [=]() {
            QByteArray err = ocrProcess->readAllStandardError();
            qDebug() << "OCR stderr:" << QString::fromUtf8(err);
        });

        // 5. 进程结束处理
        connect(ocrProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                this, [=](int exitCode, QProcess::ExitStatus exitStatus) {
                    if (exitStatus == QProcess::CrashExit) {
                        ui->statusLabel->setText("❌ OCR进程崩溃");
                        qDebug() << "OCR 进程崩溃";
                    } else if (exitCode != 0) {
                        ui->statusLabel->setText("❌ OCR识别失败，退出码: " + QString::number(exitCode));
                        qDebug() << "OCR 进程错误，退出码:" << exitCode;
                    } else {
                        ui->statusLabel->setText("✅ OCR识别完成");
                        qDebug() << "OCR 完成，退出码:" << exitCode;
                    }
                    ocrProcess->deleteLater(); // 清理进程对象
                });

        // 6. 进程启动失败处理
        connect(ocrProcess, &QProcess::errorOccurred, this, [=](QProcess::ProcessError error) {
            QString errorMsg;
            switch (error) {
            case QProcess::FailedToStart:
                errorMsg = "启动失败，请确认 EUWEOCRT.exe 是否存在且可执行";
                break;
            case QProcess::Crashed:
                errorMsg = "进程崩溃";
                break;
            case QProcess::Timedout:
                errorMsg = "等待超时";
                break;
            case QProcess::WriteError:
                errorMsg = "写入错误";
                break;
            case QProcess::ReadError:
                errorMsg = "读取错误";
                break;
            default:
                errorMsg = "未知错误";
            }
            ui->statusLabel->setText("❌ OCR进程错误: " + errorMsg);
            qDebug() << "启动 OCR 进程失败:" << error;
            ocrProcess->deleteLater();
        });

        // 7. 设置程序路径和参数
        // 假设 EUWEOCRT.exe 与主程序在同一目录下
        QString program = QCoreApplication::applicationDirPath() + "/EUWEOCRT.exe";
        // 如果希望使用相对路径（程序运行目录），可直接用 "EUWEOCRT.exe"
        // QString program = "EUWEOCRT.exe";

        QStringList arguments;
        arguments << imagePath;  // 传递图片路径

        // 可选：指定 tessdata 路径和语言（根据你的 OCR 助手支持的参数）
        // arguments << "-t" << QCoreApplication::applicationDirPath() + "/tessdata";
        // arguments << "-l" << "eng+chi_sim";

        // 8. 启动进程
        ocrProcess->start(program, arguments);
        if (!ocrProcess->waitForStarted(3000)) { // 等待最多3秒启动
            ui->statusLabel->setText("❌ OCR进程启动超时");
            ocrProcess->deleteLater();
        }
    });

    // 初始化状态
    updateStats();
    ui->statusLabel->setText("✨ 就绪 - 可以开始提取文本");
}

Widget::~Widget()
{
    delete ui;
}


void Widget::analyze(QString str){
    EnglishExtracter ee(str);
    unordered_set<QString> pre_words = ee.get();

    vector<QString> notwords;
    vector<QString> yzwords;//已知
    vector<Word> words;

    for(QString word : pre_words){

        QApplication::processEvents();//防止页面卡死

        QList<int> matchRows;
        QList<QString> match;
        QList<QString> match2;
        QList<QString> match3;

        if(wdb->findWithRowNumbers(word,matchRows,match)){
            yzwords.push_back(word);
            continue;
        }
        if(!wdb->find(word,matchRows,match,match2,match3)){
            notwords.push_back(word);
            continue;
        }
        words.push_back(Word(word,match2[0],match3[0]));
    }

    // 调试输出
    for(QString w : yzwords){
        qDebug() << "熟词: " << w;
    }
    for(QString w : notwords){
        qDebug() << "未识别: " << w;
    }
    for(Word w : words){
        qDebug() << "已识别: " << w.word  << "\t"<< w.phonetic << "\t"<< w.translation;
    }

    // 显示结果窗口
    ResultWidget* resultWidget = new ResultWidget();
    resultWidget->setDatabase(wdb);  // 设置数据库指针
    resultWidget->setNotWords(notwords);
    resultWidget->setYzWords(yzwords);
    resultWidget->setWords(words);
    resultWidget->show();

    // 连接继续识别信号
    connect(resultWidget, &ResultWidget::continueRequested, [=]() {
        // 可以在这里添加继续识别的逻辑
        ui->textEdit->setFocus();
    });

    // 更新状态
    ui->statusLabel->setText(QString("✅ 分析完成 - 熟词: %1, 生词: %2, 未识别: %3")
                                 .arg(yzwords.size())
                                 .arg(words.size())
                                 .arg(notwords.size()));
}

// 更新统计信息
void Widget::updateStats()
{
    QString text = ui->textEdit->toPlainText();

    // 字符数（不包括空格和换行）
    int charCount = 0;
    for (QChar ch : text) {
        if (!ch.isSpace()) {
            charCount++;
        }
    }

    // 单词数
    QStringList words = text.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    int wordCount = words.size();

    // 行数
    int lineCount = text.split('\n').size();
    if (text.isEmpty()) lineCount = 0;

    // 更新UI
    ui->statsLabel->setText(QString("📊 字符数: %1 | 单词数: %2 | 行数: %3")
                                .arg(charCount)
                                .arg(wordCount)
                                .arg(lineCount));
}


QString Widget::readTxt(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "无法打开文件：" << filePath;
        return QString();
    }

    QTextStream stream(&file);
    // 启用自动检测编码（可以识别带 BOM 的 UTF-8/UTF-16 等）
    stream.setAutoDetectUnicode(true);
    QString content = stream.readAll();
    file.close();

    return content;
}

QString Widget::readPdf(const QString &filePath) {
    QString allText;

    // 1. 加载 PDF 文档
    QPdfDocument document;
    QPdfDocument::Error error = document.load(filePath);
    if (error != QPdfDocument::Error::None) {
        qWarning() << "无法加载 PDF 文件：" << filePath << "，错误代码：" << static_cast<int>(error);
        return allText;
    }

    int pageCount = document.pageCount();
    qDebug() << "PDF 加载成功，总页数：" << pageCount;

    // 2. 遍历每一页提取文本
    for (int pageIndex = 0; pageIndex < pageCount; ++pageIndex) {
        // 使用 getAllText 获取整页文本
        QPdfSelection selection = document.getAllText(pageIndex);

        if (selection.isValid() && !selection.text().isEmpty()) {
            allText.append(selection.text());
        } else {
            qWarning() << "第" << pageIndex + 1 << "页没有提取到文本";
        }

        QApplication::processEvents();//防止页面卡死
        // 在页面之间添加换行符（可选）
        if (pageIndex < pageCount - 1) {
            allText.append('\n');
        }
    }

    document.close();

    if (allText.isEmpty()) {
        qWarning() << "PDF 文件没有提取到任何文本";
    } else {
        qDebug() << "PDF 文本提取完成，总字符数：" << allText.length();
    }

    return allText;
}

// 读取Word文档所有文本的函数
QString Widget::readWordDocument(const QString &filePath) {
    QString allText;

    // 1. 创建Word应用程序对象
    QAxObject *wordApp = new QAxObject("Word.Application", nullptr);
    if (!wordApp) {
        qDebug() << "Failed to initialize Word application.";
        return allText;
    }

    // 建议在调试时将Visible设为true，以便观察Word程序的启动
    // wordApp->setProperty("Visible", true);

    // 2. 获取文档集合对象
    QAxObject *documents = wordApp->querySubObject("Documents");

    // 3. 打开指定路径的文档
    // 注意：Open方法的参数取决于你的需求，这里第二个参数表示是否只读
    QAxObject *document = documents->querySubObject("Open(const QString&, bool)", filePath, true);
    if (!document) {
        qDebug() << "Failed to open document:" << filePath;
        wordApp->dynamicCall("Quit()");
        delete wordApp;
        return allText;
    }

    // 4. 获取文档的所有文本
    // 通过获取文档的Range对象，然后读取其Text属性 [citation:7]
    QAxObject *range = document->querySubObject("Range()");
    if (range) {
        allText = range->property("Text").toString();
        delete range;
    }

    // 或者，你也可以遍历Words集合来获取文本 [citation:8]
    // QAxObject *words = document->querySubObject("Words");
    // if (words) {
    //     int count = words->dynamicCall("Count()").toInt();
    //     for (int i = 1; i <= count; ++i) {
    //         QAxObject *word = words->querySubObject("Item(int)", i);
    //         if (word) {
    //             allText.append(word->dynamicCall("Text()").toString());
    //             delete word;
    //         }
    //     }
    //     delete words;
    // }

    // 5. 关闭文档并退出Word
    document->dynamicCall("Close()");
    wordApp->dynamicCall("Quit()");

    // 清理对象
    delete document;
    delete wordApp;

    return allText;
}
