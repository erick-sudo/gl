# OBJ

OBJ is a geometry definition file format first developed by Wavefront Technologies for its Advanced Visualizer animation package.

The OBJ file format is a simple data-format that represents 3D geometry alone - namely, the position of each vertex, the UV position of each texture coordinate veetex, vertex normals, and the faces that make each polygon defined as a list of verices, and texture vertices.

Vertices are stored in a counter-clockwise order by default, making explicit declaration of face normals unnecessary.

OBJ coordinates have no units, but OBJ files can contain scale information in a human readale comment line.

## File format

Anything following a hash character is a comment

```
# this is a comment
```

An OBJ file may contain vertex data, free-form curve/surface attributes, elements, free-form curve.surface body statements, connectivity between free-foorm surfaces, grouping and display/render attribute information.
The most common elements are geometric vertices, tecture coordinates, vertex normals and polygonal faces.

```obj
# List of geometric vertices, with (x, y, z, [w]) coordinates, w is optional and defaults to 1.0.
v 0.123 0.234 0.345 1.0

# List of texture coordinates, in (u, [v, w]) coordinates, these will vary between 0 and 1. v, w are optional and default to 0.
vt 0.500 1 [0]

# List of vertex normals in (x,y,z) form; normals might not be unit vectors.
vn 0.707 0.000 0.707

# Parameter space vertices in (u, [v, w]) form; free form geometry statement (see below)
vp 0.310000 3.210000 2.100000

# Polygonal face element
f 1 2 3
f 3/1 4/2 5/3
f 6/4/1 3/5/3 7/6/5
f 7//1 8//2 9//3

# Line element
l 5 8 1 2 4 9
```

### Geometric vertex
A vertex is specified via line starting with the letter `v`. Tha is folowed by (x, y, z, [w]) coordinates. W is optional and defaults to 1.0. A right hand coordinate system is used to specify the coordinate locations.

### Parameter space vertices
