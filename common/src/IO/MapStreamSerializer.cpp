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

#include "MapStreamSerializer.h"

#include "Exceptions.h"
#include "Macros.h"
#include "Model/BrushFace.h"

#include <memory>
#include <ostream>
#include <sstream>
#include <string>

namespace TrenchBroom {
    namespace IO {
        class QuakeStreamSerializer : public MapStreamSerializer {
        public:
            QuakeStreamSerializer(std::ostream& stream) :
            MapStreamSerializer(stream) {}
        private:
            virtual void doWriteBrushFace(std::ostream& stream, Model::BrushFace* face) override {
                writeFacePoints(stream, face);
                stream << " ";
                writeTextureInfo(stream, face);
                stream << "\n";
            }
        protected:
            void writeFacePoints(std::ostream& stream, Model::BrushFace* face) {
                const Model::BrushFace::Points& points = face->points();

                stream.precision(FloatPrecision);
                stream << "( " <<
                ftos(points[0].x(), FloatPrecision) << " " <<
                ftos(points[0].y(), FloatPrecision) << " " <<
                ftos(points[0].z(), FloatPrecision) <<" ) ( " <<
                ftos(points[1].x(), FloatPrecision) << " " <<
                ftos(points[1].y(), FloatPrecision) << " " <<
                ftos(points[1].z(), FloatPrecision) << " ) ( " <<
                ftos(points[2].x(), FloatPrecision) << " " <<
                ftos(points[2].y(), FloatPrecision) << " " <<
                ftos(points[2].z(), FloatPrecision) << " )";
            }

            void writeTextureInfo(std::ostream& stream, Model::BrushFace* face) {
                const std::string& textureName = face->textureName().empty() ? Model::BrushFace::NoTextureName : face->textureName();
                stream << textureName << " " <<
                ftos(face->xOffset(), FloatPrecision)  << " " <<
                ftos(face->yOffset(), FloatPrecision)  << " " <<
                ftos(face->rotation(), FloatPrecision) << " " <<
                ftos(face->xScale(), FloatPrecision)   << " " <<
                ftos(face->yScale(), FloatPrecision);
            }
        };

        class Quake2StreamSerializer : public QuakeStreamSerializer {
        public:
            Quake2StreamSerializer(std::ostream& stream) :
            QuakeStreamSerializer(stream) {}
        private:
            virtual void doWriteBrushFace(std::ostream& stream, Model::BrushFace* face) override {
                writeFacePoints(stream, face);
                stream << " ";
                writeTextureInfo(stream, face);
                // While it is possible to omit surface attributes, see MapFileSerializer for a description of why it's best to keep them.
                stream << " ";
                writeSurfaceAttributes(stream, face);
                stream << "\n";
            }
        protected:
            void writeSurfaceAttributes(std::ostream& stream, Model::BrushFace* face) {
                stream <<
                face->surfaceContents()  << " " <<
                face->surfaceFlags()     << " " <<
                ftos(face->surfaceValue(), FloatPrecision);
            }
        };

        class DaikatanaStreamSerializer : public Quake2StreamSerializer {
        public:
            DaikatanaStreamSerializer(std::ostream& stream) :
            Quake2StreamSerializer(stream) {}
        private:
            virtual void doWriteBrushFace(std::ostream& stream, Model::BrushFace* face) override {
                writeFacePoints(stream, face);
                stream << " ";
                writeTextureInfo(stream, face);
                if (face->hasSurfaceAttributes() || face->hasColor()) {
                    stream << " ";
                    writeSurfaceAttributes(stream, face);

                }
                if (face->hasColor()) {
                    stream << " ";
                    writeSurfaceColor(stream, face);
                }
                stream << "\n";
            }
        protected:
            void writeSurfaceColor(std::ostream& stream, Model::BrushFace* face) {
                stream <<
                static_cast<int>(face->color().r()) << " " <<
                static_cast<int>(face->color().g()) << " " <<
                static_cast<int>(face->color().b());
            }
        };

