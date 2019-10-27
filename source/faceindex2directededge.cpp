#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

struct Header
{
    string university;
    string module_assignment;
    string name;
    string candidate_no;
    char object_name[50];
    int verts, faces;
};

struct Vert
{
    float x = 0;
    float y = 0;
    float z = 0;
};

//parse the header of the file
Header parseHeader(ifstream &faceFile)
{
    Header hd;
    getline(faceFile, hd.university);
    getline(faceFile, hd.module_assignment);
    getline(faceFile, hd.name);
    getline(faceFile, hd.candidate_no);
    faceFile.ignore(2, '\n');
    string name;
    getline(faceFile, name);
    sscanf(name.c_str(), "# Object Name: %s", hd.object_name);
    string verticesAndFaces;
    getline(faceFile, verticesAndFaces);
    sscanf(verticesAndFaces.c_str(), "# Vertices=%d Faces=%d", &hd.verts, &hd.faces);
    faceFile.ignore(2, '\n');
    return hd;
}

//parse the vertices
vector<Vert> parseCoords(Header &hd, istream &faceFile)
{
    vector<Vert> coords(hd.verts);
    string buf;
    int index;
    float x, y, z;
    for (int i = 0; i < hd.verts; i++)
    {
        getline(faceFile, buf);
        sscanf(buf.c_str(), "Vertex %d %f %f %f", &index, &x, &y, &z);
        Vert vert;
        vert.x = x;
        vert.y = y;
        vert.z = z;
        coords[i] = vert;
    }
    return coords;
}

//parse the edge to / face index
void parseEdgeTo(Header &hd, istream &faceFile, vector<int> &outFaceIndex)
{
    vector<int> edgeTo(hd.faces * 3);
    string buf;
    int index, a, b, c;
    for (int i = 0; i < hd.faces; i++)
    {
        getline(faceFile, buf);
        sscanf(buf.c_str(), "Face %d %d %d %d", &index, &a, &b, &c);
        outFaceIndex.push_back(a);
        outFaceIndex.push_back(b);
        outFaceIndex.push_back(c);
    }
}

//write the header
void compileHeader(ostream &file, Header &hd)
{
    file << hd.university << endl
         << hd.module_assignment << endl
         << hd.name << endl
         << hd.candidate_no << endl
         << "#" << endl
         << "# Object Name: " << hd.object_name << endl
         << "# Vertices=" << hd.verts << " "
         << "Faces=" << hd.faces << endl
         << "#" << endl;
}

//write the vertex block
void compileVertexBlock(ostream &file, vector<Vert> &vertexIndex)
{
    for (int i = 0; i < vertexIndex.size(); i++)
    {
        file << "Vertex " << i << " " << vertexIndex[i].x << " " << vertexIndex[i].y << " " << vertexIndex[i].z << endl;
    }
}

//write the first edges block
void compileFirstEdgeBlock(ostream &file, vector<int> &firstEdge)
{
    for (int i = 0; i < firstEdge.size(); i++)
    {
        file << "FirstDirectedEdge " << i << " " << firstEdge[i] << endl;
    }
}

//write the faces index block / edge to block
void compileFacesIndexBlock(ostream &file, vector<int> &facesIndex)
{
    for (int j = 0; j < facesIndex.size(); j += 3)
    {
        file << "Face " << j / 3 << " " << facesIndex[j] << " " << facesIndex[j + 1] << " " << facesIndex[j + 2] << endl;
    }
}

//write the other half block
void compileOtherHalfBlock(ostream &file, vector<int> &otherHalf)
{
    for (int i = 0; i < otherHalf.size(); i++)
    {
        file << "OtherHalf " << i << " " << otherHalf[i] << endl;
    }
}

//calculate genus given number of vertices, number of edges and number of faces. Note does not work with disconnected meshes
// for disconnected meshes, the number of shells will also need to be calculated.
int genus(int v, int e, int f)
{
    int g = (v - e + f - 2) / -2;
    return g;
}

//helper function to get previous edge given an edge
int getPrevious(int index)
{
    int value;
    switch (index % 3)
    {
    case 0:                //first edge of face
        value = index + 2; //previous edge is last edge of face
        break;
    case 1:                //second edge of face
        value = index - 1; //previous edge is first edge of face
        break;
    case 2:                //third edge of face
        value = index - 1; //previous edge is second edge of face
        break;
    }
    return value;
}

