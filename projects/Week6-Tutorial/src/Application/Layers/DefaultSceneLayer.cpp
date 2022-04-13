#include "DefaultSceneLayer.h"

// GLM math library
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include <GLM/gtc/random.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <GLM/gtx/common.hpp> // for fmod (floating modulus)

#include <filesystem>

// Graphics
#include "Graphics/Buffers/IndexBuffer.h"
#include "Graphics/Buffers/VertexBuffer.h"
#include "Graphics/VertexArrayObject.h"
#include "Graphics/ShaderProgram.h"
#include "Graphics/Textures/Texture2D.h"
#include "Graphics/Textures/TextureCube.h"
#include "Graphics/Textures/Texture2DArray.h"
#include "Graphics/VertexTypes.h"
#include "Graphics/Font.h"
#include "Graphics/GuiBatcher.h"
#include "Graphics/Framebuffer.h"

// Utilities
#include "Utils/MeshBuilder.h"
#include "Utils/MeshFactory.h"
#include "Utils/ObjLoader.h"
#include "Utils/ImGuiHelper.h"
#include "Utils/ResourceManager/ResourceManager.h"
#include "Utils/FileHelpers.h"
#include "Utils/JsonGlmHelpers.h"
#include "Utils/StringUtils.h"
#include "Utils/GlmDefines.h"

// Gameplay
#include "Gameplay/Material.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"
#include "Gameplay/Components/Light.h"

// Components
#include "Gameplay/Components/IComponent.h"
#include "Gameplay/Components/Camera.h"
#include "Gameplay/Components/RotatingBehaviour.h"
#include "Gameplay/Components/JumpBehaviour.h"
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Components/MaterialSwapBehaviour.h"
#include "Gameplay/Components/TriggerVolumeEnterBehaviour.h"
#include "Gameplay/Components/SimpleCameraControl.h"

// Physics
#include "Gameplay/Physics/RigidBody.h"
#include "Gameplay/Physics/Colliders/BoxCollider.h"
#include "Gameplay/Physics/Colliders/PlaneCollider.h"
#include "Gameplay/Physics/Colliders/SphereCollider.h"
#include "Gameplay/Physics/Colliders/ConvexMeshCollider.h"
#include "Gameplay/Physics/Colliders/CylinderCollider.h"
#include "Gameplay/Physics/TriggerVolume.h"
#include "Graphics/DebugDraw.h"

// GUI
#include "Gameplay/Components/GUI/RectTransform.h"
#include "Gameplay/Components/GUI/GuiPanel.h"
#include "Gameplay/Components/GUI/GuiText.h"
#include "Gameplay/InputEngine.h"

#include "Application/Application.h"
#include "Gameplay/Components/ParticleSystem.h"
#include "Graphics/Textures/Texture3D.h"
#include "Graphics/Textures/Texture1D.h"
#include "Application/Layers/ImGuiDebugLayer.h"
#include "Application/Windows/DebugWindow.h"
#include "Gameplay/Components/ShadowCamera.h"

DefaultSceneLayer::DefaultSceneLayer() :
	ApplicationLayer()
{
	Name = "Default Scene";
	Overrides = AppLayerFunctions::OnAppLoad|AppLayerFunctions::OnUpdate;
	materials = std::vector<Gameplay::Material::Sptr>();
}

DefaultSceneLayer::~DefaultSceneLayer() = default;

void DefaultSceneLayer::OnAppLoad(const nlohmann::json& config) {
	_CreateScene();
}
//Variables
void updateMaterial(Gameplay::Material::Sptr material);
void toggleAmbient(bool isOn);
void toggleDiffuse(bool isOn);
void toggleSpecular(bool isOn);
void toggleCustom(bool isOn);
void toggleEmissive(bool isOn);
void toggleTexture(bool isOn);

