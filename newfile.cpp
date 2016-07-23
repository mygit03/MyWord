#include "newfile.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QIcon>
#include <QTextDocumentWriter>
#include <QTextCodec>

NewFile::NewFile()
{
    isUntitled = true;
    init();
}

NewFile::~NewFile()
{

}

void NewFile::init()
{
    setWindowIcon(QIcon(":/notepad.png"));

    static int count = 1;

    isUntitled = true;
    curFile = tr("文档 %1").arg(count++);
    setWindowTitle(curFile);
}

//打开文件时用，若该文件没有打开，则将该文件内容导入新打开窗口
bool NewFile::loadFile(const QString &fileName)
{
    if (!fileName.isEmpty())
    {
        if (!QFile::exists(fileName))
            return false;
        QFile file(fileName);
        if (!file.open(QFile::ReadOnly))
            return false;

        QByteArray data = file.readAll();
        QTextCodec *codec = Qt::codecForHtml(data);
        QString str = codec->toUnicode(data);
        if (Qt::mightBeRichText(str)) {
            this->setHtml(str);
        } else {
            str = QString::fromLocal8Bit(data);
            this->setPlainText(str);
        }
        setCurrentFile(fileName);
        connect(document(), SIGNAL(contentsChanged()),this,
                SLOT(slot_documentWasModified()));
        return true;
    }
}

void NewFile::setCurrentFile(const QString &fileName)
{
    curFile = QFileInfo(fileName).canonicalFilePath();
    isUntitled = false;
    document()->setModified(false);
    setWindowModified(false);
    setWindowTitle(userFriendlyCurrentFile() + "[*]");
}

QString NewFile::userFriendlyCurrentFile()
{
    return strippedName(curFile);
}

QString NewFile::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

bool NewFile::save()
{
    if (isUntitled) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool NewFile::saveAs()
{
#if 0
    QString fileName = QFileDialog::getSaveFileName(this, tr("保存"), curFile,
                                                    tr("txt(*.txt);;所有文件 (*.*)"));
#elif 1
    QString fileName = QFileDialog::getSaveFileName(this, tr("另存为"), curFile,
                                                    tr("HTML 文档 (*.html);;所有文件 (*.*)"));
#endif
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

bool NewFile::saveFile(QString fileName)
{
#if 1
    if (!(fileName.endsWith(".htm", Qt::CaseInsensitive) || fileName.endsWith(".html", Qt::CaseInsensitive))) {
        fileName += ".html"; // 默认保存为 HTML 文档
    }
    QTextDocumentWriter writer(fileName);
    bool success = writer.write(this->document());
    if (success)
        setCurrentFile(fileName);
    return success;
#elif 0
    if (!(fileName.endsWith(".txt", Qt::CaseInsensitive))) {
        fileName += ".txt"; // 默认保存为 txt文档
    }
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QMessageBox msg;
        msg.setIcon(QMessageBox::Warning);
        msg.setText("打开文件失败！");
        msg.exec();
        return false;
    }else{
        QTextStream out(&file);
        QString text = this->toPlainText();
        out << text;
        return true;
    }
#endif
}

//格式设置
void NewFile::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = this->textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
    this->mergeCurrentCharFormat(format);
}

void NewFile::setAlign(int align)
{
    if (align == 1)
        this->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
    else if (align == 2)
        this->setAlignment(Qt::AlignHCenter);
    else if (align == 3)
        this->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
    else if (align == 4)
        this->setAlignment(Qt::AlignJustify);
}

void NewFile::closeEvent(QCloseEvent *e)
{
    qDebug("close!");
    int ret;
    if (document()->isModified()){
        qDebug() << "curFile:" << curFile;
        QStringList strList = curFile.split("/");
        curFile = strList.last();
        qDebug() << "curFile:" << curFile;
        QMessageBox msg;
        msg.setWindowTitle("警告");
        msg.setText(tr("是否将更改保存到 %1？").arg(curFile));
        msg.addButton(tr("保存"), QMessageBox::AcceptRole);
        msg.addButton(tr("不保存"), QMessageBox::RejectRole);
        msg.addButton(tr("取消"), QMessageBox::NoRole);
        ret = msg.exec();
        qDebug() << "ret:" << ret;
        if (ret == 0){
            qDebug("save!");
            save();
            if (curFile.isEmpty()){
                e->ignore();
            }else{
                e->accept();
            }
        }else if (ret == 1){
            e->accept();
            qDebug("no save!");
        }else if (ret == 2){
            qDebug("ignore!");
            e->ignore();
        }
    }else{
        QTextEdit::closeEvent(e);
    }
}

void NewFile::slot_documentWasModified()
{
    setWindowModified(document()->isModified());
}

