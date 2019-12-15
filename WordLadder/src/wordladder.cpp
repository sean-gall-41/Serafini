#include <cctype>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include "simpio.h"
#include "strlib.h"
#include "console.h"
#include "stack.h"
#include "queue.h"
#include "lexicon.h"

using namespace std;

const string dictPrompt = "Dictionary file name? ";
const string reprompt = "File not recognized. Try again.";

void printIntro();
void printStack(string start, string end, Stack<string>& inStack);
Stack<string> stackCopy(Stack<string> stackCopied);
Stack<string> findWordLadder(string start, string end, Lexicon& myDict);

int main() {
    printIntro();
    bool notDone = true, badName = true;
    string wordOne = " ", wordTwo = " ";
    ifstream input; //fstream used to read in from txt file
    /*NOTE: assume once we do obtain a valid file name that it does contain the
      right info for our purposes*/
    while (badName)
    {
        string fileName = getLine(dictPrompt);
        input.open(fileName.c_str());
        if(!input.is_open()) cerr << reprompt << endl;
        else badName = false;
        //NOTE: no need to clear string bits if method "is_open()" sends fstream
        //into a failstate?
    }
    //close input fstream?
    /*Initialize lexicon object with fstream object containing contents of fileName
      in constructor*/
    Lexicon miniDict(input);
    input.close();
    while(notDone)
    {
        /*Get start and end words from user. If they press 'Enter' then set flag
          to false and drop out of program loop. Else, set input word to lower
          case for ease of comparison.*/
        wordOne = getLine("Word #1 (or Enter to quit): ");
        if (wordOne.length() == 0) notDone = false;
        else
        {
            wordOne = toLowerCase(wordOne);
            wordTwo = getLine("Word #2 (or Enter to quit): ");
            if (wordTwo.length() == 0) notDone = false;
            else
            {
                wordTwo = toLowerCase(wordTwo);

                if (wordOne.length() != wordTwo.length())
                {
                    cout << "The two words must be the same length.\n" << endl;
                }
                else
                {
                    if (wordOne == wordTwo)
                    {
                        cout << "The two words must be different.\n" << endl;
                    }
                    else
                    {
                        if (!(miniDict.contains(wordOne) && miniDict.contains(wordTwo)))
                        {
                            cout << "The two words must be found in the dictionary.\n" << endl;
                        }
                        else
                        {
                            Stack<string> aWordLadder;
                            aWordLadder = findWordLadder(wordOne, wordTwo, miniDict);
                            //if findWordLadder returns an empty stack, then no word ladder could be found
                            if (aWordLadder.isEmpty())
                            {
                                cout << "No word ladder from " << wordTwo
                                     << " to " << wordOne << ".\n" << endl;
                            }
                            else //Show user the stack
                            {
                                printStack(wordOne, wordTwo, aWordLadder);
                            }
                        }
                    }
                }
            }
        }
    }
    cout << "Have a nice day." << endl;
    //getLine("Press [Enter] to close the window... ");
    return 0;
}

void printIntro()
{
    cout << "Welcome to my wordladder.\nPlease give me two english words, and I "
         << "will change the\nfirst into the second by changing one letter at "
         << "a time.\n" << endl;
}

void printStack(string start, string end, Stack<string>& inStack)
{
    cout << "A ladder from " << end << " back to " << start << ":" << endl;
    while (!inStack.isEmpty())
    {
        cout << inStack.pop() << " ";
    }
    cout << "\n" << endl;
}

//function to copy stack one into stack two
//Leave stack copied alone or let it be 'emptied?'
Stack<string> stackCopy(Stack<string> stackCopied)
{
    string tempStr = " ";
    Stack<string> tempStack, stackCopiedTo;

    //Get items from stackCopied and place in tempStack
    while (!stackCopied.isEmpty())
    {
        tempStr = stackCopied.pop();
        tempStack.push(tempStr);
    }

    //Get items from tempStack and place in stackCopiedTo
    while (!tempStack.isEmpty())
    {
        tempStr = tempStack.pop();
        stackCopiedTo.push(tempStr);
    }
    return stackCopiedTo;

}

Stack<string> findWordLadder(string start, string end, Lexicon& myDict)
{
    string tempStr = start; //serves as the active word being checked...
    Stack<string> wordLadder, ladderCopy; //initialize an empty stack of words
    Queue<Stack<string>> ladderQueue; //initialize a queue of stacks of words
    Lexicon usedWords; //Might be uneccesary..
    wordLadder.push(tempStr); //now add the starting word onto the stack
    ladderQueue.enqueue(wordLadder); //now add the stack (containing start) to the queue
    while (!ladderQueue.isEmpty())
    {
        wordLadder = ladderQueue.dequeue(); //return active ladder from queue
        tempStr = wordLadder.peek(); //return value on top of stack w/o removing!
        for (unsigned int i = 0; i < tempStr.length(); i++)
        {
            char tempChar = tempStr[i]; //Create a temp var for char at i
            for (char test = 'a'; test <= 'z'; test++)
            {
                tempStr[i] = test;
                //Not all too nec to include below check?
                if (tempStr != start)
                {
                    if (myDict.contains(tempStr)) //For each valid neighbor...
                    {
                        //cout << tempStr << endl; //testing...
                        //cout << usedWords.toString() << endl;
                        if (!usedWords.contains(tempStr)) //Check if word has been included in previous ladders
                        {
                            if (tempStr == end) //Have found (a) shortest word ladder
                            {
                                //NOTE: we should add this word onto the word ladder first, yeah?
                                wordLadder.push(tempStr);
                                return wordLadder;
                            }
                            else
                            {
                                //cout << tempStr << endl; //testing...
                                ladderCopy = stackCopy(wordLadder); //call our function to copy current ladder
                                ladderCopy.push(tempStr); //add the current word on top of the copy stack
                                usedWords.add(tempStr); //add this word into our collection of used words
                                //cout << ladderCopy.toString() << endl;
                                ladderQueue.enqueue(ladderCopy); //enqueue the copy ladder
                            }
                        }
                    }
                }
            }
            //After the loop the value of tempStr[i] == 'z', so replace with
            //its original value before moving on.
            tempStr[i] = tempChar;
        }

    }
    //If we get through alg w/o returning, no ladder was found.
    //Need to clear the current ladder (regardless what values it contains)
    //s.t. we satisfy our condition for displaying to user that no ladder was found,
    //namely, that the ladder returned is empty.
    wordLadder.clear();
    return wordLadder;
}