void DefaultSceneLayer::OnUpdate()
{
	if (InputEngine::IsKeyDown(GLFW_KEY_1))
	{
		toggleAmbient(false);
		toggleDiffuse(false);
		toggleSpecular(false);
		toggleCustom(false);
		toggleEmissive(false);
		for (int i = 0; i < materials.size(); i++)
		{
			updateMaterial(materials[i]);
		}
	}
	if (InputEngine::IsKeyDown(GLFW_KEY_2))
	{
		toggleAmbient(true);
		toggleDiffuse(false);
		toggleSpecular(false);
		toggleCustom(false);
		toggleEmissive(false);
		for (int i = 0; i < materials.size(); i++)
		{
			updateMaterial(materials[i]);
		}
	}
	if (InputEngine::IsKeyDown(GLFW_KEY_3))
	{
		toggleAmbient(false);
		toggleDiffuse(false);
		toggleSpecular(true);
		toggleCustom(false);
		toggleEmissive(false);
		for (int i = 0; i < materials.size(); i++)
		{
			updateMaterial(materials[i]);
		}
	}
	if (InputEngine::IsKeyDown(GLFW_KEY_4))
	{
		toggleAmbient(true);
		toggleDiffuse(false);
		toggleSpecular(true);
		toggleCustom(false);
		toggleEmissive(false);
		for (int i = 0; i < materials.size(); i++)
		{
			updateMaterial(materials[i]);
		}
	}
	if (InputEngine::IsKeyDown(GLFW_KEY_5))
	{
		toggleAmbient(true);
		toggleDiffuse(false);
		toggleSpecular(true);
		toggleCustom(false);
		toggleEmissive(true);
		for (int i = 0; i < materials.size(); i++)
		{
			updateMaterial(materials[i]);
		}
	}
	if (InputEngine::IsKeyDown(GLFW_KEY_6))
	{
		toggleAmbient(true);
		toggleDiffuse(true);
		toggleSpecular(true);
		toggleCustom(false);
		toggleEmissive(true);
		for (int i = 0; i < materials.size(); i++)
		{
			updateMaterial(materials[i]);
		}
	}
	if (InputEngine::IsKeyDown(GLFW_KEY_7))
	{
		toggleAmbient(true);
		toggleDiffuse(true);
		toggleSpecular(true);
		toggleCustom(true);
		toggleEmissive(true);
		for (int i = 0; i < materials.size(); i++)
		{
			updateMaterial(materials[i]);
		}
	}
	if (InputEngine::IsKeyDown(GLFW_KEY_T))
	{
		toggleAmbient(true);
		toggleDiffuse(true);
		toggleSpecular(true);
		toggleCustom(false);
		toggleEmissive(false);
		toggleTexture(false);
		for (int i = 0; i < materials.size(); i++)
		{
			updateMaterial(materials[i]);
		}
	}
	if (InputEngine::IsKeyDown(GLFW_KEY_Y))
	{
		toggleAmbient(true);
		toggleDiffuse(true);
		toggleSpecular(true);
		toggleCustom(false);
		toggleEmissive(true);
		toggleTexture(true);
		for (int i = 0; i < materials.size(); i++)
		{
			updateMaterial(materials[i]);
		}
	}
	if (InputEngine::IsKeyDown(GLFW_KEY_R))
	{

	}
}


glm::vec3 swordRot = glm::vec3(90.0f, 0.0f, -180.0f);
//Toggle Shader Effects
bool ambientIsOn = true;
bool diffuseIsOn = true;
bool specularIsOn = true;
bool customIsOn = false;
bool emissiveIsOn = true;
bool textureIsOn = true;

void toggleAmbient(bool isOn)
{
	ambientIsOn = isOn;
}
void toggleDiffuse(bool isOn)
{
	diffuseIsOn = isOn;
}
void toggleSpecular(bool isOn)
{
	specularIsOn = isOn;
}
void toggleCustom(bool isOn)
{
	customIsOn = isOn;
}

void toggleEmissive(bool isOn)
{
	emissiveIsOn = isOn;
}

void toggleTexture(bool isOn)
{
	textureIsOn = isOn;
}



