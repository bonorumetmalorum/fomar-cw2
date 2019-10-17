#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

// void parseHeader(istream &faceFile){
//     string buffer;
//     faceFile >> buffer;
//     buffer 
// }

/*
# University of Leeds 2018-2019
# COMP 5812M Assignment 2
# Govind Venkatesh
# 201332107
#
# Object Name: 2ballout
# Vertices=12 Faces=16
# 
*/

struct Header{
    string university;
    string module_assignment;
    string name;
    string candidate_no;
    string object_name;
    int verts, faces;
};

struct Vert{
    float x = 0;
    float y = 0;
    float z = 0;
};

Header parseHeader(ifstream &faceFile){
    Header hd;
    getline(faceFile, hd.university);
    getline(faceFile, hd.module_assignment);
    getline(faceFile, hd.name);
    getline(faceFile, hd.candidate_no);
    faceFile.ignore(2, '\n');
    getline(faceFile, hd.object_name);
    string verticesAndFaces;
    getline(faceFile, verticesAndFaces);
    cout << hd.object_name << endl;
    sscanf(verticesAndFaces.c_str(), "# Vertices=%d Faces=%d", &hd.verts, &hd.faces);
    faceFile.ignore(2, '\n');
    return hd;
}

vector<Vert> parseCoords(Header &hd, istream &faceFile){
    vector<Vert> coords(hd.verts);
    string buf;
    int index;
    float x,y,z;
    for(int i = 0; i < hd.verts; i++){
        getline(faceFile, buf);
        sscanf(buf.c_str(), "Vertex %d %f %f %f", &index, &x, &y, &z);
        coords[i] = Vert{x,y,x};
    }
    return coords;
}

void parseEdgeTo(Header &hd, istream &faceFile, vector<int> &outFaceIndex){
    vector<int> edgeTo(hd.faces*3);
    string buf;
    int index, a, b, c;
    for(int i = 0; i < hd.faces; i++){
        getline(faceFile, buf);
        sscanf(buf.c_str(), "Face %d %d %d %d", &index, &a, &b, &c);
        outFaceIndex.push_back(a);
        outFaceIndex.push_back(b);
        outFaceIndex.push_back(c);
    }
}


int main(int argc, char **argv){
    ifstream faceFile;
    faceFile.open(argv[1]);
    Header hd = parseHeader(faceFile);
    vector<Vert> coords = parseCoords(hd, faceFile);
    // for(Vert v : coords){
    //     cout << v.x << " " << v.y << " " << v.z << endl;
    // }
    vector<int> faceIndex;
    parseEdgeTo(hd, faceFile, faceIndex);
    // for(int i = 0; i < faceIndex.size(); i++){
    //     cout << faceIndex[i] <<endl;
    // }
    vector<int> edgeFrom(hd.faces*3);
    for(int i = 0; i < hd.faces*3; i++){ //initialise array to -1
        edgeFrom[i] = -1;
    }

    int a, b, c, d;
    for(int i = 0; i < faceIndex.size()/3; i++){ // for this face
        
    }   
    //over complication due to ds
    // for(int i = 0; i < edgeTo.size(); i++){
    //     //for each face
    //     for(int j=0; j < 3; j++){
    //         //for each edge on this face
    //         for(int k=0; k < edgeTo.size(); k++){
    //             //for every other face
    //             if(k == i){
    //                 continue;//same face so do nothing
    //             }else{ //different face
    //                 for(int l=0; l < 3; l++){
    //                     //for every edge on every other face
    //                     a = edgeTo[i].verts[j]; b = edgeTo[i].verts[(j+1)%3];//get the end points of the current edgeTo and put them in a, b
    //                     c = edgeTo[k].verts[l]; d = edgeTo[k].verts[(l+1)%3];//get the end points of the current edgeFrom and put them in c, d
    //                     if((a == d) && (b == c)){//check if a == d and b == c
    //                         cout << a << " " << b << " -> " << c << " " << d << endl;
    //                         cout << "insertion at index: " << (j*i) -1 << " j = " << j << " i = " << i <<  endl;
    //                         if(edgeFrom[j+i] == -1){// if yes then insert l*k at index j*i iff j*i == -1
    //                             edgeFrom[(j*i)-1] = (l*k)-1;
    //                             break;
    //                         }else{// else print j*i and exit (error state)
    //                             cout << "incorrect edge: " << (j*i)-1 << "other half: " << (l*k)-1 << endl;
    //                             return -1;
    //                         }
    //                     }
    //                 }
    //             }
    //         }
    //     }
    // }



    // for(int i = 0; i < edgeFrom.size(); i++){
    //     cout << edgeFrom[i] << " " << edgeFrom[i+1] << " " << edgeFrom[i+2] <<endl;
    // }
}