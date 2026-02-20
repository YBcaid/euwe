#include "wordsdatabase.h"

// 构造函数
WordsDatabase::WordsDatabase(QObject *parent)
    : QObject{parent}
{
    // 使用唯一连接名
    QString connectionName = "WordDatabaseConnection_" + QString::number(quintptr(this));

    if (QSqlDatabase::contains(connectionName)) {
        db = QSqlDatabase::database(connectionName);
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    }

    db.setDatabaseName("euwe_words.db");

    if (!db.open()) {
        QMessageBox::information(nullptr, "错误",
                                 "打开数据库失败: " + db.lastError().text());
        return;
    }

    // 使用正确的数据库连接创建查询对象
    query = QSqlQuery(db);

    // 检查并迁移旧版表结构
    migrateIfNeeded();

    // 创建表（如果不存在则按新顺序创建）
    QString createTableSql =
        "CREATE TABLE IF NOT EXISTS Words ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "word TEXT NOT NULL UNIQUE,"
        "phonetic TEXT,"
        "translation TEXT)";  // 顺序：word, phonetic, translation

    if (!query.exec(createTableSql)) {
        QMessageBox::information(nullptr, "错误",
                                 "创建表失败: " + query.lastError().text());
    }
}

// 迁移旧版表结构（如果存在旧表且列顺序不对）
void WordsDatabase::migrateIfNeeded()
{
    // 检查表是否存在
    QSqlQuery checkQuery(db);
    if (!checkQuery.exec("SELECT name FROM sqlite_master WHERE type='table' AND name='Words'")) {
        return; // 查询失败，可能无表
    }
    if (!checkQuery.next()) {
        return; // 表不存在
    }

    // 获取当前表结构信息
    QSqlQuery pragmaQuery(db);
    if (!pragmaQuery.exec("PRAGMA table_info(Words)")) {
        return;
    }

    // 记录列名顺序
    QStringList columns;
    while (pragmaQuery.next()) {
        columns.append(pragmaQuery.value(1).toString());
    }

    // 期望顺序：id, word, phonetic, translation
    // 如果实际顺序为 id, word, translation, phonetic，则需迁移
    if (columns.size() >= 4 &&
        columns[1] == "word" &&
        columns[2] == "translation" &&
        columns[3] == "phonetic")
    {
        qDebug() << "检测到旧版数据库（word, translation, phonetic），开始迁移...";

        // 开始事务
        if (!db.transaction()) {
            qWarning() << "无法开始事务，迁移取消";
            return;
        }

        // 1. 创建新表 Words_new
        QSqlQuery createNew(db);
        if (!createNew.exec(
                "CREATE TABLE Words_new ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                "word TEXT NOT NULL UNIQUE,"
                "phonetic TEXT,"
                "translation TEXT)"
                )) {
            qWarning() << "创建 Words_new 失败:" << createNew.lastError().text();
            db.rollback();
            return;
        }

        // 2. 从旧表复制数据，同时交换 translation 和 phonetic 列
        QSqlQuery copyData(db);
        copyData.prepare("INSERT INTO Words_new (word, phonetic, translation) "
                         "SELECT word, translation, phonetic FROM Words");
        if (!copyData.exec()) {
            qWarning() << "数据迁移失败:" << copyData.lastError().text();
            db.rollback();
            return;
        }

        // 3. 删除旧表
        QSqlQuery dropOld(db);
        if (!dropOld.exec("DROP TABLE Words")) {
            qWarning() << "删除旧表失败:" << dropOld.lastError().text();
            db.rollback();
            return;
        }

        // 4. 重命名新表
        QSqlQuery renameTable(db);
        if (!renameTable.exec("ALTER TABLE Words_new RENAME TO Words")) {
            qWarning() << "重命名表失败:" << renameTable.lastError().text();
            db.rollback();
            return;
        }

        // 提交事务
        if (!db.commit()) {
            qWarning() << "提交事务失败:" << db.lastError().text();
            db.rollback();
        } else {
            qDebug() << "数据库迁移成功！";
        }
    }
    else {
        // 已经是新结构或未知结构，无需迁移
        qDebug() << "数据库结构符合要求，无需迁移。";
    }
}

// 添加单个单词（兼容旧接口，但内部顺序已调整）
void WordsDatabase::add(QString word, QString description, QString ps)
{
    // 注意：参数描述为 description（翻译），ps（音标）
    QSqlQuery insertQuery(db);
    insertQuery.prepare("INSERT OR IGNORE INTO Words (word, phonetic, translation) VALUES (?, ?, ?)");
    insertQuery.addBindValue(word);
    insertQuery.addBindValue(ps);          // 音标存入 phonetic
    insertQuery.addBindValue(description); // 翻译存入 translation
    if (!insertQuery.exec()) {
        qDebug() << "添加单词失败:" << insertQuery.lastError().text();
    } else if (insertQuery.numRowsAffected() == 0) {
        qDebug() << "单词已存在，跳过添加:" << word;
    } else {
        qDebug() << "单词添加成功:" << word;
    }
}

// 批量添加单词
int WordsDatabase::addWords(const QList<Word>& words)
{
    if (words.isEmpty()) return 0;

    int successCount = 0;
    if (!db.transaction()) {
        qWarning() << "开始事务失败";
        return 0;
    }

    QSqlQuery insertQuery(db);
    insertQuery.prepare("INSERT OR IGNORE INTO Words (word, phonetic, translation) VALUES (?, ?, ?)");

    for (const Word& w : words) {
        insertQuery.addBindValue(w.word);
        insertQuery.addBindValue(w.phonetic);      // 音标
        insertQuery.addBindValue(w.translation);   // 翻译
        if (insertQuery.exec()) {
            successCount += insertQuery.numRowsAffected();
        } else {
            qWarning() << "插入失败:" << insertQuery.lastError().text() << "单词:" << w.word;
        }
    }

    if (!db.commit()) {
        db.rollback();
        qWarning() << "提交事务失败，已回滚";
        return 0;
    }

    return successCount;
}

