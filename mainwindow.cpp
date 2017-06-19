#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mdiArea(new QMdiArea)
{
    ui->setupUi(this);
    setWindowTitle(tr("PCI Express控制器 V3.01"));
    setWindowIcon(QIcon(tr(":/images/PCIeController.png")));
    createStatusBar();
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    //mdiArea->setViewMode(QMdiArea::TabbedView);
    //mdiArea->setTabPosition(QTabWidget::South);
    //mdiArea->setTabsClosable(true);
    //mdiArea->setTabsMovable(true);
    setCentralWidget(mdiArea);

    comboWorkspace = new QComboBox();
    comboWorkspace->setEditable(true);
    comboWorkspace->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    comboDirList << "E:/Qt/Workspace" << "E:/Qt";
    comboWorkspace->addItems(comboDirList);
    ui->toolBarWorkspace->addWidget(comboWorkspace);
    strCurrentWorkspace = comboWorkspace->currentText();

    bControlWorkspace = false;

    createDockWindows();

    m_iconDriver = QIcon(":/images/driver.png");
    m_iconFolder = QIcon(":images/folder.png");
    m_iconFile = QIcon(":/images/file.png");
    // 显示文件视图
    updateFileView(strCurrentWorkspace);

    logger = new Logger();
    connect(this, SIGNAL(loggerWrite(QString)), logger, SLOT(loggerWrite(QString)));
    connect(this, SIGNAL(loggerWrite(QString)), this, SLOT(loggerOutput(QString)));
    connect(listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(on_listWidget_itemDoubleClicked(QListWidgetItem*)));
    connect(listWidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(on_listWidget_currentItemChanged(QListWidgetItem*,QListWidgetItem*)));

    // setStyleSheet("background-color:rgb(199, 237, 204)");
    GetPCIExpressList();
    //
    initPCIeConfigParameter();
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::getCurrentWorkspace() const
{
    return strCurrentWorkspace;
}

void MainWindow::setCurretWorkspace(QString strWorkspace)
{
    strCurrentWorkspace = strWorkspace;
}

void MainWindow::showCurveGraph(QCustomPlot *customPlot)
{
    QVector<double> temp(10);
    QVector<double> temp1(10);

    for(int i = 0; i < 9; i++)
    {
        num[i] = num[i+1];
    }
    num[9] = n;
    for(int i = 0; i < 10; i++)
    {
        temp[i] = i;
        temp1[i] = num[i];
    }
    //graph1 = ui->qcustomplot->addGraph();//增加一条曲线图层
    customPlot->addGraph();
    customPlot->graph(0)->setPen(QPen(Qt::blue));
    customPlot->graph(0)->setData(temp, temp1);
    customPlot->graph(0)->setName(QString("速率曲线"));

    customPlot->xAxis->setLabel("时间：秒");
    customPlot->yAxis->setLabel("存储速率：MB/s");

    customPlot->xAxis->setRange(temp[0], temp[9]);
    customPlot->yAxis->setRange(0, 100);
    customPlot->replot();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton closeButton;
    closeButton = QMessageBox::question(this,
                                        tr("退出程序"),
                                        tr("确认退出程序？"),
                                        QMessageBox::Yes | QMessageBox::No);

    if (closeButton == QMessageBox::No)
    {
        event->ignore();  // 忽略退出信号，程序继续运行
    }
    else if (closeButton == QMessageBox::Yes)
    {
        event->accept();  // 接受退出信号，程序退出
    }
}

void MainWindow::showGraph()
{
    QTime t;
    t = QTime::currentTime();
    qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
    n = qrand() % 50;
    showCurveGraph(curveWidget);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("就绪"));
}

void MainWindow::createDockWindows()
{
    QDockWidget *logView = new QDockWidget(tr("运行日志"), this);
    //dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    logWidget = new QListWidget(logView);
    logView->setWidget(logWidget);
    addDockWidget(Qt::BottomDockWidgetArea, logView);
    //
    QDockWidget* fileView = new QDockWidget(tr("文件视图"), this);
    listWidget = new QListWidget(fileView);
    fileView->setWidget(listWidget);
    addDockWidget(Qt::LeftDockWidgetArea, fileView);
    //
    QDockWidget* propertyWidget = new QDockWidget(tr("属性视图"), this);
    tableView = new QTableView();
    propertyWidget->setWidget(tableView);
    addDockWidget(Qt::RightDockWidgetArea, propertyWidget);
}

