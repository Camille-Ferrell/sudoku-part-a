#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "d_matrix.h"
#include "d_except.h"

using namespace std;

typedef int ValueType; // The type of the value in a cell
const int Blank = -1;  // Indicates that a cell is blank

const int SquareSize = 3;  // The number of cells in a small square
const int BoardSize = SquareSize * SquareSize;
const int MinValue = 1;
const int MaxValue = 9;

class board {
public:
    board();
    void clear();
    void initialize(ifstream& fin);
    void print();
    bool isBlank(int i, int j);
    ValueType getCell(int i, int j);
    void printConflicts();
    bool solve();
    bool checkConflicts(int i, int j, ValueType val);
    void setCell(int i, int j, ValueType val);
    void resetCell(int i, int j);

private:
    matrix<ValueType> value;
    vector<vector<bool>> rowConflicts;
    vector<vector<bool>> colConflicts;
    vector<vector<bool>> squareConflicts;
    int recursiveCalls;

    void updateConflicts(int i, int j, ValueType val, bool conflict);
    bool solveRecursive();
};

board::board() : value(BoardSize + 1, BoardSize + 1) {
    clear();
}

void board::clear() {
    for (int i = 1; i <= BoardSize; ++i) {
        for (int j = 1; j <= BoardSize; ++j) {
            value[i][j] = Blank;
        }
    }
    rowConflicts.assign(BoardSize + 1, vector<bool>(MaxValue + 1, false));
    colConflicts.assign(BoardSize + 1, vector<bool>(MaxValue + 1, false));
    squareConflicts.assign(BoardSize + 1, vector<bool>(MaxValue + 1, false));
    recursiveCalls = 0;
}

void board::initialize(ifstream& fin) {
    char ch;
    clear();

    for (int i = 1; i <= BoardSize; ++i) {
        for (int j = 1; j <= BoardSize; ++j) {
            fin >> ch;
            if (ch != '.') {
                setCell(i, j, ch - '0');
            }
        }
    }
}

void board::print() {
    for (int i = 1; i <= BoardSize; ++i) {
        if ((i - 1) % SquareSize == 0) {
            cout << " -";
            for (int j = 1; j <= BoardSize; ++j) {
                cout << "---";
            }
            cout << "-";
            cout << endl;
        }
        for (int j = 1; j <= BoardSize; ++j) {
            if ((j - 1) % SquareSize == 0) {
                cout << "|";
            }
            if (!isBlank(i, j)) {
                cout << " " << getCell(i, j) << " ";
            } else {
                cout << "   ";
            }
        }
        cout << "|";
        cout << endl;
    }
    cout << " -";
    for (int j = 1; j <= BoardSize; ++j) {
        cout << "---";
    }
    cout << "-";
    cout << endl;
}

bool board::isBlank(int i, int j) {
    return (getCell(i, j) == Blank);
}

ValueType board::getCell(int i, int j) {
    if (i < 1 || i > BoardSize || j < 1 || j > BoardSize) {
        throw rangeError("getCell: invalid index");
    }
    return value[i][j];
}

void board::printConflicts() {
    cout << "Row Conflicts:" << endl;
    for (int i = 1; i <= BoardSize; ++i) {
        cout << "Row " << i << ": ";
        for (int val = MinValue; val <= MaxValue; ++val) {
            if (rowConflicts[i][val]) {
                cout << val << " ";
            }
        }
        cout << endl;
    }

    cout << "Column Conflicts:" << endl;
    for (int j = 1; j <= BoardSize; ++j) {
        cout << "Column " << j << ": ";
        for (int val = MinValue; val <= MaxValue; ++val) {
            if (colConflicts[j][val]) {
                cout << val << " ";
            }
        }
        cout << endl;
    }

    cout << "Square Conflicts:" << endl;
    for (int k = 1; k <= BoardSize; ++k) {
        cout << "Square " << k << ": ";
        for (int val = MinValue; val <= MaxValue; ++val) {
            if (squareConflicts[k][val]) {
                cout << val << " ";
            }
        }
        cout << endl;
    }
}

