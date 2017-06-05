#include "vertex_data.h"

#define numElems(array) (sizeof(array) == 0 ? 0 : sizeof(array) / sizeof(array[0]));

const float triangleVertexBufTmp[] = {
  -0.0f,  0.5f, -1.0f,
  -0.5f, -0.5f, -1.0f,
   0.5f, -0.5f, -1.0f
};
const float *triangleVertexBuf = &triangleVertexBufTmp[0];
const int triangleBufElems = numElems(triangleVertexBufTmp);

#define chmf 0.45f

const float cubeVertexBufTmp[] = {
  // Back
   chmf,  chmf, -0.5f,
   chmf, -chmf, -0.5f,
  -chmf,  chmf, -0.5f,

  -chmf,  chmf, -0.5f,
   chmf, -chmf, -0.5f,
  -chmf, -chmf, -0.5f,

  // Front
   chmf,  chmf,  0.5f,
  -chmf,  chmf,  0.5f,
  -chmf, -chmf,  0.5f,

  -chmf, -chmf,  0.5f,
   chmf, -chmf,  0.5f,
   chmf,  chmf,  0.5f,

  // Right
   0.5f,  chmf,  chmf,
   0.5f, -chmf,  chmf,
   0.5f, -chmf, -chmf,

   0.5f, -chmf, -chmf,
   0.5f,  chmf, -chmf,
   0.5f,  chmf,  chmf,

  // Left
  -0.5f, -chmf, -chmf,
  -0.5f, -chmf,  chmf,
  -0.5f,  chmf,  chmf,

  -0.5f,  chmf,  chmf,
  -0.5f,  chmf, -chmf,
  -0.5f, -chmf, -chmf,

  // Top
   chmf,  0.5f,  chmf,
   chmf,  0.5f, -chmf,
  -chmf,  0.5f,  chmf,

  -chmf,  0.5f,  chmf,
   chmf,  0.5f, -chmf,
  -chmf,  0.5f, -chmf,

  // Bottom
  -chmf, -0.5f,  chmf,
   chmf, -0.5f, -chmf,
   chmf, -0.5f,  chmf,

  -chmf, -0.5f, -chmf,
   chmf, -0.5f, -chmf,
  -chmf, -0.5f,  chmf,

  // Front-Top connector

  -chmf,  0.5f,  chmf,
  -chmf,  chmf,  0.5f,
   chmf,  0.5f,  chmf,

   chmf,  0.5f,  chmf,
  -chmf,  chmf,  0.5f,
   chmf,  chmf,  0.5f,

  // Front-Bottom connector

   chmf, -0.5f,  chmf,
  -chmf, -chmf,  0.5f,
  -chmf, -0.5f,  chmf,

   chmf, -chmf,  0.5f,
  -chmf, -chmf,  0.5f,
   chmf, -0.5f,  chmf,

  // Front-Left connector

  -0.5f,  chmf,  chmf,
  -0.5f, -chmf,  chmf,
  -chmf, -chmf,  0.5f,

  -chmf, -chmf,  0.5f,
  -chmf,  chmf,  0.5f,
  -0.5f,  chmf,  chmf,

  // Front-Right connector

   chmf, -chmf,  0.5f,
   0.5f, -chmf,  chmf,
   0.5f,  chmf,  chmf,

   0.5f,  chmf,  chmf,
   chmf,  chmf,  0.5f,
   chmf, -chmf,  0.5f,

  // Back-Top connector

   chmf,  0.5f, -chmf,
  -chmf,  chmf, -0.5f,
  -chmf,  0.5f, -chmf,

   chmf,  chmf, -0.5f,
  -chmf,  chmf, -0.5f,
   chmf,  0.5f, -chmf,

  // Back-Bottom connector

  -chmf, -0.5f, -chmf,
  -chmf, -chmf, -0.5f,
   chmf, -0.5f, -chmf,

   chmf, -0.5f, -chmf,
  -chmf, -chmf, -0.5f,
   chmf, -chmf, -0.5f,

  // Back-Left connector

  -chmf, -chmf, -0.5f,
  -0.5f, -chmf, -chmf,
  -0.5f,  chmf, -chmf,

  -0.5f,  chmf, -chmf,
  -chmf,  chmf, -0.5f,
  -chmf, -chmf, -0.5f,

  // Back-Right connector

   0.5f,  chmf, -chmf,
   0.5f, -chmf, -chmf,
   chmf, -chmf, -0.5f,

   chmf, -chmf, -0.5f,
   chmf,  chmf, -0.5f,
   0.5f,  chmf, -chmf,

   // Left-Top connector

  -chmf,  0.5f, -chmf,
  -0.5f,  chmf, -chmf,
  -0.5f,  chmf,  chmf,

  -0.5f,  chmf,  chmf,
  -chmf,  0.5f,  chmf,
  -chmf,  0.5f, -chmf,

   // Left-Bottom connector

  -0.5f, -chmf,  chmf,
  -0.5f, -chmf, -chmf,
  -chmf, -0.5f, -chmf,

  -chmf, -0.5f, -chmf,
  -chmf, -0.5f,  chmf,
  -0.5f, -chmf,  chmf,

   // Right-Top connector

   0.5f,  chmf,  chmf,
   0.5f,  chmf, -chmf,
   chmf,  0.5f, -chmf,

   chmf,  0.5f, -chmf,
   chmf,  0.5f,  chmf,
   0.5f,  chmf,  chmf,

   // Right-Bottom connector

   chmf, -0.5f, -chmf,
   0.5f, -chmf, -chmf,
   0.5f, -chmf,  chmf,

   0.5f, -chmf,  chmf,
   chmf, -0.5f,  chmf,
   chmf, -0.5f, -chmf,

   // Front-Top-Left corner

  -chmf,  0.5f,  chmf,
  -0.5f,  chmf,  chmf,
  -chmf,  chmf,  0.5f,

   // Front-Top-Right corner

   chmf,  chmf,  0.5f,
   0.5f,  chmf,  chmf,
   chmf,  0.5f,  chmf,

   // Front-Bottom-Left corner

  -chmf, -chmf,  0.5f,
  -0.5f, -chmf,  chmf,
  -chmf, -0.5f,  chmf,

   // Front-Bottom-Right corner

   chmf, -0.5f,  chmf,
   0.5f, -chmf,  chmf,
   chmf, -chmf,  0.5f,

   // Back-Top-Left corner

  -chmf,  chmf, -0.5f,
  -0.5f,  chmf, -chmf,
  -chmf,  0.5f, -chmf,

   // Back-Top-Right corner

   chmf,  0.5f, -chmf,
   0.5f,  chmf, -chmf,
   chmf,  chmf, -0.5f,

   // Back-Bottom-Left corner

  -chmf, -0.5f, -chmf,
  -0.5f, -chmf, -chmf,
  -chmf, -chmf, -0.5f,

   // Back-Bottom-Right corner

   chmf, -chmf, -0.5f,
   0.5f, -chmf, -chmf,
   chmf, -0.5f, -chmf,
};
const float *cubeVertexBuf = &cubeVertexBufTmp[0];
const int cubeBufElems = numElems(cubeVertexBufTmp);