HighSpeedWindow *MainWindow::createMdiChild()
{
    HighSpeedWindow *child = new HighSpeedWindow;
    mdiArea->addSubWindow(child);

    return child;
}

void MainWindow::loggerOutput(const QString strContext)
{
    QString strTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString strlog = strTime + "\t" + strContext;
    //
    logWidget->addItem(strlog);
}

void MainWindow::updateFileView(const QString strDir)
{
    QDir dirNew(strDir);
    if (dirNew.exists())
    {
        m_dirCur = QDir(dirNew.canonicalPath());
        showItem(m_dirCur);
    }
}

void MainWindow::on_action_Back_triggered()
{
    if (comboDirList.size() <= 1)
    {
        return;
    }
    bControlWorkspace = true;
    strCurrentWorkspace = comboDirList.at(1);
    //comboWorkspace->setCurrentText(strCurrentWorkspace);
    comboWorkspace->clear();
    comboDirList.insert(0, strCurrentWorkspace);
    comboDirList.removeDuplicates();
    comboWorkspace->addItems(comboDirList);
    //
    updateFileView(strCurrentWorkspace);
    //
    emit loggerWrite("进入:" + strCurrentWorkspace);
}

void MainWindow::on_action_Forward_triggered()
{
    if (comboDirList.size() <= 1)
    {
        return;
    }
    if (bControlWorkspace)
    {
        bControlWorkspace = false;
        strCurrentWorkspace = comboDirList.at(1);
        comboWorkspace->clear();
        comboDirList.insert(0, strCurrentWorkspace);
        comboDirList.removeDuplicates();
        comboWorkspace->addItems(comboDirList);
        //
        updateFileView(strCurrentWorkspace);
        //
        emit loggerWrite("进入:" + strCurrentWorkspace);
    }
}

void MainWindow::on_action_Upper_triggered()
{
    strCurrentWorkspace = comboWorkspace->currentText();
    QDir currentDir(strCurrentWorkspace);
    if (currentDir.cdUp())
    {
        strCurrentWorkspace = currentDir.absolutePath();
        comboWorkspace->clear();
        comboDirList.insert(0, strCurrentWorkspace);
        comboDirList.removeDuplicates();
        comboWorkspace->addItems(comboDirList);
        //
        updateFileView(strCurrentWorkspace);
        //
        emit loggerWrite("进入:" + strCurrentWorkspace);
    }
}

