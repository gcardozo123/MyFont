#include "IOFile.h"
#include <iostream>
#include<sstream> //stringstream
#include<string>
#include<stdlib.h>
#include<unistd.h>

using namespace std;

IOFile::IOFile (string fileName)
{
   //myfile.open(fileName.c_str(), ofstream::out | ofstream::app);
   //myfile.open(fileName.c_str());
   inptFile.open(fileName.c_str());
}

void IOFile::println(string line)
{
   if (myfile.is_open())
   {
       myfile << line << endl;
   }
   else cout << "Unable to open file";
}

void IOFile::print(string text)
{
   if (myfile.is_open())
   {
       myfile << text;
   }
   else cout << "Unable to open file";
}

void IOFile::printInt(int n)
{
   if (myfile.is_open())
   {
       myfile << n;
   }
   else cout << "Unable to open file";
}

void IOFile::closeFile()
{
   if (myfile.is_open())
   {
       myfile.close();
   }
   if (inptFile.is_open())
   {
       inptFile.close();
   }
}

//Reads the entire file into this->fullFileSS
void IOFile::readAllFile(){
    string aux;
    while(getline(inptFile, aux)){
        fullFileSS << aux << "\n";
        //cout << "getline: " << aux << endl;
    }
    //cout << fullFileSS.str();

  /*Another way to copy a file to memory:/
  /*
  if (inptFile) {
    // get length of file:
    inptFile.seekg (0, inptFile.end);
    int length = inptFile.tellg();
    inptFile.seekg (0, inptFile.beg);

    // allocate memory:
    char * buffer = new char [length];

    // read data as a block:
    inptFile.read (buffer,length);

    inptFile.close();

    // print content:
    std::cout << buffer ;

    delete[] buffer;
  }
  /***/

}

//Fill the array of bytes with the contents of a specific line, using line-1 to know how many "words" must be read
//return the number of bytes that were read
int IOFile::fillArrayWithLine(int line, unsigned char arr[]){
    char aux[2000];
    fullFileSS.seekg(0,ios_base::beg); //go back to the beginning of the string stream

    for(int i=0; i < line-1 ; i++){
        fullFileSS.getline(aux, 2000, '\n'); //looking for the number indicating how many bytes we need to read
    }
    //cout << "\n\nNum of bytes to read from file: " << aux ;
    string bf;
    //cout << "\nLinha lida: ";
    for(int i=0; i < atoi(aux); i++){
        fullFileSS >> bf;
        arr[i] = atoi(bf.c_str());
        //cout << bf << " ";
        sleep(1);

    }
    fullFileSS.seekg(0,ios_base::beg); //go back to the beginning of the string stream

    return atoi(aux);
}



