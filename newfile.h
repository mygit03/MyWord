#ifndef NEWFILE_H
#define NEWFILE_H

#include <QTextEdit>
#include <QCloseEvent>

class NewFile : public QTextEdit
{
    Q_OBJECT
public:
    NewFile();
    ~NewFile();

    void init();
    bool loadFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString userFriendlyCurrentFile();
    QString strippedName(const QString &fullFileName);
    QString currentFile() { return curFile; }
    bool save();
    bool saveAs();
    bool saveFile(QString fileName);
    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);   //格式字体设置
    void setAlign(int align);                                           //段落对齐设置

protected:
    void closeEvent(QCloseEvent *e);

private slots:
    void slot_documentWasModified();

private:
    QString curFile;
    bool isUntitled;
};

#endif // NEWFILE_H