void updateMaterial(Gameplay::Material::Sptr material)
{
	material->Set("u_ALight.isOn", ambientIsOn);
	material->Set("u_DLight.isOn", diffuseIsOn);
	material->Set("u_SLight.isOn", specularIsOn);
	material->Set("u_Custom.isOn", customIsOn);
	material->Set("emissiveIsOn", emissiveIsOn);
	material->Set("texIsOn", textureIsOn);
}
void updateLUT(Gameplay::Scene::Sptr scene)
{
	//scene->SetColorLUT();
}
void DefaultSceneLayer::_CreateScene()
{
	using namespace Gameplay;
	using namespace Gameplay::Physics;

	Application& app = Application::Get();

	bool loadScene = false;
	// For now we can use a toggle to generate our scene vs load from file
	if (loadScene && std::filesystem::exists("scene.json")) {
		app.LoadScene("scene.json");
	} else {
		 
		// Basic gbuffer generation with no vertex manipulation
		ShaderProgram::Sptr deferredForward = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/deferred_forward.glsl" }
		});
		deferredForward->SetDebugName("Deferred - GBuffer Generation");  
		//My Shaders----------------------------------------------------------------------------------------------------------------
		// Basic gbuffer generation with no vertex manipulation
		ShaderProgram::Sptr StandardShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/Standard.glsl" }
		});

		StandardShader->SetDebugName("Standard - GBuffer Generation");

		ShaderProgram::Sptr TestShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/Test.glsl" }
		});
		TestShader->SetDebugName("Test - GBuffer Generation");

		ShaderProgram::Sptr PipeShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/basic.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/PipeShader.glsl" }
		});
		PipeShader->SetDebugName("Pipe - GBuffer Generation");
		//----------------------------------------------------------------------------------------------------------------------------
		// Our foliage shader which manipulates the vertices of the mesh
		ShaderProgram::Sptr foliageShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/foliage.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/deferred_forward.glsl" }
		});  
		foliageShader->SetDebugName("Foliage");   

		// This shader handles our multitexturing example
		ShaderProgram::Sptr multiTextureShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/vert_multitextured.glsl" },  
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/frag_multitextured.glsl" }
		});
		multiTextureShader->SetDebugName("Multitexturing"); 

		// This shader handles our displacement mapping example
		ShaderProgram::Sptr displacementShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/displacement_mapping.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/deferred_forward.glsl" }
		});
		displacementShader->SetDebugName("Displacement Mapping");

		// This shader handles our cel shading example
		ShaderProgram::Sptr celShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/displacement_mapping.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/cel_shader.glsl" }
		});
		celShader->SetDebugName("Cel Shader");


		// Load in the meshes
		MeshResource::Sptr worldMesh = ResourceManager::CreateAsset<MeshResource>("World.obj");
		MeshResource::Sptr swordMesh = ResourceManager::CreateAsset<MeshResource>("Sword.obj");
		MeshResource::Sptr ballMesh = ResourceManager::CreateAsset<MeshResource>("SliceBall.obj");
		MeshResource::Sptr pipeMesh = ResourceManager::CreateAsset<MeshResource>("Pipe.obj");		
		// Load in some textures
		Texture2D::Sptr    boxTexture   = ResourceManager::CreateAsset<Texture2D>("textures/box-diffuse.png");
		Texture2D::Sptr    boxSpec      = ResourceManager::CreateAsset<Texture2D>("textures/box-specular.png");
		Texture2D::Sptr    leafTex      = ResourceManager::CreateAsset<Texture2D>("textures/leaves.png");
		Texture2D::Sptr worldTex = ResourceManager::CreateAsset<Texture2D>("textures/WorldTexture.png");
		Texture2D::Sptr swordTex = ResourceManager::CreateAsset<Texture2D>("textures/SwordTexture_V2.png");
		Texture2D::Sptr ballTex = ResourceManager::CreateAsset<Texture2D>("textures/BallTexture.png");
		Texture2D::Sptr pipeTex = ResourceManager::CreateAsset<Texture2D>("textures/PipeTexture.png");

		//Emissive Maps
		Texture2D::Sptr worldTex_EMap = ResourceManager::CreateAsset<Texture2D>("textures/WorldTexture_EMap.png");
		Texture2D::Sptr swordTex_EMap = ResourceManager::CreateAsset<Texture2D>("textures/SwordTexture_EMap.png");
		Texture2D::Sptr ballTex_EMap = ResourceManager::CreateAsset<Texture2D>("textures/BallTexture_EMap.png");
		Texture2D::Sptr pipeTex_EMap = ResourceManager::CreateAsset<Texture2D>("textures/PipeTexture_EMap.png");

		leafTex->SetMinFilter(MinFilter::Nearest);
		leafTex->SetMagFilter(MagFilter::Nearest);

