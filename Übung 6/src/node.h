#pragma once

#include "aabb.h"

#include <string>
#include <set>
#include <list>

class Program;
class Group;

class Node
{
public:
    typedef std::list<Node *> t_nodes;
    typedef std::set<Node *> t_parents;

    enum e_ReferenceFrame
    {
        RF_Relative
    ,   RF_Absolute
    };

public:
    Node(const std::string & name = "unnamed");
    virtual ~Node();

    virtual void draw(
        Program * program
    ,   const glm::mat4 & transform) = 0;

    const std::string name() const;
    void setName(const std::string & name);

    const t_parents & parents() const;
    t_parents & parents();

    virtual const AxisAlignedBoundingBox boundingBox() const = 0;

    // transform

    const glm::mat4 & transform() const;
    void setTransform(const glm::mat4 & transform);

    const e_ReferenceFrame referenceFrame() const;
    void setReferenceFrame(const e_ReferenceFrame referenceFrame);

	// This saves casting during traversal.
    virtual Group * asGroup();

protected:
    virtual void invalidateBoundingBox();

protected:
    std::string m_name;

    t_parents m_parents;

    e_ReferenceFrame m_rf;
    glm::mat4 m_transform;

    mutable AxisAlignedBoundingBox m_aabb;

};

