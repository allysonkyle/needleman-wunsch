#include "nwAlgorithm.h"

using namespace std;

int main() {
    nwAlgorithm nw;
    nw.reader("input.txt");
    nw.print_matrix();
    nw.print_alignment();
    
    return 0;
}

