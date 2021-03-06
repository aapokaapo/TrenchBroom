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

#ifndef TRENCHBROOM_TAGVISITOR_H
#define TRENCHBROOM_TAGVISITOR_H

#include "Model/Model_Forward.h"

namespace TrenchBroom {
    namespace Model {
        class TagVisitor {
        public:
            virtual ~TagVisitor();

            virtual void visit(World& world);
            virtual void visit(Layer& layer);
            virtual void visit(Group& group);
            virtual void visit(Entity& entity);
            virtual void visit(Brush& brush);
            virtual void visit(BrushFace& face);
        };

        class ConstTagVisitor {
        public:
            virtual ~ConstTagVisitor();

            virtual void visit(const World& world);
            virtual void visit(const Layer& layer);
            virtual void visit(const Group& group);
            virtual void visit(const Entity& entity);
            virtual void visit(const Brush& brush);
            virtual void visit(const BrushFace& face);
        };
    }
}

#endif //TRENCHBROOM_TAGVISITOR_H
