#include "RenderSystem.h"
#include "GlobalVariablePool.h"
#include "OMesh.h"
#include "PreDefine.h"
#include "GpuProgram.h"
#include "Entity.h"
#include "RenderTarget.h"
#include "Camera.h"
#include "Geometry.h"
#include "EGLUtil.h"
#include "ResourceManager.h"
#include "animation_helper.h"
#include "pass.h"
#include "TextureManager.h"
namespace HW
{


	void RenderSystem::RenderPass(Camera* camera, RenderQueue & renderqueue, as_Pass* pass, RenderTarget * rt)
	{
		assert(rt != NULL);
		assert(pass != NULL);

		
		GlobalVariablePool* gp = GlobalVariablePool::GetSingletonPtr();
		Entity * entity = NULL;
		GpuProgram * program = NULL;
		program = pass->mProgram;
		program->UseProgram();

		rt->bindTarget();

		if (pass->mClearState.mclearcolorbuffer| pass->mClearState.mcleardepthbuffer| pass->mClearState.mclearstencilbuffer){
			// to be fixed
			if (pass->name == "gbuffer pass") {
				const unsigned clear_color_value[4] = { 0 };
				float colorf[4] = { 0,0,0,0 };
				const float clear_depth_value = 1.f;
				glClearBufferuiv(GL_COLOR, 3, clear_color_value);
				glClearBufferfv(GL_COLOR, 0, colorf);
				glClearBufferfv(GL_COLOR, 1, colorf);
				glClearBufferfv(GL_COLOR, 2, colorf);
				glClearBufferfv(GL_DEPTH, 0, &clear_depth_value);
				GL_CHECK();
			}
			else
			{
				Vector4 c = pass->mClearState.mClearColor;
				ClearColor(c.x, c.y, c.z, c.w);
				ClearBuffer(pass->mClearState);
			}
		}

		this->SetViewport(0, 0, rt->getWidth(), rt->getHeight());
		
		//	Logger::WriteLog("Render Queue %d", renderqueue.size());

		this->setBlendState(pass->mBlendState);
		this->setDepthStencilState(pass->mDepthStencilState);
		this->setRasterState(pass->mRasterState);


		//==================================
		//��ʱ����bone matrix
		//vector<Matrix4> temp_matrices;
		//
		//	static float t = 0;
		//	t += 0.3;
		//	if (t > 30) t = 0;
		//	as_Skeleton* ske;
		//	map<int, Matrix4> temp_bone_matrices;
		//	auto& g = GlobalResourceManager::getInstance();
		//	for (auto& skeleton : g.as_skeletonManager.resources){
		//		ske = skeleton.second;
		//	}
		//	for (auto& ani : g.as_skeletonAniManager.resources){
		//		updateSkeleton(ani.second, t, ske, temp_bone_matrices);
		//	}
		//	temp_matrices = vector<Matrix4>(temp_bone_matrices.size());
		//	for (auto& x : temp_bone_matrices){
		//		temp_matrices[x.first] = x.second;
		//		//temp_matrices[x.first] = Matrix4::IDENTITY;
		//	
		//	}
		//==================================
			if (camera != NULL)
				gp->SetCamera(camera);

			program->texture_count = 0;
			program->updateProgramData(program->m_ProgramData);

			for (auto& x : renderqueue) {
				if (x.asMesh == NULL) continue;
				if (x.entity != NULL) {

					gp->SetWorldMatrix(x.entity->getParent()->getWorldMatrix());
				}
					
				//gp->setMatrices(temp_matrices);
				if (x.asMesh != NULL)
					gp->SetMaterial(x.asMesh->material);

				program->UpdateGlobalVariable(pass->mUniformMap);

				auto& currentGeo = x.asMesh->renderable;
				if (currentGeo == NULL) {
					auto g = GlobalResourceManager::getInstance().m_GeometryFactory;
					auto& grm = GlobalResourceManager::getInstance();
					//auto eg = static_cast<GLGeometryFactory*>(g);
					currentGeo = g->create(x.asMesh->geometry, pass->mInputLayout);
				}
				DrawGeometry(currentGeo);
			}



	}
}