void MainWindow::on_action_Lookup_triggered()
{
    strCurrentWorkspace = comboWorkspace->currentText();
    //
    QString strWorkspace = QFileDialog::getExistingDirectory(this, "请选择工作空间", strCurrentWorkspace);
    if (strWorkspace.isEmpty())
    {
        return;
    }
    else
    {
        strCurrentWorkspace = strWorkspace;
    }
    comboWorkspace->setCurrentText(strCurrentWorkspace);
    comboWorkspace->clear();
    comboDirList.insert(0, strCurrentWorkspace);
    comboDirList.removeDuplicates();
    comboWorkspace->addItems(comboDirList);
    //
    updateFileView(strCurrentWorkspace);
    //
    emit loggerWrite("设置工作目录:" + strCurrentWorkspace);
}
// 绘制速率曲线
void MainWindow::on_action_Curve_triggered()
{
    // 停靠方式
    QDockWidget *dock = new QDockWidget(tr("速率曲线"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    curveWidget = new QCustomPlot();
    QBrush qBrush(QColor(255, 255, 255)); // 设置背景色
    curveWidget->setBackground(qBrush);
    // 放在多文档区域
    //mdiArea->addSubWindow(curveWidget);
    //curveWidget->move(320, 240);
    //curveWidget->show();

    for(int i = 0; i < 10; i++)
    {
        num[i] = 0;
    }
    n = 0;
    QTimer *timer = new QTimer(this);
    timer->start(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(showGraph()));

    dock->setWidget(curveWidget);
    addDockWidget(Qt::RightDockWidgetArea, dock);

    emit loggerWrite("打开速率曲线");
}

// 代码复制粘贴而来
void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    // 判断 item 的类型
    int theType = item->type();

    if(IDriver == theType)    // 如果是分区根
    {
        // 根路径
        QString strFullPath = item->text();
        // 设置当前目录
        m_dirCur = QDir(strFullPath);
        // 更新列表控件
        showItem(m_dirCur);
    }
    else if(IFolder == theType)   // 如果是文件夹
    {
        // 先对 . 和 .. 做排除
        QString strName = item->text();
        if(tr(".") == strName)
        {
            // 直接返回工作目录
            m_dirCur = QDir(strCurrentWorkspace);
            showItem(m_dirCur);
            // 返回
            return;
        }
        else if(tr("..") == strName)
        {
            if (m_dirCur.absolutePath() == strCurrentWorkspace)
            {
                return;
            }
            // 切换父目录
            if (m_dirCur.cdUp())
            {
                showItem(m_dirCur);
            }
            // 返回
            return;
        }
        // 正常的子文件夹处理
        QString strFullPath = m_dirCur.absolutePath() + tr("/") + strName;
        strFullPath = QDir::cleanPath(strFullPath); //清理多余的斜杠
        // 切换路径
        m_dirCur = QDir(strFullPath);
        // 更新列表控件
        showItem(m_dirCur);
    }
    else// 普通文件
    {
        //拼接出路径字符串
        QString strFilePath = m_dirCur.absolutePath() + tr("/") + item->text();
        strFilePath = QDir::cleanPath(strFilePath); //清理多余的斜杠
        /// 切记这样的转换，比较有用：QString转LPCWSTR
        ShellExecute(NULL, NULL, strFilePath.toStdWString().c_str(), NULL, NULL, SW_SHOW);
        //如果不是内嵌资源文件，是本地文件系统的
        if(strFilePath.startsWith(tr(":")))
        {
            //调用系统浏览器打开
            QDesktopServices::openUrl(QUrl::fromLocalFile(strFilePath));
        }
    }
}
// 代码复制粘贴而来
void MainWindow::on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    // 如果当前条目为空则返回
    if(NULL == current)
    {
        return;
    }
    // 条目信息对象
    QFileInfo fi;
    // 记录信息字符串
    QString strResult;
    // 条目类型
    int theType = current->type();
    // 条目名称
    QString strName = current->text();
    // 判断类型
    if(IDriver == theType)    //分区根
    {
        // 分区根路径
        fi = QFileInfo(strName);
        if(strName.startsWith(tr(":")))
        {
            strResult += tr("资源根 %1").arg(strName); //显示绝对路径
        }
        else
        {
            strResult += tr("分区根 %1").arg(strName); //显示绝对路径
        }
    }
    else if(IFolder == theType)   //文件夹
    {
        //完整路径
        QString strFullPath = m_dirCur.absolutePath() + tr("/") + strName;
        strFullPath = QDir::cleanPath(strFullPath); //清理多余的斜杠
        strResult += tr("文件夹 %1\r\n").arg(strFullPath); //显示绝对路径
        //获取信息，添加到结果串
        fi = QFileInfo(strFullPath);
        strResult += getFolderInfo(fi);
    }
    else    //文件
    {
        //完整文件名
        QString strFilePath = m_dirCur.absolutePath() + tr("/") + strName;
        strFilePath = QDir::cleanPath(strFilePath); //清理多余的斜杠
        strResult += tr("文件 %1\r\n").arg(strFilePath); //显示绝对路径
        //获取文件信息，添加到结果串
        fi = QFileInfo(strFilePath);
        strResult += getFileInfo(fi);
    }
    //显示结果信息
    //ui->plainTextEditInfo->setPlainText(strResult);
}

void MainWindow::showItem(const QDir &dir)
{
    if (!dir.exists())
    {
        return;
    }
    QFileInfoList li = dir.entryInfoList(QDir::NoFilter, QDir::DirsFirst);
    int nCount = li.count();
    listWidget->clear();
    for (int i = 0; i < nCount; i++)
    {
        QString name = li[i].fileName();
        if (li[i].isDir())
        {
            QListWidgetItem *itemFolder =
                    new QListWidgetItem(m_iconFolder, name, NULL, IFolder);
            listWidget->addItem(itemFolder);
        }
        else
        {
            QListWidgetItem *itemFile =
                    new QListWidgetItem(m_iconFile, name, NULL, IFile);
            listWidget->addItem(itemFile);
        }
    }
}

