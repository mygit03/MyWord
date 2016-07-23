#include "myword.h"

#include <QDebug>
#include <QMenuBar>
#include <QTextEdit>
#include <QFileInfo>
#include <QFileDialog>
#include <QMdiSubWindow>
#include <QStatusBar>
#include <QColorDialog>

MyWord::MyWord(QWidget *parent)
    : QMainWindow(parent)
{
    init();

    connectSignals();
}

MyWord::~MyWord()
{

}

void MyWord::init()
{
    resize(550, 600);
    setWindowIcon(QIcon(":/notepad.png"));

    mdiArea = new QMdiArea(this);
//    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
//    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(mdiArea);
//    connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(slot_updateMenus()));

    windowMapper = new QSignalMapper(this);
    connect(windowMapper, SIGNAL(mapped(QWidget*)), this,
            SLOT(setActiveSubWindow(QWidget*)));

    creatActions();
    creatWidgets();
    creatMenu();
    creatToolBar();
//    slot_updateMenus();
}

void MyWord::creatActions()
{
    newAct = new QAction(QIcon(":/new.png"),"新建", this);
    openAct = new QAction(QIcon(":/open.png"), "打开", this);
    saveAct = new QAction(QIcon(":/save.png"), "保存", this);
    saveAsAct = new QAction(QIcon(":/save.png"), "另存为", this);
    copyAct = new QAction(QIcon(":/copy.png"), "复制", this);
    cutAct = new QAction(QIcon(":/cut.png"), "剪切", this);
    pasteAct = new QAction(QIcon(":/paste.png"), "粘贴", this);
    undoAct = new QAction(QIcon(":/undo.png"), "撤销", this);
    redoAct = new QAction(QIcon(":/redo.png"), "恢复", this);

    QActionGroup *grp = new QActionGroup(this);
    connect(grp, SIGNAL(triggered(QAction*)), this, SLOT(slot_textAlign(QAction*)));
    leftAlign = new QAction(QIcon(":/textleft.png"), "左对齐", grp);
    leftAlign->setCheckable(true);
    centerAlign = new QAction(QIcon(":/textcenter.png"), "居中", grp);
    centerAlign->setCheckable(true);
    rightAlign = new QAction(QIcon(":/textright.png"), "右对齐", grp);
    rightAlign->setCheckable(true);
    justifyAlign = new QAction(QIcon(":/textjustify.png"), "两端对齐", grp);
    justifyAlign->setCheckable(true);

    bold = new QAction(QIcon(":/textbold.png"), "加粗", this);
    bold->setCheckable(true);
    italic = new QAction(QIcon(":/textitalic.png"), "斜体", this);
    italic->setCheckable(true);
    underLine = new QAction(QIcon(":/textunder.png"), "下划线", this);
    underLine->setCheckable(true);
    exitAct = new QAction(QIcon(":/exit.png"), "退出", this);

    QPixmap pix(20, 20);
    pix.fill(Qt::black);
    fontColor = new QAction(pix, "字体颜色", this);

    /*【窗口】菜单动作集*/
    closeAct = new QAction(tr("关闭(&O)"), this);
    closeAct->setStatusTip(tr("关闭活动文档子窗口"));
    connect(closeAct, SIGNAL(triggered()),mdiArea, SLOT(closeActiveSubWindow()));           //不用自己实现

    closeAllAct = new QAction(tr("关闭所有(&A)"), this);
    closeAllAct->setStatusTip(tr("关闭所有子窗口"));
    connect(closeAllAct, SIGNAL(triggered()),mdiArea, SLOT(closeAllSubWindows()));          //不用自己实现

    tileAct = new QAction(tr("平铺(&T)"), this);
    tileAct->setStatusTip(tr("平铺子窗口"));
    connect(tileAct, SIGNAL(triggered()), mdiArea, SLOT(tileSubWindows()));                 //不用自己实现

    cascadeAct = new QAction(tr("层叠(&C)"), this);
    cascadeAct->setStatusTip(tr("层叠子窗口"));
    connect(cascadeAct, SIGNAL(triggered()), mdiArea, SLOT(cascadeSubWindows()));           //不用自己实现

    nextAct = new QAction(tr("下一个(&X)"), this);
    nextAct->setShortcuts(QKeySequence::NextChild);
    nextAct->setStatusTip(tr("移动焦点到下一个子窗口"));
    connect(nextAct, SIGNAL(triggered()),mdiArea, SLOT(activateNextSubWindow()));           //不用自己实现

    lastAct = new QAction(tr("前一个(&V)"), this);
    lastAct->setShortcuts(QKeySequence::PreviousChild);
    lastAct->setStatusTip(tr("移动焦点到前一个子窗口"));
    connect(lastAct, SIGNAL(triggered()),mdiArea, SLOT(activatePreviousSubWindow()));   //不用自己实现

}

