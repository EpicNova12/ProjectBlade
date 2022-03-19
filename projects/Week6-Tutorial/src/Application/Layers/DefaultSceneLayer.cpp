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

void DefaultSceneLayer::OnUpdate()
{
	if (InputEngine::IsKeyDown(GLFW_KEY_1))
	{
		toggleAmbient(false);
		toggleDiffuse(false);
		toggleSpecular(false);
		toggleCustom(false);
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
		toggleCustom(true);
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
		for (int i = 0; i < materials.size(); i++)
		{
			updateMaterial(materials[i]);
		}
	}
}


glm::vec3 swordRot = glm::vec3(90.0f, 0.0f, -180.0f);
//Toggle Shader Effects
bool ambientIsOn = true;
bool diffuseIsOn = true;
bool specularIsOn = true;
bool customIsOn = false;

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

void updateMaterial(Gameplay::Material::Sptr material)
{
	material->Set("u_ALight.isOn", ambientIsOn);
	material->Set("u_DLight.isOn", diffuseIsOn);
	material->Set("u_SLight.isOn", specularIsOn);
	material->Set("u_Custom.isOn", customIsOn);

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
		MeshResource::Sptr birdMesh = ResourceManager::CreateAsset<MeshResource>("bird.obj");

		// Load in some textures
		Texture2D::Sptr    boxTexture   = ResourceManager::CreateAsset<Texture2D>("textures/box-diffuse.png");
		Texture2D::Sptr    boxSpec      = ResourceManager::CreateAsset<Texture2D>("textures/box-specular.png");
		Texture2D::Sptr    leafTex      = ResourceManager::CreateAsset<Texture2D>("textures/leaves.png");
		Texture2D::Sptr worldTex = ResourceManager::CreateAsset<Texture2D>("textures/WorldTexture.png");
		Texture2D::Sptr swordTex = ResourceManager::CreateAsset<Texture2D>("textures/SwordTexture.png");
		Texture2D::Sptr ballTex = ResourceManager::CreateAsset<Texture2D>("textures/BallTexture.png");
		Texture2D::Sptr pipeTex = ResourceManager::CreateAsset<Texture2D>("textures/PipeTexture.png");


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
		TextureCube::Sptr testCubemap = ResourceManager::CreateAsset<TextureCube>("cubemaps/ocean/ocean.jpg");
		ShaderProgram::Sptr      skyboxShader = ResourceManager::CreateAsset<ShaderProgram>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shaders/skybox_vert.glsl" },
			{ ShaderPartType::Fragment, "shaders/fragment_shaders/skybox_frag.glsl" } 
		});
		  
		// Create an empty scene
		Scene::Sptr scene = std::make_shared<Scene>();  

		// Setting up our enviroment map
		scene->SetSkyboxTexture(testCubemap); 
		scene->SetSkyboxShader(skyboxShader);
		// Since the skybox I used was for Y-up, we need to rotate it 90 deg around the X-axis to convert it to z-up 
		scene->SetSkyboxRotation(glm::rotate(MAT4_IDENTITY, glm::half_pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f)));

		// Loading in a color lookup table
		Texture3D::Sptr lutCool = ResourceManager::CreateAsset<Texture3D>("luts/LUT_Cool.CUBE");
		Texture3D::Sptr lutWarm = ResourceManager::CreateAsset<Texture3D>("luts/LUT_Warm.CUBE");
		Texture3D::Sptr lutCustom = ResourceManager::CreateAsset<Texture3D>("luts/LUT_Custom.CUBE");
		
		// Configure the color correction LUT
		scene->SetColorLUT(lutCustom);
		
		// Create our materials
		// This will be our box material, with no environment reflections
		Material::Sptr boxMaterial = ResourceManager::CreateAsset<Material>(deferredForward);
		{
			boxMaterial->Name = "Box";
			boxMaterial->Set("u_Material.AlbedoMap", boxTexture);
			boxMaterial->Set("u_Material.Shininess", 0.1f);
			boxMaterial->Set("u_Material.NormalMap", normalMapDefault);
		}
		
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
			worldMat->Set("texColor", worldTex);
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
			swordMat->Set("toggleColorCorrect", true);
			swordMat->Set("texColor", swordTex);
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
			ballMat->Set("toggleColorCorrect", true);
			ballMat->Set("texColor", ballTex);

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
			pipeMat->Set("texColor", pipeTex);
		}
		materials.push_back(pipeMat);

		// This will be the reflective material, we'll make the whole thing 90% reflective
		
		// This will be the reflective material, we'll make the whole thing 50% reflective
		
		Material::Sptr displacementTest = ResourceManager::CreateAsset<Material>(displacementShader);
		{
			Texture2D::Sptr displacementMap = ResourceManager::CreateAsset<Texture2D>("textures/displacement_map.png");
			Texture2D::Sptr normalMap       = ResourceManager::CreateAsset<Texture2D>("textures/normal_map.png");
			Texture2D::Sptr diffuseMap      = ResourceManager::CreateAsset<Texture2D>("textures/bricks_diffuse.png");

			displacementTest->Name = "Displacement Map";
			displacementTest->Set("u_Material.AlbedoMap", diffuseMap);
			displacementTest->Set("u_Material.NormalMap", normalMap);
			displacementTest->Set("s_Heightmap", displacementMap);
			displacementTest->Set("u_Material.Shininess", 0.5f);
			displacementTest->Set("u_Scale", 0.1f);
		}

		Material::Sptr normalmapMat = ResourceManager::CreateAsset<Material>(deferredForward);
		{
			Texture2D::Sptr normalMap       = ResourceManager::CreateAsset<Texture2D>("textures/normal_map.png");
			Texture2D::Sptr diffuseMap      = ResourceManager::CreateAsset<Texture2D>("textures/bricks_diffuse.png");

			normalmapMat->Name = "Tangent Space Normal Map";
			normalmapMat->Set("u_Material.AlbedoMap", diffuseMap);
			normalmapMat->Set("u_Material.NormalMap", normalMap);
			normalmapMat->Set("u_Material.Shininess", 0.5f);
			normalmapMat->Set("u_Scale", 0.1f);
		}

		Material::Sptr multiTextureMat = ResourceManager::CreateAsset<Material>(multiTextureShader);
		{
			Texture2D::Sptr sand  = ResourceManager::CreateAsset<Texture2D>("textures/terrain/sand.png");
			Texture2D::Sptr grass = ResourceManager::CreateAsset<Texture2D>("textures/terrain/grass.png");

			multiTextureMat->Name = "Multitexturing";
			multiTextureMat->Set("u_Material.DiffuseA", sand);
			multiTextureMat->Set("u_Material.DiffuseB", grass);
			multiTextureMat->Set("u_Material.NormalMapA", normalMapDefault);
			multiTextureMat->Set("u_Material.NormalMapB", normalMapDefault);
			multiTextureMat->Set("u_Material.Shininess", 0.5f);
			multiTextureMat->Set("u_Scale", 0.1f); 
		}

		// Create some lights for our scene
		GameObject::Sptr lightParent = scene->CreateGameObject("Lights");

		for (int ix = 0; ix < 1; ix++) {
			//light->SetPostion(glm::vec3(glm::diskRand(25.0f), 1.0f));
			GameObject::Sptr light = scene->CreateGameObject("Light");
			light->SetPostion(glm::vec3(-12.1,0.0,10.0));
			//lightParent->AddChild(light);

			Light::Sptr lightComponent = light->Add<Light>();
			lightComponent->SetColor(glm::vec3(1.0f,1.0f,1.0f));
			lightComponent->SetRadius(5000.0f);
			lightComponent->SetIntensity(10);
		}

		// We'll create a mesh that is a simple plane that we can resize later
		MeshResource::Sptr planeMesh = ResourceManager::CreateAsset<MeshResource>();
		planeMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(1.0f)));
		planeMesh->GenerateMesh();

		MeshResource::Sptr sphere = ResourceManager::CreateAsset<MeshResource>();
		sphere->AddParam(MeshBuilderParam::CreateIcoSphere(ZERO, ONE, 5));
		sphere->GenerateMesh();

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
		
		// Set up all our sample objects
		/*GameObject::Sptr plane = scene->CreateGameObject("Plane");
		{
			// Make a big tiled mesh
			MeshResource::Sptr tiledMesh = ResourceManager::CreateAsset<MeshResource>();
			tiledMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(100.0f), glm::vec2(20.0f)));
			tiledMesh->GenerateMesh();

			// Create and attach a RenderComponent to the object to draw our mesh
			RenderComponent::Sptr renderer = plane->Add<RenderComponent>();
			renderer->SetMesh(tiledMesh);
			renderer->SetMaterial(boxMaterial);

			// Attach a plane collider that extends infinitely along the X/Y axis
			RigidBody::Sptr physics = plane->Add<RigidBody>(static by default);
			physics->AddCollider(BoxCollider::Create(glm::vec3(50.0f, 50.0f, 1.0f)))->SetPosition({ 0,0,-1 });
		}*/

		/*GameObject::Sptr monkey1 = scene->CreateGameObject("Monkey 1");
		{
			// Set position in the scene
			monkey1->SetPostion(glm::vec3(1.5f, 0.0f, 1.0f));

			// Add some behaviour that relies on the physics body
			monkey1->Add<JumpBehaviour>();

			// Create and attach a renderer for the monkey
			RenderComponent::Sptr renderer = monkey1->Add<RenderComponent>();
			renderer->SetMesh(monkeyMesh);
			renderer->SetMaterial(monkeyMaterial);

			// Example of a trigger that interacts with static and kinematic bodies as well as dynamic bodies
			TriggerVolume::Sptr trigger = monkey1->Add<TriggerVolume>();
			trigger->SetFlags(TriggerTypeFlags::Statics | TriggerTypeFlags::Kinematics);
			trigger->AddCollider(BoxCollider::Create(glm::vec3(1.0f)));

			monkey1->Add<TriggerVolumeEnterBehaviour>();
		}*/

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
			sword->SetRotation(glm::vec3(90.0f, 0.0f, -180.0f));
			RenderComponent::Sptr renderer = sword->Add<RenderComponent>();
			renderer->SetMesh(swordMesh);
			renderer->SetMaterial(swordMat);
		}
		GameObject::Sptr ball = scene->CreateGameObject("Ball");
		{
			ball->SetPostion(glm::vec3(0.0f, 0.0f, 5.0f));
			ball->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
			RenderComponent::Sptr renderer = ball->Add<RenderComponent>();
			renderer->SetMesh(ballMesh);
			renderer->SetMaterial(ballMat);
		}
		GameObject::Sptr pipe = scene->CreateGameObject("Pipe");
		{
			pipe->SetPostion(glm::vec3(0.0f, 0.0f, 1.18f));
			pipe->SetRotation(glm::vec3(90.0f, 0.0f, -92.0f));
			RenderComponent::Sptr renderer = pipe->Add<RenderComponent>();
			renderer->SetMesh(pipeMesh);
			renderer->SetMaterial(pipeMat);
		}

		GameObject::Sptr demoBase = scene->CreateGameObject("Demo Parent");
		// Box to showcase the specular material
		/*GameObject::Sptr specBox = scene->CreateGameObject("Specular Object");
		{
			MeshResource::Sptr boxMesh = ResourceManager::CreateAsset<MeshResource>();
			boxMesh->AddParam(MeshBuilderParam::CreateCube(ZERO, ONE));
			boxMesh->GenerateMesh();

			// Set and rotation position in the scene
			specBox->SetPostion(glm::vec3(0, -4.0f, 1.0f));

			// Add a render component
			RenderComponent::Sptr renderer = specBox->Add<RenderComponent>();
			renderer->SetMesh(boxMesh);
			renderer->SetMaterial(testMaterial); 

			demoBase->AddChild(specBox);
		}

		// sphere to showcase the foliage material
		GameObject::Sptr foliageBall = scene->CreateGameObject("Foliage Sphere");
		{
			// Set and rotation position in the scene
			foliageBall->SetPostion(glm::vec3(-4.0f, -4.0f, 1.0f));

			// Add a render component
			RenderComponent::Sptr renderer = foliageBall->Add<RenderComponent>();
			renderer->SetMesh(sphere);
			renderer->SetMaterial(foliageMaterial);

			demoBase->AddChild(foliageBall);
		}*/

		// Create a trigger volume for testing how we can detect collisions with objects!
		GameObject::Sptr trigger = scene->CreateGameObject("Trigger");
		{
			TriggerVolume::Sptr volume = trigger->Add<TriggerVolume>();
			CylinderCollider::Sptr collider = CylinderCollider::Create(glm::vec3(3.0f, 3.0f, 1.0f));
			collider->SetPosition(glm::vec3(0.0f, 0.0f, 0.5f));
			volume->AddCollider(collider);

			trigger->Add<TriggerVolumeEnterBehaviour>();
		}

		//Lightbird
		
		/////////////////////////// UI //////////////////////////////
		/*
		GameObject::Sptr canvas = scene->CreateGameObject("UI Canvas");
		{
			RectTransform::Sptr transform = canvas->Add<RectTransform>();
			transform->SetMin({ 16, 16 });
			transform->SetMax({ 256, 256 });

			GuiPanel::Sptr canPanel = canvas->Add<GuiPanel>();


			GameObject::Sptr subPanel = scene->CreateGameObject("Sub Item");
			{
				RectTransform::Sptr transform = subPanel->Add<RectTransform>();
				transform->SetMin({ 10, 10 });
				transform->SetMax({ 128, 128 });

				GuiPanel::Sptr panel = subPanel->Add<GuiPanel>();
				panel->SetColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

				panel->SetTexture(ResourceManager::CreateAsset<Texture2D>("textures/upArrow.png"));

				Font::Sptr font = ResourceManager::CreateAsset<Font>("fonts/Roboto-Medium.ttf", 16.0f);
				font->Bake();

				GuiText::Sptr text = subPanel->Add<GuiText>();
				text->SetText("Hello world!");
				text->SetFont(font);

				monkey1->Get<JumpBehaviour>()->Panel = text;
			}

			canvas->AddChild(subPanel);
		}
		*/

		/*GameObject::Sptr particles = scene->CreateGameObject("Particles");
		{
			ParticleSystem::Sptr particleManager = particles->Add<ParticleSystem>();  
			particleManager->AddEmitter(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 10.0f), 10.0f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)); 
		}*/

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
