/*  Document Analyzer - Document Analyzer is a program that reads in a text document and analyze the occurence of words in the document.
    Copyright (C) 2016  Chang Zhang, Ryan Waer, Dr. Paul J Wilkinson

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDialogButtonBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Setup Widget below

    ui->verticalLayout->addWidget(DisplayArea = new QTextBrowser(this));
    ui->verticalLayout->addWidget(CurrentFileLoadedLabel = new QLabel("File Loaded: None",this));
    this->setWindowIcon(QIcon(":/new/icon/resource/blackquill.png"));
    this->setWindowTitle("Document Analyzer");

    waitNotifyBox = new QMessageBox(this);
    waitNotifyBox->setIcon(QMessageBox::Information);
    waitNotifyBox->setWindowIcon(QIcon(":/new/icon/resource/blackquill.png"));
    waitNotifyBox->setWindowTitle("Document Analyzer");

    saveDialogue = NULL;
    reportTabWidget = NULL;

    //Setup other member variable below
    filePath = "";
    fileProcessed = false;
    saveChoice = WantSaveAll;
    wo = NULL;
    workthread = NULL;
    savethread = NULL;

    //Signal and Slots

    connect(this,SIGNAL(fileNameLoaded(QString)),CurrentFileLoadedLabel,SLOT(setText(QString))); //Connect the fileName string in the mainWindow.
}

MainWindow::~MainWindow()
{
    delete ui;
    delete wo;
}

void MainWindow::on_OpenFileButton_clicked() //open file
{
    QString file = QFileDialog::getOpenFileName(this,
        tr("Open Text file"), "", tr("Text Files (*.txt)"));

    fileProcessed = false;
    filePath  = file.toStdString();

    ui->ProcessFileButton->setEnabled(true);
    ui->SaveReportButton->setEnabled(false);
    ui->showReportButton->setEnabled(false);


    //To show fileName in GUI
    QFileInfo fi(file);
    emit fileNameLoaded("File Loaded: " + fi.fileName());
}

void MainWindow::on_ProcessFileButton_clicked() //process report
{
    if(filePath == "")
    {

        showWarningMessage("File not loaded.");

    }
    else if(fileProcessed == true)
    {

        showWarningMessage("File already processed.");

    }
    else
    {
        if(wo != NULL)
        {
           deallocateWorkerAndSaveThread();
           deallocateBrowserArray();
        }
        DisplayArea->clear();
        setupWorkProcess();
        doWork();
    }
}

void MainWindow::on_SaveReportButton_clicked() //save report
{
    if(fileProcessed == false)
    {
        showWarningMessage("File not processed.");

    }
    else
    {
        if(saveDialogue == NULL)
            setupSaveWindow();

        saveDialogue->show();
    }
}

void MainWindow::on_showReportButton_clicked()
{
    if(fileProcessed == false)
    {
        showWarningMessage("File not processed.");

    }
    else
    {
        if(reportTabWidget == NULL)
            setupReportWindow();
        reportTabWidget->show();
    }
}


void MainWindow::setupSaveWindow()
{
    saveDialogue = new QDialog(this,Qt::WindowSystemMenuHint | Qt::WindowTitleHint);
    saveLayout = new QVBoxLayout(saveDialogue);

    saveLayout->addWidget(saveAll = new QRadioButton("Save All Data",this));
    saveLayout->addWidget(saveSummary = new QRadioButton("Save Summary",this));
    saveLayout->addWidget(saveWordStat = new QRadioButton("Save WordStat",this));

    saveButtonBox = new QDialogButtonBox(Qt::Horizontal);
    saveButtonBox->addButton(saveButton = new QPushButton("Save"),QDialogButtonBox::ActionRole);
    saveButtonBox->addButton(QDialogButtonBox::Close);
    saveLayout->addWidget(saveButtonBox);

    saveDialogue->setLayout(saveLayout);
    saveDialogue->setWindowTitle("Save Options");
    saveAll->setChecked(true);

    //SIGNAL AND SLOT for save dialogue
    connect(saveButtonBox, SIGNAL(rejected()), saveDialogue, SLOT(reject()));
    connect(saveButton, SIGNAL(clicked()), this, SLOT(saveOperation()));
    connect(saveAll,SIGNAL(clicked(bool)),this,SLOT(saveAllSlot(bool)));
    connect(saveSummary,SIGNAL(clicked(bool)),this,SLOT(saveSummarySlot(bool)));
    connect(saveWordStat,SIGNAL(clicked(bool)),this,SLOT(SaveWordStatSlot(bool)));

    saveDialogue->setModal(true); //this disables background GUI
    saveDialogue->show();

}



void MainWindow::doWork()
{
    workthread->start();
}

void MainWindow::processStarted()
{
    waitNotifyBox->setText("Processing....Wait patiently");
    waitNotifyBox->setStandardButtons(0); //makes it not have Okay button
    waitNotifyBox->exec();
}

void MainWindow::processEnded()
{

    waitNotifyBox->setText("DONE!");
    waitNotifyBox->setStandardButtons(QMessageBox::Ok);

    fileProcessed = true;
    ui->SaveReportButton->setEnabled(true);
    ui->showReportButton->setEnabled(true);
}


void MainWindow::saveStartedSlot()
{
    waitNotifyBox->setText("Saving....Wait patiently");
    waitNotifyBox->setStandardButtons(0); //makes it not have Okay button
    waitNotifyBox->exec();
}

void MainWindow::saveEndedSlot()
{

    waitNotifyBox->setText("DONE!");
    waitNotifyBox->setStandardButtons(QMessageBox::Ok);

    //saveDialogue->close(); //TRIAL

}

double MainWindow::reading_ease_score(TextParser* T)
{
    double result = 206.835-1.015*((double)T->theWordCount() / T->theSentenceCount())
                    - 84.6*((double)T->theSyllableCount()/T->theWordCount());
    return result;
    /*
    if(result>=90.0) //average 11 yrs old
        return 'A';
    else if(result>= 60.0 || result <= 70.0) // 13-15 yrs old
        return 'B';
    else if(result >= 0 || result <= 30.0) //University grads level.
        return 'C'
    */
}

