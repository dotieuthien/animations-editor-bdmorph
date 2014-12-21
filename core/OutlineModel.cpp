#include "OutlineModel.h"
#include <QtOpenGL>
#include <fstream>
#include <stdio.h>

extern "C" {
#include "triangle.h"
}

/******************************************************************************************************************************/

void eatTokens(std::ifstream &ifile, int count)
{
	std::string dummy;
	for (int i =0 ; i < count ;i++)
		ifile >> dummy;
}

OutlineModel::OutlineModel() : selectedVertex(-1)
{
	width = 1;
	height = 1;
	center.x = 0.5;
	center.y = 0.5;
	setScale(1,1);
}

/******************************************************************************************************************************/

OutlineModel::OutlineModel(MeshModel *from): selectedVertex(-1)
{
	width = 1;
	height = 1;
	center.x = 0.5;
	center.y = 0.5;
	setScale(1,1);

	std::map<Vertex,Vertex> theirToOurBoundaryVertices;

	for (auto iter = from->boundaryVertices->begin() ; iter != from->boundaryVertices->end() ; iter++)
	{
		Vertex v = theirToOurBoundaryVertices.size();
		theirToOurBoundaryVertices.insert(std::make_pair(*iter,v));
	}

	for (auto iter = from->faces->begin() ; iter != from->faces->end() ; iter++) {
		Vertex a = iter->a();
		Vertex b = iter->b();
		Vertex c = iter->c();

		if (theirToOurBoundaryVertices.count(a) && theirToOurBoundaryVertices.count(b))
			edges.insert(Edge(theirToOurBoundaryVertices[a], theirToOurBoundaryVertices[b]));

		if (theirToOurBoundaryVertices.count(b) && theirToOurBoundaryVertices.count(c))
			edges.insert(Edge(theirToOurBoundaryVertices[b], theirToOurBoundaryVertices[c]));

		if (theirToOurBoundaryVertices.count(c) && theirToOurBoundaryVertices.count(a))
			edges.insert(Edge(theirToOurBoundaryVertices[c], theirToOurBoundaryVertices[a]));


		vertices.resize(theirToOurBoundaryVertices.size());
		for (auto iter = theirToOurBoundaryVertices.begin() ; iter != theirToOurBoundaryVertices.end() ; iter++) {
			vertices[iter->second] = (*from->texCoords)[iter->first];
		}

		numFaces = 0;
		numVertices = theirToOurBoundaryVertices.size();
	}
}

/******************************************************************************************************************************/
OutlineModel::~OutlineModel()
{}

/******************************************************************************************************************************/
bool OutlineModel::mouseReleaseAction(Point2 pos, bool moved, double radius, bool rightButton)
{
	if (moved) return false;

	//if (pos.x > 1 || pos.y > 1 || pos.x < 0 || pos.y < 0)
	//	return false;

	if (!rightButton)
	{
		/* Left button adds vertexes */
		Vertex newSelectedVertex = getClosestVertex(pos, false, radius);
		bool vertexAdded = false;

		if (newSelectedVertex == -1) {
			newSelectedVertex = addVertex(pos);
			vertexAdded = true;
		}

		if (selectedVertex != newSelectedVertex && selectedVertex != -1)
		{
			edges.insert(Edge(selectedVertex,newSelectedVertex));
			selectedVertex = vertexAdded ? newSelectedVertex : -1;
		} else
			selectedVertex = selectedVertex != -1 ? -1 : newSelectedVertex;
		return true;

	} else {

		Vertex toDelete = getClosestVertex(pos, false, radius);
		if (toDelete == -1)
			return false;
		deleteVertex(toDelete);
		return true;
	}
}
/******************************************************************************************************************************/

bool OutlineModel::moveAction(Point2 pos1, Point2 pos2, double radius)
{

	Vertex oldVertex = getClosestVertex(pos1, false, radius);
	if (oldVertex != -1) {
		vertices[oldVertex] = pos2;
		return true;
	}
	return false;
}

/******************************************************************************************************************************/

void OutlineModel::renderFaces()
{

	glPushAttrib(GL_ENABLE_BIT|GL_CURRENT_BIT|GL_LINE_BIT);
	glEnable(GL_TEXTURE_2D);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	renderInternal();

	glDisable(GL_TEXTURE_2D);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glColor3f(0,0,0);
	glLineWidth(1.5);
	renderInternal();

	glPopAttrib();
}


void OutlineModel::renderWireframe()
{
}

/******************************************************************************************************************************/