void MyWord::creatWidgets()
{
    fontCBox = new QFontComboBox(this);
    fontCBox->setToolTip("字体");

    sizeCBox = new QComboBox(this);
    sizeCBox->setEditable(true);
    sizeCBox->setToolTip("字体大小");
    QFontDatabase db;
    foreach (int size, db.standardSizes()) {
        sizeCBox->addItem(QString::number(size));
    }
    qDebug() << fontCBox->size() << sizeCBox->size();
}

void MyWord::creatMenu()
{
    fileMenu = menuBar()->addMenu("文件(&F)");
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addAction(exitAct);
    editMenu = menuBar()->addMenu("编辑(&E)");
    editMenu->addAction(copyAct);
    editMenu->addAction(cutAct);
    editMenu->addAction(pasteAct);
    //【窗口】主菜单
    winMenu = menuBar()->addMenu(tr("窗口(&W)"));
    slot_winMenu();
    connect(winMenu, SIGNAL(aboutToShow()), this, SLOT(slot_winMenu()));
    menuBar()->addSeparator();
}

void MyWord::creatToolBar()
{
    fileToolbar = addToolBar("文件");
    fileToolbar->addAction(newAct);
    fileToolbar->addAction(openAct);
    fileToolbar->addAction(saveAct);

    editToolbar = addToolBar("编辑");
    editToolbar->addAction(copyAct);
    editToolbar->addAction(cutAct);
    editToolbar->addAction(pasteAct);
    editToolbar->addAction(undoAct);
    editToolbar->addAction(redoAct);
    editToolbar->addSeparator();
    editToolbar->addAction(bold);
    editToolbar->addAction(italic);
    editToolbar->addAction(underLine);
    editToolbar->addSeparator();
    editToolbar->addAction(leftAlign);
    editToolbar->addAction(centerAlign);
    editToolbar->addAction(rightAlign);
    editToolbar->addAction(justifyAlign);
    editToolbar->addSeparator();
    editToolbar->addAction(exitAct);

    addToolBarBreak(Qt::TopToolBarArea);

    fontToolbar = addToolBar("字体");
    fontToolbar->addWidget(fontCBox);
    fontToolbar->addWidget(sizeCBox);
    fontToolbar->addAction(fontColor);
}

void MyWord::connectSignals()
{
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
    connect(newAct, SIGNAL(triggered()), this, SLOT(slot_newFile()));
    connect(openAct, SIGNAL(triggered()), this, SLOT(slot_openFile()));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(slot_saveFile()));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(slot_saveAsFile()));
    connect(copyAct, SIGNAL(triggered()), this, SLOT(slot_copy()));
    connect(cutAct, SIGNAL(triggered()), this, SLOT(slot_cut()));
    connect(pasteAct, SIGNAL(triggered()), this, SLOT(slot_paste()));
    connect(undoAct, SIGNAL(triggered()), this, SLOT(slot_undo()));
    connect(redoAct, SIGNAL(triggered()), this, SLOT(slot_redo()));
    connect(bold, SIGNAL(triggered()), this, SLOT(slot_bold()));
    connect(italic, SIGNAL(triggered()), this, SLOT(slot_italic()));
    connect(underLine, SIGNAL(triggered()), this, SLOT(slot_underLine()));

    connect(fontCBox, SIGNAL(activated(QString)), this, SLOT(slot_font(QString)));
    connect(sizeCBox, SIGNAL(activated(QString)), this, SLOT(slot_fontSize(QString)));
    connect(fontColor, SIGNAL(triggered()), this, SLOT(slot_fontColor()));
}

