#ifndef MAINWINDOW_H
#define MAINWINDOW_H
//UI Related
#include <QMainWindow>
#include <QtCore>
#include <QFileDialog>
#include <QLabel>
#include <QTextBrowser>
#include <QMessageBox>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QDialogButtonBox>
//Algorithm Related
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include "textparser.h"
#include "heap.h"
#include <time.h>
#include "pQueue.h"
#include "pQueueNode.h"
#include <algorithm>
#include "worker.h"
#include <QThread>
#include <QDebug>

enum saveEnum{None,wantSaveSummary, wantSavewordStat,WantSaveAll};

namespace Ui {
class MainWindow;
}
class worker;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    double reading_ease_score(TextParser* T);
    void summary(TextParser* Text, int processtime);
    double reading_level(TextParser *T);
    void outputalphabetfreq(TextParser*Text);
    void outputalphabetfreq(ostream& out,TextParser& Text);
    QThread* getSaveThread();
    QTextBrowser * getDisplayArea();
    int setWordStat(QTextBrowser* QTB, size_t alphabetindex, char& prevChar, char &curChar);
    void setTextBrowserArray(vector<QTextBrowser*>*);




signals:
    void fileNameLoaded(QString);

private slots:

    void processStarted();
    void processEnded();
    void on_OpenFileButton_clicked();
    void on_ProcessFileButton_clicked();
    void on_SaveReportButton_clicked();

    void saveAllSlot(bool);
    void saveSummarySlot(bool);
    void SaveWordStatSlot(bool);
    void saveOperation();

    void setSummary();
    void saveStartedSlot();
    void saveEndedSlot();
    void setupSaveThreadWithWorker();
    void allocateSaveThread();
    void allocateReportThread();

    void on_actionAbout_triggered();

    void on_showReportButton_clicked();

private:

    //UI related
    Ui::MainWindow *ui;
    std::string filePath;
    QLabel *CurrentFileLoadedLabel;
    QTextBrowser *DisplayArea;
    QMessageBox *waitNotifyBox;

    QDialog *saveDialogue;
    QVBoxLayout *saveLayout;
    QRadioButton *saveAll;
    QRadioButton *saveSummary;
    QRadioButton *saveWordStat;
    QPushButton *saveButton;

    QDialogButtonBox* saveButtonBox;

    QTabWidget *reportTabWidget;
    vector<QTextBrowser*> *browserArray;

    //Algorithm Related
    TextParser *parser;
    worker *wo;
    QThread *workthread;
    QThread *savethread;
    QThread *reportthread;
    saveEnum saveChoice;

    bool fileProcessed;

    void doWork();
    void input(ifstream &file, string& fileToParse);
    void setupSaveWindow();
    void deallocateWorkerAndSaveThread();
    void deallocateBrowserArray();

    void setupWorkProcess();
    void showWarningMessage(std::string);
    void setupReportWindow();


};


#endif // MAINWINDOW_H
