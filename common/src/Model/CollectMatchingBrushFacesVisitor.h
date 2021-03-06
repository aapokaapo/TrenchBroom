/*
 Copyright (C) 2010-2017 Kristian Duske

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
 along with TrenchBroom. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TrenchBroom_CollectMatchingBrushFacesVisitor
#define TrenchBroom_CollectMatchingBrushFacesVisitor

#include "Model/Brush.h"
#include "Model/BrushFace.h"
#include "Model/BrushFacePredicates.h"
#include "Model/Model_Forward.h"
#include "Model/NodeVisitor.h"

#include <vector>

namespace TrenchBroom {
    namespace Model {
        template <typename P>
        class CollectMatchingBrushFacesVisitor : public NodeVisitor {
        private:
            P m_p;
            std::vector<BrushFace*> m_faces;
        public:
            CollectMatchingBrushFacesVisitor(const P& p = P()) : m_p(p) {}
            const std::vector<BrushFace*>& faces() const { return m_faces; }
        private:
            void doVisit(World*)  override {}
            void doVisit(Layer*)  override {}
            void doVisit(Group*)  override {}
            void doVisit(Entity*) override {}
            void doVisit(Brush* brush) override {
                for (BrushFace* face : brush->faces()) {
                    if (m_p(face))
                        m_faces.push_back(face);
                }
            }
        };

        using CollectBrushFacesVisitor = CollectMatchingBrushFacesVisitor<BrushFacePredicates::True>;
    }
}

#endif /* defined(TrenchBroom_CollectMatchingBrushFacesVisitor) */