void OutlineModel::renderOverlay(double scale)
{
	glPushAttrib(GL_ENABLE_BIT|GL_CURRENT_BIT|GL_LINE_BIT);

	glLineWidth(1.5);
	glColor4f(0,0,0,1);

	glBegin(GL_LINES);
	for (auto iter = edges.begin() ; iter != edges.end() ; iter++) {
		glVertex2f(vertices[iter->v0].x,vertices[iter->v0].y);
		glVertex2f(vertices[iter->v1].x,vertices[iter->v1].y);
	}
	glEnd();

	glColor3f(1,1,0);
	for (Vertex v = 0 ; v < (int)numVertices ; v++)
		renderVertex(v,scale/1.4);

	if (selectedVertex != -1) {
		glColor3f(0,1,0);
		renderVertex(selectedVertex,scale);
	}


	glPopAttrib();
}

/******************************************************************************************************************************/

Vertex OutlineModel::addVertex(Point2 p)
{
	Vertex retval;
	retval = numVertices++;
	vertices.push_back(p);
	return retval;
}
/******************************************************************************************************************************/

void OutlineModel::deleteVertex(Vertex v)
{
	std::set<Edge> editedEdges;

	for (auto iter = edges.begin() ; iter != edges.end();)
	{
		if (iter->v0 == v || iter->v1 == v)
			edges.erase(iter++);
		else 
		{
			Edge e = *iter;

			if (e.v0 < v && e.v1 < v) {
				iter++;
			} else {

				if (e.v0 >= v)
					e.v0--;

				if (e.v1 >= v)
					e.v1--;

				editedEdges.insert(Edge(e.v0,e.v1));
				iter = edges.erase(iter);
			}
		}
	}

	edges.insert(editedEdges.begin(),editedEdges.end());
	vertices.erase(vertices.begin()+v);
	numVertices = vertices.size();

	if (selectedVertex == v)
		selectedVertex = -1;
}
/******************************************************************************************************************************/

void OutlineModel::historySnapshot()
{
	/* TODO*/
}
/******************************************************************************************************************************/

void OutlineModel::historyReset()
{
	edges.clear();
	vertices.clear();
	numVertices = 0;
	/* TODO*/
}
/******************************************************************************************************************************/

bool OutlineModel::historyRedo()
{
	/* TODO*/
	return false;
}
/******************************************************************************************************************************/

bool OutlineModel::historyUndo()
{
	/* TODO*/
	return false;
}

/******************************************************************************************************************************/
void OutlineModel::getVertices(std::set<Vertex> &standaloneVertices, std::set<Vertex> &normalVertices)
{

	for (unsigned int i= 0 ; i < numVertices ;i++)
			standaloneVertices.insert(i);

	for (auto iter = edges.begin() ; iter != edges.end() ; iter++) {
		standaloneVertices.erase(iter->v0);
		standaloneVertices.erase(iter->v1);
	}

	for (unsigned int i = 0 ; i < numVertices ; i++)
		if (standaloneVertices.count(i) == 0)
			normalVertices.insert(i);
}

/******************************************************************************************************************************/
bool OutlineModel::saveToFile(std::string filename)
{
	/* We write here .poly file but only for now, later we will use triangle directly */
	std::ofstream ofile(filename);

	std::set<Vertex> standaloneVertices;
	std::set<Vertex> normalVertexes;
	getVertices(standaloneVertices,normalVertexes);

	/* First section - vertices */
	ofile << normalVertexes.size() << " 2 0 1" << std::endl;
	for (auto iter = normalVertexes.begin() ; iter != normalVertexes.end() ; iter++)
		ofile << *iter << " " << vertices[*iter].x << " " << vertices[*iter].y  << " 1" << std::endl;

	/* Second section - edges */
	ofile << edges.size() << " 1" << std::endl;
	int i = 0;
	for (auto iter = edges.begin() ; iter != edges.end() ; iter++) {
		ofile << i << " " << iter->v0 << " " << iter->v1 <<  " 1" << std::endl;
	}

	/* Third section - holes */
	ofile << standaloneVertices.size() << std::endl;
	i = 0;
	for (auto iter = standaloneVertices.begin() ; iter != standaloneVertices.end() ; iter++)
	{
		ofile << i << " " << vertices[*iter].x << " " << vertices[*iter].y << std::endl;
	}

	return true;
}

