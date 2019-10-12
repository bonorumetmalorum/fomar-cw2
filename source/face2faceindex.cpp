#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

struct Vert{
    float x = 0;
    float y = 0;
    float z = 0;
};

int main(int argc, char **argv){
    ifstream triangleSoup;
    triangleSoup.open(argv[1]);
    long nTriangles = 0;
    triangleSoup >> nTriangles;
    cout << "number of traingles in file: " << nTriangles << endl;
    long nVertices = 3*nTriangles;
    vector<Vert> vertices(nVertices);
    for(int i = 0; i < nVertices; i++){
        triangleSoup >> vertices[i].x >> vertices[i].y >> vertices[i].z;
    }
    for(Vert v : vertices){
        cout << v.x << " " << v.y << " " << v.z << endl;
    }
}