#ifndef OCTREE_H
#define OCTREE_H

#include "utils.hpp"

//
// STL
//
#include <map>

unsigned int RENDERLEVEL = 5;
unsigned int MAXOCTREELEVEL = 8;

struct Bounds {
    vec3 llf;
    vec3 urb;

    Bounds(const vec3& llf_, const vec3& urb_)
        : llf(llf_), urb(urb_)
    {
        //
    }

    Bounds()
        : llf(), urb()
    {
        //
    }

    bool containsPoint(const vec3& p) const {
        return (llf[0] <= p[0]) && (p[0] <= urb[0]) &&
               (llf[1] <= p[1]) && (p[1] <= urb[1]) &&
               (llf[2] <= p[2]) && (p[2] <= urb[2]);
    }

    vec3 getLLF() const {
        return llf;
    }

    vec3 getURB() const {
        return urb;
    }

    vec3 center() const {
        return (urb + llf) / 2.0f;
    }

	vec3 size() const {
		return urb - llf;
	}

};

class OctreeNode {
    public:

    OctreeNode(Bounds area, unsigned int level, OctreeNode* parent = 0)
        : area_(area), parent_(parent), level_(level)
    {
		this->center = area.center();
		this->midPoint_ = vec3(0.0f, 0.0f, 0.0f);
    }

    ~OctreeNode() {
        vertices_.clear();
        children_.clear();
    }

    vec3 getPosition(unsigned int vertexID) const {
        return vec3(vertexData_[vertexID]->x, vertexData_[vertexID]->y, vertexData_[vertexID]->z);
    }

    vec3 computeMidpoints() {
        /**************************************************************
        * Aufgabe 2A - Begin
        ***************************************************************/

        if(!this->isLeaf())
		{
			unsigned int counter = 0;
			for(int i = 0; i < 8; ++i)
			{
				vec3 childMidpoint = this->children_[i]->computeMidpoints();
				if(childMidpoint == vec3(0.0f, 0.0f, 0.0f))
				{
					continue;
				}
				this->midPoint_ += childMidpoint;
				counter++;
			}

			this->midPoint_ /= counter;
			return this->midPoint_;
		}

		for each(unsigned int id in this->vertices_)
			this->midPoint_ += this->getPosition(id);

		if(this->vertices_.size() == 0)
			this->midPoint_ = vec3(0.0f, 0.0f, 0.0f);
		else
			this->midPoint_ /= this->vertices_.size();

        /**************************************************************
        * Aufgabe 2A - End
        ***************************************************************/
        
        return this->midPoint_;
    }

    int AABBInFrustum(Bounds& b, float frustum[6][4]) {
        /**************************************************************
        * Aufgabe 2B - Begin
        ***************************************************************/

		int cornersInFrustum = 0;
		vec3 size = b.size();
		for(int z = 0; z < 2; ++z)
			for(int y = 0; y < 2; ++y)
				for(int x = 0; x < 2; ++x)
				{
					vec3 currentPoint = vec3(b.llf.x + x * size.x, b.llf.y + y * size.y, b.llf.z + z * size.z);
					int frustumCounter = 0;
					for(int i = 0; i < 6; ++i)
					{
						
						if(frustum[i][0] * currentPoint.x + frustum[i][1] * currentPoint.y + frustum[i][2] * currentPoint.z + frustum[i][3] > 0)
						{
							frustumCounter++;
						}
					}
					if(frustumCounter == 6)
						cornersInFrustum++;
				}
       
		if(cornersInFrustum == 0)
			return 0;
		if(cornersInFrustum == 8)
			return 2;
		return 1;

        /**************************************************************
        * Aufgabe 2B - End
        ***************************************************************/
    }

    unsigned int renderPoints(float frustum[6][4]) {
        /**************************************************************
        * Aufgabe 1B/2A/2B - Begin
        ***************************************************************/
		unsigned int renderedPoints = 0;

		if(this->level_ == RENDERLEVEL)
		{
			if(this->midPoint_ == vec3(0.0f, 0.0f, 0.0f))
			{
				return 0;
			}
			glVertex3f(this->midPoint_.x, this->midPoint_.y, this->midPoint_.z);
			return 1;
		}

        if(!this->isLeaf())
		{
			for(int i = 0; i < 8; ++i)
			{
				if(this->AABBInFrustum(this->children_[i]->area_, frustum) > 0)
					renderedPoints += this->children_[i]->renderPoints(frustum);
			}
			return renderedPoints;
		}

		for each(int id in this->vertices_)
		{
			vec3 position = this->getPosition(id);
			glVertex3f(position.x, position.y, position.z);
		}


        return this->vertices_.size();

        /**************************************************************
        * Aufgabe 1B/2A/2B - End
        ***************************************************************/
    }

