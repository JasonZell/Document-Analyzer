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

#include "textparser.h"
#include <algorithm>
#include <time.h>
using namespace std;

TextParser::TextParser()
{
    word_count = 0;
    paragraph_count = 0;
    line_count = 0;
    sentence_count = 0;
    syllable_count = 0;
}

TextParser::~TextParser()
{
    paragraph_count= 0; //Total paragraph count.
    sentence_count = 0; // total sentence count.
    word_count = 0; //total word count.
    line_count = 0;
    syllable_count = 0;
    word_frequency.clear();
    word_data.clear();
    WordList.clear();
    TopTenWord.clear();
}

TextParser::TextParser (const TextParser& other)
{
    copy(other);
}

TextParser& TextParser::operator=(const TextParser& other)
{
    if(this != &other)
    {
        copy(other);
    }
    return *this;
}


void tolowercase(string &s)
{
    for(size_t i = 0; i<s.size(); ++i)
    {
        s[i] = tolower(s[i]);
    }
}

istream& operator>>(istream& in, TextParser& parser)
{
    size_t loc;
    string str,tempstr;
    node<string> tempnode;
    char nextchar,afternextchar;

    while(in>>str)
    {
        if(((loc = str.find_first_of("-")) != string::npos)) //for case with "--", it breaks it into two words
        {
            tempstr=str.substr(loc);
            str=str.substr(0,loc);
            tolowercase(tempstr);
            parser.parse(tempstr,tempnode);
        }
        tolowercase(str);
        parser.parse(str,tempnode);
        in.get(nextchar);
        afternextchar = in.peek();
        if(nextchar == '\n')
            parser.line_count++;
        if(nextchar == '\n' && afternextchar == '\n')
        {
            parser.paragraph_count++;
            parser.line_count++;
        }
    }
    cout<<"Parsing Done!!\n\n";
    return in;
}

istream& readIntoParser(istream& in, TextParser* parser)
{
    size_t loc;
    string str,tempstr;
    node<string> tempnode;
    char nextchar,afternextchar;

    while(in>>str)
    {
        if(((loc = str.find_first_of("-")) != string::npos)) //for case with "--", it breaks it into two words
        {
            tempstr=str.substr(loc);
            str=str.substr(0,loc);
            tolowercase(tempstr);
            parser->parse(tempstr,tempnode);
        }
        tolowercase(str);
        parser->parse(str,tempnode);
        in.get(nextchar);
        afternextchar = in.peek();
        if(nextchar == '\n')
            parser->line_count++;
        if(nextchar == '\n' && afternextchar == '\n')
        {
            parser->paragraph_count++;
            parser->line_count++;
        }
    }
    cout<<"Parsing Done!!\n\n";
    return in;
}

template <typename string>
void TextParser::parse(string& str,node<string>& tempnode)
{
    size_t loc;
    bool firstdot = false,eos=false;

    while((loc = str.find_first_not_of("abcdefghijklmnopqrstuvwxyz")) != string::npos) //if it does found something other than alphabets
    {
        if(str[loc] == '!' || str[loc] == '?') //this is absolutely end of a sentence regardless of where it is at!
        {
            ++sentence_count;
            eos = true;
        }

        else if(!eos && !firstdot && str[loc] == '.' && (loc == str.length()-1 || loc == str.length()-2)) //if found at the end of word
        {
            firstdot = 1;
            eos = true;
            ++sentence_count;
        }
        str.erase(loc,1); //erase punctuations.
    }

    if(str.empty())
        return;

    tempnode.theData() = str;
    tempnode.theline() = line_count+1;
    tempnode.thepara() = paragraph_count+1;
    tempnode>>WordStore[toupper(str[0])-65];

    word_frequency[str] +=1;
    syllable_count += (str.length()/3) ? (str.length()/3) : 1;
    ++word_count;
    WordStore[toupper(str[0])-65].alphacount() += 1;
}

