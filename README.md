English Unfamiliar Words Extractor (EUWE)
https://logo.ico

EUWE 是一款基于 Qt 开发的智能英语生词提取工具。它能够从纯文本、PDF、Word 文档甚至图片（通过 OCR）中快速提取所有英文单词，并与内置的熟词库及词典进行比对，自动将单词分为“熟词”、“生词”和“未识别”三类，帮助学习者高效积累生词。

✨ 功能特性
多格式文本导入
支持直接输入、粘贴文本，或从 TXT、PDF、Word (.doc/.docx) 文件中读取内容。

图片 OCR 识别
集成独立的 OCR 辅助工具（基于 Tesseract），可从图片中提取文字并追加到待处理文本中。

智能单词分类
提取文本中的所有字母组合（忽略数字、符号），通过与本地 SQLite 熟词库及词典（StarDict 格式）比对，自动区分为：

熟词：已存在于熟词库中的单词

生词：在词典中可查到但尚未录入熟词库的单词（附带音标与释义）

未识别：词典中查不到的字母组合（可能是缩写、拼写错误或非英语词汇）

生词管理

生词以表格形式展示（单词、音标、释义）

可批量将选中的生词一键录入熟词库

支持导出生词表为 CSV、PDF、DOC 文件

熟词库管理

独立的管理窗口，支持增删改查

快速搜索、精确/模糊查找

导入/导出 CSV 或 JSON 格式的单词数据

实时统计单词总数

美观的现代化界面
采用扁平化设计，操作直观，支持暗色/亮色自适应（取决于系统主题）。

📸 界面预览
由于本项目暂无截图，您可以自行编译体验。

🔧 依赖项
主程序 (EUWE)
Qt 6.x 或 Qt 5.15+（推荐 Qt 6）

Qt 模块：core、gui、widgets、sql、pdf、axcontainer（Windows 下读取 Word 必需）

编译器：支持 C++17 的 MSVC、MinGW 或 Clang

OCR 辅助工具 (EUWEOCRTool)
Tesseract OCR 4.x

Leptonica

预编译的 tessdata 语言包（至少包含 eng.traineddata）