// 显示数据库管理窗口
void WordsDatabase::show()
{
    QWidget* w = new QWidget();
    w->setWindowTitle("熟词库管理系统");
    w->resize(1100, 600);

    // 设置全局样式（与原来相同，略）
    w->setStyleSheet(
        "QWidget {"
        "   background-color: #f5f5f5;"
        "}"
        "QTableView {"
        "   background-color: white;"
        "   alternate-background-color: #f9f9f9;"
        "   selection-background-color: #e3f2fd;"
        "   selection-color: #1976d2;"
        "   gridline-color: #e0e0e0;"
        "   font-size: 13px;"
        "}"
        "QTableView::item:selected {"
        "   background-color: #bbdefb;"
        "   color: #0d47a1;"
        "}"
        "QHeaderView::section {"
        "   background-color: #e0e0e0;"
        "   padding: 8px;"
        "   border: none;"
        "   font-weight: bold;"
        "   font-size: 13px;"
        "}"
        "QPushButton {"
        "   border-radius: 4px;"
        "   padding: 8px 12px;"
        "   font-size: 13px;"
        "   font-weight: 500;"
        "}"
        "QLineEdit {"
        "   padding: 8px;"
        "   border: 2px solid #ddd;"
        "   border-radius: 4px;"
        "   background-color: white;"
        "   font-size: 13px;"
        "}"
        "QLineEdit:focus {"
        "   border-color: #2196F3;"
        "}"
        "QLabel {"
        "   color: #333;"
        "}"
        );

    // 创建主布局
    QVBoxLayout* mainVLayout = new QVBoxLayout(w);
    mainVLayout->setContentsMargins(0, 0, 0, 0);
    mainVLayout->setSpacing(0);

    // ===== 底部状态栏 =====
    QStatusBar* statusBar = new QStatusBar(w);
    statusBar->setObjectName("statusBar");
    statusBar->setStyleSheet(
        "QStatusBar {"
        "   background-color: #e0e0e0;"
        "   padding: 5px;"
        "}"
        );

    // 查询数据库获取真实记录数
    int totalCount = 0;
    QSqlQuery countQuery(db);
    if (countQuery.exec("SELECT COUNT(*) FROM Words")) {
        if (countQuery.next()) {
            totalCount = countQuery.value(0).toInt();
        }
    }
    QLabel* recordCountLabel = new QLabel(QString("总记录数: %1 条").arg(totalCount));
    statusBar->addWidget(recordCountLabel);

    QLabel* currentItemLabel = new QLabel("未选中");
    statusBar->addPermanentWidget(currentItemLabel);

    // ===== 顶部工具栏 =====
    QToolBar* toolbar = new QToolBar(w);
    toolbar->setMovable(false);
    toolbar->setIconSize(QSize(20, 20));
    toolbar->setStyleSheet(
        "QToolBar {"
        "   background-color: #FFFFFF;"
        "   padding: 8px 20px;"
        "   spacing: 15px;"
        "   border-bottom: 1px solid #EEEEEE;"
        "}"
        "QToolButton {"
        "   color: #333333;"
        "   background-color: transparent;"
        "   border: none;"
        "   border-radius: 4px;"
        "   padding: 6px 12px;"
        "   font-weight: normal;"
        "}"
        "QToolButton:hover {"
        "   background-color: #F5F5F5;"
        "}"
        "QToolBar::separator {"
        "   width: 1px;"
        "   background-color: #DDDDDD;"
        "   margin: 5px 0;"
        "}"
        );

    QLabel* titleLabel = new QLabel("📚 熟词库管理");
    titleLabel->setStyleSheet(
        "color: #333333;"
        "font-size: 16px;"
        "font-weight: 500;"
        "padding: 0 5px;"
        );
    toolbar->addWidget(titleLabel);
    toolbar->addSeparator();

    QLabel* statusIndicator = new QLabel("● 已连接");
    statusIndicator->setStyleSheet(
        "color: #00C853;"
        "font-size: 13px;"
        "padding: 4px 8px;"
        );
    toolbar->addWidget(statusIndicator);

    // 弹簧 - 将搜索框推到右边
    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    toolbar->addWidget(spacer);

    // ===== 搜索框 =====
    QLineEdit* globalSearchEdit = new QLineEdit();
    globalSearchEdit->setPlaceholderText("🔍 快速搜索单词...");
    globalSearchEdit->setFixedWidth(220);
    globalSearchEdit->setStyleSheet(
        "QLineEdit {"
        "   background-color: #F5F5F5;"
        "   color: #333333;"
        "   border: 1px solid #E0E0E0;"
        "   border-radius: 4px;"
        "   padding: 6px 10px;"
        "   font-size: 13px;"
        "}"
        "QLineEdit:focus {"
        "   border: 1px solid #2196F3;"
        "   background-color: #FAFAFA;"
        "}"
        "QLineEdit::placeholder {"
        "   color: #9E9E9E;"
        "   font-style: italic;"
        "}"
        );

    toolbar->addWidget(globalSearchEdit);

    // 清除按钮
    QPushButton* globalSearchClearBtn = new QPushButton("✕");
    globalSearchClearBtn->setFixedSize(22, 22);
    globalSearchClearBtn->setCursor(Qt::PointingHandCursor);
    globalSearchClearBtn->setVisible(false);
    globalSearchClearBtn->setStyleSheet(
        "QPushButton {"
        "   background-color: #BDBDBD;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 11px;"
        "   font-size: 12px;"
        "   font-weight: bold;"
        "   margin-left: 5px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #757575;"
        "}"
        );

    toolbar->addWidget(globalSearchClearBtn);

    mainVLayout->addWidget(toolbar);

    // ===== 主内容区域 =====
    QWidget* mainContent = new QWidget(w);
    QHBoxLayout* mainLayout = new QHBoxLayout(mainContent);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    // 初始化模型 - 使用正确的数据库连接
    model = new QSqlTableModel(this, db);
    model->setTable("Words");

    if (!model->select()) {
        QMessageBox::critical(w, "错误",
                              "数据库查询失败:\n" + model->lastError().text());
        return;
    }

    model->setEditStrategy(QSqlTableModel::OnFieldChange);

    // 强制一次性加载所有数据
    QApplication::setOverrideCursor(Qt::WaitCursor);
    while (model->canFetchMore()) {
        model->fetchMore();
        QApplication::processEvents();
    }
    QApplication::restoreOverrideCursor();

    connect(model, &QSqlTableModel::dataChanged,
            this, &WordsDatabase::onDataChanged);

    // 创建表格视图
    view = new QTableView(mainContent);
    view->setModel(model);
    view->setColumnWidth(0, 10);   // id 列隐藏
    view->setColumnWidth(1, 180);  // word
    view->setColumnWidth(2, 150);  // phonetic (音标)
    view->setColumnWidth(3, 300);  // translation (翻译)
    view->hideColumn(0);

    view->setAlternatingRowColors(true);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setSelectionMode(QAbstractItemView::ExtendedSelection);
    view->setEditTriggers(QAbstractItemView::DoubleClicked |
                          QAbstractItemView::EditKeyPressed |
                          QAbstractItemView::SelectedClicked);
    view->setSortingEnabled(true);
    view->verticalHeader()->setVisible(false);
    view->setShowGrid(true);
    view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    mainLayout->addWidget(view, 3);

    // ===== 右侧标签页（与原来基本相同，但调整了搜索逻辑中的列索引）=====
    QTabWidget* rightTabWidget = new QTabWidget(mainContent);
    rightTabWidget->setFixedWidth(280);
    rightTabWidget->setStyleSheet(
        "QTabWidget::pane {"
        "   border: 1px solid #ddd;"
        "   border-radius: 4px;"
        "   background-color: white;"
        "}"
        "QTabBar::tab {"
        "   background-color: #f0f0f0;"
        "   padding: 8px 15px;"
        "   margin-right: 2px;"
        "   border-top-left-radius: 4px;"
        "   border-top-right-radius: 4px;"
        "}"
        "QTabBar::tab:selected {"
        "   background-color: white;"
        "   border-bottom-color: white;"
        "}"
        "QTabBar::tab:hover {"
        "   background-color: #e0e0e0;"
        "}"
        );

    // ===== 标签页1: 基本操作 =====
    QWidget* basicOpsTab = new QWidget();
    QVBoxLayout* basicOpsLayout = new QVBoxLayout(basicOpsTab);
    basicOpsLayout->setContentsMargins(10, 15, 10, 15);
    basicOpsLayout->setSpacing(10);

    QLabel* basicTitle = new QLabel("基本操作");
    basicTitle->setStyleSheet("font-weight: bold; font-size: 14px; color: #2196F3;");
    basicOpsLayout->addWidget(basicTitle);

    QFrame* sep1 = new QFrame();
    sep1->setFrameShape(QFrame::HLine);
    sep1->setStyleSheet("background-color: #ddd; max-height: 1px;");
    basicOpsLayout->addWidget(sep1);

    // 插入按钮
    QPushButton *insertBtn = createStyledButton(
        "➕ 插入新词", "#4CAF50", "#45a049", "#3d8b40");
    connect(insertBtn, &QPushButton::clicked, this, [=]() {
        int row = model->rowCount();
        model->insertRow(row);
        model->setData(model->index(row, 1), "新单词");
        model->setData(model->index(row, 2), "新音标");
        model->setData(model->index(row, 3), "新释义");

        if (!model->submitAll()) {
            QMessageBox::warning(view, "错误",
                                 "插入失败: " + model->lastError().text());
            model->revertRow(row);
        } else {
            view->scrollToBottom();
            view->selectRow(row);
            QSqlQuery countQuery(db);
            if (countQuery.exec("SELECT COUNT(*) FROM Words") && countQuery.next()) {
                recordCountLabel->setText(QString("总记录数: %1 条").arg(countQuery.value(0).toInt()));
            }
        }
    });
    basicOpsLayout->addWidget(insertBtn);

    // 删除按钮
    QPushButton *deleteBtn = createStyledButton(
        "❌ 删除选中", "#f44336", "#da190b", "#b71c1c");
    connect(deleteBtn, &QPushButton::clicked, this, [=]() {
        QModelIndexList selectedRows = view->selectionModel()->selectedRows();
        if (selectedRows.isEmpty()) {
            QMessageBox::warning(view, "警告", "请先选中要删除的行");
            return;
        }

        QStringList wordsToDelete;
        for (const QModelIndex& index : selectedRows) {
            int row = index.row();
            QString word = model->data(model->index(row, 1)).toString();
            wordsToDelete.append(word);
        }

        QString confirmMsg;
        if (wordsToDelete.size() == 1) {
            confirmMsg = QString("确定要删除单词 '%1' 吗？").arg(wordsToDelete.first());
        } else {
            confirmMsg = QString("确定要删除选中的 %1 个单词吗？\n\n").arg(wordsToDelete.size());
            for (int i = 0; i < qMin(5, wordsToDelete.size()); ++i) {
                confirmMsg += "  • " + wordsToDelete[i] + "\n";
            }
            if (wordsToDelete.size() > 5) {
                confirmMsg += "  ...等\n";
            }
        }

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(view, "确认删除", confirmMsg,
                                      QMessageBox::Yes | QMessageBox::No);
        if (reply != QMessageBox::Yes) {
            return;
        }

        QSqlDatabase::database().transaction();
        QStringList placeholders;
        for (int i = 0; i < wordsToDelete.size(); ++i) {
            placeholders << "?";
        }
        QString sql = QString("DELETE FROM Words WHERE word IN (%1)").arg(placeholders.join(", "));
        QSqlQuery deleteQuery(db);
        deleteQuery.prepare(sql);
        for (const QString& word : wordsToDelete) {
            deleteQuery.addBindValue(word);
        }

        bool success = deleteQuery.exec();
        if (success) {
            QSqlDatabase::database().commit();
            model->select();
            view->clearSelection();
            QMessageBox::information(view, "成功", QString("成功删除 %1 个单词").arg(wordsToDelete.size()));
            QSqlQuery countQuery(db);
            if (countQuery.exec("SELECT COUNT(*) FROM Words") && countQuery.next()) {
                recordCountLabel->setText(QString("总记录数: %1 条").arg(countQuery.value(0).toInt()));
            }
        } else {
            QSqlDatabase::database().rollback();
            QMessageBox::warning(view, "错误",
                                 "删除失败: " + deleteQuery.lastError().text());
        }
    });
    basicOpsLayout->addWidget(deleteBtn);

    basicOpsLayout->addSpacing(10);

    // ===== 标签页2: 查找功能（修正列索引）=====
    QWidget* searchTab = new QWidget();
    QVBoxLayout* searchLayout2 = new QVBoxLayout(searchTab);
    searchLayout2->setContentsMargins(10, 15, 10, 15);
    searchLayout2->setSpacing(10);

    QGroupBox* searchGroup = new QGroupBox("查找选项");
    searchGroup->setStyleSheet("QGroupBox { font-weight: bold; }");
    QVBoxLayout* searchGroupLayout = new QVBoxLayout(searchGroup);

    QComboBox* searchFieldCombo = new QComboBox();
    searchFieldCombo->addItems({"单词", "翻译", "音标", "全部字段"});
    searchGroupLayout->addWidget(new QLabel("查找字段:"));
    searchGroupLayout->addWidget(searchFieldCombo);

    QCheckBox* caseSensitiveCheck = new QCheckBox("区分大小写");
    QCheckBox* exactMatchCheck = new QCheckBox("精确匹配");
    searchGroupLayout->addWidget(caseSensitiveCheck);
    searchGroupLayout->addWidget(exactMatchCheck);

    searchLayout2->addWidget(searchGroup);

    QLineEdit *searchEdit = new QLineEdit();
    searchEdit->setPlaceholderText("输入要查找的内容...");
    searchEdit->setClearButtonEnabled(true);
    searchLayout2->addWidget(new QLabel("查找内容:"));
    searchLayout2->addWidget(searchEdit);

    QHBoxLayout* searchBtnLayout = new QHBoxLayout();
    QPushButton *searchBtn = createStyledButton("🔍 查找", "#FF9800", "#e68900", "#cc7c00");
    QPushButton *clearBtn = new QPushButton("清除");
    clearBtn->setStyleSheet(
        "QPushButton {"
        "   background-color: #9e9e9e;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 4px;"
        "   padding: 8px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #757575;"
        "}"
        );

    searchBtnLayout->addWidget(searchBtn);
    searchBtnLayout->addWidget(clearBtn);
    searchLayout2->addLayout(searchBtnLayout);

    QLabel* searchResultLabel = new QLabel("准备就绪");
    searchResultLabel->setAlignment(Qt::AlignCenter);
    searchResultLabel->setStyleSheet("color: #666; padding: 5px;");
    searchLayout2->addWidget(searchResultLabel);
    searchLayout2->addStretch();

    auto performSearch = [this, searchEdit, searchFieldCombo,
                          caseSensitiveCheck, exactMatchCheck, searchResultLabel]() {
        QString searchText = searchEdit->text().trimmed();
        if (searchText.isEmpty()) {
            searchResultLabel->setText("请输入查找内容");
            searchResultLabel->setStyleSheet("color: #f44336; padding: 5px;");
            return;
        }

        view->clearSelection();

        int matchCount = 0;
        int searchField = searchFieldCombo->currentIndex();
        Qt::CaseSensitivity cs = caseSensitiveCheck->isChecked() ?
                                     Qt::CaseSensitive : Qt::CaseInsensitive;

        for (int row = 0; row < model->rowCount(); ++row) {
            bool matched = false;

            // 单词列 (col1)
            if (searchField == 0 || searchField == 3) {
                QString word = model->data(model->index(row, 1)).toString();
                if (exactMatchCheck->isChecked()) {
                    if (word.compare(searchText, cs) == 0) matched = true;
                } else {
                    if (word.contains(searchText, cs)) matched = true;
                }
            }

            // 翻译列 (col3) 对应 searchField == 1
            if (!matched && (searchField == 1 || searchField == 3)) {
                QString translation = model->data(model->index(row, 3)).toString();
                if (exactMatchCheck->isChecked()) {
                    if (translation.compare(searchText, cs) == 0) matched = true;
                } else {
                    if (translation.contains(searchText, cs)) matched = true;
                }
            }

            // 音标列 (col2) 对应 searchField == 2
            if (!matched && (searchField == 2 || searchField == 3)) {
                QString phonetic = model->data(model->index(row, 2)).toString();
                if (exactMatchCheck->isChecked()) {
                    if (phonetic.compare(searchText, cs) == 0) matched = true;
                } else {
                    if (phonetic.contains(searchText, cs)) matched = true;
                }
            }

            if (matched) {
                view->selectRow(row);
                matchCount++;

                if (matchCount == 1) {
                    view->scrollTo(model->index(row, 1),
                                   QAbstractItemView::PositionAtCenter);
                }
            }
        }

        if (matchCount > 0) {
            searchResultLabel->setText(QString("找到 %1 个匹配项").arg(matchCount));
            searchResultLabel->setStyleSheet("color: #4CAF50; font-weight: bold; padding: 5px;");
        } else {
            searchResultLabel->setText("未找到匹配项");
            searchResultLabel->setStyleSheet("color: #f44336; padding: 5px;");
        }
    };

    connect(searchBtn, &QPushButton::clicked, performSearch);
    connect(searchEdit, &QLineEdit::returnPressed, performSearch);
    connect(clearBtn, &QPushButton::clicked, [=]() {
        searchEdit->clear();
        view->clearSelection();
        searchResultLabel->setText("准备就绪");
        searchResultLabel->setStyleSheet("color: #666; padding: 5px;");
    });

    // ===== 标签页3: 导入导出 =====
    QWidget* importExportTab = new QWidget();
    QVBoxLayout* ieLayout = new QVBoxLayout(importExportTab);
    ieLayout->setContentsMargins(10, 15, 10, 15);
    ieLayout->setSpacing(10);

    QLabel* ieTitle = new QLabel("数据导入/导出");
    ieTitle->setStyleSheet("font-weight: bold; font-size: 14px; color: #2196F3;");
    ieLayout->addWidget(ieTitle);

    QFrame* sep2 = new QFrame();
    sep2->setFrameShape(QFrame::HLine);
    sep2->setStyleSheet("background-color: #ddd; max-height: 1px;");
    ieLayout->addWidget(sep2);

    QPushButton *importCsvBtn = createStyledButton(
        "📄 导入CSV", "#9C27B0", "#7B1FA2", "#6A1B9A");
    connect(importCsvBtn, &QPushButton::clicked, this,
            &WordsDatabase::importFromCsv);
    ieLayout->addWidget(importCsvBtn);

    QPushButton *exportCsvBtn = createStyledButton(
        "📤 导出CSV", "#FF9800", "#e68900", "#cc7c00");
    connect(exportCsvBtn, &QPushButton::clicked, this,
            &WordsDatabase::exportToCsv);
    ieLayout->addWidget(exportCsvBtn);

    QPushButton *exportJsonBtn = createStyledButton(
        "📋 导出JSON", "#607D8B", "#546E7A", "#455A64");
    connect(exportJsonBtn, &QPushButton::clicked, this,
            &WordsDatabase::exportToJson);
    ieLayout->addWidget(exportJsonBtn);

    ieLayout->addStretch();

    // ===== 标签页4: 统计信息 =====
    QWidget* statsTab = new QWidget();
    QVBoxLayout* statsLayout = new QVBoxLayout(statsTab);
    statsLayout->setContentsMargins(10, 15, 10, 15);
    statsLayout->setSpacing(10);

    QLabel* statsTitle = new QLabel("统计信息");
    statsTitle->setStyleSheet("font-weight: bold; font-size: 14px; color: #2196F3;");
    statsLayout->addWidget(statsTitle);

    QFrame* sep3 = new QFrame();
    sep3->setFrameShape(QFrame::HLine);
    sep3->setStyleSheet("background-color: #ddd; max-height: 1px;");
    statsLayout->addWidget(sep3);

    QLabel* totalCountLabel = new QLabel();
    totalCountLabel->setStyleSheet("font-size: 13px; padding: 5px;");
    statsLayout->addWidget(totalCountLabel);

    QLabel* wordLengthLabel = new QLabel();
    statsLayout->addWidget(wordLengthLabel);

    QLabel* recentAddedLabel = new QLabel();
    statsLayout->addWidget(recentAddedLabel);

    QPushButton* refreshStatsBtn = new QPushButton("🔄 刷新统计");
    refreshStatsBtn->setStyleSheet(
        "QPushButton {"
        "   background-color: #607D8B;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 4px;"
        "   padding: 8px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #455A64;"
        "}"
        );

    auto refreshStats = [=]() {
        int total = 0;
        QSqlQuery countQuery(db);
        if (countQuery.exec("SELECT COUNT(*) FROM Words")) {
            if (countQuery.next()) {
                total = countQuery.value(0).toInt();
            }
        }
        totalCountLabel->setText(QString("📊 总单词数: %1 个").arg(total));

        if (total > 0) {
            int totalLength = 0;
            for (int i = 0; i < qMin(100, total); ++i) {
                QString word = model->data(model->index(i, 1)).toString();
                totalLength += word.length();
            }
            double avgLength = totalLength / (double)qMin(100, total);
            wordLengthLabel->setText(QString("📏 平均长度: %1 字符")
                                         .arg(QString::number(avgLength, 'f', 1)));
        }

        recentAddedLabel->setText("🕒 最近更新: 刚刚刷新");
    };

    statsLayout->addWidget(refreshStatsBtn);
    statsLayout->addStretch();

    // 添加标签页
    rightTabWidget->addTab(basicOpsTab, "基本操作");
    rightTabWidget->addTab(searchTab, "查找");
    rightTabWidget->addTab(importExportTab, "导入/导出");
    rightTabWidget->addTab(statsTab, "统计");

    mainLayout->addWidget(rightTabWidget, 1);
    mainVLayout->addWidget(mainContent, 1);

    // 底部状态栏连接（与原来相同）
    connect(view->selectionModel(), &QItemSelectionModel::currentChanged,
            [=](const QModelIndex &current, const QModelIndex &) {
                if (current.isValid()) {
                    QString word = model->data(model->index(current.row(), 1)).toString();
                    currentItemLabel->setText(QString("当前选中: %1").arg(word));
                } else {
                    currentItemLabel->setText("未选中");
                }
            });

    connect(model, &QSqlTableModel::rowsInserted, [=]() {
        QSqlQuery countQuery(db);
        if (countQuery.exec("SELECT COUNT(*) FROM Words") && countQuery.next()) {
            recordCountLabel->setText(QString("总记录数: %1 条").arg(countQuery.value(0).toInt()));
        }
    });

    connect(model, &QSqlTableModel::rowsRemoved, [=]() {
        QSqlQuery countQuery(db);
        if (countQuery.exec("SELECT COUNT(*) FROM Words") && countQuery.next()) {
            recordCountLabel->setText(QString("总记录数: %1 条").arg(countQuery.value(0).toInt()));
        }
    });

    mainVLayout->addWidget(statusBar);

    // 连接刷新统计按钮
    connect(refreshStatsBtn, &QPushButton::clicked, [=]() {
        refreshStats();
        QSqlQuery countQuery2(db);
        if (countQuery2.exec("SELECT COUNT(*) FROM Words") && countQuery2.next()) {
            recordCountLabel->setText(QString("总记录数: %1 条").arg(countQuery2.value(0).toInt()));
        }
    });

    // 初始化统计信息
    refreshStats();

    // ===== 右上角搜索框功能实现（修正列索引）=====
    QTimer* searchTimer = new QTimer();
    searchTimer->setSingleShot(true);
    searchTimer->setInterval(300);

    auto performGlobalSearch = [this, globalSearchEdit, globalSearchClearBtn, statusBar]() {
        QString searchText = globalSearchEdit->text().trimmed();

        if (searchText.isEmpty()) {
            for (int row = 0; row < model->rowCount(); ++row) {
                view->setRowHidden(row, false);
            }
            globalSearchClearBtn->setVisible(false);
            globalSearchEdit->setStyleSheet(
                "QLineEdit {"
                "   background-color: #F5F5F5;"
                "   color: #333333;"
                "   border: 1px solid #E0E0E0;"
                "   border-radius: 4px;"
                "   padding: 6px 10px;"
                "   font-size: 13px;"
                "}"
                "QLineEdit:focus {"
                "   border: 1px solid #2196F3;"
                "   background-color: #FAFAFA;"
                "}"
                );
            statusBar->showMessage("显示所有记录", 2000);
            return;
        }

        globalSearchClearBtn->setVisible(true);
        int matchCount = 0;
        Qt::CaseSensitivity cs = Qt::CaseInsensitive;
        int firstMatchRow = -1;

        for (int row = 0; row < model->rowCount(); ++row) {
            bool matched = false;

            // 单词列 (col1)
            QString word = model->data(model->index(row, 1)).toString();
            if (word.contains(searchText, cs)) matched = true;

            // 翻译列 (col3)
            if (!matched) {
                QString translation = model->data(model->index(row, 3)).toString();
                if (translation.contains(searchText, cs)) matched = true;
            }

            // 音标列 (col2)
            if (!matched) {
                QString phonetic = model->data(model->index(row, 2)).toString();
                if (phonetic.contains(searchText, cs)) matched = true;
            }

            view->setRowHidden(row, !matched);
            if (matched) {
                matchCount++;
                if (firstMatchRow == -1) firstMatchRow = row;
            }
        }

        if (matchCount > 0) {
            globalSearchEdit->setStyleSheet(
                "QLineEdit {"
                "   background-color: #E8F5E9;"
                "   color: #333333;"
                "   border: 1px solid #4CAF50;"
                "   border-radius: 4px;"
                "   padding: 6px 10px;"
                "   font-size: 13px;"
                "}"
                );
            if (firstMatchRow != -1) {
                view->scrollTo(model->index(firstMatchRow, 1), QAbstractItemView::PositionAtCenter);
                view->selectRow(firstMatchRow);
            }
            statusBar->showMessage(QString("找到 %1 个匹配的单词").arg(matchCount), 3000);
        } else {
            globalSearchEdit->setStyleSheet(
                "QLineEdit {"
                "   background-color: #FFEBEE;"
                "   color: #333333;"
                "   border: 1px solid #F44336;"
                "   border-radius: 4px;"
                "   padding: 6px 10px;"
                "   font-size: 13px;"
                "}"
                );
            statusBar->showMessage("未找到匹配的单词", 3000);
        }
    };

    connect(globalSearchEdit, &QLineEdit::textChanged, [=](const QString&) {
        searchTimer->start();
    });
    connect(searchTimer, &QTimer::timeout, performGlobalSearch);
    connect(globalSearchEdit, &QLineEdit::returnPressed, [=]() {
        searchTimer->stop();
        performGlobalSearch();
    });
    connect(globalSearchClearBtn, &QPushButton::clicked, [=]() {
        globalSearchEdit->clear();
        globalSearchEdit->setFocus();
        globalSearchEdit->setStyleSheet(
            "QLineEdit {"
            "   background-color: #F5F5F5;"
            "   color: #333333;"
            "   border: 1px solid #E0E0E0;"
            "   border-radius: 4px;"
            "   padding: 6px 10px;"
            "   font-size: 13px;"
            "}"
            );
        for (int row = 0; row < model->rowCount(); ++row) {
            view->setRowHidden(row, false);
        }
        globalSearchClearBtn->setVisible(false);
        statusBar->showMessage("已清除搜索", 2000);
    });

    connect(w, &QWidget::destroyed, [searchTimer]() {
        searchTimer->deleteLater();
    });

    QShortcut* searchShortcut = new QShortcut(QKeySequence("Ctrl+F"), w);
    connect(searchShortcut, &QShortcut::activated, [=]() {
        globalSearchEdit->setFocus();
        globalSearchEdit->selectAll();
    });

    QShortcut* escShortcut = new QShortcut(QKeySequence("Esc"), w);
    connect(escShortcut, &QShortcut::activated, [=]() {
        if (!globalSearchEdit->text().isEmpty()) {
            globalSearchClearBtn->click();
        }
    });

    w->show();
}