double MainWindow::reading_level(TextParser *T)
{
    double grade = 0.39*((double)T->theWordCount()/T->theSentenceCount())
                    + 11.8*((double)T->theSyllableCount()/T->theWordCount())-15.59;
    return grade;
}

void MainWindow::outputalphabetfreq(TextParser*Text)
{
    DisplayArea->insertPlainText("Number of Words That Begin With Each Alphabet:\n");

    DisplayArea->insertPlainText("A: ");
    DisplayArea->insertPlainText(QString::number(Text->theHeap(A).alphacount()));
    DisplayArea->insertPlainText("   ");

    DisplayArea->insertPlainText("B: ");
    DisplayArea->insertPlainText(QString::number(Text->theHeap(B).alphacount()));
    DisplayArea->insertPlainText("   ");

    DisplayArea->insertPlainText("C: ");
    DisplayArea->insertPlainText(QString::number(Text->theHeap(C).alphacount()));
    DisplayArea->insertPlainText("   ");

    DisplayArea->insertPlainText("D: ");
    DisplayArea->insertPlainText(QString::number(Text->theHeap(D).alphacount()));
    DisplayArea->insertPlainText("   ");

    DisplayArea->insertPlainText("E: ");
    DisplayArea->insertPlainText(QString::number(Text->theHeap(E).alphacount()));
    DisplayArea->insertPlainText("   ");

    DisplayArea->insertPlainText("F: ");
    DisplayArea->insertPlainText(QString::number(Text->theHeap(F).alphacount()));
    DisplayArea->insertPlainText("   ");

    DisplayArea->insertPlainText("G: ");
    DisplayArea->insertPlainText(QString::number(Text->theHeap(G).alphacount()));
    DisplayArea->insertPlainText("   ");

    DisplayArea->insertPlainText("\n");

    DisplayArea->insertPlainText("H: ");
    DisplayArea->insertPlainText(QString::number(Text->theHeap(H).alphacount()));
    DisplayArea->insertPlainText("   ");

    DisplayArea->insertPlainText("I: ");
    DisplayArea->insertPlainText(QString::number(Text->theHeap(I).alphacount()));
    DisplayArea->insertPlainText("   ");

    DisplayArea->insertPlainText("J: ");
    DisplayArea->insertPlainText(QString::number(Text->theHeap(J).alphacount()));
    DisplayArea->insertPlainText("   ");

    DisplayArea->insertPlainText("K: ");
    DisplayArea->insertPlainText(QString::number(Text->theHeap(K).alphacount()));
    DisplayArea->insertPlainText("   ");

    DisplayArea->insertPlainText("L: ");
    DisplayArea->insertPlainText(QString::number(Text->theHeap(L).alphacount()));
    DisplayArea->insertPlainText("   ");

    DisplayArea->insertPlainText("M: ");
    DisplayArea->insertPlainText(QString::number(Text->theHeap(M).alphacount()));
    DisplayArea->insertPlainText("   ");

    DisplayArea->insertPlainText("N: ");
    DisplayArea->insertPlainText(QString::number(Text->theHeap(N).alphacount()));
    DisplayArea->insertPlainText("   ");

    DisplayArea->insertPlainText("\n");

    DisplayArea->insertPlainText("O: ");
    DisplayArea->insertPlainText(QString::number(Text->theHeap(O).alphacount()));
    DisplayArea->insertPlainText("   ");

    DisplayArea->insertPlainText("P: ");
    DisplayArea->insertPlainText(QString::number(Text->theHeap(P).alphacount()));
    DisplayArea->insertPlainText("   ");

    DisplayArea->insertPlainText("Q: ");
    DisplayArea->insertPlainText(QString::number(Text->theHeap(Q).alphacount()));
    DisplayArea->insertPlainText("   ");

    DisplayArea->insertPlainText("R: ");
    DisplayArea->insertPlainText(QString::number(Text->theHeap(R).alphacount()));
    DisplayArea->insertPlainText("   ");

    DisplayArea->insertPlainText("S: ");
    DisplayArea->insertPlainText(QString::number(Text->theHeap(S).alphacount()));
    DisplayArea->insertPlainText("   ");

    DisplayArea->insertPlainText("T: ");
    DisplayArea->insertPlainText(QString::number(Text->theHeap(T).alphacount()));
    DisplayArea->insertPlainText("   ");

    DisplayArea->insertPlainText("\n");

    DisplayArea->insertPlainText("U: ");
    DisplayArea->insertPlainText(QString::number(Text->theHeap(U).alphacount()));
    DisplayArea->insertPlainText("   ");

    DisplayArea->insertPlainText("V: ");
    DisplayArea->insertPlainText(QString::number(Text->theHeap(V).alphacount()));
    DisplayArea->insertPlainText("   ");

    DisplayArea->insertPlainText("W: ");
    DisplayArea->insertPlainText(QString::number(Text->theHeap(W).alphacount()));
    DisplayArea->insertPlainText("   ");

    DisplayArea->insertPlainText("X: ");
    DisplayArea->insertPlainText(QString::number(Text->theHeap(X).alphacount()));
    DisplayArea->insertPlainText("   ");

    DisplayArea->insertPlainText("Y: ");
    DisplayArea->insertPlainText(QString::number(Text->theHeap(Y).alphacount()));
    DisplayArea->insertPlainText("   ");

    DisplayArea->insertPlainText("Z: ");
    DisplayArea->insertPlainText(QString::number(Text->theHeap(Z).alphacount()));
    DisplayArea->insertPlainText("   ");

}

