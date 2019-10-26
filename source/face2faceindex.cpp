#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

struct Vert{
    float x = 0;
    float y = 0;
    float z = 0;
};

//returns true if two verts are equal, false otherwise
bool compareVert(Vert &left, Vert &right){
    return left.x == right.x && left.y == right.y && left.z == right.z;
}

//time complexity: O(n)
//returns the index of the matching vertex, otherwise -1
int findVertex(Vert &vertex, vector<Vert> &verts){
    int size = verts.size();
    for(int i = 0; i < size; i++){
        if(compareVert(vertex, verts[i])){
            return i;
        }
    }
    return -1;
}

int main(int argc, char **argv){
    //read the file
    ifstream triangleSoup;
    triangleSoup.open(argv[1]);
    long nTriangles = 0;
    //get the number of triangles
    triangleSoup >> nTriangles;
    cout << "number of traingles in file: " << nTriangles << endl;
    //number fo vertices
    long nVertices = 3*nTriangles;
    //index of vertices
    vector<Vert> vertices;
    //faces index
    vector<int> faces;

    //Vertex variable
    Vert vert;
    
    //time complexity: O(n^2)
    for(int i = 0; i < nVertices; i++){
        triangleSoup >> vert.x >> vert.y >> vert.z;
        //check if the vertex is already in the array
        int index = findVertex(vert, vertices);
        if(index == -1){// if it is not then add at the end and add its index to the faces array
            vertices.push_back(vert);
            faces.push_back(vertices.size()-1);
        }else{//if it does exist then add its index to the faces array
            faces.push_back(index);
        }
    }
    triangleSoup.close();

    //make the file
    ofstream faceFile;
    string filename = argv[2];
    filename.append(".face");
    faceFile.open(filename);
    //header content
    faceFile << "# University of Leeds 2018-2019" << endl
    << "# COMP 5812M Assignment 2" << endl
    << "# Govind Venkatesh" << endl
    << "# 201332107" << endl
    << "#" << endl
    << "# Object Name: " << argv[2] << endl
    << "# Vertices=" << vertices.size() << " " << "Faces=" << faces.size()/3 << endl
    << "#" << endl;
    for(int i = 0; i < vertices.size(); i++){
        faceFile << "Vertex " << i << " " << vertices[i].x << " " << vertices[i].y << " " <<  vertices[i].z << endl;
    }
    for(int j = 0; j < faces.size(); j+=3){
        faceFile << "Face " << j/3 << " " << faces[j] << " " << faces[j+1] << " " << faces[j+2] << endl;
    }
    faceFile.close();
}