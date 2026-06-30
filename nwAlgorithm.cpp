#include "nwAlgorithm.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <string>
#include <algorithm>
#include <iomanip>

using namespace std;

// read sequences
void nwAlgorithm::reader(const string& filename) {
    // open file
    ifstream fin(filename);

    // if file won't open
    if (!fin) {
        cout << "could not open file" << endl;
        return;
    }

    // get each sequence and output for checking
    getline(fin, seq1);
    cout << seq1 << endl;
    getline(fin, seq2);
    cout << seq2 << endl;

    // output table into a separate file when past standard terminal size (80 characters)
    if(seq1.length() >= 80 || seq2.length() >= 80) {
        requireFileOutput = true;
    }

    // close input file and return
    fin.close();
    return;
}

// scoring diagonal cells
int nwAlgorithm::score_cell(char a, char b) {
    return (a == b) ? match : mismatch;
}

// create and fill the matrix
vector<vector<nwAlgorithm::cell>> nwAlgorithm::build_matrix () {
    // reset values
    match_total = mismatch_total = gap_total = 0;
    while (!optimal_s1.empty()) optimal_s1.pop();
    while (!optimal_s2.empty()) optimal_s2.pop();
    
    // get matrix dimensions
    int width = seq1.length() + 1;
    int height = seq2.length() + 1;
    
    // add spaces for display
    x_seq = " " + seq1;
    y_seq = " " + seq2;
    
    // create matrix of cells
    vector<vector<nwAlgorithm::cell>> cells(width, vector<nwAlgorithm::cell>(height)); 
    
    // initialize first row (gaps)
    for (int x = 0; x < width; x++) {
        cells[x][0].score = x * -2;
        cells[x][0].direction = "h"; // left/horizontal
    }
    
    // initialize first column (gaps)
    for (int y = 0; y < height; y++) {
        cells[0][y].score = y * -2;
        cells[0][y].direction = "v"; // up/vertical
    }
    
    // fill the rest of the matrix --can only match diagonally
    for (int y = 1; y < height; y++) {
        for (int x = 1; x < width; x++) {
            // match/mismatch score (diagonal)
            int d_score = cells[x - 1][y - 1].score + score_cell(seq1[x - 1], seq2[y - 1]);
            
            // gap (horizontal)
            int h_score = cells[x-1][y].score - 2;
            
            // gap (vertical)
            int v_score = cells[x][y-1].score - 2;
            
            // take max and track direction (multiple if needed)
            int max_score = max({d_score, h_score, v_score});
            cells[x][y].score = max_score;
            //cells[x][y].direction = "";
            
            // set cell direction
            if (d_score >= h_score && d_score >= v_score) cells[x][y].direction = "d";
            else if (h_score >= v_score) cells[x][y].direction = "h";
            else cells[x][y].direction = "v";

        }
    }
    // perform traceback to build alignment
    trace_back(cells, seq1.length(), seq2.length());
    return cells;
}

void nwAlgorithm::print_matrix() {
    // build matrix and get dimensions
    vector<vector<nwAlgorithm::cell>> matrix = nwAlgorithm::build_matrix();
    int width = matrix.size();
    int height = matrix[0].size();
    
    // column width for evenly spacing display
    int col_width = 8;
    
    // choose output stream based on requireFileOutput
    ofstream fout;
    ostream* data_stream = &cout; // originally initialized to cout
    
    // if file output is needed
    if (requireFileOutput) {
        // tell user that data is in *.txt
        cout << "Sequence(s) exceed standard terminal size, outputting to table.txt..." << endl;
        fout.open("table.txt");
        if (!fout) {
            cout << "Could not open table.txt for writing" << endl;
            return;
        }
        data_stream = &fout; // switches reference to fout
    }
    
    // print header row with x_seq labels
    *data_stream << setw(col_width) << " ";
    for (int x = 0; x < width; x++) {
        *data_stream << setw(col_width) << x_seq[x];
    }
    *data_stream << endl;
    
    // print rows with y_seq labels
    for (int y = 0; y < height; y++) {
        if (y == 0) {
            *data_stream << setw(col_width) << " ";
        } else {
            *data_stream << setw(col_width) << y_seq[y];
        }
        
        // print row values and direction(s)
        for (int x = 0; x < width; x++) {
            string cell_str = to_string(matrix[x][y].score) + " " + matrix[x][y].direction;
            *data_stream << setw(col_width) << cell_str;
        }
        *data_stream << endl;
    }
    
    // close file if it was opened
    if (requireFileOutput) fout.close();
}

void nwAlgorithm::trace_back (const std::vector<std::vector<nwAlgorithm::cell>> &matrix, int x, int y) {
    // iterative traceback to avoid out-of-bounds
    while (x > 0 || y > 0) {
        if (x < 0) x = 0;
        if (y < 0) y = 0;

        const std::string &dir = matrix[x][y].direction;

        if (dir == "d") {
            // add character to top of stack as it travels backward
            optimal_s1.push(seq1[x - 1]);
            optimal_s2.push(seq2[y - 1]);

            if (optimal_s1.top() == optimal_s2.top()) match_total += 1; // if a match add to total match count
            else mismatch_total += 1; // else add to total mismatch

            // set next cell location
            x -= 1; y -= 1;

        } else if (dir == "h") {
            // add character to top of stack as it travels backward
            optimal_s1.push(seq1[x - 1]);
            optimal_s2.push('_');

            gap_total += 1; // add to gap total

            x -= 1; // travel one cell to the left

        } else if (dir == "v") {
            // add character to top of stack as it travels backward
            optimal_s1.push('_');
            optimal_s2.push(seq2[y - 1]);

            gap_total += 1; // add to gap total

            y -= 1; // travel one cell up

        } else {
            break;
        }
    }
}

void nwAlgorithm::print_alignment () {
    // choose output stream based on requireFileOutput
    ofstream fout;
    ostream* data_stream = &cout; // originally reference cout for terminal print
    
    // if file output is needed
    if (requireFileOutput) {
        cout << "Alignment data exceeds standard terminal size, outputting to alignment.txt..." << endl;
        fout.open("alignment.txt");
        if (!fout) {
            cout << "Could not open alignment.txt for writing" << endl;
            return;
        }
        data_stream = &fout; // reference fout for file output
    }
    
    *data_stream << endl; 
    // while stacks are not empty
    while (!optimal_s1.empty()) {
        *data_stream << optimal_s1.top() << "  "; // print top value and spacing after
        optimal_s1.pop(); // remove top value
    }
    *data_stream << endl;
    while (!optimal_s2.empty()) {
        *data_stream << optimal_s2.top() << "  ";
        optimal_s2.pop();
    }
    // print totals along with final alignment score
    *data_stream << "\n" << "Matches: " << match_total;
    *data_stream << "  Mismatches: " << mismatch_total;
    *data_stream << "  Gaps: " << gap_total << endl;
    *data_stream << "Alignment Score: " << ((gap_total * gap) + match_total + (mismatch_total * mismatch)) << endl;
    
    // close file if it was opened
    if (requireFileOutput) fout.close();
    return;
}