void MainWindow::outputalphabetfreq(ostream& out,TextParser&Text)
{
    out<<"Number of Words That Begin With Each Alphabet:\n";
    out<<"A: "<<Text.theHeap(A).alphacount() << "   ";
    out<<"B: "<<Text.theHeap(B).alphacount() << "   ";
    out<<"C: "<<Text.theHeap(C).alphacount() << "   ";
    out<<"D: "<<Text.theHeap(D).alphacount() << "   ";
    out<<"E: "<<Text.theHeap(E).alphacount() << "   ";
    out<<"F: "<<Text.theHeap(F).alphacount() << "   ";
    out<<"G: "<<Text.theHeap(G).alphacount() << "   ";
    out<< endl;
    out<<"H: "<<Text.theHeap(H).alphacount() << "   ";
    out<<"I: "<<Text.theHeap(I).alphacount() << "   ";
    out<<"J: "<<Text.theHeap(J).alphacount() << "   ";
    out<<"K: "<<Text.theHeap(K).alphacount() << "   ";
    out<<"L: "<<Text.theHeap(L).alphacount() << "   ";
    out<<"M: "<<Text.theHeap(M).alphacount() << "   ";
    out<<"N: "<<Text.theHeap(N).alphacount() << "   ";
    out<< endl;
    out<<"O: "<<Text.theHeap(O).alphacount() << "   ";
    out<<"P: "<<Text.theHeap(P).alphacount() << "   ";
    out<<"Q: "<<Text.theHeap(Q).alphacount() << "   ";
    out<<"R: "<<Text.theHeap(R).alphacount() << "   ";
    out<<"S: "<<Text.theHeap(S).alphacount() << "   ";
    out<<"T: "<<Text.theHeap(T).alphacount() << "   ";
    out<<endl;
    out<<"U: "<<Text.theHeap(U).alphacount() << "   ";
    out<<"V: "<<Text.theHeap(V).alphacount() << "   ";
    out<<"W: "<<Text.theHeap(W).alphacount() << "   ";
    out<<"X: "<<Text.theHeap(X).alphacount() << "   ";
    out<<"Y: "<<Text.theHeap(Y).alphacount() << "   ";
    out<<"Z: "<<Text.theHeap(Z).alphacount() << "   ";
}


