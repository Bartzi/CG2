#pragma once

#include <GL/freeglut.h>

#include "node.h"
#include "common.h"


class PolygonalGeometry : public Node
{
public:
    PolygonalGeometry(const std::string & name = "unnamed");
    virtual ~PolygonalGeometry();

	const GLenum mode() const;
	void setMode(const GLenum mode);	

    const t_vec3s & vertices() const;
    t_vec3s & vertices();  
    const t_vec3s & normals() const;
    t_vec3s & normals();
    const t_vec2s & texcs() const;
    t_vec2s & texcs();
    const t_uints & indices() const;
    t_uints & indices();

    virtual void draw(
        Program * program
    ,   const glm::mat4 & transform);

    virtual const AxisAlignedBoundingBox boundingBox() const;

protected:
    t_vec3s m_vertices;
    t_vec3s m_normals;
    t_vec2s m_texcs;

    t_uints m_indices;

	GLenum  m_mode;
};