void TextParser::copy(const TextParser& other)
{
    word_count = other.word_count;
    paragraph_count = other.paragraph_count;
    sentence_count = other.sentence_count;
    syllable_count = other.syllable_count;


    for(size_t i = 0; i < 26; ++i)
    {
        WordStore[i] = other.WordStore[i];
    }
}

void TextParser::nukem()
{
    word_count = 0;
    paragraph_count = 0;
    sentence_count = 0;
    syllable_count = 0;
}
int& TextParser::theSentenceCount()
{
    return sentence_count;
}

int& TextParser::theParagraphCount()
{
    return paragraph_count;
}

int& TextParser::theWordCount()
{
    return word_count;
}

double& TextParser::theSyllableCount()
{
    return syllable_count;
}

int TextParser::theHeapSize(ALPHABET n)
{
    return (int)WordStore[n].size();
}

int& TextParser::alphabet_count(ALPHABET n)
{
    return WordStore[n].alphacount();
}

int& TextParser::data_frequency(string s)
{
    return word_frequency[s];
}

heap<string>& TextParser::theHeap(ALPHABET n)
{
    return WordStore[(int)n];
}

deque<string>& TextParser::Word_List()
{
    return WordList;
}


void TextParser::DataProcessing()
{
    int clockT;//DEBUG

    vector<int>::iterator paraiterBegin, paraiterEnd,uniqueiter;
    vector<string>::iterator iter;
    vector<string> tempvec;
    node<string>* tempnode;

    for(int i = 0; i < 26; ++i) //pull words out from all the heaps.
    {
        while(!WordStore[i].empty())
        {
            tempnode = WordStore[i].pop();
            (word_data[tempnode->theData()])[tempnode->thepara()].push_back(tempnode->theline());
            para_list[tempnode->theData()].push_back(tempnode->thepara());
            tempvec.push_back(tempnode->theData());
        }
    }
    //Remove duplicates in tempvec
    iter = unique(tempvec.begin(), tempvec.end());
    tempvec.resize(distance(tempvec.begin(),iter));

    //FIND TOP TEN WORDS and Put into container
    Word_Ranking.resize(tempvec.size()+1);//EDITED THIS AND FOR LOOP BELOW
    for(size_t i = 0; i < tempvec.size(); ++i)
    {
        Word_Ranking.enqueue(tempvec[i],word_frequency[tempvec[i]]);
        WordList.push_back(tempvec[i]);
    }

    clockT = clock();//DEBUG

    //Process the data for line num and para num
    for(size_t m = 0; m < WordList.size(); ++m)
    {
        string str = WordList[m];

        //Sort Para_List
        paraiterBegin = para_list[str].begin();
        paraiterEnd = para_list[str].end();
        sort(paraiterBegin,paraiterEnd);
        //remove duplicate in para list
        uniqueiter = unique(para_list[str].begin(), para_list[str].end());//get rid of duplicates in tempvec
        para_list[str].resize(distance(para_list[str].begin(),uniqueiter));//go with uniqu func.

        for(size_t i = 0; i < para_list[str].size(); ++i)
        {
            //sort and remove duplicate in line num vector
            paraiterBegin = (word_data[str])[para_list[str].at(i)].begin();
            paraiterEnd = (word_data[str])[para_list[str].at(i)].end();
            sort(paraiterBegin,paraiterEnd);
            //Remove duplicate in line num vector
            uniqueiter = unique((word_data[str])[para_list[str].at(i)].begin(),
                    (word_data[str])[para_list[str].at(i)].end());
            (word_data[str])[para_list[str].at(i)].resize(
                        distance((word_data[str])[para_list[str].at(i)].begin(),uniqueiter));//go with unique func.
        }
    }
    cout<<"TIME FOR DATA PROCESS:"<<clock()-clockT<<endl; //DEBUG
}

map<int,vector<int> >& TextParser::WordDataMap(string& s)
{
    return word_data[s];
}

pQueue<string,int>& TextParser::word_rank()
{
    return Word_Ranking;
}

vector<pair<string, int> > &TextParser::TopTen()
{
    return TopTenWord;
}

vector<int> & TextParser::ParaList(string& s)
{
    return para_list[s];
}