//打开文件时用，遍历该文件是否已经打开
QMdiSubWindow *MyWord::findOpenFile(const QString &fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    foreach (QMdiSubWindow *window, mdiArea->subWindowList()) {
        NewFile *myChild = qobject_cast<NewFile *>(window->widget());
        if (myChild->currentFile() == canonicalFilePath)
            return window;
    }
    return 0;
}

NewFile *MyWord::createNewFile()
{
    NewFile *file = new NewFile;
    mdiArea->addSubWindow(file);

    return file;
}

NewFile *MyWord::activeMyChild()
{
    if (QMdiSubWindow *activeSubWindow = mdiArea->activeSubWindow())
        return qobject_cast<NewFile *>(activeSubWindow->widget());
    return 0;
}

void MyWord::colorChanged(const QColor &color)
{
    QPixmap pix(20, 20);
    pix.fill(color);
    fontColor->setIcon(pix);
}

void MyWord::closeEvent(QCloseEvent *e)
{
    mdiArea->closeAllSubWindows();
    if (mdiArea->currentSubWindow()) {
        e->ignore();
    } else {
        e->accept();
    }
}

void MyWord::slot_updateMenus()
{
    //至少有一个子文档打开着的情况
    bool hasMyChild = (activeMyChild()!=0);

    saveAct->setEnabled(hasMyChild);
    saveAsAct->setEnabled(hasMyChild);
//    printAct->setEnabled(hasMyChild);
//    printPreviewAct->setEnabled(hasMyChild);

    pasteAct->setEnabled(hasMyChild);

    closeAct->setEnabled(hasMyChild);
    closeAllAct->setEnabled(hasMyChild);
    tileAct->setEnabled(hasMyChild);
    cascadeAct->setEnabled(hasMyChild);
    nextAct->setEnabled(hasMyChild);
    lastAct->setEnabled(hasMyChild);
    //文档打开着并且其中有内容被选中的情况
    bool hasSelection = (activeMyChild() && activeMyChild()->textCursor().hasSelection());

    cutAct->setEnabled(hasSelection);
    copyAct->setEnabled(hasSelection);

    bold->setEnabled(hasSelection);
    italic->setEnabled(hasSelection);
    underLine->setEnabled(hasSelection);
    leftAlign->setEnabled(hasSelection);
    centerAlign->setEnabled(hasSelection);
    rightAlign->setEnabled(hasSelection);
    justifyAlign->setEnabled(hasSelection);
    fontColor->setEnabled(hasSelection);
}

void MyWord::slot_newFile()
{
    NewFile *child = createNewFile();
//    child->init();
    child->show();
}

void MyWord::slot_openFile()
{
#if 0
    QString fileName = QFileDialog::getOpenFileName(this, tr("打开"), QString(),
                                                    tr("txt(*.txt);;所有文件 (*.*)"));
#elif 1
    QString fileName = QFileDialog::getOpenFileName(this, tr("打开"), QString(),
                                                    tr("HTML 文档 (*.html);;所有文件 (*.*)"));

#endif
    if (!fileName.isEmpty()) {
        QMdiSubWindow *existing = findOpenFile(fileName);
        if (existing) {
            mdiArea->setActiveSubWindow(existing);
            return;
        }
        NewFile *file = createNewFile();
        if (file->loadFile(fileName)) {
            statusBar()->showMessage(tr("文件已载入"), 2000);
            file->show();
//            enabledText();      //使得字体设置菜单可用
        } else {
            file->close();
        }
    }
}

