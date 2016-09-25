#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <string>
#include <QDebug>

//Algorithm Related
#include "mainwindow.h"
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
#include <QThread>

enum workersaveEnum{Nada,wantSaveSum, wantSaveword,WantSaveEverything};

class MainWindow; //define first

class worker : public QObject
{
    Q_OBJECT

public:
    explicit worker(QObject *parent = 0);

    ~worker();

    void putFilePath(std::string s);
    int getTime();

    TextParser* getParser();

    void setSaveOption(workersaveEnum);
    void setSaveFileName(string);

    double reading_ease_score(TextParser* T);
    double reading_level(TextParser *T);
    void outputalphabetfreq(ostream& out,TextParser&Text);
    void setMainWindowPtr(MainWindow*);

signals:
    void processFinished();
    void workerSaveProcessFinished();

public slots:
    void startProcess();
    void startSaveProcess();

private:
    TextParser *parser;
    int debugtime;
    int clockstart,clocktime;
    std::string pathToFile;
    string saveFileName;
    workersaveEnum SaveOption;

    MainWindow *MainWinPtr;
    void input(ifstream &file, string& fileToParse);

};

#endif // WORKER_H