    void renderDebugGeometry() {
        vec3 llf_ = area_.getLLF();
        vec3 urb_ = area_.getURB();

        /**************************************************************
        * Aufgabe 1C - Begin
        ***************************************************************/

		if(!this->isLeaf() && this->level_ < RENDERLEVEL)
			for( int i = 0; i < 8; ++i)
					this->children_[i]->renderDebugGeometry();

		if(this->vertices_.size() == 0 && this->isLeaf())
			return;

		glVertex3f(llf_.x, llf_.y, llf_.z);
		glVertex3f(urb_.x, llf_.y, llf_.z);

		glVertex3f(urb_.x, llf_.y, llf_.z);
		glVertex3f(urb_.x, llf_.y, urb_.z);

		glVertex3f(urb_.x, llf_.y, urb_.z);
		glVertex3f(llf_.x, llf_.y, urb_.z);

		glVertex3f(llf_.x, llf_.y, urb_.z);
		glVertex3f(llf_.x, llf_.y, llf_.z);

		glVertex3f(llf_.x, urb_.y, llf_.z);
		glVertex3f(urb_.x, urb_.y, llf_.z);

		glVertex3f(urb_.x, urb_.y, llf_.z);
		glVertex3f(urb_.x, urb_.y, urb_.z);

		glVertex3f(urb_.x, urb_.y, urb_.z);
		glVertex3f(llf_.x, urb_.y, urb_.z);

		glVertex3f(llf_.x, urb_.y, urb_.z);
		glVertex3f(llf_.x, urb_.y, llf_.z);

		glVertex3f(llf_.x, llf_.y, llf_.z);
		glVertex3f(llf_.x, urb_.y, llf_.z);

		glVertex3f(urb_.x, llf_.y, llf_.z);
		glVertex3f(urb_.x, urb_.y, llf_.z);

		glVertex3f(urb_.x, llf_.y, urb_.z);
		glVertex3f(urb_.x, urb_.y, urb_.z);

		glVertex3f(llf_.x, llf_.y, urb_.z);
		glVertex3f(llf_.x, urb_.y, urb_.z);

        /**************************************************************
        * Aufgabe 1C - End
        ***************************************************************/
    }

    virtual void insertVertex(unsigned int vertexID, const vec3& center) {
        addVertexToOctree(vertexID, center);
    }

    bool isLeaf() {
        return !children_[0] && !children_[1] && !children_[2] && !children_[3] && !children_[4] && !children_[5] && !children_[6] && !children_[7];
    }

	Bounds createBoundsFromLLF(vec3 llf)
	{
		return Bounds(llf, llf + (this->center - this->area_.getLLF()));
	}

    void addVertexToOctree(unsigned int vertexID, const vec3& p) {
        // append to octree
        if(!area_.containsPoint(p))
            return;

        /**************************************************************
        * Aufgabe 1A - Begin
        ***************************************************************/

		if(this->level_ >= MAXOCTREELEVEL)
		{
			this->vertices_.push_back(vertexID);
			return;
		}

		if(!this->isLeaf())
		{
			for(int i = 0; i < 8; ++i)
			{
				OctreeNode* currentNode = this->children_[i];
				if(currentNode->area_.containsPoint(p))
				{
					currentNode->addVertexToOctree(vertexID, p);
					return;
				}
			}
			return;
		}

		if(this->vertices_.size() == 0)
		{
			this->vertices_.push_back(vertexID);
			return;
		}

		for(int z = 0; z <= 1; ++z)
			for(int y = 0; y <= 1; ++y)
				for(int x = 0; x <= 1; ++x)
				{
					vec3 llf = this->area_.getLLF();
					vec3 newLlf = vec3(
						x == 0 ? llf.x : center.x, 
						y == 0 ? llf.y : center.y, 
						z == 0 ? llf.z : center.z);
					OctreeNode* newNode = new OctreeNode(this->createBoundsFromLLF(newLlf), this->level_ + 1, this);
					this->children_[x + y * 2 + z * 4] = newNode;
					if(newNode->area_.containsPoint(p))
						newNode->addVertexToOctree(vertexID, p);
					unsigned int id = this->vertices_.front();
					if(newNode->area_.containsPoint(this->getPosition(id)))
						newNode->addVertexToOctree(id, this->getPosition(id)); 					
				} 

		this->vertices_.clear();

        /**************************************************************
        * Aufgabe 1A - End
        ***************************************************************/
    }

    static PlyVertex** vertexData_;

    private:
        Bounds area_;
        OctreeNode* parent_;
        unsigned int level_;

        std::vector<unsigned int> vertices_;
        std::map<int, OctreeNode*> children_;

        vec3 midPoint_;
		vec3 center;
};

PlyVertex** OctreeNode::vertexData_ = 0;

#endif //OCTREE_H
