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

#include "worker.h"

worker::worker(QObject *parent) : QObject(parent)
{
    parser = new TextParser();
    saveFileName = "";
    SaveOption = WantSaveEverything;
}

worker::~worker()
{
    delete parser;
}

void worker::startProcess()
{
    ifstream infile;
    input(infile,pathToFile);
    clockstart = clock();
    readIntoParser(infile,parser);
    infile.close();
    parser->DataProcessing();
    clocktime = clock() - clockstart;

    moveToThread(QCoreApplication::instance()->thread()); //move back to main thread

    emit processFinished();
}

void worker::startSaveProcess()
{
    ofstream outfile;
    outfile.open(saveFileName.c_str());
    TextParser* Text = this->getParser();

    if(SaveOption == wantSaveSum || SaveOption == WantSaveEverything)
    {
        outfile << "Word Count: "<<Text->theWordCount()<<endl;
        outfile << "Sentence Count: "<<Text->theSentenceCount()<<endl;
        outfile<< "Paragraph Count: "<<Text->theParagraphCount()<<endl;
        outfile <<"Reading Score: "<<reading_ease_score(Text)<<endl;
        outfile <<"Reading level: "<<(int)reading_level(Text)<<endl;
        outfile<<"Ten Most Frequently Used Words: "<<endl;
        for(size_t i = 0; i < Text->TopTen().size(); ++i)
        {
            outfile<<"\""<<Text->TopTen()[i].first<<"\" "<<"("<<Text->TopTen()[i].second<<")"<<endl;
        }
        outfile<<endl;

        outputalphabetfreq(outfile,*Text);
        outfile<<endl;
        outfile<<"Total Processing time: "<< this->getTime() << " ticks(milliseconds)" <<endl<<endl;
    }

    if(SaveOption == wantSaveword || SaveOption == WantSaveEverything)
    {

        string temps, str;
        int count = 0;
        outfile<<"Word Statistics:\n";
        outfile<<endl;

        for(size_t k = 0; k < Text->Word_List().size(); ++k)
        {

             temps = str = Text->Word_List()[k];
             temps[0] = toupper(temps[0]);

             outfile<<"\n---------------------------------------  \""<< temps <<"\"  ---------------------------------------\n\n";

             for(size_t i = 0; i < Text->ParaList(str).size(); ++i)
             {
                 count++;
                 outfile<<"Paragraph #: "<<Text->ParaList(str).at(i)<<endl;
                 outfile<<"Line #: ";
                 for(size_t j = 0; j < (Text->WordDataMap(str))[Text->ParaList(str).at(i)].size(); ++j)
                 {
                     outfile<< (Text->WordDataMap(str))[Text->ParaList(str).at(i)].at(j)<<". ";
                     if(count%10==0)
                        outfile<<endl;
                 }
                 count = 0;
                 outfile<<endl<<endl;
             }

             outfile<<"-------------------------------------------------------------------------------------------\n\n";
        }
        outfile<<"End of Output.";
    }
    outfile.close(); //CLOSE OUTPUT FILE STREAM.
    setSaveFileName(""); //reset back to null, not necessary actually...

    moveToThread(QCoreApplication::instance()->thread()); //move back to main thread

    emit workerSaveProcessFinished();
}


void worker::input(ifstream &file, std::string& fileToParse)
{
    file.open(fileToParse.c_str());
    if(file.fail())
    {
        file.clear(); //clear error flag.
        file.close();
        exit(1);
    }
}

void worker::putFilePath(std::string s)
{
    pathToFile = s;
}

int worker::getTime()
{
    return clocktime;
}

TextParser* worker::getParser()
{
    return parser;
}

void worker::setSaveOption(workersaveEnum s)
{
    SaveOption = s;
}
void worker::setSaveFileName(string s)
{
    saveFileName = s;
}


double worker::reading_ease_score(TextParser* T)
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

double worker::reading_level(TextParser *T)
{
    double grade = 0.39*((double)T->theWordCount()/T->theSentenceCount())
                    + 11.8*((double)T->theSyllableCount()/T->theWordCount())-15.59;
    return grade;
}

void worker::outputalphabetfreq(ostream& out,TextParser&Text)
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

void worker::setMainWindowPtr(MainWindow * m)
{
    MainWinPtr = m;
}
