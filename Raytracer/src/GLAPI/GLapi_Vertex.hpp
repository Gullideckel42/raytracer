#pragma once
// Defines a vertex point
namespace h3dgl {
	struct Vertex {
		// Vector3f: Position vector
		float x;
		float y;
		float z;
		// Vector2f: Texture vector
		float u;
		float v;
		// Vector3f: Normal vector
		float nx;
		float ny;
		float nz;
	};
}

// Simplest cube with incorrect normals. Indices provided counterclockwise (for cubemaps) (8 Vertices, 36 Indices)
#define H3D_CUBE_VERTICES(x,y,z) {										\
		h3dgl::Vertex {-x, -y,  z,  0.0,  0.0,  0,0, 1},				\
        h3dgl::Vertex { x, -y,  z,  1.0,  0.0,  0,0, 1},				\
        h3dgl::Vertex { x,  y,  z,  1.0,  1.0,  0,0, 1},				\
        h3dgl::Vertex {-x,  y,  z,  0.0,  1.0,  0,0, 1},				\
        h3dgl::Vertex {-x, -y, -z,  0.0,  0.0,  0,0,-1},				\
        h3dgl::Vertex { x, -y, -z,  1.0,  0.0,  0,0,-1},				\
        h3dgl::Vertex { x,  y, -z,  1.0,  1.0,  0,0,-1},				\
        h3dgl::Vertex {-x,  y, -z,  0.0,  1.0,  0,0,-1}					\
}

#define H3D_CUBE_INDICES {0,1,2,2,3,0,1,5,6,1,6,2,7,6,5, 5,4,7,0,3,4,4,3,7,0,4,5,5,1,0,3,2,6,6,7,3}


// Cube with correct normals and uv's. Indices provided counterclockwise  (24 Vertices, 36 Indices)
#define H3D_NORMAL_CUBE_VERTICES(x,y,z) { \
		/*Front*/ \
 	h3dgl::Vertex{ -x,-y, z, 0.0f, 0.0f, 0,0,1 }, 	/*0-00*/\
	h3dgl::Vertex{  x,-y, z, 1.0f, 0.0f, 0,0,1 },	/*1-01*/\
	h3dgl::Vertex{  x, y, z, 1.0f, 1.0f, 0,0,1 },	/*2-02*/\
	h3dgl::Vertex{ -x, y, z, 0.0f, 1.0f, 0,0,1 },	/*3-03*/\
		/*Right*/ \
	h3dgl::Vertex{  x,-y, z, 1.0f, 0.0f, 1,0,0 },	/*1-04*/\
	h3dgl::Vertex{  x,-y,-z, 0.0f, 0.0f, 1,0,0 },	/*5-05*/\
	h3dgl::Vertex{  x, y,-z, 0.0f, 1.0f, 1,0,0 },	/*6-06*/\
	h3dgl::Vertex{  x, y, z, 1.0f, 1.0f, 1,0,0 },	/*2-07*/\
		/*Back*/ \
	h3dgl::Vertex{ x,-y,-z, 0.0f, 0.0f, 0,0,-1 },	/*5-08*/\
	h3dgl::Vertex{ x, y,-z, 0.0f, 1.0f, 0,0,-1 },	/*6-09*/\
	h3dgl::Vertex{-x, y,-z, 1.0f, 1.0f, 0,0,-1 },	/*7-10*/\
	h3dgl::Vertex{-x,-y,-z, 1.0f, 0.0f, 0,0,-1 },	/*4-11*/\
		/*Left*/ \
	h3dgl::Vertex{-x,-y,-z, 1.0f, 0.0f, -1,0,0 },	/*4-12*/\
	h3dgl::Vertex{-x, y,-z, 1.0f, 1.0f, -1,0,0 },	/*7-13*/\
	h3dgl::Vertex{-x, y, z, 0.0f, 1.0f, -1,0,0 },	/*3-14*/\
	h3dgl::Vertex{-x,-y, z, 0.0f, 0.0f, -1,0,0 },	/*0-15*/\
		/*Bottom*/ \
	h3dgl::Vertex{-x,-y, z, 0.0f, 0.0f, 0,-1,0 },	/*0-16*/\
	h3dgl::Vertex{ x,-y, z, 1.0f, 0.0f, 0,-1,0 },	/*1-17*/\
	h3dgl::Vertex{ x,-y,-z, 1.0f, 1.0f, 0,-1,0 },	/*5-18*/\
	h3dgl::Vertex{-x,-y,-z, 0.0f, 1.0f, 0,-1,0 },	/*4-19*/\
		/* Top */ \
	h3dgl::Vertex{-x, y, z, 0.0f, 0.0f, 0, 1,0 },	/*3-20*/\
	h3dgl::Vertex{ x, y, z, 1.0f, 0.0f, 0, 1,0 },	/*2-21*/\
	h3dgl::Vertex{ x, y,-z, 1.0f, 1.0f, 0, 1,0 },	/*6-22*/\
	h3dgl::Vertex{-x, y,-z, 0.0f, 1.0f, 0, 1,0 },	/*7-23*/\
\
}

/** Counterclockwise */
#define H3D_NORMAL_CUBE_INDICES {0,1,2,2,3,0,4,5,6,6,7,4,8,11,10,10,9,8,12,15,14,14,13,12,16,19,18,18,17,16,21,22,23,23,20,21};