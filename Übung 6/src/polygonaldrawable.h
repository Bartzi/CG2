#pragma once

#include <GL/freeglut.h>

#include "node.h"

#include <map>
#include <vector>

#include <glm/glm.hpp>


class PolygonalGeometry;
class BufferObject;
class Program;


class PolygonalDrawable : public Node
{
public:
    PolygonalDrawable(const std::string & name = "unnamed");
    virtual ~PolygonalDrawable();

    void setGeometry(PolygonalGeometry * geometry);
    PolygonalGeometry * geometry();

    virtual void draw(
        Program * program
    ,   const glm::mat4 & transform);

    virtual const AxisAlignedBoundingBox boundingBox() const;

protected:

    const glm::mat4 & transform() const;

	void setupBuffers();
    void deleteBuffers();

    virtual void invalidateBoundingBox();

protected:
    GLuint m_vao;

    PolygonalGeometry * m_geometry;

	typedef std::vector<BufferObject *> t_bufferObjects;
	t_bufferObjects m_elementArrayBOs;

	typedef std::map<std::string, BufferObject *> t_bufferObjectsByAttribute;
	t_bufferObjectsByAttribute m_arrayBOsByAttribute;	

};