//time complexity: No. unique verts * number of edges + incident edges on unique vertex O(uv*(e + i)) = O(n^2)
// calculate the pinch point if there is one, return true if none return false store the vertex pinch point in errorVert out parameter
bool pinchPoints(vector<Vert> &verts, vector<int> faceIndex, vector<int> &firstEdge, vector<int> &otherHalf, int &errorVert)
{
    for (int vertex = 0; vertex < verts.size(); vertex++)
    { //for the current vertex
        //first we want to determine how many times this vertex appears in edges
        int sumOccurence = 0; //how many edges have this vertex
        for (int i = 0; i < faceIndex.size(); i++)
        { //for every edge
            if (faceIndex[i] == vertex)
            {                   //if this is the vertex we're looking for
                sumOccurence++; //increment since this edge starts with it
            }
        }
        int first = firstEdge[vertex];
        int currentEdge = first; //find first edge index
        int nextEdge = getPrevious(currentEdge);
        int counter = 1;
        while (otherHalf[nextEdge] != first)
        { //while there are more edges to cycle through
            currentEdge = otherHalf[nextEdge]; //find other side of last edge to get first edge of next face
            nextEdge = getPrevious(currentEdge);
            counter++; //increment the counter since we have not reached the end of the cycle
        }
        if (counter < sumOccurence)
        { // if this number is less than the number of edges containg this vertex, we have a pinch point
            errorVert = vertex;
            return true;
        }
    }

    return false;
}

//time complexity: No. faces * (3 * ((No. faces - 1)*3)) which is bounded by O(n^2)
//calculate the other half of every edge, return -1 if any edge already has an other half or doesnt have one. if all is okay return 0
//otherHalf is an out parameter
int otherHalf(vector<int> &otherHalf, Header &hd, vector<int> &faceIndex)
{
    for (int i = 0; i < hd.faces * 3; i++)
    { //initialise array to -1
        otherHalf[i] = -1;
    }
    int a, b, c, d;
    for (int i = 0; i < faceIndex.size(); i += 3)
    { // for this face
        for (int j = 0; j < 3; j++)
        { //for edges on this face
            a = faceIndex[i + j]; //current edge
            b = faceIndex[i + ((j + 1) % 3)]; //current edge
            for (int k = 0; k < faceIndex.size(); k += 3)
            { //for every other face
                if (k != i)
                { // if it is not the current face
                    for (int l = 0; l < 3; l++)
                    { //for edges on other face
                        c = faceIndex[k + l]; //current edge on other face
                        d = faceIndex[k + ((l + 1) % 3)]; //current edge on other face
                        if (a == d && b == c)
                        { // if the edges match
                            if (otherHalf[i + j] == -1)
                            { // if there is no value assigned
                                otherHalf[i + j] = k + l;
                            }
                            else
                            { //current edge already has an other half, error
                                cout << "error not manifold: CurrentEdge = " << a << " " << b << endl;
                                return -1;
                            }
                        }
                    }
                }
                continue;
            }
            if (otherHalf[i + j] == -1)
            { //this edge did not have another half, error
                cout << "error not manifold, did not find a half edge for edge: " << i + j << " (" << a << " " << b << ")" << endl;
                return -1;
            }
        }
    }
    return 0;
}

int main(int argc, char **argv)
{
    ifstream faceFile;
    faceFile.open(argv[1]);
    Header hd = parseHeader(faceFile);
    vector<Vert> coords = parseCoords(hd, faceFile);

    vector<int> faceIndex;
    parseEdgeTo(hd, faceFile, faceIndex);

    vector<int> firstEdge(hd.verts);
    //time complexity: O(unique vertices * number of vertices) = O(n)
    for (int i = 0; i < hd.verts; i++)
    { //for each unique vertex
        for (int j = 0; j < faceIndex.size(); j++)
        { //for every edge find the corresponding vertex i
            if (faceIndex[j] == i)
            { //if we have a match add it
                firstEdge[i] = j;
            } //otherwise continue
        }
    }
    //calculate the otherhalf array
    vector<int> outOtherHalf(hd.faces * 3);
    int success = otherHalf(outOtherHalf, hd, faceIndex);
    if (success == -1)
    {//if we have failed to create the otherHalf index
        return -1; //terminate
    }
    //calculate any pinch points
    int pinch;
    bool error = pinchPoints(coords, faceIndex, firstEdge, outOtherHalf, pinch);
    if (error == true)
    {
        cout << "pinch point at unique vertex index: " << pinch << endl;
        return -1;
    }

    ofstream diredge;
    string filename;
    filename.append(hd.object_name).append(".diredge");
    diredge.open(filename);                       //set up the file
    compileHeader(diredge, hd);                   //put the header in
    compileVertexBlock(diredge, coords);          //put the vertex block in
    compileFirstEdgeBlock(diredge, firstEdge);    //put the first edge block in
    compileFacesIndexBlock(diredge, faceIndex);   //put the facesIndex block in
    compileOtherHalfBlock(diredge, outOtherHalf); //put the other half block in
    diredge.close();                              //end

    cout << "genus of mesh is: " << genus(hd.verts, (hd.faces * 3) / 2, hd.faces) << endl;

    return 0;
}