#pragma region Basic Texture Creation
		
			Texture2DDescription singlePixelDescriptor;
			singlePixelDescriptor.Width = singlePixelDescriptor.Height = 1;
			singlePixelDescriptor.Format = InternalFormat::RGB8;

			float normalMapDefaultData[3] = { 0.5f, 0.5f, 1.0f };
			Texture2D::Sptr normalMapDefault = ResourceManager::CreateAsset<Texture2D>(singlePixelDescriptor);
			normalMapDefault->LoadData(1, 1, PixelFormat::RGB, PixelType::Float, normalMapDefaultData);

			float solidBlack[3] = { 0.5f, 0.5f, 0.5f };
			Texture2D::Sptr solidBlackTex = ResourceManager::CreateAsset<Texture2D>(singlePixelDescriptor);
			solidBlackTex->LoadData(1, 1, PixelFormat::RGB, PixelType::Float, solidBlack);

			float solidGrey[3] = { 0.0f, 0.0f, 0.0f };
			Texture2D::Sptr solidGreyTex = ResourceManager::CreateAsset<Texture2D>(singlePixelDescriptor);
			solidGreyTex->LoadData(1, 1, PixelFormat::RGB, PixelType::Float, solidGrey);

			float solidWhite[3] = { 1.0f, 1.0f, 1.0f };
			Texture2D::Sptr solidWhiteTex = ResourceManager::CreateAsset<Texture2D>(singlePixelDescriptor);
			solidWhiteTex->LoadData(1, 1, PixelFormat::RGB, PixelType::Float, solidWhite);
