#include <iostream>
#include <fstream>
#include <vector>

using namespace std;


int main(int argc, char **argv){
    ifstream faceFile;
    faceFile.open(argv[1]);
    vector<string> header(8);
    for(int i = 0; i < 8; i++){
        getline(faceFile, header[i]);
        cout << header[i] << endl;
    }

}