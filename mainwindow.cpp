#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mdiArea(new QMdiArea)
{
    ui->setupUi(this);
    setWindowTitle(tr("PCI Express控制器 V3.00"));
    setWindowIcon(QIcon(tr(":/images/PCIeController.png")));
    createStatusBar();
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(mdiArea);

    comboWorkspace = new QComboBox();
    comboWorkspace->setEditable(true);
    comboWorkspace->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    comboDirList << "E:/Qt/Workspace" << "E:/Qt";
    comboWorkspace->addItems(comboDirList);
    ui->toolBarWorkspace->addWidget(comboWorkspace);

    bControlWorkspace = false;
/*
    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // toolBar is a pointer to an existing toolbar
    ui->toolBarWorkspace->addWidget(spacer);
    ui->toolBarWorkspace->addAction("Right-aligned button");
*/
    createDockWindows();
    //setStyleSheet("background-color:rgb(199, 237, 204)");
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

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton closeButton;
    closeButton = QMessageBox::question(this, tr("退出程序"),
                                   QString(tr("确认退出程序?")),
                                   QMessageBox::Yes | QMessageBox::No);
    //
    if (closeButton == QMessageBox::No)
    {
        event->ignore();  // 忽略退出信号，程序继续运行
    }
    else if (closeButton == QMessageBox::Yes)
    {
        event->accept();  // 接受退出信号，程序退出
    }
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("就绪"));
}

void MainWindow::createDockWindows()
{
    QDockWidget *logWidget = new QDockWidget(tr("运行日志"), this);
    //dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    customerList = new QListWidget(logWidget);
    customerList->addItems(QStringList()
            << "John Doe, Harmony Enterprises, 12 Lakeside, Ambleton"
            << "Jane Doe, Memorabilia, 23 Watersedge, Beaton"
            << "Tammy Shea, Tiblanka, 38 Sea Views, Carlton"
            << "Tim Sheen, Caraba Gifts, 48 Ocean Way, Deal"
            << "Sol Harvey, Chicos Coffee, 53 New Springs, Eccleston"
            << "Sally Hobart, Tiroli Tea, 67 Long River, Fedula");
    logWidget->setWidget(customerList);
    addDockWidget(Qt::BottomDockWidgetArea, logWidget);

    QDockWidget* modelWidget = new QDockWidget(tr("模型视图"), this);
    dirModel = new QDirModel();
    dirModel->setReadOnly(false);
    dirModel->setSorting(QDir::DirsFirst | QDir::IgnoreCase | QDir::Name);
    treeView = new QTreeView();
    treeView->setModel(dirModel);
    /*
    QModelIndex index = dirModel->index("D:/");
    ui->treeView->expand(index);
    ui->treeView->scrollTo(index);
    ui->treeView->setCurrentIndex(index);
    ui->treeView->resizeColumnToContents(0);
*/
    modelWidget->setWidget(treeView);
    addDockWidget(Qt::LeftDockWidgetArea, modelWidget);

    QDockWidget* propertyWidget = new QDockWidget(tr("属性视图"), this);
    tableView = new QTableView();
    propertyWidget->setWidget(tableView);
    addDockWidget(Qt::RightDockWidgetArea, propertyWidget);
}
/*
MdiChild *MainWindow::createMdiChild()
{
    MdiChild *child = new MdiChild;
    mdiArea->addSubWindow(child);

    return child;
}
*/
HighSpeedWindow *MainWindow::createMdiChild()
{
    HighSpeedWindow *child = new HighSpeedWindow;
    mdiArea->addSubWindow(child);

    return child;
}
void MainWindow::on_action_Add_Device_triggered()
{
    static int nOffset = 0;
    HighSpeedWindow *child = createMdiChild();
    child->resize(320 + nOffset, 240 + nOffset);
    nOffset += 10;
    child->show();
}
/*
void MainWindow::on_action_Add_Device_triggered()
{
    MdiChild *child = createMdiChild();
    child->newFile();
    child->show();
}
*/

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
}

void MainWindow::on_action_Curve_triggered()
{
    QDockWidget *dock = new QDockWidget(tr("速率曲线"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    QListWidget* qlist = new QListWidget(dock);
    qlist->addItems(QStringList()
            << "John Doe, Harmony Enterprises, 12 Lakeside, Ambleton"
            << "Sol Harvey, Chicos Coffee, 53 New Springs, Eccleston"
            << "Sally Hobart, Tiroli Tea, 67 Long River, Fedula");
    dock->setWidget(qlist);
    addDockWidget(Qt::RightDockWidgetArea, dock);
}