#pragma endregion 

		// Loading in a 1D LUT
		Texture1D::Sptr toonLut = ResourceManager::CreateAsset<Texture1D>("luts/toon-1D.png"); 
		toonLut->SetWrap(WrapMode::ClampToEdge);

		// Here we'll load in the cubemap, as well as a special shader to handle drawing the skybox
		TextureCube::Sptr SkyCubemap = ResourceManager::CreateAsset<TextureCube>("cubemaps/CityScape/CityScape.png");
		ShaderProgram::Sptr      skyboxShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/skybox_vert.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/skybox_frag.glsl" } 
		});
		  
		// Create an empty scene
		Scene::Sptr scene = std::make_shared<Scene>();  

		//Skybox in Maitnence
		// Setting up our enviroment map
		scene->SetSkyboxTexture(SkyCubemap);
		scene->SetSkyboxShader(skyboxShader);
		// Since the skybox I used was for Y-up, we need to rotate it 90 deg around the X-axis to convert it to z-up 
		scene->SetSkyboxRotation(glm::rotate(MAT4_IDENTITY, glm::half_pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f)));
		
		// Create our materials
		// This will be our box material, with no environment reflections
		Material::Sptr worldMat = ResourceManager::CreateAsset<Material>(StandardShader);
		{
			worldMat->Name = "WorldMaterial";
			worldMat->Set("u_ALight.color", glm::vec3(1.0f, 1.0f, 1.0f));
			worldMat->Set("u_ALight.isOn", ambientIsOn);
			worldMat->Set("u_ALight.ambientStrength", 1.0f);
			worldMat->Set("u_DLight.color", glm::vec3(1.0f, 1.0f, 1.0f));
			worldMat->Set("u_DLight.direction", glm::vec3(0.0f, -1.0f, -1.0f));
			worldMat->Set("u_DLight.factor", 1.0f);
			worldMat->Set("u_DLight.isOn", diffuseIsOn);
			worldMat->Set("u_SLight.specularStrength", 0.0f);
			worldMat->Set("u_SLight.isOn", specularIsOn);
			worldMat->Set("u_Custom.rimColor", glm::vec3(0.3f, 3.2f, 6.5f));
			worldMat->Set("u_Custom.rim", 1.0f);
			worldMat->Set("u_Custom.isOn", false);
			worldMat->Set("toggleColorCorrect",false);
			worldMat->Set("emissiveMap", worldTex_EMap);
			worldMat->Set("emissiveColor", glm::vec3(0.0f, 1.0f, 0.0f));
			worldMat->Set("emissiveIntensity", 1.0f);
			worldMat->Set("emissiveIsOn", true);

			worldMat->Set("texColor", worldTex);
			worldMat->Set("texIsOn", textureIsOn);
		}
		materials.push_back(worldMat);
		Material::Sptr swordMat = ResourceManager::CreateAsset<Material>(StandardShader);
		{
			swordMat->Name = "SwordMaterial";
			swordMat->Set("u_ALight.color", glm::vec3(1.0f, 1.0f, 1.0f));
			swordMat->Set("u_ALight.isOn", ambientIsOn);
			swordMat->Set("u_ALight.ambientStrength", 1.0f);
			swordMat->Set("u_DLight.color", glm::vec3(1.0f, 1.0f, 1.0f));
			swordMat->Set("u_DLight.direction", glm::vec3(0.0f, 0.0f, -1.0f));
			swordMat->Set("u_DLight.factor", 1.0f);
			swordMat->Set("u_DLight.isOn", diffuseIsOn);
			swordMat->Set("u_SLight.specularStrength", 1.0f);
			swordMat->Set("u_SLight.isOn", specularIsOn);
			swordMat->Set("u_Custom.rimColor", glm::vec3(1.0f, 1.0f, 1.0f));
			swordMat->Set("u_Custom.rim", 1.0f);
			swordMat->Set("u_Custom.isOn", false);
			swordMat->Set("toggleColorCorrect", false);
			swordMat->Set("emissiveMap", swordTex_EMap);
			swordMat->Set("emissiveColor", glm::vec3(0.0f, 1.0f, 1.0f));
			swordMat->Set("emissiveIntensity", 1.0f);
			swordMat->Set("emissiveIsOn", true);

			swordMat->Set("texColor", swordTex);
			swordMat->Set("texIsOn", textureIsOn);
		}
		materials.push_back(swordMat);
		
		Material::Sptr ballMat = ResourceManager::CreateAsset<Material>(StandardShader);
		{
			ballMat->Name = "BallMaterial";
			ballMat->Set("u_ALight.color", glm::vec3(1.0f, 1.0f, 1.0f));
			ballMat->Set("u_ALight.isOn", ambientIsOn);
			ballMat->Set("u_ALight.ambientStrength", 1.0f);
			ballMat->Set("u_DLight.color", glm::vec3(1.0f, 1.0f, 1.0f));
			ballMat->Set("u_DLight.direction", glm::vec3(0.0f, 0.0f, -1.0f));
			ballMat->Set("u_DLight.factor", 1.0f);
			ballMat->Set("u_DLight.isOn", diffuseIsOn);
			ballMat->Set("u_SLight.specularStrength", 5.0f);
			ballMat->Set("u_SLight.isOn", specularIsOn);
			ballMat->Set("u_Custom.rimColor", glm::vec3(1.1f, 1.7f, 3.5f));
			ballMat->Set("u_Custom.rim", 1.0f);
			ballMat->Set("u_Custom.isOn", false);
			ballMat->Set("toggleColorCorrect", false);
			ballMat->Set("emissiveMap", ballTex_EMap);
			ballMat->Set("emissiveColor", glm::vec3(1.0f,0.0f,0.0f));
			ballMat->Set("emissiveIntensity", 1.0f);
			ballMat->Set("emissiveIsOn", true);

			ballMat->Set("texColor", ballTex);
			ballMat->Set("texIsOn", textureIsOn);

		}
		materials.push_back(ballMat);

		Material::Sptr testMat = ResourceManager::CreateAsset<Material>(TestShader);
		{
			testMat->Name = "TestMaterial";
			testMat->Set("rimColor", glm::vec3(1.0f, 1.0f, 1.0f));
			testMat->Set("rim", 1.0f);
			testMat->Set("texColor", ballTex);
		}
		Material::Sptr pipeMat = ResourceManager::CreateAsset<Material>(StandardShader);
		{
			pipeMat->Name = "PipeMaterial";
			pipeMat->Set("u_ALight.color", glm::vec3(1.0f, 1.0f, 1.0f));
			pipeMat->Set("u_ALight.isOn", ambientIsOn);
			pipeMat->Set("u_ALight.ambientStrength", 1.0f);
			pipeMat->Set("u_DLight.color", glm::vec3(1.0f, 1.0f, 1.0f));
			pipeMat->Set("u_DLight.direction", glm::vec3(0.0f, 0.0f, -1.0f));
			pipeMat->Set("u_DLight.factor", 1.0f);
			pipeMat->Set("u_DLight.isOn", diffuseIsOn);
			pipeMat->Set("u_SLight.specularStrength", 2.0f);
			pipeMat->Set("u_SLight.isOn", specularIsOn);
			pipeMat->Set("u_Custom.rimColor", glm::vec3(1.5f, 1.1f, 0.8f));
			pipeMat->Set("u_Custom.rim", 0.7f);
			pipeMat->Set("u_Custom.isOn", false);
			pipeMat->Set("toggleColorCorrect", false);
			pipeMat->Set("emissiveMap", pipeTex_EMap);
			pipeMat->Set("emissiveColor", glm::vec3(0.77f, 0.0f, 1.0f));
			pipeMat->Set("emissiveIntensity", 1.0f);
			pipeMat->Set("emissiveIsOn", true);

			pipeMat->Set("texColor", pipeTex);
			pipeMat->Set("texIsOn", textureIsOn);
		}
		materials.push_back(pipeMat);

		// Create some lights for our scene
		GameObject::Sptr lightParent = scene->CreateGameObject("Lights");

		for (int ix = 0; ix < 1; ix++) {
			//light->SetPostion(glm::vec3(glm::diskRand(25.0f), 1.0f));
			GameObject::Sptr light = scene->CreateGameObject("Light");
			//Old Pos: light->SetPostion(glm::vec3(-17.1, 0.0, -3.8));
			light->SetPostion(glm::vec3(30.0, 0.0, 30.0));
			//lightParent->AddChild(light);

			Light::Sptr lightComponent = light->Add<Light>();
			lightComponent->SetColor(glm::vec3(1.0f,1.0f,1.0f));
			lightComponent->SetRadius(20.0f);
			lightComponent->SetIntensity(18.0f);
		}

		// Set up the scene's camera
		GameObject::Sptr camera = scene->MainCamera->GetGameObject()->SelfRef();
		{
			camera->SetPostion({ -7.275, 0.267, 5.262 });
			camera->LookAt(glm::vec3(0.0f,0.0f,5.0f));

			camera->Add<SimpleCameraControl>();

			// This is now handled by scene itself!
			//Camera::Sptr cam = camera->Add<Camera>();
			// Make sure that the camera is set as the scene's main camera!
			//scene->MainCamera = cam;
		}

		GameObject::Sptr world = scene->CreateGameObject("World");
		{
			world->SetPostion(glm::vec3(0.00, 0.0f, 0.0f));
			world->SetRotation(glm::vec3(90.0f, 0.0f, 180.0f));
			world->SetScale(glm::vec3(0.8f, 0.8f, 0.8f));
			RenderComponent::Sptr renderer = world->Add<RenderComponent>();
			renderer->SetMesh(worldMesh);
			renderer->SetMaterial(worldMat);
		}
		
		GameObject::Sptr sword = scene->CreateGameObject("Sword");
		{
			sword->SetPostion(glm::vec3(-3.05f, -2.75f, 2.83f));
			sword->SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
			RenderComponent::Sptr renderer = sword->Add<RenderComponent>();
			renderer->SetMesh(swordMesh);
			renderer->SetMaterial(swordMat);

			//Particles
			float SpawnRate = 1.0f / 1.0f;
			GameObject::Sptr particles = scene->CreateGameObject("Particles");
			sword->AddChild(particles);
			particles->SetPostion(glm::vec3(0.0f, 0.0f, 4.0f));

			ParticleSystem::Sptr particleManager = particles->Add<ParticleSystem>();

			particleManager->_gravity = glm::vec3(0.0f,0.0f,-9.81);

			ParticleSystem::ParticleData emitter;
			emitter.Type = ParticleType::BoxEmitter;
			//emitter.TexID = 0;
			emitter.Position = glm::vec3(0.0f, 0.0f, 0.0f);
			emitter.Color = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
			emitter.Lifetime = 1.0f;
			emitter.Velocity = glm::vec3(0.0f, 0.0f, 0.0f);

			emitter.BoxEmitterData.Timer = SpawnRate;
			emitter.BoxEmitterData.HalfExtents = glm::vec3(0.3f, 0.1f, 3.0f);
			emitter.BoxEmitterData.SizeRange = glm::vec2(0.2f, 0.2f);
			emitter.BoxEmitterData.LifeRange = glm::vec2(5.0f, 5.0f);

			particleManager->AddEmitter(emitter);
		}
		GameObject::Sptr ball = scene->CreateGameObject("Ball");
		{
			ball->SetPostion(glm::vec3(0.0f, 0.0f, 5.0f));
			ball->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
			RenderComponent::Sptr renderer = ball->Add<RenderComponent>();
			renderer->SetMesh(ballMesh);
			renderer->SetMaterial(ballMat);

			//Particles
			float SpawnRate = 1.0f / 0.5f;
			GameObject::Sptr particles = scene->CreateGameObject("Particles");
			ball->AddChild(particles);
			particles->SetPostion(glm::vec3(0.0f, 0.0f, 0.0f));

			ParticleSystem::Sptr particleManager = particles->Add<ParticleSystem>();

			particleManager->_gravity = glm::vec3(0.0f,0.0f,-9.81f);

			ParticleSystem::ParticleData emitter;
			emitter.Type = ParticleType::StreamEmitter;
			//emitter.TexID = 0;
			emitter.Position = glm::vec3(0.0f, 0.0f, -1.0f);
			emitter.Color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
			emitter.Lifetime = 1.0f;
			emitter.Velocity = glm::vec3(0.0f, 0.0f, 0.0f);

			emitter.ConeEmitterData.Angle = glm::radians(30.0f);
			emitter.ConeEmitterData.Timer = SpawnRate;
			emitter.ConeEmitterData.SizeRange = glm::vec2(0.15f, 0.2f);
			emitter.ConeEmitterData.LifeRange = glm::vec2(5.0f, 5.0f);

			particleManager->AddEmitter(emitter);
		}
		GameObject::Sptr pipe = scene->CreateGameObject("Pipe");
		{
			pipe->SetPostion(glm::vec3(0.0f, 0.0f, 1.18f));
			pipe->SetRotation(glm::vec3(90.0f, 0.0f, -92.0f));
			RenderComponent::Sptr renderer = pipe->Add<RenderComponent>();
			renderer->SetMesh(pipeMesh);
			renderer->SetMaterial(pipeMat);
			//Particles
			float SpawnRate = 1.0f / 1000.0f;
			GameObject::Sptr particles = scene->CreateGameObject("Particles");
			pipe->AddChild(particles);
			particles->SetPostion(glm::vec3(0.0f, 0.0f, 0.0f));

			ParticleSystem::Sptr particleManager = particles->Add<ParticleSystem>();

			particleManager->_gravity = glm::vec3(0.0f);

			ParticleSystem::ParticleData emitter;
			emitter.Type = ParticleType::ConeEmitter;
			//emitter.TexID = 0;
			emitter.Position = glm::vec3(0.0f,0.0f,0.0f);
			emitter.Color = glm::vec4(0.7f, 0.0f, 1.0f, 1.0f);
			emitter.Lifetime = 1.0f;
			emitter.Velocity = glm::vec3(0.0f, 30.0f, 0.0f);
				
			emitter.ConeEmitterData.Angle = glm::radians(30.0f);
			emitter.ConeEmitterData.Timer = SpawnRate;
			emitter.ConeEmitterData.SizeRange = glm::vec2(0.2f, 0.2f);
			emitter.ConeEmitterData.LifeRange = glm::vec2(0.0f, 0.2f);

			particleManager->AddEmitter(emitter);
		}

		GameObject::Sptr demoBase = scene->CreateGameObject("Demo Parent");

		// Create a trigger volume for testing how we can detect collisions with objects!
		GameObject::Sptr trigger = scene->CreateGameObject("Trigger");
		{
			TriggerVolume::Sptr volume = trigger->Add<TriggerVolume>();
			CylinderCollider::Sptr collider = CylinderCollider::Create(glm::vec3(3.0f, 3.0f, 1.0f));
			collider->SetPosition(glm::vec3(0.0f, 0.0f, 0.5f));
			volume->AddCollider(collider);

			trigger->Add<TriggerVolumeEnterBehaviour>();
		}
		
		/*GameObject::Sptr shadowCaster = scene->CreateGameObject("Shadow Light");
		{
			// Set position in the scene
			shadowCaster->SetPostion(glm::vec3(3.0f, 3.0f, 5.0f));
			shadowCaster->LookAt(glm::vec3(0.0f));

			// Create and attach a renderer for the monkey
			ShadowCamera::Sptr shadowCam = shadowCaster->Add<ShadowCamera>();
			shadowCam->SetProjection(glm::perspective(glm::radians(120.0f), 1.0f, 0.1f, 100.0f));
		}*/

		/////////////////////////// UI //////////////////////////////
		GuiBatcher::SetDefaultTexture(ResourceManager::CreateAsset<Texture2D>("textures/ui-sprite.png"));
		GuiBatcher::SetDefaultBorderRadius(8);
	
		// Save the asset manifest for all the resources we just loaded
		ResourceManager::SaveManifest("scene-manifest.json");
		// Save the scene to a JSON file
		scene->Save("scene.json");

		// Send the scene to the application
		app.LoadScene(scene);
	}
}