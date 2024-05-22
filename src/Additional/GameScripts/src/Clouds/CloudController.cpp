#include <Clouds/CloudController.h>
#include <processes/SynchronizedProcess.h>
#include <graphic/Window.h>
#include <graphic/manager/ShaderManager.h>

using namespace Twin2Engine::Processes;
using namespace Twin2Engine::Graphic;
using namespace Twin2Engine::Manager;

class CloudControllerSyncProc : public SynchronizedProcess {
	public:
		CloudControllerSyncProc() : SynchronizedProcess() {}

		~CloudControllerSyncProc() {
			CloudController::DeleteInstance();
		}

		void Initialize() override {
			SynchronizedProcess::Initialize();
		}

		void Update() {
			CloudController::Instance()->RenderCloudBackDepthMap();
		}
};

CloudController* CloudController::instance = nullptr;
const int CloudController::CLOUD_DEPTH_MAP_ID = 20;

CloudController::CloudController() {
	glm::ivec2 size = Window::GetInstance()->GetContentSize();
	CloudDepthShader = ShaderManager::GetShaderProgram("origin/CloudDepthShader");
	CloudShader = ShaderManager::GetShaderProgram("origin/CloudShader");
	CloudShader->SetInt("viewerBackDepthMap", CLOUD_DEPTH_MAP_ID);

	//glGenFramebuffers(1, &depthmapFBO);
	//
	//glGenTextures(1, &depthmap);
	//glActiveTexture(GL_TEXTURE0 + CLOUD_DEPTH_MAP_ID);
	//glBindTexture(GL_TEXTURE_2D, depthmap);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, size.x, size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	//
	//glBindFramebuffer(GL_FRAMEBUFFER, depthmapFBO);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthmap, 0);
	//glDrawBuffer(GL_NONE);
	//glReadBuffer(GL_NONE);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);


	glGenFramebuffers(1, &depthmapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthmapFBO);

	// Color Attachment Texture
	glGenTextures(1, &depthmap);
	glBindTexture(GL_TEXTURE_2D, depthmap);

	//glTexImage2D(GL_TEXTURE_2D, 0, r_res, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depthmap, 0);
	glReadBuffer(GL_NONE);


	glActiveTexture(GL_TEXTURE0 + CLOUD_DEPTH_MAP_ID);
	glBindTexture(GL_TEXTURE_2D, depthmap);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);


	SynchronizedProcess* proc = new CloudControllerSyncProc();
	//SynchronizedProcess* proc = new SynchronizedProcess([this]() {
	//	this->RenderCloudBackDepthMap();
	//	});
}

CloudController::~CloudController() {
	glDeleteTextures(1, &depthmap);
	glDeleteFramebuffers(1, &depthmapFBO);
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


	glActiveTexture(GL_TEXTURE0 + CLOUD_DEPTH_MAP_ID);
	glBindTexture(GL_TEXTURE_2D, depthmap);

	glCullFace(GL_BACK);
}