/******************************************************************************************************************************/
bool OutlineModel::createMesh(MeshModel *output,int triCount)
{
	struct triangulateio in, out;
	memset(&in,0,sizeof(triangulateio));
	memset(&out,0,sizeof(triangulateio));

	BBOX b = getActualBBox();
	double approxArea = b.width()*b.height();

	double triArea = approxArea/triCount;


	printf("OutlineModel: creating mesh with approximate %d triangles\n",triCount);

	/* Define input points. */
	std::set<Vertex> standaloneVertices;
	std::set<Vertex> normalVertexes;
	getVertices(standaloneVertices,normalVertexes);

	if (numVertices < 3 || edges.size() == 0)
		return false;

	in.numberofpoints = numVertices;
	in.pointlist = (REAL *) malloc(in.numberofpoints * 2 * sizeof(REAL));

	for (int i = 0 ; i < in.numberofpoints ; i++)
	{
	  in.pointlist[2*i] = vertices[i].x;
	  in.pointlist[2*i+1] = vertices[i].y;
	}

	/* Define input segments */
	in.numberofsegments = edges.size();
	in.segmentlist = (int*) malloc(in.numberofsegments*2*sizeof(int));

	int i = 0;
	for (auto iter = edges.begin() ; iter != edges.end() ;++iter,++i)
	{
		in.segmentlist[2*i] = iter->v0;
		in.segmentlist[2*i+1] = iter->v1;
	}


	/* Define input holes */
	in.numberofholes = standaloneVertices.size();
	in.holelist = (REAL*)malloc(in.numberofholes * 2 * sizeof(REAL));

	i = 0;
	for (auto iter = standaloneVertices.begin() ; iter != standaloneVertices.end() ;++iter,++i) {
		in.holelist[2*i]    =  vertices[*iter].x;
		in.holelist[2*i+1]  =  vertices[*iter].y;
	}

	char commandline[100];
	sprintf(commandline, "p -q -a%15.15f -D -j -P -z", triArea);
	triangulate(commandline, &in, &out, NULL);

	output->faces->clear();
	output->vertices.clear();

	for (int i = 0 ; i < out.numberofpoints ; i++)
		output->vertices.push_back(Point2(out.pointlist[2*i],out.pointlist[2*i+1] ));

	for (int i = 0 ; i < out.numberoftriangles ; i++)
		output->faces->push_back(Face(out.trianglelist[i*3],out.trianglelist[i*3+1],out.trianglelist[i*3+2] ));

	trifree((VOID*)out.pointlist);
	trifree((VOID*)out.trianglelist);
	trifree((VOID*)out.pointmarkerlist);

	output->numVertices = out.numberofpoints;
	output->numFaces = out.numberoftriangles;

	output->identityTexCoords();

	for (unsigned int i = 0 ; i < output->numVertices ;i++)
	{
		(*output->texCoords)[i][0] /= scaleX;
		(*output->texCoords)[i][1] /= scaleY;
	}

	return output->updateMeshInfo();
}

/******************************************************************************************************************************/
bool OutlineModel::loadFromFile(const std::string &filename)
{
	std::ifstream ifile(filename);
	if (ifile.bad())
		return false;

	if (!ends_with(filename, "poly"))
		return false;

	int dimision, attribNum, markerNum;
	ifile >> numVertices >> dimision >> attribNum >> markerNum;
	vertices.reserve(numVertices);

	for (unsigned int i = 0 ; i < numVertices ;i++)
	{
		unsigned int v;
		double x, y;
		ifile >> v >> x >> y;
		eatTokens(ifile, attribNum+markerNum);

		if (v>= vertices.size()) {
			vertices.resize(v+1);
		}

		vertices[v] = Point2(x,y);
	}

	unsigned int edgesCount;
	ifile >> edgesCount >> markerNum;

	for (unsigned int i = 0 ; i < edgesCount ;i++) {
		int e;
		int a,b;

		ifile >> e >> a >> b;
		eatTokens(ifile, markerNum);
		edges.insert(Edge(a,b));
	}

	unsigned int holesCount;
	ifile >> holesCount;

	for (unsigned int i = 0 ; i < holesCount ;i++)
	{
		unsigned int v;
		double x, y;
		ifile >> v >> x >> y;

		if (v>= vertices.size())
			vertices.resize(v+1);
		vertices[v] = Point2(x,y);
	}

	numVertices = vertices.size();
	return true;
}


Point2 OutlineModel::adjustAspectRatioMul(Point2 in)
{
	Point2 out = in;
	out.x *= scaleX;
	out.y *= scaleY;
	return out;
}


void OutlineModel::renderInternal()
{
	Point2 point1(0,0);
	Point2 point2(1,1);

	point1 = adjustAspectRatioMul(point1);
	point2 = adjustAspectRatioMul(point2);

	glBegin(GL_QUADS);

	glTexCoord2f(0,0);
	glVertex2f(point1.x,point1.y);

	glTexCoord2f(1,0);
	glVertex2f(point2.x,point1.y);

	glTexCoord2f(1,1);
	glVertex2f(point2.x,point2.y);

	glTexCoord2f(0,1);
	glVertex2f(point1.x,point2.y);
	glEnd();
}