        class ValveStreamSerializer : public QuakeStreamSerializer {
        public:
            ValveStreamSerializer(std::ostream& stream) :
            QuakeStreamSerializer(stream) {}
        private:
            void doWriteBrushFace(std::ostream& stream, Model::BrushFace* face) override {

                writeFacePoints(stream, face);
                stream << " ";
                writeValveTextureInfo(stream, face);
                stream << "\n";
            }
        private:
            void writeValveTextureInfo(std::ostream& stream, Model::BrushFace* face) {
                const std::string& textureName = face->textureName().empty() ? Model::BrushFace::NoTextureName : face->textureName();
                const vm::vec3& xAxis = face->textureXAxis();
                const vm::vec3& yAxis = face->textureYAxis();

                stream.precision(6);
                stream <<
                textureName     << " " <<
                "[ " <<
                xAxis.x() << " " <<
                xAxis.y() << " " <<
                xAxis.z() << " " <<
                face->xOffset()   <<
                " ] [ " <<
                yAxis.x() << " " <<
                yAxis.y() << " " <<
                yAxis.z() << " " <<
                face->yOffset()   <<
                " ] " <<
                face->rotation() << " " <<
                face->xScale()   << " " <<
                face->yScale();
            }
        };

        class Hexen2StreamSerializer : public QuakeStreamSerializer {
        public:
            Hexen2StreamSerializer(std::ostream& stream) :
            QuakeStreamSerializer(stream) {}
        private:
            virtual void doWriteBrushFace(std::ostream& stream, Model::BrushFace* face) override {
                writeFacePoints(stream, face);
                stream << " ";
                writeTextureInfo(stream, face);
                stream << " 0\n"; // extra value written here
            }
        };

        std::unique_ptr<NodeSerializer> MapStreamSerializer::create(const Model::MapFormat format, std::ostream& stream) {
            switch (format) {
                case Model::MapFormat::Standard:
                    return std::make_unique<QuakeStreamSerializer>(stream);
                case Model::MapFormat::Quake2:
                    // TODO 2427: Implement Quake3 serializers and use them
                case Model::MapFormat::Quake3:
                case Model::MapFormat::Quake3_Legacy:
                    return std::make_unique<Quake2StreamSerializer>(stream);
                case Model::MapFormat::Daikatana:
                    return std::make_unique<DaikatanaStreamSerializer>(stream);
                case Model::MapFormat::Valve:
                    return std::make_unique<ValveStreamSerializer>(stream);
                case Model::MapFormat::Hexen2:
                    return std::make_unique<Hexen2StreamSerializer>(stream);
                case Model::MapFormat::Unknown:
                    throw FileFormatException("Unknown map file format");
                switchDefault()
            }
        }

        MapStreamSerializer::MapStreamSerializer(std::ostream& stream) :
        m_stream(stream) {}

        MapStreamSerializer::~MapStreamSerializer() = default;

        template <typename T, typename P>
        std::string ftos_helper(const T v, const P precision) {
            std::ostringstream strout;
            strout.precision(static_cast<std::streamsize>(precision));
            strout << std::fixed  << v;

            std::string str = strout.str() ;
            size_t end = str.find_last_not_of('0');
            if (str[end] == '.') {
                --end;
            }
            return str.erase(end + 1);
        }

        std::string MapStreamSerializer::ftos(const float v, const int precision) {
            return ftos_helper(v, precision);
        }

        std::string MapStreamSerializer::ftos(const double v, const int precision) {
            return ftos_helper(v, precision);
        }

        void MapStreamSerializer::doBeginFile() {}
        void MapStreamSerializer::doEndFile() {}

        void MapStreamSerializer::doBeginEntity(const Model::Node* /* node */) {
            m_stream << "// entity " << entityNo() << "\n";
            m_stream << "{\n";
        }

        void MapStreamSerializer::doEndEntity(Model::Node* /* node */) {
            m_stream << "}\n";
        }

        void MapStreamSerializer::doEntityAttribute(const Model::EntityAttribute& attribute) {
            m_stream << "\"" << escapeEntityAttribute(attribute.name()) << "\" \"" << escapeEntityAttribute(attribute.value()) << "\"\n";
        }

        void MapStreamSerializer::doBeginBrush(const Model::Brush* /* brush */) {
            m_stream << "// brush " << brushNo() << "\n";
            m_stream << "{\n";
        }

        void MapStreamSerializer::doEndBrush(Model::Brush* /* brush */) {
            m_stream << "}\n";
        }

        void MapStreamSerializer::doBrushFace(Model::BrushFace* face) {
            doWriteBrushFace(m_stream, face);
        }
    }
}