bool board::checkConflicts(int i, int j, ValueType val) {
    int square = SquareSize * ((i - 1) / SquareSize) + (j - 1) / SquareSize + 1;
    return rowConflicts[i][val] || colConflicts[j][val] || squareConflicts[square][val];
}

void board::setCell(int i, int j, ValueType val) {
    value[i][j] = val;
    updateConflicts(i, j, val, true);
}

void board::resetCell(int i, int j) {
    int val = value[i][j];
    value[i][j] = Blank;
    updateConflicts(i, j, val, false);
}

void board::updateConflicts(int i, int j, ValueType val, bool conflict) {
    int square = SquareSize * ((i - 1) / SquareSize) + (j - 1) / SquareSize + 1;
    rowConflicts[i][val] = conflict;
    colConflicts[j][val] = conflict;
    squareConflicts[square][val] = conflict;
}

bool board::solve() {
    recursiveCalls = 0;
    bool solved = solveRecursive();
    cout << "Number of recursive calls: " << recursiveCalls << endl;
    return solved;
}

bool board::solveRecursive() {
    ++recursiveCalls;

    for (int i = 1; i <= BoardSize; ++i) {
        for (int j = 1; j <= BoardSize; ++j) {
            if (isBlank(i, j)) {
                for (int val = MinValue; val <= MaxValue; ++val) {
                    if (!checkConflicts(i, j, val)) {
                        setCell(i, j, val);
                        if (solveRecursive()) {
                            return true;
                        }
                        resetCell(i, j);
                    }
                }
                return false;
            }
        }
    }
    return true;
}

int main() {
    ifstream fin;
    int fileNumber;
    vector<string> files = {"sudoku1.txt", "sudoku2.txt", "sudoku3.txt", "sudoku.txt", "sudoku1-3.txt"};

    while (true) {
        cout << "Enter the file number (1, 2, 3, 4, or 5): ";
        cin >> fileNumber;

        if (fileNumber < 1 || fileNumber > 5) {
            cout << "Invalid file number. Please enter 1, 2, 3, 4, or 5." << endl;
            continue;
        }

        string fileName = files[fileNumber - 1];
        fin.open(fileName);
        if (!fin) {
            cerr << "Cannot open " << fileName << endl;
            return 1;
        }

        int totalRecursiveCalls = 0;
        int numBoards = 0;

        try {
            board b;
            while (fin && fin.peek() != 'Z') {
                b.initialize(fin);
                b.print();
                if (b.solve()) {
                    cout << "Solved board:" << endl;
                    b.print();
                } else {
                    cout << "No solution exists for this board." << endl;
                }
                totalRecursiveCalls += b.solve();
                ++numBoards;
            }
        } catch (indexRangeError &ex) {
            cout << ex.what() << endl;
            return 1;
        }

        fin.close();

        if (numBoards > 0) {
            cout << "Total number of recursive calls: " << totalRecursiveCalls << endl;
            cout << "Average number of recursive calls: " << totalRecursiveCalls / numBoards << endl;
        }

        char choice;
        cout << "Do you want to process another file? (y/n): ";
        cin >> choice;

        if (choice != 'y' && choice != 'Y') {
            break;
        }
    }

    return 0;
}