// 创建统一样式的按钮（与原来相同）
QPushButton* WordsDatabase::createStyledButton(
    const QString& text,
    const QString& bgColor,
    const QString& hoverColor,
    const QString& pressedColor)
{
    QPushButton* btn = new QPushButton(text);
    btn->setMinimumHeight(40);
    btn->setStyleSheet(QString(
                           "QPushButton {"
                           "   background-color: %1;"
                           "   color: white;"
                           "   border: none;"
                           "   border-radius: 5px;"
                           "   font-size: 13px;"
                           "   font-weight: bold;"
                           "   text-align: left;"
                           "   padding-left: 15px;"
                           "}"
                           "QPushButton:hover {"
                           "   background-color: %2;"
                           "}"
                           "QPushButton:pressed {"
                           "   background-color: %3;"
                           "}"
                           ).arg(bgColor, hoverColor, pressedColor));
    return btn;
}

// 更新计数标签（未使用，保留）
void WordsDatabase::updateCountLabel(QLabel *label)
{
    if (label && model) {
        label->setText(QString("总记录数: %1 条").arg(model->rowCount()));
    }
}

// 保存更改
void WordsDatabase::onSaveChanges()
{
    model->submitAll();
}

// 查找单词（在stardict词典中，与 Words 表无关，不变）
bool WordsDatabase::find(QString word, QList<int> &rowNumbers,
                         QList<QString> &matchWords,
                         QList<QString> &matchTranslation,
                         QList<QString> &matchYB)
{
    QString processedSearchWord = keepOnlyEnglishLetters(word);

    if (processedSearchWord.isEmpty()) {
        return false;
    }

    rowNumbers.clear();
    matchWords.clear();
    matchTranslation.clear();
    matchYB.clear();

    query.prepare("SELECT id, word, phonetic, translation FROM stardict WHERE word = ? COLLATE NOCASE");
    query.addBindValue(processedSearchWord);

    if (query.exec()) {
        while (query.next()) {
            int id = query.value(0).toInt();
            QString dbWord = query.value(1).toString();
            QString dbPhonetic = query.value(2).toString();
            QString dbTranslation = query.value(3).toString();

            QString processedDbWord = keepOnlyEnglishLetters(dbWord);

            if (processedDbWord == processedSearchWord) {
                int rowNumber = id - 1;
                rowNumbers.append(rowNumber);
                matchWords.append(dbWord);
                matchYB.append(dbPhonetic);
                matchTranslation.append(dbTranslation);

                qDebug() << "匹配:" << dbWord << "->" << processedDbWord;
            }
        }
        return !rowNumbers.isEmpty();
    }
    return false;
}

