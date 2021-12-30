#include "Luddite/Utilities/YamlParsers.hpp"
void operator >>(const YAML::Node& node, float& f)
{
        f = node.as<float>();
}
void operator >>(const YAML::Node& node, int32_t& i)
{
        i = node.as<int32_t>();
}
void operator >>(const YAML::Node& node, uint32_t& u)
{
        u = node.as<uint32_t>();
}
void operator >>(const YAML::Node& node, glm::vec2& v)
{
        node[0] >> v.x;
        node[1] >> v.y;
}
void operator >>(const YAML::Node& node, glm::vec3& v)
{
        node[0] >> v.x;
        node[1] >> v.y;
        node[2] >> v.z;
}
void operator >>(const YAML::Node& node, glm::vec4& v)
{
        node[0] >> v.x;
        node[1] >> v.y;
        node[2] >> v.z;
        node[3] >> v.w;
}
void operator >>(const YAML::Node& node, glm::ivec2& v)
{
        node[0] >> v.x;
        node[1] >> v.y;
}
void operator >>(const YAML::Node& node, glm::ivec3& v)
{
        node[0] >> v.x;
        node[1] >> v.y;
        node[2] >> v.z;
}
void operator >>(const YAML::Node& node, glm::ivec4& v)
{
        node[0] >> v.x;
        node[1] >> v.y;
        node[2] >> v.z;
        node[3] >> v.w;
}
void operator >>(const YAML::Node& node, glm::uvec2& v)
{
        node[0] >> v.x;
        node[1] >> v.y;
}
void operator >>(const YAML::Node& node, glm::uvec3& v)
{
        node[0] >> v.x;
        node[1] >> v.y;
        node[2] >> v.z;
}
void operator >>(const YAML::Node& node, glm::uvec4& v)
{
        node[0] >> v.x;
        node[1] >> v.y;
        node[2] >> v.z;
        node[3] >> v.w;
}
void operator >>(const YAML::Node& node, glm::mat3& m)
{
        node[0] >> m[0][0];
        node[1] >> m[0][1];
        node[2] >> m[0][2];
        node[3] >> m[1][0];
        node[4] >> m[1][1];
        node[5] >> m[1][2];
        node[6] >> m[2][0];
        node[7] >> m[2][1];
        node[8] >> m[2][2];
}
void operator >>(const YAML::Node& node, glm::mat4& m)
{
        node[0] >> m[0][0];
        node[1] >> m[0][1];
        node[2] >> m[0][2];
        node[3] >> m[0][3];
        node[4] >> m[1][0];
        node[5] >> m[1][1];
        node[6] >> m[1][2];
        node[7] >> m[1][3];
        node[8] >> m[2][0];
        node[9] >> m[2][1];
        node[10] >> m[2][2];
        node[11] >> m[2][3];
        node[12] >> m[3][0];
        node[13] >> m[3][1];
        node[14] >> m[3][2];
        node[15] >> m[3][3];
}
void operator >>(const YAML::Node& node, Luddite::ColorRGB& c)
{
        node[0] >> c.GetVec3().x;
        node[1] >> c.GetVec3().y;
        node[2] >> c.GetVec3().z;
}
void operator >>(const YAML::Node& node, Luddite::ColorRGBA& c)
{
        node[0] >> c.GetVec4().x;
        node[1] >> c.GetVec4().y;
        node[2] >> c.GetVec4().z;
        node[2] >> c.GetVec4().w;
}
