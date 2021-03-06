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

#include "NodeWriter.h"

#include "IO/MapFileSerializer.h"
#include "IO/MapStreamSerializer.h"
#include "Model/AssortNodesVisitor.h"
#include "Model/Brush.h"
#include "Model/Entity.h"
#include "Model/Group.h"
#include "Model/Layer.h"
#include "Model/Node.h"
#include "Model/World.h"

#include <list>
#include <vector>

namespace TrenchBroom {
    namespace IO {
        class NodeWriter::CollectEntityBrushesStrategy {
        public:
            using AssortNodesVisitor = Model::AssortNodesVisitorT<Model::SkipLayersStrategy, Model::CollectGroupsStrategy, Model::CollectEntitiesStrategy, CollectEntityBrushesStrategy>;
        private:
            EntityBrushesMap m_entityBrushes;
            std::vector<Model::Brush*> m_worldBrushes;

            class VisitParent : public Model::NodeVisitor {
            private:
                Model::Brush* m_brush;
                EntityBrushesMap& m_entityBrushes;
                std::vector<Model::Brush*>& m_worldBrushes;
            public:
                VisitParent(Model::Brush* brush, EntityBrushesMap& entityBrushes, std::vector<Model::Brush*>& worldBrushes) :
                m_brush(brush),
                m_entityBrushes(entityBrushes),
                m_worldBrushes(worldBrushes) {}
            private:
                void doVisit(Model::World* /* world */) override { m_worldBrushes.push_back(m_brush);  }
                void doVisit(Model::Layer* /* layer */) override { m_worldBrushes.push_back(m_brush);  }
                void doVisit(Model::Group* /* group */) override { m_worldBrushes.push_back(m_brush);  }
                void doVisit(Model::Entity* entity )    override { m_entityBrushes[entity].push_back(m_brush); }
                void doVisit(Model::Brush* /* brush */) override {}
            };
        public:
            const EntityBrushesMap& entityBrushes() const {
                return m_entityBrushes;
            }

            const std::vector<Model::Brush*>& worldBrushes() const {
                return m_worldBrushes;
            }

            void addBrush(Model::Brush* brush) {
                VisitParent visitParent(brush, m_entityBrushes, m_worldBrushes);
                Model::Node* parent = brush->parent();
                parent->accept(visitParent);
            }
        };

        class NodeWriter::WriteNode : public Model::NodeVisitor {
        private:
            NodeSerializer& m_serializer;
            const std::list<Model::EntityAttribute> m_parentAttributes;
        public:
            WriteNode(NodeSerializer& serializer, const Model::Node* parent = nullptr) :
            m_serializer(serializer),
            m_parentAttributes(m_serializer.parentAttributes(parent)) {}

            void doVisit(Model::World* /* world */) override   { stopRecursion(); }
            void doVisit(Model::Layer* /* layer */) override   { stopRecursion(); }

            void doVisit(Model::Group* group) override   {
                m_serializer.group(group, m_parentAttributes);
                WriteNode visitor(m_serializer, group);
                group->iterate(visitor);
                stopRecursion();
            }

            void doVisit(Model::Entity* entity) override {
                m_serializer.entity(entity, entity->attributes(), m_parentAttributes, entity);
                stopRecursion();
            }

            void doVisit(Model::Brush* /* brush */) override   { stopRecursion();  }
        };

        NodeWriter::NodeWriter(Model::World& world, FILE* stream) :
        m_world(world),
        m_serializer(MapFileSerializer::create(m_world.format(), stream)) {}

        NodeWriter::NodeWriter(Model::World& world, std::ostream& stream) :
        m_world(world),
        m_serializer(MapStreamSerializer::create(m_world.format(), stream)) {}

        NodeWriter::NodeWriter(Model::World& world, NodeSerializer* serializer) :
        m_world(world),
        m_serializer(serializer) {}

        void NodeWriter::writeMap() {
            m_serializer->beginFile();
            writeDefaultLayer();
            writeCustomLayers();
            m_serializer->endFile();
        }

        void NodeWriter::writeDefaultLayer() {
            m_serializer->defaultLayer(m_world);

            const std::vector<Model::Node*>& children = m_world.defaultLayer()->children();
            WriteNode visitor(*m_serializer);
            Model::Node::accept(std::begin(children), std::end(children), visitor);
        }

        void NodeWriter::writeCustomLayers() {
            const std::vector<Model::Layer*> customLayers = m_world.customLayers();
            for (auto* layer : customLayers) {
                writeCustomLayer(layer);
            }
        }

        void NodeWriter::writeCustomLayer(Model::Layer* layer) {
            m_serializer->customLayer(layer);

            const std::vector<Model::Node*>& children = layer->children();
            WriteNode visitor(*m_serializer, layer);
            Model::Node::accept(std::begin(children), std::end(children), visitor);
        }

        void NodeWriter::writeNodes(const std::vector<Model::Node*>& nodes) {
            using CollectNodes = Model::AssortNodesVisitorT<Model::SkipLayersStrategy, Model::CollectGroupsStrategy, Model::CollectEntitiesStrategy, CollectEntityBrushesStrategy>;

            m_serializer->beginFile();

            CollectNodes collect;
            Model::Node::accept(std::begin(nodes), std::end(nodes), collect);

            writeWorldBrushes(collect.worldBrushes());
            writeEntityBrushes(collect.entityBrushes());

            const std::vector<Model::Group*>& groups = collect.groups();
            const std::vector<Model::Entity*>& entities = collect.entities();

            WriteNode visitor(*m_serializer);
            Model::Node::accept(std::begin(groups), std::end(groups), visitor);
            Model::Node::accept(std::begin(entities), std::end(entities), visitor);

            m_serializer->endFile();
        }

        void NodeWriter::writeWorldBrushes(const std::vector<Model::Brush*>& brushes) {
            if (!brushes.empty()) {
                m_serializer->entity(&m_world, m_world.attributes(), {}, brushes);
            }
        }

        void NodeWriter::writeEntityBrushes(const EntityBrushesMap& entityBrushes) {
            for (const auto& [entity, brushes] : entityBrushes) {
                m_serializer->entity(entity, entity->attributes(), {}, brushes);
            }
        }

        void NodeWriter::writeBrushFaces(const std::vector<Model::BrushFace*>& faces) {
            m_serializer->beginFile();
            m_serializer->brushFaces(faces);
            m_serializer->endFile();
        }
    }
}
