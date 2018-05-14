#ifdef GEODESIC_EXPORTS
#define GEODESIC_API __declspec(dllexport)
#else
#define GEODESIC_API __declspec(dllimport)
#endif

class GEODESIC_API Geodesic {
public:
    Geodesic();
    ~Geodesic();

public: // mesh loading
    void meshClear();
    void meshAddVertex(double x, double y, double z);
    bool meshAddFace(int v0, int v1, int v2);

public: // mesh access
    int meshNVertices() const;
    int meshNFaces() const;
    bool meshVertex(int v, double& x, double& y, double& z) const;

public: // global geodesic computations
    bool computeExactGeodesics(int s);
    bool computeApproxGeodesics(int s, double tolerance); // 0 < tolerance < 1
    bool findPathToVertex(int t);
    double distanceAtVertex(int v); // returns -1 if the distance is not defined

public: // fast path-only computation
    bool findPathBetweenTwoVertices(int s, int t);

public: // path access
    // A path is defined by a sequence of points on the mesh surface. Every point
    // is described by triplet (v0, v1, b), where v0 and v1 are two vertices sharing
    // edge v0->v1, and b, 0 <= b <= 1, is the barycentric coordinate of the point
    // on edge v0->v1.
    bool pathEmpty() const;
    bool pathPopNextStep(int& v0, int& v1, double& b);

private:
    class GeodesicImp;
    GeodesicImp* m_imp;
};

