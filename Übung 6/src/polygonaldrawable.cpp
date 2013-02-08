
#include <GL/glew.h>

#include "polygonaldrawable.h"

#include "aabb.h"
#include "bufferobject.h"
#include "polygonalgeometry.h"
#include "program.h"


using namespace glm;

static const std::string TRANSFORM_UNIFORM ("transform");


PolygonalDrawable::PolygonalDrawable(const std::string & name)
:   Node(name)
,   m_vao(-1)
,   m_geometry(nullptr)
{
}

PolygonalDrawable::~PolygonalDrawable()
{
    setGeometry(nullptr);
    deleteBuffers();
}

const AxisAlignedBoundingBox PolygonalDrawable::boundingBox() const
{
    if(!m_geometry)
        return AxisAlignedBoundingBox();

    return m_geometry->boundingBox();
}

void PolygonalDrawable::setGeometry(PolygonalGeometry * geometry)
{
    if(m_geometry)
    {
        m_geometry->parents().erase(this);
        if(m_geometry->parents().empty())
            delete m_geometry;

		deleteBuffers();
    }

    m_geometry = geometry;

    if(m_geometry)
        m_geometry->parents().insert(this);
}

PolygonalGeometry * PolygonalDrawable::geometry()
{
    return m_geometry;
}

void PolygonalDrawable::draw(
    Program * program
,   const glm::mat4 & transform)
{
	if(!program)
		return;	// TODO: fixed function not supported

    if(!m_geometry || m_geometry->indices().empty())
        return;

	if(m_arrayBOsByAttribute.empty() || m_elementArrayBOs.empty())
		setupBuffers();

	program->use();
    program->setUniform(TRANSFORM_UNIFORM, transform);
	


    glBindVertexArray(m_vao);                                                                  
	glError();

	// bind all buffers to their attributes

	t_bufferObjectsByAttribute::const_iterator i(m_arrayBOsByAttribute.begin());
	const t_bufferObjectsByAttribute::const_iterator iEnd(m_arrayBOsByAttribute.end());

	for(; i != iEnd; ++i)
		i->second->bind(program->attributeLocation(i->first));

	t_bufferObjects::const_iterator e(m_elementArrayBOs.begin());
	const t_bufferObjects::const_iterator eEnd(m_elementArrayBOs.end());

	for(; e != eEnd; ++e)
		(*e)->draw(m_geometry->mode());

	for(i = m_arrayBOsByAttribute.begin(); i != iEnd; ++i)
		i->second->release(program->attributeLocation(i->first));

    program->release();

    glBindVertexArray(0);
	glError();
}

void PolygonalDrawable::invalidateBoundingBox()
{
    return Node::invalidateBoundingBox();
}

void PolygonalDrawable::setupBuffers()
{
	assert(m_geometry);

    deleteBuffers();

    glGenVertexArrays(1, &m_vao);
	glError();

    glBindVertexArray(m_vao);                                                                  
	glError();

	// setup element array buffers

	BufferObject * indexBO(new BufferObject(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW));
	indexBO->data<uint>(m_geometry->indices(), GL_UNSIGNED_INT, 1);

	m_elementArrayBOs.push_back(indexBO);

	// setup array buffers

	BufferObject * vertexBO(new BufferObject(GL_ARRAY_BUFFER, GL_STATIC_DRAW));
	vertexBO->data<vec3>(m_geometry->vertices(), GL_FLOAT, 3);

	BufferObject * normalBO(new BufferObject(GL_ARRAY_BUFFER, GL_STATIC_DRAW));
	normalBO->data<vec3>(m_geometry->normals(), GL_FLOAT, 3);

	m_arrayBOsByAttribute["a_vertex"] = vertexBO;
    m_arrayBOsByAttribute["a_normal"] = normalBO;

    glBindVertexArray(0);
	glError();
}

void PolygonalDrawable::deleteBuffers()
{
	m_elementArrayBOs.clear();
	m_arrayBOsByAttribute.clear();

    if(-1 != m_vao)
    {
        glDeleteVertexArrays(1, &m_vao);
	    glError();
    }
}