QString MainWindow::getFileInfo(const QFileInfo &fi)
{
    QString strResult;  //用于返回的串
    //判断文件信息
    if(fi.isReadable())
    {
        strResult += tr("可读：是\r\n");
    }
    else
    {
        strResult += tr("可读：否\r\n");
    }
    if(fi.isWritable())
    {
        strResult += tr("可写：是\r\n");
    }
    else
    {
        strResult += tr("可写：否\r\n");
    }
    if(fi.isExecutable())
    {
        strResult += tr("可执行：是\r\n");
    }
    else
    {
        strResult += tr("可执行：否\r\n");
    }
    //类型
    strResult += tr("类型：%1\r\n").arg(fi.suffix());
    //大小
    strResult += tr("大小：%1 B\r\n").arg(fi.size());
    //时间
    QDateTime dtCreate = fi.created();
    QDateTime dtModify = fi.lastModified();
    strResult += tr("创建时间：%1\r\n").arg(dtCreate.toString("yyyy-MM-dd HH:mm:ss"));
    strResult += tr("修改时间：%1\r\n").arg(dtModify.toString("yyyy-MM-dd HH:mm:ss"));

    //返回
    return strResult;
}

QString MainWindow::getFolderInfo(const QFileInfo &fi)
{
    QString strResult;  //用于返回的串
    //判断文件夹信息
    if(fi.isReadable()) //是否可读
    {
        strResult += tr("可读：是\r\n");
    }
    else
    {
        strResult += tr("可读：否\r\n");
    }
    if(fi.isWritable()) //是否可写
    {
        strResult += tr("可写：是\r\n");
    }
    else
    {
        strResult += tr("可写：否\r\n");
    }
    //时间
    QDateTime dtCreate = fi.created();
    QDateTime dtModify = fi.lastModified();
    strResult += tr("创建时间：%1\r\n").arg(dtCreate.toString("yyyy-MM-dd HH:mm:ss"));
    strResult += tr("修改时间：%1\r\n").arg(dtModify.toString("yyyy-MM-dd HH:mm:ss"));
    //返回
    return strResult;
}

void MainWindow::on_action_Database_triggered()
{
    QString strPath = QDir::currentPath();
    strPath += tr("/SQLiteSpy.exe");
    //
    QProcess *process = new QProcess;
    process->start(strPath);

    emit loggerWrite("启动数据库管理软件：" + strPath);
}

void MainWindow::on_action_Parameter_triggered()
{
    emit loggerWrite("启动参数设置");
}

void MainWindow::on_action_AddHighSpee_triggered()
{
    static int nOffset = 0;
    HighSpeedWindow *child = createMdiChild();
    // 日志信号之间连接combo
    child->setWorkspace(comboWorkspace->currentText());
    //
    connect(child, SIGNAL(loggerWrite(QString)), this, SIGNAL(loggerWrite(QString)));
    //
    child->resize(320 + nOffset, 240 + nOffset);
    nOffset += 10;
    child->show();

    QSqlQuery query;
    query.prepare("insert into tbl_highspeed values(:windowTitle, :typeId,"
                  ":srcAddrOffset, :srcAddrValue,"
                  ":dstAddrOffset, :dstAddrValue,"
                  ":typeStateOffset, :typeStateValue,"
                  ":velocityIdOffset, :velocityIdValue,"
                  ":validSizeOffset, :validSizeValue,"
                  ":timeGapOffset, :timeGapValue,"
                  ":startOrstopOffset, :startOrstopValue)");
    query.bindValue(":windowTitle", child->getWindowTitle());
    query.bindValue(":typeId", "2"); // 高速标记2
    query.bindValue(":srcAddrOffset", "60");
    query.bindValue(":srcAddrValue", "55");
    query.bindValue(":dstAddrOffset", "64");
    query.bindValue(":dstAddrValue", "77");
    query.bindValue(":typeStateOffset", "68");
    query.bindValue(":typeStateValue", "69");
    query.bindValue(":velocityIdOffset", "6c");
    query.bindValue(":velocityIdValue", "3c");
    query.bindValue(":validSizeOffset", "70");
    query.bindValue(":validSizeValue", "100");
    query.bindValue(":timeGapOffset", "74");
    query.bindValue(":timeGapValue", "50");
    query.bindValue(":startOrstopOffset", "78");
    query.bindValue(":startOrstopValue", "0");
    bool bOk = query.exec();
    //
    emit loggerWrite("创建" + child->getWindowTitle());
}

void MainWindow::on_action_AddMediumSpeed_triggered()
{
    emit loggerWrite("添加1Gbps中速PCIe窗口");
}

void MainWindow::on_action_AddSingleWire_triggered()
{
    emit loggerWrite("添加100Mbps单线PCIe窗口");
}

void MainWindow::on_action_AddTripleWire_triggered()
{
    emit loggerWrite("添加10Mbps三线PCIe窗口");
}

void MainWindow::on_action_Save_triggered()
{
    QList<QMdiSubWindow *> subWindowList = mdiArea->subWindowList();

    emit loggerWrite("保存软件参数");
}
