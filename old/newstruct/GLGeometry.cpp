#include "GLGeometry.h"
#include "opengl_abstraction.h"

void GLGeometry::Create(as_Geometry* geo, as_InputLayout* inputlayout)
{
	m_RawGeometryData = geo;
	CreateVAO(gldata.vao);
	BindVAO(gldata.vao);
	auto AttributeList = inputlayout->getElements();
	for (auto& att : AttributeList){
		int size;
		string type;
		inputlayout->getAttributeDataType(att, type, size);
		vector<float>* data = NULL;
		if (att->semantic == "POSITION"){ data = &geo->position; }
		if (att->semantic == "NORMAL"){ data = &geo->normal; }
		if (att->semantic == "TEXCOORD"){ data = &geo->texcoord; }
		if (att->semantic == "BONEWEIGHT"){ data = &geo->boneWeight; }
		if (data != NULL&&data->size() > 0){
			CreateVertexBuffer(*data, gldata.vbo[att->index]);
			AttachVBOtoVAO_float(gldata.vbo[att->index], gldata.vao, att->index, size);
		}

		if (att->semantic == "BONEID"){
			if (geo->has_skeleton&&geo->boneID.size() > 0){
				CreateVertexBuffer(geo->boneID, gldata.vbo[4]);
				AttachVBOtoVAO_int(gldata.vbo[4], gldata.vao, 4, 4);
			}
		}
	}
	if (geo->indices.size() > 0){
		CreateIndexBuffer(geo->indices, gldata.elementbuffer);
		gldata.indices_size = geo->indices.size();
	}
	m_bResourceValid = true;
}

void GLGeometry::Draw()
{
	if (m_bResourceValid){
		DrawElements(gldata.vao, gldata.elementbuffer, gldata.indices_size);
	}
}

Geometry* GLGeometryFactory::create() {
	return new GLGeometry;
}

Geometry* GLGeometryFactory::create(as_Geometry* geo, as_InputLayout* inputlayout) {
	Geometry* g = new GLGeometry;
	g->Create(geo, inputlayout);
	return g;
}