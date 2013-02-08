
#include "polygonalgeometry.h"

#include "aabb.h"


PolygonalGeometry::PolygonalGeometry(const std::string & name)
:   Node(name)
,	m_mode(GL_TRIANGLES)
{
}

PolygonalGeometry::~PolygonalGeometry()
{
}

const GLenum PolygonalGeometry::mode() const
{
	return m_mode;
}

void PolygonalGeometry::setMode(const GLenum mode)
{
	m_mode = mode;
}

const t_vec3s & PolygonalGeometry::vertices() const
{
    return m_vertices;
}

t_vec3s & PolygonalGeometry::vertices()
{
    invalidateBoundingBox();
    return m_vertices;
}

const t_vec3s & PolygonalGeometry::normals() const
{
    return m_normals;
}

t_vec3s & PolygonalGeometry::normals()
{
    return m_normals;
}

const t_vec2s & PolygonalGeometry::texcs() const
{
    return m_texcs;
}
t_vec2s & PolygonalGeometry::texcs()
{
    return m_texcs;
}

const t_uints & PolygonalGeometry::indices() const
{
    return m_indices;
}

t_uints & PolygonalGeometry::indices()
{
    return m_indices;
}

const AxisAlignedBoundingBox PolygonalGeometry::boundingBox() const
{
    if(m_aabb.valid())
        return m_aabb;

    t_vec3s::const_iterator i(m_vertices.begin());
    const t_vec3s::const_iterator iEnd(m_vertices.end());

    for(; i != iEnd; ++i)
        m_aabb.extend(*i);

    return m_aabb;
}

void PolygonalGeometry::draw(
    Program * program
,   const glm::mat4 & transform)
{
}
