#include <Clouds/CloudController.h>
#include <processes/SynchronizedProcess.h>
#include <graphic/Window.h>
#include <graphic/manager/MeshRenderingManager.h>
#include <graphic/manager/ShaderManager.h>
#include <graphic/InstantiatingMesh.h>

using namespace Twin2Engine::Processes;
using namespace Twin2Engine::Graphic;
using namespace Twin2Engine::Manager;

class CloudControllerSyncProc : public SynchronizedProcess {
	public:
		CloudControllerSyncProc() : SynchronizedProcess() {}

		~CloudControllerSyncProc() {
			CloudController::DeleteInstance();
		}

		virtual void Initialize() override {
			SynchronizedProcess::Initialize();
		}

		virtual void Update() {
			CloudController::Instance()->RenderCloudBackDepthMap();
		}
};

CloudController* CloudController::instance = nullptr;
const int CloudController::CLOUD_DEPTH_MAP_ID = 20;

CloudController::CloudController() {
	glm::ivec2 size = Window::GetInstance()->GetContentSize();
	CloudDepthShader = ShaderManager::GetShaderProgram("origin/CloudDepthShader");
	CloudShader = ShaderManager::GetShaderProgram("origin/CloudShader");
	CloudShader->Use();
	//CloudShader->SetInt("lightFrontDepthMap", 0);
	CloudShader->SetInt("viewerBackDepthMap", CLOUD_DEPTH_MAP_ID);

	//glGenFramebuffers(1, &depthmapFBO);
	//glBindFramebuffer(GL_FRAMEBUFFER, depthmapFBO);
	//
	//glGenTextures(1, &depthmap);
	//glActiveTexture(GL_TEXTURE0 + CLOUD_DEPTH_MAP_ID);
	//glBindTexture(GL_TEXTURE_2D, depthmap);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_FLOAT, NULL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	////GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	////glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	//
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depthmap, 0);
	////glDrawBuffer(GL_NONE);
	//glReadBuffer(GL_NONE);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);


	glGenFramebuffers(1, &depthmapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthmapFBO);
	
	glGenTextures(1, &dmap);
	glActiveTexture(GL_TEXTURE0 + CLOUD_DEPTH_MAP_ID);
	glBindTexture(GL_TEXTURE_2D, dmap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, size.x, size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);


	// Color Attachment Texture
	glGenTextures(1, &depthmap);
	glActiveTexture(GL_TEXTURE0 + CLOUD_DEPTH_MAP_ID);
	glBindTexture(GL_TEXTURE_2D, depthmap);
	
	//glTexImage2D(GL_TEXTURE_2D, 0, r_res, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_FLOAT, NULL);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, dmap, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depthmap, 0);
	glReadBuffer(GL_NONE);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glBindTexture(GL_TEXTURE_2D, 0);


	SynchronizedProcess* proc = new CloudControllerSyncProc();
	proc->Initialize();
	//SynchronizedProcess* proc = new SynchronizedProcess([this]() {
	//	this->RenderCloudBackDepthMap();
	//	});
}

CloudController::~CloudController() {
	glDeleteTextures(1, &dmap);
	glDeleteTextures(1, &depthmap);
	glDeleteFramebuffers(1, &depthmapFBO);
}

void CloudController::RegisterCloud(Cloud* cloud) {
	glm::mat4 transformationMat = cloud->GetTransform()->GetTransformMatrix();
	MeshRenderer* mr = cloud->GetGameObject()->GetComponent<MeshRenderer>();
	for (int i = mr->GetMeshCount() - 1; i >= 0; --i) {
		depthQueue[mr->GetMesh(i)].push_back(transformationMat);
	}
}

void CloudController::UnregisterCloud(Cloud* cloud) {
	glm::mat4 transformationMat = cloud->GetTransform()->GetTransformMatrix();
	MeshRenderer* mr = cloud->GetGameObject()->GetComponent<MeshRenderer>();

	for (int i = mr->GetMeshCount() - 1; i >= 0; --i) {
		auto& vec = depthQueue[mr->GetMesh(i)];
		auto itr = std::find(vec.begin(), vec.end(), transformationMat);
		vec.erase(itr);
	}
}

CloudController* CloudController::Instance() {
	if (instance == nullptr) {
		instance = new CloudController();
	}

	return instance;
}

void CloudController::DeleteInstance() {
	if (instance != nullptr) {
		delete instance;
	}
}


void CloudController::RenderCloudBackDepthMap() {
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_FRONT);

	CloudDepthShader->Use();

	glBindFramebuffer(GL_FRAMEBUFFER, depthmapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	MeshRenderingManager::RenderCloudDepthMap(depthQueue);

	glActiveTexture(GL_TEXTURE0 + CLOUD_DEPTH_MAP_ID);
	glBindTexture(GL_TEXTURE_2D, depthmap);

	glCullFace(GL_BACK);
}