void MainWindow::saveAllSlot(bool)
{
    saveChoice = WantSaveAll;
    wo->setSaveOption(workersaveEnum::WantSaveEverything);
}

void MainWindow::saveSummarySlot(bool)
{
    saveChoice = wantSaveSummary;
    wo->setSaveOption(workersaveEnum::wantSaveSum);

}

void MainWindow::SaveWordStatSlot(bool)
{
    saveChoice = wantSavewordStat;
    wo->setSaveOption(workersaveEnum::wantSaveword);
}

void MainWindow::saveOperation()
{
  QString str = QFileDialog::getSaveFileName(this,"Save File",QDir::currentPath(),"Text files (*.txt)");
  string path = str.toStdString();

    if(!str.isNull())
    {
        wo->setSaveFileName(path);
        setupSaveThreadWithWorker(); //move to savethread and connect signal and slots
        savethread->start();
    }
}

void MainWindow::deallocateWorkerAndSaveThread()
{
    disconnect(wo,SIGNAL(processFinished()),this,SLOT(processEnded())); //notify box, summary process from main thread
    wo->deleteLater();
    savethread->quit();
    savethread->deleteLater();
}


void MainWindow::setupWorkProcess()
{
    wo = new worker(); //QObject cannot have parent otherwise it won't be moved to QThread.
    wo->setMainWindowPtr(this);

    workthread = new QThread(this);

    wo->moveToThread(workthread);

    connect(workthread,SIGNAL(started()),this,SLOT(processStarted())); //wait notify box, from main thread

    connect(workthread,SIGNAL(started()),wo,SLOT(startProcess())); //start process algorithm in the workthread

    connect(wo,SIGNAL(processFinished()),this,SLOT(setSummary())); //set the summary in the displayarea

    //Process done, include moving worker to savethread

    connect(wo,SIGNAL(processFinished()),this,SLOT(processEnded())); //GUI notify box, summary process from main thread

    connect(wo,SIGNAL(processFinished()),workthread,SLOT(quit())); //worker tells workthread to quit

    connect(workthread,SIGNAL(finished()),workthread,SLOT(deleteLater())); // deallocate workthread

    connect(wo,SIGNAL(processFinished()),this,SLOT(allocateSaveThread()));

    connect(wo,SIGNAL(processFinished()),this,SLOT(allocateReportThread()));

    wo->putFilePath(filePath); //g the filepath from GUI to worker
}