// 只保留英文字母（不变）
QString WordsDatabase::keepOnlyEnglishLetters(const QString &text)
{
    QString result;
    result.reserve(text.length());

    for (const QChar &ch : text) {
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')) {
            result.append(ch.toLower());
        } else if (ch.unicode() >= 0x0100 && ch.unicode() <= 0x017F) {
            result.append(ch.toLower());
        } else if (ch == '-') {
            result.append(ch);
        }
    }

    return result;
}

// 查找熟词库单词（只涉及 word 列，不变）
bool WordsDatabase::findWithRowNumbers(QString word, QList<int> &rowNumbers,
                                       QList<QString> &matchWords)
{
    QString processedSearchWord = keepOnlyEnglishLetters(word);

    if (processedSearchWord.isEmpty()) {
        return false;
    }

    rowNumbers.clear();
    matchWords.clear();

    query.prepare("SELECT id, word FROM Words WHERE word = ? COLLATE NOCASE");
    query.addBindValue(processedSearchWord);

    if (query.exec()) {
        while (query.next()) {
            int id = query.value(0).toInt();
            QString dbWord = query.value(1).toString();

            QString processedDbWord = keepOnlyEnglishLetters(dbWord);

            if (processedDbWord == processedSearchWord) {
                int rowNumber = id - 1;
                rowNumbers.append(rowNumber);
                matchWords.append(dbWord);

                qDebug() << "匹配:" << dbWord << "->" << processedDbWord;
            }
        }
        return !rowNumbers.isEmpty();
    }
    return false;
}

