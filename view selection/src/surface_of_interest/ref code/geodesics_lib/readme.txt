Geodesics Library 1.0.1

Copyright (C) 2008 Vitaly Surazhsky. All rights reserved.

Abstract
--------
The library provides some functionality presented in the paper
“Fast Exact and Approximate Geodesics on Meshes” by
Vitaly Surazhsky, Tatiana Surazhsky, Danil Kirsanov, Steven J. Gortler
and Hugues Hoppe, ACM Transactions on Graphics, 24(3) (SIGGRAPH 2005),
2005, pages 553-560.

Versions:
1.0.1 Jan 16, 2011:
 - fixed: distances at some boundary vertices have incorrect values (always 1)
 - fixed: path output has the source and target vertices with incorrect parameter (1 instead of 0)
1.0.0 Mar 12, 2008:
 - initial release

The package
-----------
inc/Geodesic.h
 -- the C++ interface to the library
lib/Geodesic.lib
 -- the .lib interface to the library dll
bin/Geodesic.dll
 -- the library dll
bin/GeoTest.exe
 -- a demonstration program using the library
bin/eight.obj
 -- an example of a mesh in .obj format
GeoTest.cpp
 -- the source code of the demonstration program

Functionality
-------------
1) Given a source vertex, compute the geodesic distances
   for all the mesh vertices.
2) Given a source vertex and tolerance, compute approximate
   geodesic distances for all the mesh vertices.
3) After 1) or 2) is computed, find a path from any vertex
   to the source vertex.
4) Find the shortest path between two vertices without computing
   geodesic distances for the entire mesh.

Path representation
-------------------
A path is defined by a sequence of points on the mesh surface. Every point
is described by triplet (v0, v1, b), where v0 and v1 are two vertices sharing
edge v0->v1, and b, 0 <= b <= 1, is the barycentric coordinate of the point
on edge v0->v1.

Mesh indices
----------
1) The mesh indices within the library API are 0-based.
2) The mesh indices within .obj mesh file format are 1-based.

GeoTest.exe usage
-----------------

GeoTest.exe mesh.obj -s VID [-d] [-a Tol] [-t VID] [-p0 VID] [-p1 VID]

-s VID: specify the source vertex VID;
  Note: if -t is not specifed the geodesics are computed for the entire mesh;
-t VID: find the shortest path between the source and target (VID) vertices only;
-d: output the distance into a file;
-a Tol: run approximate algorithm given tolerance, 0 < Tol < 1;
-p0 VID: find the shortest path between the source vertex and VID;
-p1 VID: find the shortest path between the source vertex and VID;

Examples using GeoTest.exe
--------------------------
> GeoTest.exe eight.obj -s 0 -d
  -- computes the geodesic distances from vertex 0 to all the mesh vertices
     and outputs them into a file 

> GeoTest.exe eight.obj -s 0 -p0 12 -p1 14
  -- computes the geodesic distances from vertex 0 to all the mesh vertices,
     finds the paths from vertices 12 and 14 to vertex 0
     and outputs the two paths into two files respectively

> GeoTest.exe eight.obj -s 0 -t 12
  -- computes the shortest path between vertex 0 and 12
     and outputs the path into a file

FAQ
---
1) Q. Why are there two options –p0 and –p1 in ‘GeoTest’ to find a path
      from a vertex to the source vertex?
   A. To demonstrate that after geodesic distances are computed, a path
      from any vertex to the source vertex can be computed multiple times.
      The path computation is very fast.

2) Q. Is the library available for Linux?
   A. No. But this is due to the lack of requests for a Linux version of the library.


License
-------
This library is free software, however the library may NOT be redistributed
for any commercial purposes, which includes but is not limited to, selling
or reselling the library, distributing the library together with commercial
software, linking to the library from software that is commercially distributed.

This program is distributed in the hope that it will be useful, but WITHOUT 
ANY WARRANTY OR LIABILITY; Use the library at your own risk. Any liability
for damage of any sort is denied.
