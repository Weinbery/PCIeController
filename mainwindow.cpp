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

    createDockWindows();
    setStyleSheet("background-color:rgb(199, 237, 204)");
}

MainWindow::~MainWindow()
{
    delete ui;
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
    modelWidget->setWidget(&treeWidget);
    addDockWidget(Qt::LeftDockWidgetArea, modelWidget);

    QDockWidget* propertyWidget = new QDockWidget(tr("属性视图"), this);
    propertyWidget->setWidget(&tableWidget);
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
