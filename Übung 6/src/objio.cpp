
#include "objio.h"

#include "group.h"
#include "polygonaldrawable.h"
#include "polygonalgeometry.h"

#include <glm/glm.hpp>

#include <iostream>
#include <fstream>
#include <map>
#include <hash_map>

using namespace std;
using namespace glm;

// http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring

// trim from start
static inline void trim(std::string & str) 
{
    // trim trailing spaces
    const size_t endpos = str.find_last_not_of(" \t");
    if(string::npos != endpos)
        str.substr(0, endpos + 1).swap(str);

    // trim leading spaces
    const size_t startpos = str.find_first_not_of(" \t");
    if(string::npos != startpos)
        str.substr(startpos).swap(str);
}

ObjIO::ObjObject::~ObjObject()
{
    groups.clear();
}

Group * ObjIO::groupFromObjFile(const string & filePath)
{
    // http://en.wikipedia.org/wiki/Wavefront_.obj_file
    // http://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Load_OBJ

#ifdef _DEBUG
    cout << "Reading geometry from \"" << filePath << "\"." << endl;
#endif

    ifstream stream(filePath, ios::in);
    if(!stream)
    {
        cerr << "Read from \"" << filePath << "\" failed." << endl;
        return NULL;
    }

    t_objects objects;

    string line;
    while(getline(stream, line))
    {
        if(line.empty() || line.length() < 2)
            continue;

        const string type(line.substr(0, 2));
        istringstream s(line.substr(2));

        if("o " == type)
        {
            parseO (s, objects);
            continue;
        }
        if("# " == type)
            continue;

        if("v " != type && "vt" != type && "vn" != type && "f " != type && "g " != type)
            continue;


        // at this point an object is required!

        if(objects.empty())
            objects.push_back(new ObjObject);

        // always work on the last object
        ObjObject & object(*objects.back());

        if     ("v " == type)
            parseV (s, object);
        else if("vt" == type)
            parseVT(s, object);
        else if("vn" == type)
            parseVN(s, object);
        else if("f " == type)
            parseF (s, object);
        else if("g " == type)
            parseG (s, object);
    }
	stream.close();

    Group * group = toGroup(objects);
    group->setName(filePath);

    return group;
}

inline void ObjIO::parseV(
    istringstream & line
,   ObjObject & object)
{
    vec3 v;

    line >> v.x;
    line >> v.y;
    line >> v.z;

    float w(1.f);

    if(line >> w)
        v /= w;

    object.vs.push_back(v);
}

inline void ObjIO::parseVT(
    istringstream & line
,   ObjObject & object)
{
    vec2 vt;

    line >> vt.s;
    line >> vt.t;

    float w(1.f);
    if(line >> w)
        vt /= w;

    object.vts.push_back(vt);
}

inline void ObjIO::parseVN(
    istringstream & line
,   ObjObject & object)
{
    vec3 vn;

    line >> vn.x;
    line >> vn.y;
    line >> vn.z;

    object.vns.push_back(vn);
}

inline const ObjIO::e_FaceFormat ObjIO::parseFaceFormat(const istringstream & line)
{
    string s(line.str());
    trim(s);

    size_t l(s.find(" "));
    if(string::npos == l)
        l = s.length();

    const size_t f0(s.find("/", 0));

    if(string::npos == f0)
        return FF_V;

    const size_t f1(s.find("/", f0 + 1));

    if(string::npos == f1 || f1 > l)
        return FF_VT;

    if(string::npos == s.find("//", 0))
        return FF_VTN;
    else
        return FF_VN;
}

inline void ObjIO::parseF(
    istringstream & line
,   ObjObject & object)
{
    ObjGroup & group = object.groups.empty() ? object : *object.groups.back();

    const e_FaceFormat format(parseFaceFormat(line));

    group.foffs.push_back(static_cast<uint>(group.vis.size()));

    uint offset(0);
    uint i;

    while(line >> i)
    {
        group.vis.push_back(--i);

        switch(format)
        {
        case FF_V:  // v0 v1 ...

            break;

        case FF_VT: // v0/vt0 v1/vt1 ...

            line.ignore(1); // ignore slash
            line >> i;
            group.vtis.push_back(--i);

            break;
             
        case FF_VN: // v0//vn0 v1//vn1 ...

            line.ignore(2); // ignore slashes
            line >> i;
            group.vnis.push_back(--i);

            break;

        case FF_VTN: // v0/vt0/vn0 v1/vt1/vn1 ...

            line.ignore(1); // ignore slash
            line >> i;
            group.vtis.push_back(--i);

            line.ignore(1); // ignore slash
            line >> i;
            group.vnis.push_back(--i);

        default:
            break;
        }
    }
}

inline void ObjIO::parseO(
    istringstream & line
,   ObjIO::t_objects & objects)
{
    ObjObject * object(new ObjObject);
    line >> object->name;

    objects.push_back(object);
}

inline void ObjIO::parseG(
    istringstream & line
,   ObjObject & object)
{
    ObjGroup * group(new ObjGroup);
    line >> group->name;

    object.groups.push_back(group);
}

Group * ObjIO::toGroup(const t_objects & objects)
{
    std::vector<Group *> groups;

    t_objects::const_iterator io(objects.begin());
    const t_objects::const_iterator ioEnd(objects.end());

    for(; io != ioEnd; ++io)
    {
        const ObjObject & oobject(**io);

        Group * group(new Group(oobject.name));
        groups.push_back(group);

        if(!oobject.vis.empty())
            group->append(createPolygonalDrawable(oobject, oobject));

        t_groups::const_iterator ig(oobject.groups.begin());
        const t_groups::const_iterator igEnd(oobject.groups.end());

        for(; ig != igEnd; ++ig)
        {
            const ObjGroup & ogroup(**ig);

            assert(!ogroup.vis.empty());
            group->append(createPolygonalDrawable(oobject, ogroup));
        }
    }

    // wrap group if required and return

    if(groups.empty())
        return new Group;

    if(groups.size() == 1)
        return groups[0];

    Group * group(new Group);

    for(int i = 0; i < groups.size(); ++i)
        group->append(groups[i]);

    return group;
}

PolygonalDrawable * ObjIO::createPolygonalDrawable(
    const ObjObject & object
,   const ObjGroup & group)
{
    if(group.vis.empty())
        return nullptr;

    // TODO: this should test if all consecutive offsets are equal 3.
    // The current expression could return false positives.
    if(group.vis.size() / 3 != group.foffs.size())
    {
        cerr << "Ignore polygon with num vertices != 3 (only triangles are supported)." << endl;
        return nullptr;
    }

    const bool usesTexCoordIndices(!group.vtis.empty());
    const bool usesNormalIndices(!group.vnis.empty());

    PolygonalGeometry * geom(new PolygonalGeometry(object.name + " geometry"));

    const uint size(static_cast<uint>(group.vis.size()));

    for(uint i = 0; i < size; ++i)
    {
        // add vertex and its new index based on obj index

		// TODO: make use of vertex reuse!

        geom->indices().push_back(i);
        geom->vertices().push_back(object.vs[group.vis[i]]);

        if(usesTexCoordIndices)
            geom->texcs().push_back(object.vts[group.vtis[i]]);
        if(usesNormalIndices)
            geom->normals().push_back(object.vns[group.vnis[i]]);
    }

	// TODO: support other modes here!
	geom->setMode(GL_TRIANGLES);


    PolygonalDrawable * drawable(new PolygonalDrawable(object.name));
    drawable->setGeometry(geom);

    return drawable;
}