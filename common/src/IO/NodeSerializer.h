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

#ifndef TrenchBroom_NodeSerializer
#define TrenchBroom_NodeSerializer

#include "IO/IO_Forward.h"
#include "Model/EntityAttributes.h"
#include "Model/Model_Forward.h"

#include <list>
#include <string>
#include <unordered_map>
#include <vector>

namespace TrenchBroom {
    namespace IO {
        class NodeSerializer {
        private:
            class BrushSerializer;
        protected:
            static const int FloatPrecision = 17;
            using ObjectNo = unsigned int;
        private:
            class IdManager {
            private:
                using IdMap = std::unordered_map<const Model::Node*, std::string>;
                mutable IdMap m_ids;
            public:
                const std::string& getId(const Model::Node* t) const;
            private:
                Model::IdType makeId() const;
                std::string idToString(const Model::IdType nodeId) const;
            };

            IdManager m_layerIds;
            IdManager m_groupIds;

            ObjectNo m_entityNo;
            ObjectNo m_brushNo;
        public:
            NodeSerializer();
            virtual ~NodeSerializer();
        protected:
            ObjectNo entityNo() const;
            ObjectNo brushNo() const;
        public:
            void beginFile();
            void endFile();
        public:
            void defaultLayer(Model::World& world);
            void customLayer(Model::Layer* layer);
            void group(Model::Group* group, const std::list<Model::EntityAttribute>& parentAttributes);

            void entity(Model::Node* node, const std::list<Model::EntityAttribute>& attributes, const std::list<Model::EntityAttribute>& parentAttributes, Model::Node* brushParent);
            void entity(Model::Node* node, const std::list<Model::EntityAttribute>& attributes, const std::list<Model::EntityAttribute>& parentAttributes, const std::vector<Model::Brush*>& entityBrushes);
        private:
            void beginEntity(const Model::Node* node, const std::list<Model::EntityAttribute>& attributes, const std::list<Model::EntityAttribute>& extraAttributes);
            void beginEntity(const Model::Node* node);
            void endEntity(Model::Node* node);

            void entityAttributes(const std::list<Model::EntityAttribute>& attributes);
            void entityAttribute(const Model::EntityAttribute& attribute);

            void brushes(const std::vector<Model::Brush*>& brushes);
            void brush(Model::Brush* brush);

            void beginBrush(const Model::Brush* brush);
            void endBrush(Model::Brush* brush);
        public:
            void brushFaces(const std::vector<Model::BrushFace*>& faces);
        private:
            void brushFace(Model::BrushFace* face);
        private:
            class GetParentAttributes;
        public:
            std::list<Model::EntityAttribute> parentAttributes(const Model::Node* node);
        private:
            std::list<Model::EntityAttribute> layerAttributes(const Model::Layer* layer);
            std::list<Model::EntityAttribute> groupAttributes(const Model::Group* group);
        protected:
            std::string escapeEntityAttribute(const std::string& str) const;
        private:
            virtual void doBeginFile() = 0;
            virtual void doEndFile() = 0;

            virtual void doBeginEntity(const Model::Node* node) = 0;
            virtual void doEndEntity(Model::Node* node) = 0;
            virtual void doEntityAttribute(const Model::EntityAttribute& attribute) = 0;

            virtual void doBeginBrush(const Model::Brush* brush) = 0;
            virtual void doEndBrush(Model::Brush* brush) = 0;
            virtual void doBrushFace(Model::BrushFace* face) = 0;
        };
    }
}

#endif /* defined(TrenchBroom_NodeSerializer) */
