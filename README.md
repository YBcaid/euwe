# EUWE英语生词提取工具 English Unfamiliar Words Extractor


EUWE 是一款基于 Qt 开发的英语生词提取工具。它能够从纯文本、PDF、Word 文档以及图片（通过 OCR）中快速提取所有英文单词，并与内置的熟词库及词典进行比对，自动将单词分为“熟词”、“生词”和“未识别”三类，帮助学习者高效积累生词。

---

## ✨ 功能特性

### 📂 多格式文本导入
支持直接输入、粘贴文本，或从 **TXT、PDF、Word (.doc/.docx)** 文件中读取内容。

### 🖼️ 图片 OCR 识别
集成独立的 OCR 辅助工具（基于 Tesseract），可从图片中提取文字并追加到待处理文本中。

### 🧠 智能单词分类
提取文本中的所有字母组合（忽略数字、符号），通过与本地 SQLite 熟词库及词典（StarDict 格式）比对，自动区分为：

- **熟词**：已存在于熟词库中的单词
- **生词**：在词典中可查到但尚未录入熟词库的单词（附带音标与释义）
- **未识别**：词典中查不到的字母组合（可能是缩写、拼写错误或非英语词汇）

### 📋 生词管理
- 生词以表格形式展示（单词、音标、释义）
- 可批量将选中的生词一键录入熟词库
- 支持导出生词表为 **CSV、PDF、DOC** 文件

### 📚 熟词库管理
- 独立的管理窗口，支持增删改查
- 快速搜索、精确/模糊查找
- 导入/导出 CSV 或 JSON 格式的单词数据
- 实时统计单词总数

### 🎨 美观的现代化界面
采用扁平化设计，操作直观。

## 🔧 依赖项

### 主程序 (EUWE)
- **Qt** 推荐 Qt 6.8.3
- **Qt 模块**：`core`、`gui`、`widgets`、`sql`、`pdf`、`axcontainer`（Windows 下读取 Word 必需）
- **编译器**：支持 C++17 的 MinGW

### OCR 辅助工具 (EUWEOCRTool)
- [Tesseract OCR](https://github.com/tesseract-ocr/tesseract) 4.x
- [Leptonica](http://www.leptonica.org/)
- 预编译的 **tessdata** 语言包（至少包含 `eng.traineddata`）
- 注意：已经将tesseracte_lib压缩,在 EUWEOCRTool/ 下,直接解压到该目录下即可
- **编译器**：MSVC

## 注意
**由于英语词典数据库过大(接近1G),euwe_words.db文件并没有上传到github**

**该文件可以自行去 https://github.com/skywind3000/ECDICT/releases/tag/1.0.28 下载ecdict-sqlite-28.zip文件并解压**

**使用时候将db文件改名为euwe_words.db后放在编译出来的euwe主应用的exe同目录下即可**

---

## Third-Party Licenses

This project uses the following third-party libraries:

- **Tesseract OCR** (Apache License 2.0) - [Full License](./licenses/LICENSE-Tesseract.txt)
- **Leptonica** (BSD 2-Clause) - [Full License](./licenses/LICENSE-Leptonica.txt)
- **ECDICT** (MIT License) - [Full License](./licenses/LICENSE-ECDICT.txt)

Please refer to the individual license files for the full terms.
