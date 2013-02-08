
#include "aabb.h"

#include <cfloat>


using namespace glm;


AxisAlignedBoundingBox::AxisAlignedBoundingBox()
:   m_invalidated(true)
{
    invalidate();
}

AxisAlignedBoundingBox::~AxisAlignedBoundingBox()
{
}

const bool AxisAlignedBoundingBox::extend(const vec3 & vertex)
{
    const vec3 llf(m_llf);
    const vec3 urb(m_urb);

    m_llf = glm::min(vertex, m_llf);
    m_urb = glm::max(vertex, m_urb);

    const bool extended(urb != m_urb && llf != m_llf);

    if(extended)
    {
        m_center =  m_llf + (m_urb - m_llf) * .5f;
        m_radius = (m_urb - m_llf).length() * .5f;

        if(m_invalidated)
            m_invalidated = false;
    }
    return extended;
}

const bool AxisAlignedBoundingBox::extend(const AxisAlignedBoundingBox & aabb)
{
    return extend(aabb.m_llf) | extend(aabb.m_urb);
}

const vec3 & AxisAlignedBoundingBox::center() const
{
    return m_center;
}

const float_t AxisAlignedBoundingBox::radius() const
{
    return m_radius;
}

const vec3 & AxisAlignedBoundingBox::llf() const
{
    return m_llf;
}

const vec3 & AxisAlignedBoundingBox::urb() const
{
    return m_urb;
}

const bool AxisAlignedBoundingBox::inside(const vec3 & vertex) const
{
    return vertex.x >= m_llf.x && vertex.x <= m_urb.x
        && vertex.y >= m_llf.y && vertex.y <= m_urb.y
        && vertex.z >= m_llf.z && vertex.z <= m_urb.z;
}

const bool AxisAlignedBoundingBox::outside(const vec3 & vertex) const
{
    return !inside(vertex);
}

const void AxisAlignedBoundingBox::invalidate()
{
    m_urb = vec3(-FLT_MAX,-FLT_MAX,-FLT_MAX);
    m_llf = vec3(+FLT_MAX,+FLT_MAX,+FLT_MAX);

    m_center = vec3(0.f);
    m_radius = 0.f;

    m_invalidated = true;
}

const bool AxisAlignedBoundingBox::valid() const
{
    return !m_invalidated;
}

const AxisAlignedBoundingBox & AxisAlignedBoundingBox::operator*(const glm::mat4 & rhs) const
{
    return AxisAlignedBoundingBox(*this) *= rhs;
}

AxisAlignedBoundingBox & AxisAlignedBoundingBox::operator*=(const glm::mat4 & rhs)
{

    m_urb = vec3(vec4(m_urb, 1.f) * rhs);
    m_llf = vec3(vec4(m_llf, 1.f) * rhs);

    m_center =  m_llf + (m_urb - m_llf) * .5f;
    m_radius = (m_urb - m_llf).length() * .5f;

    return *this;
}