#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char **argv){
    ifstream triangleSoup;
    triangleSoup.open(argv[1]);
    long noTriangle = 0;
    triangleSoup >> noTriangle;
    cout << "number of traingles in file: " << noTriangle << endl;
}