// 导入CSV文件（假设列顺序：单词, 音标, 翻译）
void WordsDatabase::importFromCsv()
{
    QString fileName = QFileDialog::getOpenFileName(view,
                                                    "选择CSV文件",
                                                    QDir::homePath(),
                                                    "CSV文件 (*.csv);;所有文件 (*.*)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(view, "错误", "无法打开文件: " + file.errorString());
        return;
    }

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    QString allData = stream.readAll();
    file.close();

    // 状态机解析 CSV
    QList<QStringList> rows;
    QStringList currentRow;
    QString currentField;
    bool inQuotes = false;
    bool afterQuote = false;

    for (int i = 0; i < allData.size(); ++i) {
        QChar ch = allData[i];
        if (inQuotes) {
            if (ch == '"') {
                if (i + 1 < allData.size() && allData[i + 1] == '"') {
                    currentField += '"';
                    ++i;
                } else {
                    inQuotes = false;
                    afterQuote = true;
                }
            } else {
                currentField += ch;
            }
        } else {
            if (ch == '"') {
                inQuotes = true;
                afterQuote = false;
            } else if (ch == ',') {
                currentRow.append(currentField);
                currentField.clear();
                afterQuote = false;
            } else if (ch == '\n' || ch == '\r') {
                if (ch == '\r' && i + 1 < allData.size() && allData[i + 1] == '\n') {
                    ++i;
                }
                if (!currentField.isEmpty() || afterQuote || !currentRow.isEmpty()) {
                    currentRow.append(currentField);
                    rows.append(currentRow);
                }
                currentRow.clear();
                currentField.clear();
                afterQuote = false;
            } else {
                currentField += ch;
                afterQuote = false;
            }
        }
    }

    if (!currentField.isEmpty() || !currentRow.isEmpty() || afterQuote) {
        currentRow.append(currentField);
        rows.append(currentRow);
    }

    // 开始事务
    if (!db.transaction()) {
        QMessageBox::warning(view, "错误", "无法开始事务");
        return;
    }

    int successCount = 0;
    int skipCount = 0;
    int errorCount = 0;
    QStringList errors;
    int lineNum = 0;

    for (const QStringList &fields : rows) {
        lineNum++;

        // 跳过空行
        bool allEmpty = true;
        for (const QString &f : fields) {
            if (!f.isEmpty()) { allEmpty = false; break; }
        }
        if (allEmpty) continue;

        // 跳过可能的表头行（第一列包含“单词”或“word”）
        if (lineNum == 1 && !fields.isEmpty()) {
            QString firstFieldLower = fields[0].toLower();
            if (firstFieldLower.contains("单词") || firstFieldLower.contains("word")) {
                continue;
            }
        }

        // 提取字段：假设 CSV 列为 单词, 音标, 翻译
        QString word = fields.size() > 0 ? fields[0] : QString();
        QString phonetic = fields.size() > 1 ? fields[1] : QString();
        QString translation = fields.size() > 2 ? fields[2] : QString();

        word = word.trimmed();
        phonetic = phonetic.trimmed();
        translation = translation.trimmed();

        if (word.isEmpty()) {
            errorCount++;
            errors << QString("第%1行: 单词字段为空").arg(lineNum);
            continue;
        }

        QSqlQuery insertQuery(db);
        insertQuery.prepare("INSERT OR IGNORE INTO Words (word, phonetic, translation) VALUES (?, ?, ?)");
        insertQuery.addBindValue(word);
        insertQuery.addBindValue(phonetic);
        insertQuery.addBindValue(translation);

        if (insertQuery.exec()) {
            if (insertQuery.numRowsAffected() > 0) {
                successCount++;
            } else {
                skipCount++;
            }
        } else {
            errorCount++;
            errors << QString("第%1行: 插入失败 - %2")
                          .arg(lineNum).arg(insertQuery.lastError().text());
        }
    }

    if (!db.commit()) {
        QMessageBox::warning(view, "错误", "提交事务失败: " + db.lastError().text());
        db.rollback();
        return;
    }

    if (successCount > 0 || skipCount > 0)
        model->select();

    QString resultMsg = QString("CSV导入完成:\n"
                                "成功导入: %1 个新单词\n"
                                "跳过(已存在): %2 个\n"
                                "失败: %3 行")
                            .arg(successCount)
                            .arg(skipCount)
                            .arg(errorCount);

    if (!errors.isEmpty()) {
        resultMsg += "\n\n详细错误信息:\n";
        for (int i = 0; i < qMin(10, errors.size()); ++i)
            resultMsg += errors[i] + "\n";
        if (errors.size() > 10)
            resultMsg += QString("... 还有 %1 条错误").arg(errors.size() - 10);
    }

    QMessageBox::information(view, "导入结果", resultMsg);
}

