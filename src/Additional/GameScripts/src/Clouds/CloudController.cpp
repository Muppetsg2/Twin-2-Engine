#include <Clouds/CloudController.h>
#include <processes/SynchronizedProcess.h>
#include <graphic/Window.h>
#include <graphic/manager/MeshRenderingManager.h>
#include <graphic/manager/ShaderManager.h>
#include <graphic/manager/TextureManager.h>
#include <graphic/InstantiatingMesh.h>
#include <graphic/LightingController.h>
#include <core/Time.h>

#include <Clouds/stb_perlin.h>

using namespace Twin2Engine::Processes;
using namespace Twin2Engine::Graphic;
using namespace Twin2Engine::Manager;
using namespace Twin2Engine::Core;


GLuint create3DNoiseTexture(const std::vector<float>& noiseData, int width, int height, int depth) {
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_3D, textureID);

	glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, width, height, depth, 0, GL_RED, GL_FLOAT, noiseData.data());

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //GL_NEAREST
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //GL_LINEAR

	return textureID;
}


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
	NoiseTexture = TextureManager::GetTexture2D("res/textures/density_noise.png");
	NoiseTexture->SetWrapModeS(TextureWrapMode::MIRRORED_REPEAT);
	NoiseTexture->SetWrapModeT(TextureWrapMode::MIRRORED_REPEAT);

	const int width = 32;
	const int height = 32;
	const int depth = 32;

	//auto noiseData = generate3DPerlinNoise(width, height, depth, 20.0f);
	auto noiseData = generate3DPerlinWorleyNoise(width, height, depth, 10.0f);
	noiseTexture3d = create3DNoiseTexture(noiseData, width, height, depth);


	CloudDepthShader = ShaderManager::GetShaderProgram("origin/CloudDepthShader");
	CloudShader = ShaderManager::GetShaderProgram("origin/CloudShader");


	//glGenFramebuffers(1, &depthmapFBO);
	//
	//glGenTextures(1, &dmap);
	//glActiveTexture(GL_TEXTURE0 + CLOUD_DEPTH_MAP_ID);
	//glBindTexture(GL_TEXTURE_2D, dmap);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, size.x, size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	//
	//glBindFramebuffer(GL_FRAMEBUFFER, depthmapFBO);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, dmap, 0);
	//glDrawBuffer(GL_NONE);
	//glReadBuffer(GL_NONE);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);


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

	glm::ivec2 size = Window::GetInstance()->GetContentSize();

	glGenFramebuffers(1, &depthmapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthmapFBO);
	
	//glGenTextures(1, &dmap);
	//glActiveTexture(GL_TEXTURE0 + CLOUD_DEPTH_MAP_ID + 1);
	//glBindTexture(GL_TEXTURE_2D, dmap);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, size.x, size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, dmap, 0);
	
	// Color Attachment Texture
	glGenTextures(1, &depthmap);
	glActiveTexture(GL_TEXTURE0 + CLOUD_DEPTH_MAP_ID);
	glBindTexture(GL_TEXTURE_2D, depthmap);
	
	//glTexImage2D(GL_TEXTURE_2D, 0, r_res, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, size.x, size.y, 0, GL_RGBA, GL_FLOAT, NULL);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	glGenTextures(1, &lightdepthmap);
	glActiveTexture(GL_TEXTURE0 + CLOUD_DEPTH_MAP_ID + 3);
	glBindTexture(GL_TEXTURE_2D, lightdepthmap);

	//glTexImage2D(GL_TEXTURE_2D, 0, r_res, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, LightingController::SHADOW_WIDTH, LightingController::SHADOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depthmap, 0);
	glReadBuffer(GL_NONE);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glBindTexture(GL_TEXTURE_2D, 0);



	CloudShader->Use();
	//CloudShader->SetInt("lightFrontDepthMap", 0);
	CloudShader->SetInt("viewerBackDepthMap", CLOUD_DEPTH_MAP_ID);

	glActiveTexture(GL_TEXTURE0 + CLOUD_DEPTH_MAP_ID + 1);
	glBindTexture(GL_TEXTURE_2D, NoiseTexture->GetId());
	CloudShader->SetInt("noiseTexture", CLOUD_DEPTH_MAP_ID + 1);
	glActiveTexture(GL_TEXTURE0 + CLOUD_DEPTH_MAP_ID + 2);
	glBindTexture(GL_TEXTURE_3D, noiseTexture3d);
	CloudShader->SetInt("noiseTexture3d", CLOUD_DEPTH_MAP_ID + 2);
	glActiveTexture(GL_TEXTURE0 + CLOUD_DEPTH_MAP_ID + 3);
	glBindTexture(GL_TEXTURE_2D, lightdepthmap);
	CloudShader->SetInt("lightFrontDepthMap", CLOUD_DEPTH_MAP_ID + 3);

	SynchronizedProcess* proc = new CloudControllerSyncProc();
	proc->Initialize();
	proc->DontDestroyOnLoad = true;
	//SynchronizedProcess* proc = new SynchronizedProcess([this]() {
	//	this->RenderCloudBackDepthMap();
	//	});
}

CloudController::~CloudController() {
	glDeleteTextures(1, &lightdepthmap);
	glDeleteTextures(1, &noiseTexture3d);
	//glDeleteTextures(1, &dmap);
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


	glBindFramebuffer(GL_FRAMEBUFFER, depthmapFBO);

	//MeshRenderingManager::RenderCloudDepthMap(depthQueue);
	//glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glCullFace(GL_FRONT);
	CloudDepthShader->Use();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depthmap, 0);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	MeshRenderingManager::RenderCloudDepthMap(depthQueue);
	glCullFace(GL_BACK);


	ShaderManager::CloudLightDepthShader->Use();
	glViewport(0, 0, LightingController::SHADOW_WIDTH, LightingController::SHADOW_HEIGHT);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lightdepthmap, 0);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	MeshRenderingManager::RenderCloudDepthMap(depthQueue);
	glm::ivec2 wSize = Twin2Engine::Graphic::Window::GetInstance()->GetContentSize();
	glViewport(0, 0, wSize.x, wSize.y);


	CloudShader->Use();
	CloudShader->SetFloat("time", Time::GetTime());

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CloudController::RenderClouds() {
	//glEnable(GL_DEPTH_TEST);
	CloudShader->Use();
	CloudShader->SetFloat("time", Time::GetTime());
	MeshRenderingManager::RenderCloudDepthMap(depthQueue);
}