/* PART B BUT NOT DONE *********************************
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "d_matrix.h"
#include "d_except.h"

using namespace std;

typedef int ValueType; // The type of the value in a cell
const int Blank = -1;  // Indicates that a cell is blank

const int SquareSize = 3;  // The number of cells in a small square
const int BoardSize = SquareSize * SquareSize;
const int MinValue = 1;
const int MaxValue = 9;

class board {
public:
    board();
    void clear();
    void initialize(ifstream& fin);
    void print();
    bool isBlank(int i, int j);
    ValueType getCell(int i, int j);
    void printConflicts();
    bool solve();
    bool checkConflicts(int i, int j, ValueType val);
    void setCell(int i, int j, ValueType val);
    void resetCell(int i, int j);

private:
    matrix<ValueType> value;
    vector<vector<bool>> rowConflicts;
    vector<vector<bool>> colConflicts;
    vector<vector<bool>> squareConflicts;
    int recursiveCalls;

    void updateConflicts(int i, int j, ValueType val, bool conflict);
    bool solveRecursive();
};

board::board() : value(BoardSize + 1, BoardSize + 1) {
    clear();
}

void board::clear() {
    for (int i = 1; i <= BoardSize; ++i) {
        for (int j = 1; j <= BoardSize; ++j) {
            value[i][j] = Blank;
        }
    }
    rowConflicts.assign(BoardSize + 1, vector<bool>(MaxValue + 1, false));
    colConflicts.assign(BoardSize + 1, vector<bool>(MaxValue + 1, false));
    squareConflicts.assign(BoardSize + 1, vector<bool>(MaxValue + 1, false));
    recursiveCalls = 0;
}

void board::initialize(ifstream& fin) {
    char ch;
    clear();

    for (int i = 1; i <= BoardSize; ++i) {
        for (int j = 1; j <= BoardSize; ++j) {
            fin >> ch;
            if (ch != '.') {
                setCell(i, j, ch - '0');
            }
        }
    }
}

void board::print() {
    for (int i = 1; i <= BoardSize; ++i) {
        if ((i - 1) % SquareSize == 0) {
            cout << " -";
            for (int j = 1; j <= BoardSize; ++j) {
                cout << "---";
            }
            cout << "-";
            cout << endl;
        }
        for (int j = 1; j <= BoardSize; ++j) {
            if ((j - 1) % SquareSize == 0) {
                cout << "|";
            }
            if (!isBlank(i, j)) {
                cout << " " << getCell(i, j) << " ";
            } else {
                cout << "   ";
            }
        }
        cout << "|";
        cout << endl;
    }
    cout << " -";
    for (int j = 1; j <= BoardSize; ++j) {
        cout << "---";
    }
    cout << "-";
    cout << endl;
}

bool board::isBlank(int i, int j) {
    return (getCell(i, j) == Blank);
}

ValueType board::getCell(int i, int j) {
    if (i < 1 || i > BoardSize || j < 1 || j > BoardSize) {
        throw rangeError("getCell: invalid index");
    }
    return value[i][j];
}

void board::printConflicts() {
    cout << "Row Conflicts:" << endl;
    for (int i = 1; i <= BoardSize; ++i) {
        cout << "Row " << i << ": ";
        for (int val = MinValue; val <= MaxValue; ++val) {
            if (rowConflicts[i][val]) {
                cout << val << " ";
            }
        }
        cout << endl;
    }

    cout << "Column Conflicts:" << endl;
    for (int j = 1; j <= BoardSize; ++j) {
        cout << "Column " << j << ": ";
        for (int val = MinValue; val <= MaxValue; ++val) {
            if (colConflicts[j][val]) {
                cout << val << " ";
            }
        }
        cout << endl;
    }

    cout << "Square Conflicts:" << endl;
    for (int k = 1; k <= BoardSize; ++k) {
        cout << "Square " << k << ": ";
        for (int val = MinValue; val <= MaxValue; ++val) {
            if (squareConflicts[k][val]) {
                cout << val << " ";
            }
        }
        cout << endl;
    }
}

bool board::checkConflicts(int i, int j, ValueType val) {
    int square = SquareSize * ((i - 1) / SquareSize) + (j - 1) / SquareSize + 1;
    return rowConflicts[i][val] || colConflicts[j][val] || squareConflicts[square][val];
}

void board::setCell(int i, int j, ValueType val) {
    value[i][j] = val;
    updateConflicts(i, j, val, true);
}

void board::resetCell(int i, int j) {
    int val = value[i][j];
    value[i][j] = Blank;
    updateConflicts(i, j, val, false);
}

void board::updateConflicts(int i, int j, ValueType val, bool conflict) {
    int square = SquareSize * ((i - 1) / SquareSize) + (j - 1) / SquareSize + 1;
    rowConflicts[i][val] = conflict;
    colConflicts[j][val] = conflict;
    squareConflicts[square][val] = conflict;
}

bool board::solve() {
    recursiveCalls = 0;
    bool solved = solveRecursive();
    cout << "Number of recursive calls: " << recursiveCalls << endl;
    return solved;
}

bool board::solveRecursive() {
    ++recursiveCalls;

    for (int i = 1; i <= BoardSize; ++i) {
        for (int j = 1; j <= BoardSize; ++j) {
            if (isBlank(i, j)) {
                for (int val = MinValue; val <= MaxValue; ++val) {
                    if (!checkConflicts(i, j, val)) {
                        setCell(i, j, val);
                        if (solveRecursive()) {
                            return true;
                        }
                        resetCell(i, j);
                    }
                }
                return false;
            }
        }
    }
    return true;
}

int main() {
    ifstream fin;
    int fileNumber;
    vector<string> files = {"sudoku1.txt", "sudoku2.txt", "sudoku3.txt"};

    while (true) {
        cout << "Enter the file number (1, 2, or 3): ";
        cin >> fileNumber;

        if (fileNumber < 1 || fileNumber > 3) {
            cout << "Invalid file number. Please enter 1, 2, or 3." << endl;
            continue;
        }

        string fileName = files[fileNumber - 1];
        fin.open(fileName);
        if (!fin) {
            cerr << "Cannot open " << fileName << endl;
            return 1;
        }

        int totalRecursiveCalls = 0;
        int numBoards = 0;

        try {
            board b;
            while (fin && fin.peek() != 'Z') {
                b.initialize(fin);
                b.print();
                if (b.solve()) {
                    cout << "Solved board:" << endl;
                    b.print();
                } else {
                    cout << "No solution exists for this board." << endl;
                }
                totalRecursiveCalls += b.solve();
                ++numBoards;
            }
        } catch (indexRangeError &ex) {
            cout << ex.what() << endl;
            return 1;
        }

        fin.close();

        if (numBoards > 0) {
            cout << "Average number of recursive calls: " << totalRecursiveCalls / numBoards << endl;
        }

        char choice;
        cout << "Do you want to process another file? (y/n): ";
        cin >> choice;

        if (choice != 'y' && choice != 'Y') {
            break;
        }
    }

    return 0;
}





Part A**********************

#include <iostream>
#include <limits.h>
#include "d_matrix.h"
#include "d_except.h"
#include <list>
#include <fstream>
#include <vector>

using namespace std;

typedef int ValueType; // The type of the value in a cell
const int Blank = -1;  // Indicates that a cell is blank
 
const int SquareSize = 3;  // The number of cells in a small square
                           // (usually 3).  The board has
                           // SquareSize^2 rows and SquareSize^2
                           // columns.

const int BoardSize = SquareSize * SquareSize;

const int MinValue = 1;
const int MaxValue = 9;

int numSolutions = 0;

class board
// Stores the entire Sudoku board
{
   public:
      board(int);
      void clear();
      void initialize(ifstream &fin);
      void print();
      void printConflicts();
      bool isBlank(int, int);
      ValueType getCell(int, int);
      void setCell(int, int, ValueType);
      void resetCell(int, int);
      bool fullySolved();
      
   private:

      // The following matrices go from 1 to BoardSize in each
      // dimension, i.e., they are each (BoardSize+1) * (BoardSize+1)

      matrix<ValueType> value;

      // Conflict vectors
      vector<vector<bool>> rowConflicts;
      vector<vector<bool>> colConflicts;
      vector<vector<bool>> squareConflicts;

      void updateConflicts();
};

board::board(int sqSize)
   : value(BoardSize+1, BoardSize+1),
     rowConflicts(BoardSize+1, vector<bool>(MaxValue+1, false)),
     colConflicts(BoardSize+1, vector<bool>(MaxValue+1, false)),
     squareConflicts(BoardSize+1, vector<bool>(MaxValue+1, false))
// Board constructor
{
   clear();
}

void board::clear()
// Mark all possible values as legal for each board entry
{
   for (int i = 1; i <= BoardSize; i++)
      for (int j = 1; j <= BoardSize; j++)
      {
         value[i][j] = Blank;
      }

   for (int i = 1; i <= BoardSize; i++) {
      fill(rowConflicts[i].begin(), rowConflicts[i].end(), false);
      fill(colConflicts[i].begin(), colConflicts[i].end(), false);
      fill(squareConflicts[i].begin(), squareConflicts[i].end(), false);
   }
}

void board::initialize(ifstream &fin)
// Read a Sudoku board from the input file.
{
   char ch;

   clear();
   
   for (int i = 1; i <= BoardSize; i++)
      for (int j = 1; j <= BoardSize; j++)
	    {
	       fin >> ch;

          // If the read char is not Blank
	      if (ch != '.')
             setCell(i,j,ch-'0');   // Convert char to int
        }
}

int squareNumber(int i, int j)
// Return the square number of cell i,j (counting from left to right,
// top to bottom.  Note that i and j each go from 1 to BoardSize
{
   // Note that (int) i/SquareSize and (int) j/SquareSize are the x-y
   // coordinates of the square that i,j is in.  

   return SquareSize * ((i-1)/SquareSize) + (j-1)/SquareSize + 1;
}

ostream &operator<<(ostream &ostr, vector<int> &v)
// Overloaded output operator for vector class.
{
   for (int i = 0; i < v.size(); i++)
      ostr << v[i] << " ";
   cout << endl;
}

ValueType board::getCell(int i, int j)
// Returns the value stored in a cell.  Throws an exception
// if bad values are passed.
{
   if (i >= 1 && i <= BoardSize && j >= 1 && j <= BoardSize)
      return value[i][j];
   else
      throw rangeError("bad value in getCell");
}

bool board::isBlank(int i, int j)
// Returns true if cell i,j is blank, and false otherwise.
{
   if (i < 1 || i > BoardSize || j < 1 || j > BoardSize)
      throw rangeError("bad value in setCell");

   return (getCell(i,j) == Blank);
}

void board::setCell(int i, int j, ValueType val)
// Sets the value of cell i,j to val and updates conflicts.
{
   if (i < 1 || i > BoardSize || j < 1 || j > BoardSize || val < MinValue || val > MaxValue)
      throw rangeError("bad value in setCell");

   value[i][j] = val;
   rowConflicts[i][val] = true;
   colConflicts[j][val] = true;
   squareConflicts[squareNumber(i, j)][val] = true;
}

void board::resetCell(int i, int j)
// Resets the value of cell i,j to blank and updates conflicts.
{
   if (i < 1 || i > BoardSize || j < 1 || j > BoardSize)
      throw rangeError("bad value in resetCell");

   int val = getCell(i, j);
   value[i][j] = Blank;
   rowConflicts[i][val] = false;
   colConflicts[j][val] = false;
   squareConflicts[squareNumber(i, j)][val] = false;
}

void board::print()
// Prints the current board.
{
   for (int i = 1; i <= BoardSize; i++)
   {
      if ((i-1) % SquareSize == 0)
      {
         cout << " -";
	 for (int j = 1; j <= BoardSize; j++)
	    cout << "---";
         cout << "-";
	 cout << endl;
      }
      for (int j = 1; j <= BoardSize; j++)
      {
	 if ((j-1) % SquareSize == 0)
	    cout << "|";
	 if (!isBlank(i,j))
	    cout << " " << getCell(i,j) << " ";
	 else
	    cout << "   ";
      }
      cout << "|";
      cout << endl;
   }

   cout << " -";
   for (int j = 1; j <= BoardSize; j++)
      cout << "---";
   cout << "-";
   cout << endl;
}

// ********* Prints Conflicts as numbers ***************************
void board::printConflicts()
// Prints the conflicts at their specific positions.
{
    cout << "Row Conflicts:" << endl;
    for (int i = 1; i <= BoardSize; ++i) {
        cout << "Row " << i << ": ";
        for (int val = MinValue; val <= MaxValue; ++val) {
            if (rowConflicts[i][val]) {
                cout << val << " ";
            }
        }
        cout << endl;
    }

    cout << "Column Conflicts:" << endl;
    for (int j = 1; j <= BoardSize; ++j) {
        cout << "Column " << j << ": ";
        for (int val = MinValue; val <= MaxValue; ++val) {
            if (colConflicts[j][val]) {
                cout << val << " ";
            }
        }
        cout << endl;
    }

    cout << "Square Conflicts:" << endl;
    for (int k = 1; k <= BoardSize; ++k) {
        cout << "Square " << k << ": ";
        for (int val = MinValue; val <= MaxValue; ++val) {
            if (squareConflicts[k][val]) {
                cout << val << " ";
            }
        }
        cout << endl;
    }
}

//******** Prints Conflicts on Grid ************
/*
void board::printConflicts()
// Prints the conflicts.
{
   cout << "Row Conflicts:" << endl;
   for (int i = 1; i <= BoardSize; i++) {
      for (int j = 1; j <= MaxValue; j++) {
         cout << rowConflicts[i][j] << " ";
      }
      cout << endl;
   }

   cout << "Column Conflicts:" << endl;
   for (int i = 1; i <= BoardSize; i++) {
      for (int j = 1; j <= MaxValue; j++) {
         cout << colConflicts[i][j] << " ";
      }
      cout << endl;
   }

   cout << "Square Conflicts:" << endl;
   for (int i = 1; i <= BoardSize; i++) {
      for (int j = 1; j <= MaxValue; j++) {
         cout << squareConflicts[i][j] << " ";
      }
      cout << endl;
   }
}

bool board::fullySolved()
// Checks if the board is fully solved.
{
   for (int i = 1; i <= BoardSize; i++) {
      for (int j = 1; j <= BoardSize; j++) {
         if (isBlank(i, j))
            return false;
      }
   }

   for (int i = 1; i <= BoardSize; i++) {
      for (int val = 1; val <= MaxValue; val++) {
         if (!rowConflicts[i][val] || !colConflicts[i][val] || !squareConflicts[squareNumber(i, 1)][val])
            return false;
      }
   }

   cout << "The board is fully solved!" << endl;
   return true;
}

int main()
{
   vector<string> fileNames = {"sudoku1.txt", "sudoku2.txt", "sudoku3.txt"};
   
   int fileChoice;
   cout << "Enter the number of the file to read (1, 2, or 3): ";
   cin >> fileChoice;

   if (fileChoice < 1 || fileChoice > 3) {
      cerr << "Invalid choice. Please enter 1, 2, or 3." << endl;
      return 1;
   }

   string fileName = fileNames[fileChoice - 1];

   ifstream fin;
   fin.open(fileName.c_str());
   if (!fin)
   {
      cerr << "Cannot open " << fileName << endl;
      return 1;
   }

   try
   {
      board b1(SquareSize);

      while (fin && fin.peek() != 'Z')
      {
         b1.initialize(fin);
         b1.print();
         b1.printConflicts();
         if (b1.fullySolved())
            cout << "The board is solved." << endl;
         else
            cout << "The board is not solved." << endl;
      }
   }
   catch  (indexRangeError &ex)
   {
      cout << ex.what() << endl;
      return 1;
   }

   return 0;
}

/*
#include <iostream>
#include <limits.h>
#include "d_matrix.h"
#include "d_except.h"
#include <list>
#include <fstream>
#include <vector>

using namespace std;

typedef int ValueType; // The type of the value in a cell
const int Blank = -1;  // Indicates that a cell is blank
 
const int SquareSize = 3;  // The number of cells in a small square
                           // (usually 3).  The board has
                           // SquareSize^2 rows and SquareSize^2
                           // columns.

const int BoardSize = SquareSize * SquareSize;

const int MinValue = 1;
const int MaxValue = 9;

int numSolutions = 0;

class board
// Stores the entire Sudoku board
{
   public:
      board(int);
      void clear();
      void initialize(ifstream &fin);
      void print();
      void printConflicts();
      bool isBlank(int, int);
      ValueType getCell(int, int);
      void setCell(int, int, ValueType);
      void resetCell(int, int);
      bool fullySolved();
      
   private:

      // The following matrices go from 1 to BoardSize in each
      // dimension, i.e., they are each (BoardSize+1) * (BoardSize+1)

      matrix<ValueType> value;

      // Conflict vectors
      vector<vector<bool>> rowConflicts;
      vector<vector<bool>> colConflicts;
      vector<vector<bool>> squareConflicts;

      void updateConflicts();
};

board::board(int sqSize)
   : value(BoardSize+1, BoardSize+1),
     rowConflicts(BoardSize+1, vector<bool>(MaxValue+1, false)),
     colConflicts(BoardSize+1, vector<bool>(MaxValue+1, false)),
     squareConflicts(BoardSize+1, vector<bool>(MaxValue+1, false))
// Board constructor
{
   clear();
}

void board::clear()
// Mark all possible values as legal for each board entry
{
   for (int i = 1; i <= BoardSize; i++)
      for (int j = 1; j <= BoardSize; j++)
      {
         value[i][j] = Blank;
      }

   for (int i = 1; i <= BoardSize; i++) {
      fill(rowConflicts[i].begin(), rowConflicts[i].end(), false);
      fill(colConflicts[i].begin(), colConflicts[i].end(), false);
      fill(squareConflicts[i].begin(), squareConflicts[i].end(), false);
   }
}

void board::initialize(ifstream &fin)
// Read a Sudoku board from the input file.
{
   char ch;

   clear();
   
   for (int i = 1; i <= BoardSize; i++)
      for (int j = 1; j <= BoardSize; j++)
	    {
	       fin >> ch;

          // If the read char is not Blank
	      if (ch != '.')
             setCell(i,j,ch-'0');   // Convert char to int
        }
}

int squareNumber(int i, int j)
// Return the square number of cell i,j (counting from left to right,
// top to bottom.  Note that i and j each go from 1 to BoardSize
{
   // Note that (int) i/SquareSize and (int) j/SquareSize are the x-y
   // coordinates of the square that i,j is in.  

   return SquareSize * ((i-1)/SquareSize) + (j-1)/SquareSize + 1;
}

ostream &operator<<(ostream &ostr, vector<int> &v)
// Overloaded output operator for vector class.
{
   for (int i = 0; i < v.size(); i++)
      ostr << v[i] << " ";
   cout << endl;
}

ValueType board::getCell(int i, int j)
// Returns the value stored in a cell.  Throws an exception
// if bad values are passed.
{
   if (i >= 1 && i <= BoardSize && j >= 1 && j <= BoardSize)
      return value[i][j];
   else
      throw rangeError("bad value in getCell");
}

bool board::isBlank(int i, int j)
// Returns true if cell i,j is blank, and false otherwise.
{
   if (i < 1 || i > BoardSize || j < 1 || j > BoardSize)
      throw rangeError("bad value in setCell");

   return (getCell(i,j) == Blank);
}

void board::setCell(int i, int j, ValueType val)
// Sets the value of cell i,j to val and updates conflicts.
{
   if (i < 1 || i > BoardSize || j < 1 || j > BoardSize || val < MinValue || val > MaxValue)
      throw rangeError("bad value in setCell");

   value[i][j] = val;
   rowConflicts[i][val] = true;
   colConflicts[j][val] = true;
   squareConflicts[squareNumber(i, j)][val] = true;
}

void board::resetCell(int i, int j)
// Resets the value of cell i,j to blank and updates conflicts.
{
   if (i < 1 || i > BoardSize || j < 1 || j > BoardSize)
      throw rangeError("bad value in resetCell");

   int val = getCell(i, j);
   value[i][j] = Blank;
   rowConflicts[i][val] = false;
   colConflicts[j][val] = false;
   squareConflicts[squareNumber(i, j)][val] = false;
}

void board::print()
// Prints the current board.
{
   for (int i = 1; i <= BoardSize; i++)
   {
      if ((i-1) % SquareSize == 0)
      {
         cout << " -";
	 for (int j = 1; j <= BoardSize; j++)
	    cout << "---";
         cout << "-";
	 cout << endl;
      }
      for (int j = 1; j <= BoardSize; j++)
      {
	 if ((j-1) % SquareSize == 0)
	    cout << "|";
	 if (!isBlank(i,j))
	    cout << " " << getCell(i,j) << " ";
	 else
	    cout << "   ";
      }
      cout << "|";
      cout << endl;
   }

   cout << " -";
   for (int j = 1; j <= BoardSize; j++)
      cout << "---";
   cout << "-";
   cout << endl;
}

void board::printConflicts()
// Prints the conflicts.
{
   cout << "Row Conflicts:" << endl;
   for (int i = 1; i <= BoardSize; i++) {
      for (int j = 1; j <= MaxValue; j++) {
         cout << rowConflicts[i][j] << " ";
      }
      cout << endl;
   }

   cout << "Column Conflicts:" << endl;
   for (int i = 1; i <= BoardSize; i++) {
      for (int j = 1; j <= MaxValue; j++) {
         cout << colConflicts[i][j] << " ";
      }
      cout << endl;
   }

   cout << "Square Conflicts:" << endl;
   for (int i = 1; i <= BoardSize; i++) {
      for (int j = 1; j <= MaxValue; j++) {
         cout << squareConflicts[i][j] << " ";
      }
      cout << endl;
   }
}

bool board::fullySolved()
// Checks if the board is fully solved.
{
   for (int i = 1; i <= BoardSize; i++) {
      for (int j = 1; j <= BoardSize; j++) {
         if (isBlank(i, j))
            return false;
      }
   }

   for (int i = 1; i <= BoardSize; i++) {
      for (int val = 1; val <= MaxValue; val++) {
         if (!rowConflicts[i][val] || !colConflicts[i][val] || !squareConflicts[squareNumber(i, 1)][val])
            return false;
      }
   }

   cout << "The board is fully solved!" << endl;
   return true;
}

int main()
{
   vector<string> fileNames = {"sudoku1.txt", "sudoku2.txt", "sudoku3.txt"};

   for (const string &fileName : fileNames) {
      ifstream fin;
      fin.open(fileName.c_str());
      if (!fin)
      {
         cerr << "Cannot open " << fileName << endl;
         continue;
      }

      try
      {
         board b1(SquareSize);

         while (fin && fin.peek() != 'Z')
         {
            b1.initialize(fin);
            b1.print();
            b1.printConflicts();
            if (b1.fullySolved())
               cout << "The board is solved." << endl;
            else
               cout << "The board is not solved." << endl;
         }
      }
      catch  (indexRangeError &ex)
      {
         cout << ex.what() << endl;
         exit(1);
      }

      fin.close();
   }
}



bool board::isBlank(int i, int j)
// Returns true if cell i,j is blank, and false otherwise.
{
   if (i < 1 || i > BoardSize || j < 1 || j > BoardSize)
      throw rangeError("bad value in setCell");

   return (getCell(i,j) == Blank);
}

void board::print()
// Prints the current board.
{
   for (int i = 1; i <= BoardSize; i++)
   {
      if ((i-1) % SquareSize == 0)
      {
         cout << " -";
	 for (int j = 1; j <= BoardSize; j++)
	    cout << "---";
         cout << "-";
	 cout << endl;
      }
      for (int j = 1; j <= BoardSize; j++)
      {
	 if ((j-1) % SquareSize == 0)
	    cout << "|";
	 if (!isBlank(i,j))
	    cout << " " << getCell(i,j) << " ";
	 else
	    cout << "   ";
      }
      cout << "|";
      cout << endl;
   }

   cout << " -";
   for (int j = 1; j <= BoardSize; j++)
      cout << "---";
   cout << "-";
   cout << endl;
}

int main()
{
   ifstream fin;
   
   // Read the sample grid from the file.
   string fileName = "sudoku.txt";

   fin.open(fileName.c_str());
   if (!fin)
   {
      cerr << "Cannot open " << fileName << endl;
      exit(1);
   }

   try
   {
      board b1(SquareSize);

      while (fin && fin.peek() != 'Z')
      {
	 b1.initialize(fin);
	 b1.print();
	 b1.printConflicts();
      }
   }
   catch  (indexRangeError &ex)
   {
      cout << ex.what() << endl;
      exit(1);
   }
}
*/