void MyWord::slot_saveFile()
{
    if (activeMyChild() && activeMyChild()->save())
        statusBar()->showMessage(tr("保存成功"), 2000);
}

void MyWord::slot_saveAsFile()
{
    if (activeMyChild() && activeMyChild()->saveAs())
        statusBar()->showMessage(tr("保存成功"), 2000);
}

void MyWord::slot_copy()
{
    if (activeMyChild())
        activeMyChild()->copy();
}

void MyWord::slot_cut()
{
    if (activeMyChild())
        activeMyChild()->cut();
}

void MyWord::slot_paste()
{
    if (activeMyChild())
        activeMyChild()->paste();
}

void MyWord::slot_undo()
{
    if (activeMyChild())
        activeMyChild()->undo();
}

void MyWord::slot_redo()
{
    if (activeMyChild())
        activeMyChild()->redo();
}

void MyWord::slot_bold()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(bold->isChecked() ? QFont::Bold : QFont::Normal);
    if(activeMyChild())
        activeMyChild()->mergeFormatOnWordOrSelection(fmt);
}

void MyWord::slot_italic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(italic->isChecked());
    if(activeMyChild())
        activeMyChild()->mergeFormatOnWordOrSelection(fmt);
}

void MyWord::slot_underLine()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(underLine->isChecked());
    if(activeMyChild())
        activeMyChild()->mergeFormatOnWordOrSelection(fmt);
}

void MyWord::slot_font(const QString &f)
{
    QTextCharFormat fmt;
    fmt.setFontFamily(f);
    if(activeMyChild())
        activeMyChild()->mergeFormatOnWordOrSelection(fmt);
}

void MyWord::slot_fontSize(const QString &p)
{
    qreal pointSize = p.toFloat();
    if (p.toFloat() > 0) {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        if(activeMyChild())
            activeMyChild()->mergeFormatOnWordOrSelection(fmt);
    }
}

void MyWord::slot_fontColor()
{
    if(activeMyChild())
    {
        QColor color = QColorDialog::getColor(activeMyChild()->textColor(),
                                              this, "颜色对话框");
        if (!color.isValid())
            return;
        QTextCharFormat fmt;
        fmt.setForeground(color);
        activeMyChild()->mergeFormatOnWordOrSelection(fmt);
        colorChanged(color);
    }
}

void MyWord::slot_textAlign(QAction *a)
{
    if(activeMyChild())
    {
        if (a == leftAlign)
            activeMyChild()->setAlign(1);
        else if (a == centerAlign)
            activeMyChild()->setAlign(2);
        else if (a == rightAlign)
            activeMyChild()->setAlign(3);
        else if (a == justifyAlign)
            activeMyChild()->setAlign(4);
    }
}

void MyWord::slot_winMenu()
{
    winMenu->clear();
    winMenu->addAction(closeAct);
    winMenu->addAction(closeAllAct);
    winMenu->addSeparator();
    winMenu->addAction(tileAct);
    winMenu->addAction(cascadeAct);
    winMenu->addSeparator();
    winMenu->addAction(nextAct);
    winMenu->addAction(lastAct);

    QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
    //显示当前打开着的文档子窗口项
    for (int i = 0; i < windows.size(); ++i) {
        NewFile *child = qobject_cast<NewFile *>(windows.at(i)->widget());

        QString text;
        if (i < 9) {
            text = tr("&%1 %2").arg(i + 1).arg(child->userFriendlyCurrentFile());
        } else {
            text = tr("%1 %2").arg(i + 1).arg(child->userFriendlyCurrentFile());
        }
        QAction *action  = winMenu->addAction(text);
        action->setCheckable(true);
        action ->setChecked(child == activeMyChild());
        connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
        windowMapper->setMapping(action, windows.at(i));
    }
}

void MyWord::setActiveSubWindow(QWidget *window)
{
    if (!window)
        return;
    mdiArea->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}