// 导出到CSV文件（顺序：单词, 音标, 翻译）
void WordsDatabase::exportToCsv()
{
    QString fileName = QFileDialog::getSaveFileName(view,
                                                    "导出CSV文件",
                                                    QDir::homePath() + "/words_export.csv",
                                                    "CSV文件 (*.csv);;所有文件 (*.*)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(view, "错误", "无法创建文件: " + file.errorString());
        return;
    }

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);

    int exportCount = 0;

    for (int row = 0; row < model->rowCount(); ++row) {
        QString word = model->data(model->index(row, 1)).toString();
        QString phonetic = model->data(model->index(row, 2)).toString();
        QString translation = model->data(model->index(row, 3)).toString();

        auto needsQuotes = [](const QString &field) {
            return field.contains(',') || field.contains('"') ||
                   field.contains('\n') || field.contains('\r');
        };

        auto escapeField = [](QString field) -> QString {
            if (field.contains('"')) {
                field.replace("\"", "\"\"");
            }
            return field;
        };

        if (needsQuotes(word)) {
            word = "\"" + escapeField(word) + "\"";
        }
        if (needsQuotes(phonetic)) {
            phonetic = "\"" + escapeField(phonetic) + "\"";
        }
        if (needsQuotes(translation)) {
            translation = "\"" + escapeField(translation) + "\"";
        }

        stream << word << "," << phonetic << "," << translation << "\n";
        exportCount++;
    }

    file.close();

    QMessageBox::information(view, "导出成功",
                             QString("成功导出 %1 条记录到:\n%2").arg(exportCount).arg(fileName));
}

