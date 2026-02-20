#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QDebug>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("OCR Helper");
    QCoreApplication::setApplicationVersion("1.0");

    // 命令行解析器
    QCommandLineParser parser;
    parser.setApplicationDescription("OCR Helper: Extract text from image.");
    parser.addHelpOption();
    parser.addVersionOption();

    // 位置参数：图片路径（必需）
    parser.addPositionalArgument("image", "Path to the image file to process.");

    // 可选参数：指定 tessdata 目录（包含 .traineddata 文件）
    QCommandLineOption tessdataOption(QStringList() << "t" << "tessdata",
                                      "Path to tessdata directory (containing traineddata files).", "tessdata_path");
    parser.addOption(tessdataOption);

    // 可选参数：指定识别语言（例如 eng, chi_sim, eng+chi_sim）
    QCommandLineOption langOption(QStringList() << "l" << "lang",
                                  "Recognition language (default: eng+chi_sim).", "language", "eng+chi_sim");
    parser.addOption(langOption);

    parser.process(app);

    // 获取图片路径
    const QStringList args = parser.positionalArguments();
    if (args.isEmpty()) {
        qCritical() << "Error: No image file specified.";
        parser.showHelp(1);
        return 1;
    }
    QString imagePath = args.first();

    // 检查图片文件是否存在
    if (!QFile::exists(imagePath)) {
        qCritical() << "Error: Image file does not exist:" << imagePath;
        return 1;
    }

    // 确定 tessdata 路径
    QString tessdataPath = parser.value(tessdataOption);
    if (tessdataPath.isEmpty()) {
        tessdataPath = "./tessdata";   // 默认当前目录下的 tessdata 文件夹
    }

    // 确定识别语言
    QString lang = parser.value(langOption);

    // 简单检查 eng.traineddata 是否存在（可选，帮助调试）
    // 注意：如果用户指定了其他语言组合，这里可能检查不全，故可省略或改为通用提示
    QFileInfo langFile(tessdataPath + "/eng.traineddata");
    if (!langFile.exists() && lang.contains("eng")) {
        qWarning() << "Warning: eng.traineddata not found in tessdata path:" << tessdataPath;
    }

    // 初始化 Tesseract
    tesseract::TessBaseAPI ocr;
    if (ocr.Init(tessdataPath.toLocal8Bit().toStdString().c_str(), lang.toLocal8Bit().toStdString().c_str())) {
        qCritical() << "Could not initialize tesseract. Check tessdata path and language files.";
        return 1;
    }

    // 读取图片（Leptonica 函数要求路径为本地编码，Qt 中 toLocal8Bit 即可）
    Pix *image = pixRead(imagePath.toLocal8Bit().toStdString().c_str());
    if (!image) {
        qCritical() << "Could not read image:" << imagePath;
        ocr.End();
        return 1;
    }

    ocr.SetImage(image);

    // 执行识别
    char *text = ocr.GetUTF8Text();
    if (text) {
        // 将结果输出到标准输出（UTF-8 编码）
        QTextStream cout(stdout, QIODevice::WriteOnly);
        cout << QString::fromUtf8(text) << Qt::endl;
        delete[] text;
    } else {
        qCritical() << "Failed to recognize text.";
        pixDestroy(&image);
        ocr.End();
        return 1;
    }

    // 清理资源
    pixDestroy(&image);
    ocr.End();

    return 0;
}
