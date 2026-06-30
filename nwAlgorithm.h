#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <string>
#include <algorithm>
#include <iomanip>

class nwAlgorithm
{
private:
    // if the length is too long for the terminal to display properly (~80 characters)
    bool requireFileOutput = false;

    // initialize sequences
    std::string seq1 = "";
    std::string seq2 = "";

    // sequences specifically for printing
    // space at *_seq[0] so the alignment is easier when printing
    std::string y_seq = "";
    std::string x_seq = "";

    // optimal alignment strings
    std::stack<char> optimal_s1;
    std::stack<char> optimal_s2;

    // penalties/scores
    int match = 1;
    int mismatch = -2;
    int gap = -2;

    // type counters
    int gap_total = 0;
    int mismatch_total = 0;
    int match_total = 0;

public:
    nwAlgorithm(/* args */) {}; // ignore
    ~nwAlgorithm() {}; // ignore

    struct cell {
        int score = 0;
        // h = horizontal, v = vertical, d = diagonal 
        std::string direction;
    };

    // read sequences from file
    void reader(const std::string& filename);

    // determine cell score 
    int score_cell(char a, char b);

    // create and fill matrix
    std::vector<std::vector<cell>> build_matrix ();

    // print complete matrix with axis labels and directions
    void print_matrix();

    // traceback optimal alignment (matrix passed by const ref)
    void trace_back (const std::vector<std::vector<cell>> &matrix, int x, int y);

    // print alignment
    void print_alignment();
};