// 辅助函数：转义 JSON 字符串中的特殊字符
static QString escapeJsonString(const QString &s)
{
    QString escaped;
    escaped.reserve(s.size() * 1.2);
    for (QChar ch : s) {
        if (ch == '\\') escaped += "\\\\";
        else if (ch == '"') escaped += "\\\"";
        else if (ch == '\n') escaped += "\\n";
        else if (ch == '\r') escaped += "\\r";
        else if (ch == '\t') escaped += "\\t";
        else escaped += ch;
    }
    return escaped;
}

// 导出到JSON文件（键名保持 word, phonetic, translation，但值来源已调整）
void WordsDatabase::exportToJson()
{
    QString fileName = QFileDialog::getSaveFileName(view,
                                                    "导出JSON文件",
                                                    QDir::homePath() + "/words_export.json",
                                                    "JSON文件 (*.json);;所有文件 (*.*)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(view, "错误", "无法创建文件: " + file.errorString());
        return;
    }

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);

    stream << "{\n";
    stream << "  \"words\": [\n";

    int exportCount = 0;

    for (int row = 0; row < model->rowCount(); ++row) {
        QString word = model->data(model->index(row, 1)).toString();
        QString phonetic = model->data(model->index(row, 2)).toString();
        QString translation = model->data(model->index(row, 3)).toString();

        word = escapeJsonString(word);
        phonetic = escapeJsonString(phonetic);
        translation = escapeJsonString(translation);

        stream << "    {\n";
        stream << "      \"word\": \"" << word << "\",\n";
        stream << "      \"phonetic\": \"" << phonetic << "\",\n";
        stream << "      \"translation\": \"" << translation << "\"\n";
        stream << "    }";

        if (row < model->rowCount() - 1) {
            stream << ",";
        }
        stream << "\n";
        exportCount++;
    }

    stream << "  ]\n";
    stream << "}\n";

    file.close();

    QMessageBox::information(view, "导出成功",
                             QString("成功导出 %1 条记录到:\n%2").arg(exportCount).arg(fileName));
}
