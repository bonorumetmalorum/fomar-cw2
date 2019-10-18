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

void compileHeader(ostream &file, Header &hd){
    file << hd.university << endl
    << hd.module_assignment << endl
    << hd.name << endl
    << hd.candidate_no << endl
    << "#" << endl
    << hd.object_name << endl
    << "# Vertices=" << hd.verts << " " << "Faces=" << hd.faces << endl
    << "#" << endl;
}

void compileVertexBlock(ostream &file, vector<Vert> &vertexIndex){
    for(int i = 0; i < vertexIndex.size(); i++){
        file << "Vertex " << i << " " << vertexIndex[i].x << " " << vertexIndex[i].y << " " <<  vertexIndex[i].z << endl;
    }
}

void compileFirstEdgeBlock(ostream &file, vector<int> &firstEdge){
    for(int i = 0; i < firstEdge.size(); i++){
        file << "FirstDirectedEdge " << i << " " << firstEdge[i] << endl;
    }
}

void compileFacesIndexBlock(ostream &file, vector<int> &facesIndex){
    for(int j = 0; j < facesIndex.size(); j+=3){
        file << "Face " << j/3 << " " << facesIndex[j] << " " << facesIndex[j+1] << " " << facesIndex[j+2] << endl;
    }
}

void compileOtherHalfBlock(ostream &file, vector<int> &otherHalf){
    for(int i = 0; i < otherHalf.size(); i++){
        file << "OtherHalf " << i << " " << otherHalf[i] << endl;
    }
}

int genus(int v, int e, int f){
    return (v - e + f - 2)/-2;
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
    cout << faceIndex.size() << endl;
    // for(int i = 0; i < faceIndex.size(); i++){
    //     cout << faceIndex[i] <<endl;
    // }

    vector<int> firstEdge(hd.verts);
    for(int i = 0; i < hd.verts; i++){//for each unique vertex 
        for(int j = 0; j < faceIndex.size(); j++){//for every edge find the corresponding vertex i
            if(faceIndex[j] == i){ //if we have a match add it
                firstEdge[i] = j;
            }//otherwise continue
        }
    }

    // for(int i = 0; i < firstEdge.size(); i++){
    //     cout << "vertex: " << i << " first edge: " << firstEdge[i] << endl;
    // }

    vector<int> otherHalf(hd.faces*3);
    for(int i = 0; i < hd.faces*3; i++){ //initialise array to -1
        otherHalf[i] = -1;
    }
    int a, b, c, d;
    for(int i = 0; i < faceIndex.size(); i+=3){ // for this face
        for(int j = 0; j < 3; j++){//for edges on this face
            a = faceIndex[i+j]; b = faceIndex[i+((j+1)%3)]; //current edge
            for(int k = 0; k < faceIndex.size(); k+=3){//for every other face
                if(k != i){ // if it is not the current face
                    for(int l = 0; l < 3; l++){//for edges on other face
                        c = faceIndex[k+l]; d=faceIndex[k+((l+1)%3)];//current edge on other face
                        if(a == d && b == c){ // if the edges match
                            //cout << a << " " << b << " -> " << c << " " << d << endl;
                            if(otherHalf[i+j] == -1){// if there is no value assigned
                                otherHalf[i+j] = k+l;
                            }else{
                                cout << "error";
                                return -1;
                            }
                        }
                    }
                }
                //have to check if the value was set, if not there is a missing half edge, we must error here
                continue;
            }
        }
    }   
    // for(int i = 0; i < edgeFrom.size(); i+=3){
    //     cout << edgeFrom[i] << " " << edgeFrom[i+1] << " " << edgeFrom[i+2] <<endl;
    // }

    ofstream diredge;
    string filename;
    filename.append(hd.object_name).append(".diredge");
    diredge.open(filename); //set up the file
    compileHeader(diredge, hd);//put the header in
    compileVertexBlock(diredge, coords);//put the vertex block in
    compileFirstEdgeBlock(diredge, firstEdge);//put the first edge block in
    compileFacesIndexBlock(diredge, faceIndex);//put the facesIndex block in
    compileOtherHalfBlock(diredge, otherHalf);//put the other half block in
    diredge.close();//end

    cout << "genus of mesh is: " << genus(hd.verts, (hd.faces*3)/2, hd.faces);

    return 0;
}