void MainWindow::showWarningMessage(std::string s)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Warning");
    msgBox.setWindowIcon(QIcon(":/new/icon/resource/blackquill.png"));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText(QString::fromStdString(s));
    msgBox.exec();
}

void MainWindow::allocateSaveThread()
{
      savethread = new QThread(this);
}

void MainWindow::allocateReportThread()
{
      reportthread = new QThread(this);
}


void MainWindow::setupSaveThreadWithWorker()
{

    wo->moveToThread(savethread);

    connect(savethread,SIGNAL(started()),this,SLOT(saveStartedSlot())); //wait notify box, from main thread

    connect(wo,SIGNAL(workerSaveProcessFinished()),this,SLOT(saveEndedSlot())); //notify box, summary process from main thread

    connect(savethread,SIGNAL(started()),wo,SLOT(startSaveProcess())); //start saving process algorithm in the workthread

    connect(wo,SIGNAL(workerSaveProcessFinished()),savethread,SLOT(quit())); //worker signal thread to quit
}

QThread* MainWindow::getSaveThread()
{
    return savethread;
}

QTextBrowser* MainWindow::getDisplayArea()
{
    return DisplayArea;
}

void MainWindow::setSummary()
{
    summary(wo->getParser(),wo->getTime());
}

void MainWindow::summary(TextParser* Text, int processtime)
{
    pair<string,int> temppair;

   DisplayArea->insertPlainText("SUMMARY:\n\n");

   DisplayArea->insertPlainText("Word Count: ");
   DisplayArea->insertPlainText(QString::number(Text->theWordCount()));
   DisplayArea->insertPlainText("\n");

   DisplayArea->insertPlainText( "Paragraph Count: ");
   DisplayArea->insertPlainText(QString::number(Text->theParagraphCount()));
   DisplayArea->insertPlainText("\n");

   DisplayArea->insertPlainText( "Reading Score: ");
   DisplayArea->insertPlainText(QString::number(reading_ease_score(Text)));
   DisplayArea->insertPlainText("\n");

   DisplayArea->insertPlainText( "Reading Grade Level: ");
   DisplayArea->insertPlainText( QString::number((int)reading_level(Text)));
   DisplayArea->insertPlainText("\n\n");

   DisplayArea->insertPlainText( "sentence count:");
   DisplayArea->insertPlainText(QString::number(Text->theSentenceCount()));
   DisplayArea->insertPlainText("\n");

    //TEST WORD RANK
   DisplayArea->insertPlainText("Top Ten Most Frequently Used Words:\n");

    for(size_t i = 0; i<10; ++i)//
    {
        temppair.first = Text->word_rank().front();
        temppair.second = Text->word_rank().frontRanking();
       DisplayArea->insertPlainText("\"");
       DisplayArea->insertPlainText(QString::fromStdString(temppair.first));
       DisplayArea->insertPlainText("\"");
       DisplayArea->insertPlainText(" ");

       DisplayArea->insertPlainText("(");
       DisplayArea->insertPlainText(QString::number(temppair.second));
       DisplayArea->insertPlainText(")");
       DisplayArea->insertPlainText("\n");

        Text->word_rank().dequeue();
        Text->TopTen().push_back(temppair);
    }
   DisplayArea->insertPlainText("\n\n");

    outputalphabetfreq(Text);//output number of word that start out with each alphabet

   DisplayArea->insertPlainText("\n\n");
    //END TEST WORD RANK

   DisplayArea->insertPlainText("Total Processing Time: ");
   DisplayArea->insertPlainText(QString::number(processtime));
   DisplayArea->insertPlainText(" ticks/milliseconds.\n");
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("About this program");
    msgBox.setWindowIcon(QIcon(":/new/icon/resource/blackquill.png"));
    msgBox.setIcon(QMessageBox::Information	);
    msgBox.setText("Document Analyzer is a program that reads in a text document and analyze the occurence of words in the document.\n\n"
                   "Contributor:\n"
                   "TextParser: Chang Zhang, Ryan Waer\n"
                   "GUI: Change Zhang"
                   "\nProject for CS8 (Data Structure) at Pasadena City College\n"
                   "Many thanks to Dr. Paul J Wilkinson for teaching and guidance\n");
    msgBox.exec();
}


