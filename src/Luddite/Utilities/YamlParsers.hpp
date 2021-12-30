#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Graphics/Color.hpp"
void operator >>(const YAML::Node& node, float& f);
void operator >>(const YAML::Node& node, int32_t& i);
void operator >>(const YAML::Node& node, uint32_t& u);
void operator >>(const YAML::Node& node, glm::vec2& v);
void operator >>(const YAML::Node& node, glm::vec3& v);
void operator >>(const YAML::Node& node, glm::vec4& v);
void operator >>(const YAML::Node& node, glm::ivec2& v);
void operator >>(const YAML::Node& node, glm::ivec3& v);
void operator >>(const YAML::Node& node, glm::ivec4& v);
void operator >>(const YAML::Node& node, glm::uvec2& v);
void operator >>(const YAML::Node& node, glm::uvec3& v);
void operator >>(const YAML::Node& node, glm::uvec4& v);
void operator >>(const YAML::Node& node, glm::mat3& m);
void operator >>(const YAML::Node& node, glm::mat4& m);
void operator >>(const YAML::Node& node, Luddite::ColorRGB& c);
void operator >>(const YAML::Node& node, Luddite::ColorRGBA& c);
