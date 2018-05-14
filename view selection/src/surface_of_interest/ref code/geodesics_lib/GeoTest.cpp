#include <iostream>
#include <fstream>
#include "inc/Geodesic.h"

void loadOBJ(const char* file, Geodesic& g)
{
    g.meshClear();

    using namespace std;

    ifstream is(file);
    printf("Reading \'%s\' ... ", file);
    fflush(stdout);

    while (is) {
        char str[1024];
        is.getline(str, 1024);
        bool verticesDone = false;

        switch (str[0]) {
            case 'v':
                if (isspace(str[1]) && !verticesDone) { // 'vt', 'vp', 'vn' are ignored
                    double x, y, z;
                    int res = sscanf(&str[2], "%lf %lf %lf", &x, &y, &z);
                    if (res == 3) // for inputs with 'v x y z w', w is ignored
                        g.meshAddVertex(x, y, z);
                }
                break;
            case 'f':
                const int n = g.meshNVertices();
                int i, j, k;
                // for inputs with n-gons only a triangle for every n-gon is built
                int res = sscanf(str, "f %i %i %i", &i, &j, &k);
                if (res == 3 && 1 <= i && i <= n && 1 <= j && j <= n && 1 <= k && k <= n)
                {
                    g.meshAddFace(i-1, j-1, k-1);
                }
                break;
        }
    }

    printf("Done\n");
    fflush(stdout);
}

void savePath(Geodesic& g, int s, int t, const char* meshFile)
{
    char str[1024];
    sprintf(str, "%s.%d.%d.path", meshFile, s, t);
    std::ofstream os(str);

    while (!g.pathEmpty()) {
        int vs, vt;
        double b;
        g.pathPopNextStep(vs, vt, b);
        sprintf(str, "%d %d %.14g\n", vs, vt, b);
        os << str;
    }
}

void saveDistances(Geodesic& g, int s, const char* meshFile)
{
    char str[1024];
    sprintf(str, "%s.%d.dist", meshFile, s);
    std::ofstream os(str);

    const int n = g.meshNVertices();
    for (int i = 0; i < n; ++i) {
        const double d = g.distanceAtVertex(i);
        sprintf(str, "%.14g\n", d);
        os << str;
    }
}

void printUsage()
{
    printf("Usage:\n\n");
    printf("GeoTest.exe mesh.obj -s VID [-d] [-a Tol] [-t VID] [-p0 VID] [-p1 VID]\n\n");
    printf("-s VID: specify the source vertex VID;\n");
    printf("  Note: if -t is not specifed the geodesics are computed for the entire mesh;\n");
    printf("-t VID: find the shortest path between the source and target (VID) vertices only;\n");
    printf("-d: output the distance into a file;\n");
    printf("-a Tol: run approximate algorithm given tolerance, 0 < Tol < 1;\n");
    printf("-p0 VID: find the shortest path between the source vertex and VID;\n");
    printf("-p1 VID: find the shortest path between the source vertex and VID;\n");
}

int main(int argc, char *argv[])
{
    printf("Version: 1.0\n");
    fflush(stdout);

    if (argc < 2) {
        printUsage();
        return 0;
    }

    const char* meshFile = 0;
    int sourceVertexID = -1;
    int targetVertexID = -1;
    int pathToVertexIDs[2] = { -1, -1 };
    bool printDistances = false;
    double tolerance = -1.0;

    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
            case 's':
                {
                    int id;
                    int res = sscanf(argv[i+1], "%d", &id);
                    if (res == 1 && i >= 0) {
                        sourceVertexID = id;
                        ++i;
                    }
                    else {
                        printf("Error: -s option requires numeric parameter. \n");
                        printUsage();
                        return 0;
                    }
                }
                break;
            case 't':
                {
                    int id;
                    int res = sscanf(argv[i+1], "%d", &id);
                    if (res == 1 && i >= 0) {
                        targetVertexID = id;
                        ++i;
                    }
                    else {
                        printf("Error: -t option requires numeric parameter. \n");
                        printUsage();
                        return 0;
                    }
                }
                break;
            case 'd':
                {
                    printDistances = true;
                }
                break;
            case 'a':
                {
                    if (i + 1 == argc) {
                        printf("Error: -a option requires a valid tolerance parameter. \n");
                        printUsage();
                        return 0;
                    }
                    double tol;
                    int res = sscanf(argv[i+1], "%lf", &tol);
                    if (res == 1 && 0.0 < tol && tol < 1.0) {
                        tolerance = tol;
                        ++i;
                    }
                    else {
                        printf("Error: -a option requires a valid tolerance parameter. \n");
                        printUsage();
                        return 0;
                    }
                }
                break;
            case 'p':
                {
                    int id;
                    int res = sscanf(argv[i+1], "%d", &id);
                    if ((argv[i][2] == '0' || argv[i][2] == '1') && res == 1 && i >= 0) {
                        if (argv[i][2] == '0')
                            pathToVertexIDs[0] = id;
                        else if (argv[i][2] == '1')
                            pathToVertexIDs[1] = id;
                        ++i;
                    }
                    else {
                        printf("Error: -pi option requires numeric parameter. \n");
                        printUsage();
                        return 0;
                    }
                }
                break;
            default:
                {
                    printf("Warning: Option %s has been ignored.\n", argv[i]);
                }
            }
        }
        else
            meshFile = argv[i];
    }

    if (meshFile == 0) {
        printf("Error: No mesh file has been specified. \n");
        printUsage();
        return 0;
    }

    Geodesic geodesic;

    loadOBJ(meshFile, geodesic);

    if (geodesic.meshNFaces() == 0) {
        printf("Error reading the mesh file '%s' or the input mesh does not have any faces.\n", meshFile);
        return 0;
    }
    else {
        printf("The mesh in '%s' has %d vertices and %d faces.\n",
            meshFile, geodesic.meshNVertices(), geodesic.meshNFaces());
    }

    if (sourceVertexID == -1) {
        printf("Error: The source vertex has not been specified.\n");
        printUsage();
        return 0;
   }

    if (targetVertexID != -1) {
        geodesic.findPathBetweenTwoVertices(sourceVertexID, targetVertexID);
        savePath(geodesic, sourceVertexID, targetVertexID, meshFile);
        return 1;
    }

    if (tolerance == -1.0)
        geodesic.computeExactGeodesics(sourceVertexID);
    else
        geodesic.computeApproxGeodesics(sourceVertexID, tolerance);

    if (pathToVertexIDs[0] != -1) {
        geodesic.findPathToVertex(pathToVertexIDs[0]);
        savePath(geodesic, sourceVertexID, pathToVertexIDs[0], meshFile);
    }

    if (pathToVertexIDs[1] != -1) {
        geodesic.findPathToVertex(pathToVertexIDs[1]);
        savePath(geodesic, sourceVertexID, pathToVertexIDs[1], meshFile);
    }

    if (printDistances)
        saveDistances(geodesic, sourceVertexID, meshFile);

    return 1;
}