void MainWindow::setupReportWindow()
{
    QString alphabet("A");
    QString label("&#");

    reportTabWidget = new QTabWidget();
    browserArray = new vector<QTextBrowser*>();
    setTextBrowserArray(browserArray);

    reportTabWidget->setWindowIcon(QIcon(":/new/icon/resource/blackquill.png"));
    reportTabWidget->setWindowTitle("Document Analyzer Report");

    for(int i = 0; i < 26; ++i)
    {
        label.replace(1,1,alphabet);
        reportTabWidget->addTab(browserArray->at(i),label);
        alphabet[0] = alphabet[0].unicode() + 1;
    }
}

void MainWindow::setTextBrowserArray(vector<QTextBrowser*> *t)
{
    QTextBrowser *QTBPtr;
    char prevChar = 'A';
    char curChar = 'A';
    int currentIndex = 0;
    bool isFirstRun = true;

    for (size_t i = 0; i < 26; ++i)
    {
        cout<<"Alphabet done:" <<i <<endl;

        QTBPtr = new QTextBrowser();

        currentIndex = setWordStat(QTBPtr,currentIndex,prevChar,curChar);
        t->push_back(QTBPtr);
        isFirstRun = false;


        while(((prevChar+1) != curChar) && !isFirstRun && prevChar <= 'Z') //if skipped an alphabet, and not at beginning, assign none an skip.
        {
            QTBPtr = new QTextBrowser();
            QTBPtr->insertPlainText("None");
            t->push_back(QTBPtr);
            prevChar = prevChar + 1;
            cout<<"prevChar (in): "<<prevChar<<endl;
        }

        prevChar = curChar;
       // prevChar = prevChar + 1;
        cout<<"prevChar(out): "<<prevChar<<endl;

    }

}


int MainWindow::setWordStat(QTextBrowser* QTB, size_t curIndex, char& prevChar, char& curChar)
{
    string word, str;
    TextParser* Text = wo->getParser();

    int count = 0;

    size_t wordSize = Text->Word_List().size();

    int k = curIndex;
    //Since wordList has all words, i need a way to split the words by their first alphabet, so i use prevChar to signal change in alphabet and curIndex for next call of this function
    for(; k < wordSize; ++k)
    {

         word = str = Text->Word_List()[k];
         word[0] = toupper(word[0]);

         if(prevChar != word[0])
         {
             curChar = word[0];
             break;
         }


         curChar = word[0];

         QTB->insertPlainText("\n-------------------------------------------------------------------------------------------");
         QTB->insertPlainText(QString::fromStdString(word));
         QTB->insertPlainText("-------------------------------------------------------------------------------------------\n\n");

         for(size_t i = 0; i < Text->ParaList(str).size(); ++i)
         {
             count++;
             QTB->insertPlainText("Paragraph #: ");
             QTB->insertPlainText(QString::number(Text->ParaList(str).at(i)));
             QTB->insertPlainText("\n");
             QTB->insertPlainText("Line #: ");
             for(size_t j = 0; j < (Text->WordDataMap(str))[Text->ParaList(str).at(i)].size(); ++j)
             {
                  QTB->insertPlainText(QString::number(Text->WordDataMap(str)[Text->ParaList(str).at(i)].at(j)));
                  QTB->insertPlainText(". ");
                 if(count%10==0)
                    QTB->insertPlainText("\n");
             }
             count = 0;
             QTB->insertPlainText("\n\n");
         }

         QTB->insertPlainText("-------------------------------------------------------------------------------------------");
         QTB->insertPlainText("-------------------------------------------------------------------------------------------\n\n");

    }

    return k; // return the current Index

    //QTB->insertPlainText("End of Output.");
}



void MainWindow::deallocateBrowserArray()
{
    for(int i = 0; i < 26; ++i)
    {
        delete browserArray->at(i);
    }

    delete browserArray;

    delete reportTabWidget;
    reportTabWidget = NULL;
}
