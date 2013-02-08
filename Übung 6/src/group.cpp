
#include "group.h"

#include "polygonaldrawable.h"
#include "program.h"

#include <algorithm>

using namespace glm;


Group::Group(const std::string & name)
:   Node(name)
{
}

Group::~Group()
{
    while(!m_children.empty())
        removeLast();
}

void Group::draw(
    Program * program
,   const mat4 & transform)
{
	t_nodes::const_iterator i(m_children.begin());
	const t_nodes::const_iterator iEnd(m_children.end());

	for(; i != iEnd; ++i)
	{
		Node * node(*i);
		
        if(RF_Absolute == m_rf)
            node->draw(program, this->transform());
        else
            node->draw(program, transform * this->transform());
	}	
}

const bool Group::contains(Node * node) const
{
    return m_children.end() != std::find(m_children.begin(), m_children.end(), node);
}

void Group::insert(
    const Group::t_nodes::iterator & before
,   Group * group)
{
    return insert(before, dynamic_cast<Node *>(group));
}

void Group::insert(
    const Group::t_nodes::iterator & before
,   PolygonalDrawable * drawable)
{
    return insert(before, dynamic_cast<Node *>(drawable));
}

void Group::insert(
    const Group::t_nodes::iterator & before
,   Node * node)
{
    if(!node)
        return;

    if(!contains(node))
        node->parents().insert(this);

    m_children.insert(before, node);
}

void Group::prepend(Group * group)
{
    return prepend(dynamic_cast<Node *>(group));
}

void Group::prepend(PolygonalDrawable * drawable)
{
    return prepend(dynamic_cast<Node *>(drawable));
}

void Group::prepend(Node * node)
{
    if(!node)
        return;

    if(!contains(node))
        node->parents().insert(this);

    m_children.push_front(node);
}

void Group::append(Group * group)
{
    return append(dynamic_cast<Node *>(group));
}

void Group::append(PolygonalDrawable * drawable)
{
    return append(dynamic_cast<Node *>(drawable));
}

void Group::append(Node * node)
{
    if(!node)
        return;

    if(!contains(node))
        node->parents().insert(this);

    m_children.push_back(node);
}

void Group::removeFirst()
{
    if(m_children.empty())
        return;

    Node * node(m_children.front());

    node->parents().erase(this);
    m_children.pop_front();

    if(node->parents().empty())
        delete node;
}

void Group::removeLast()
{
    if(m_children.empty())
        return;

    Node * node(m_children.back());

    node->parents().erase(this);
    m_children.pop_back();

    if(node->parents().empty())
        delete node;
}

const void Group::remove(
    Node * node
,   const bool deleteIfParentsEmpty)
{
    if(!contains(node))
        node->parents().erase(this);

    m_children.remove(node);

    if(deleteIfParentsEmpty && node->parents().empty())
        delete node;
}

const Group::t_nodes & Group::children() const
{
    return m_children;
}

const AxisAlignedBoundingBox Group::boundingBox() const
{
    if(m_aabb.valid())
        return m_aabb;

    t_nodes::const_iterator i(m_children.begin());
    const t_nodes::const_iterator iEnd(m_children.end());

    if(RF_Relative == m_rf)
        for(; i != iEnd; ++i)
            m_aabb.extend((*i)->boundingBox() * transform());
    else
        for(; i != iEnd; ++i)
            m_aabb.extend((*i)->boundingBox());

    return m_aabb;
}

Group * Group::asGroup()
{
	return this;
}