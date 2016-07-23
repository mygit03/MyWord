#ifndef MYWORD_H
#define MYWORD_H

#include <QMainWindow>
#include <QMdiArea>
#include <QAction>
#include <QToolBar>
#include <QFontComboBox>
#include <QFontDatabase>
#include <QComboBox>
#include <QCloseEvent>
#include <QSignalMapper>

#include "newfile.h"

class MyWord : public QMainWindow
{
    Q_OBJECT

public:
    MyWord(QWidget *parent = 0);
    ~MyWord();

    void init();
    void creatActions();
    void creatWidgets();
    void creatMenu();
    void creatToolBar();
    void connectSignals();
    QMdiSubWindow *findOpenFile(const QString &fileName);

    NewFile *createNewFile();
    NewFile *activeMyChild();

    void colorChanged(const QColor &color);

protected:
    void closeEvent(QCloseEvent *e);

private slots:
    void slot_updateMenus();
    void slot_newFile();
    void slot_openFile();
    void slot_saveFile();
    void slot_saveAsFile();
    void slot_copy();
    void slot_cut();
    void slot_paste();
    void slot_undo();
    void slot_redo();
    void slot_bold();
    void slot_italic();
    void slot_underLine();

    void slot_font(const QString &f);
    void slot_fontSize(const QString &p);
    void slot_fontColor();

    void slot_textAlign(QAction*a);

    void slot_winMenu();
    void setActiveSubWindow(QWidget *window);

private:
    QMdiArea *mdiArea;

    QMenu *fileMenu, *editMenu, *winMenu;

    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *copyAct;
    QAction *cutAct;
    QAction *pasteAct;
    QAction *undoAct;
    QAction *redoAct;
    QAction *exitAct;
    QAction *leftAlign;
    QAction *centerAlign;
    QAction *rightAlign;
    QAction *justifyAlign;
    QAction *bold;
    QAction *italic;
    QAction *underLine;
    QAction *fontColor;

    QAction *closeAct;
    QAction *closeAllAct;
    QAction *lastAct;
    QAction *nextAct;
    QAction *tileAct;           //平铺
    QAction *cascadeAct;        //级联、层叠

    QFontComboBox *fontCBox;
    QComboBox *sizeCBox;

    QToolBar *fileToolbar, *editToolbar, *fontToolbar;

    QSignalMapper *windowMapper;
};

#endif // MYWORD_H
