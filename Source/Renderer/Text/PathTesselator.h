/*
 Copyright (C) 2010-2012 Kristian Duske
 
 This file is part of TrenchBroom.
 
 TrenchBroom is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 TrenchBroom is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with TrenchBroom.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __TrenchBroom__PathTesselator__
#define __TrenchBroom__PathTesselator__

#include "Renderer/Text/PathMesh.h"
#include "Utility/GLee.h"
#include "Utility/VecMath.h"

#include <vector>

using namespace TrenchBroom::Math;

namespace TrenchBroom {
    namespace Renderer {
        namespace Text {
            class Path;
            
            namespace PathTesselatorCallback {
                void CALLBACK gluTessBeginData(GLenum type, PathMesh* mesh);
                void CALLBACK gluTessVertexData(Vec2f* vertex, PathMesh* mesh);
                void CALLBACK gluTessCombineData(GLdouble coords[3], void *vertexData[4], GLfloat weight[4], void **outData, PathMesh* mesh);
                void CALLBACK gluTessEndData(PathMesh* mesh);
            }

            class PathTesselator {
            protected:
                GLUtesselator* m_gluTess;
            public:
                PathTesselator();
                ~PathTesselator();
                
                PathMeshPtr tesselate(const Path* path);
            };
        }
    }
}

#endif /* defined(__TrenchBroom__PathTesselator__) */