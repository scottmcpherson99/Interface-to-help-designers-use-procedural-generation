// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"

App1::App1()
{
	m_Terrain = nullptr;
	videoDemo = nullptr;
	shader = nullptr;
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	//load textures for the terrain
	textureMgr->loadTexture(L"grass", L"res/grass.png");
	textureMgr->loadTexture(L"rock", L"res/rock.jpg");
	textureMgr->loadTexture(L"snow", L"res/snow.jpg");
	textureMgr->loadTexture(L"water", L"res/water.jpg");
	textureMgr->loadTexture(L"sand", L"res/sand.jpg");

	// Create Mesh object and shader object
	m_Terrain = new TerrainMesh(renderer->getDevice(), renderer->getDeviceContext());
	videoDemo = new VideoDemo(renderer->getDevice(), renderer->getDeviceContext());
	shader = new LightShader(renderer->getDevice(), hwnd);

	//set lighting and camera
	light = new Light;
	light->setAmbientColour( 0.25f, 0.25f, 0.25f, 1.0f );
	light->setDiffuseColour(0.75f, 0.75f, 0.75f, 1.0f);
	light->setDirection(1.0f,-0.0f, 0.0f);

	camera->setPosition(0.0f, 45.0f, -30.0f);
	camera->setRotation( 0.0f, 30.0f,30.0f );

	terrainResolution = m_Terrain->GetResolution();

	screenWidth_ = screenWidth;
	screenHeight_ = screenHeight;

	for (int i = 0; i < 4; i++)
	{
		faultSection[i] = false;
		midPointSection[i] = false;
		ParticleDepositionSection[i] = false;
		VoronioRegionsSection[i] = false;
	}

}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.
	if ( m_Terrain )
	{
		delete m_Terrain;
		m_Terrain = 0;
	}

	if (videoDemo)
	{
		delete videoDemo;
		videoDemo = 0;
	}

	if (shader)
	{
		delete shader;
		shader = 0;
	}
}


bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}
	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	checkTrainingComplete();
	
	return true;
}

//run check to see if all training sections are complete before allowing user access to the fere terrain
void App1::checkTrainingComplete()
{
	//check to see if training is complete
	if (faultComplete == true)
	{
		if (particleDepositionComplete == true)
		{
			if (voronoiRegionsComplete == true)
			{
				if (midPointDisplacementComplete == true)
				{
					trainingComplete = true;
				}
			}
		}
	}
	else
	{
		trainingComplete = true;
	}
}

//render the scene
bool App1::render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	//gamestate system to show the correct gamescreen
	switch (gameState)
	{
		//render the terrain for the free terrain mode
	case 1:
		// Clear the scene. (default blue colour)
		renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

		// Generate the view matrix based on the camera's position.
		camera->update();

		// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
		worldMatrix = renderer->getWorldMatrix();
		viewMatrix = camera->getViewMatrix();
		projectionMatrix = renderer->getProjectionMatrix();

		// Send geometry data, set shader parameters, render object with shader
		m_Terrain->sendData(renderer->getDeviceContext());
		shader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"water"), textureMgr->getTexture(L"sand"), textureMgr->getTexture(L"grass"), textureMgr->getTexture(L"snow"), textureMgr->getTexture(L"rock"), light);
		shader->render(renderer->getDeviceContext(), m_Terrain->getIndexCount());

		// Render GUI
		gui();

		// Swap the buffers
		renderer->endScene();
		break;

		//render the interface to allow user too choose what mode they would like to use
	case 0:
		// Clear the scene. (default blue colour)
		renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

		// Generate the view matrix based on the camera's position.
		camera->update();

		// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
		worldMatrix = renderer->getWorldMatrix();
		viewMatrix = camera->getViewMatrix();
		projectionMatrix = renderer->getProjectionMatrix();

		gui();

		// Swap the buffers
		renderer->endScene();
		break;

		//render the training menu to let the user choose which algorithm to learn
	case 2:
		// Clear the scene. (default blue colour)
		renderer->beginScene(0.f, 0.f, 1.f, 1.0f);

		// Generate the view matrix based on the camera's position.
		camera->update();

		// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
		worldMatrix = renderer->getWorldMatrix();
		viewMatrix = camera->getViewMatrix();
		projectionMatrix = renderer->getProjectionMatrix();

		gui();

		// Swap the buffers
		renderer->endScene();
		break;

		//render the interface to teach the user how to use the faulting algorithm
	case 3:
		// Clear the scene. (default blue colour)
		renderer->beginScene(0.f, 0.f, 1.f, 1.0f);

		// Generate the view matrix based on the camera's position.
		camera->update();

		// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
		worldMatrix = renderer->getWorldMatrix();
		viewMatrix = camera->getViewMatrix();
		projectionMatrix = renderer->getProjectionMatrix();

		faultGUI();

		// Swap the buffers
		renderer->endScene();
		break;

		//render the interface to teach the user how to use the particle deposition algorithm
	case 4:
		// Clear the scene. (default blue colour)
		renderer->beginScene(0.f, 0.f, 1.f, 1.0f);

		// Generate the view matrix based on the camera's position.
		camera->update();

		// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
		worldMatrix = renderer->getWorldMatrix();
		viewMatrix = camera->getViewMatrix();
		projectionMatrix = renderer->getProjectionMatrix();

		ParticleDepositionGUI();

		// Swap the buffers
		renderer->endScene();
		break;

		//render the interface to teach the user how to use the voronoi regions algorithm
	case 5:
		// Clear the scene. (default blue colour)
		renderer->beginScene(0.f, 0.f, 1.f, 1.0f);

		// Generate the view matrix based on the camera's position.
		camera->update();

		// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
		worldMatrix = renderer->getWorldMatrix();
		viewMatrix = camera->getViewMatrix();
		projectionMatrix = renderer->getProjectionMatrix();

		VoronioRegionsGUI();

		// Swap the buffers
		renderer->endScene();
		break;

		//render the interface to teach the user how to use the mid point displacement algorithm
	case 6:
		// Clear the scene. (default blue colour)
		renderer->beginScene(0.f, 0.f, 1.f, 1.0f);

		// Generate the view matrix based on the camera's position.
		camera->update();

		// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
		worldMatrix = renderer->getWorldMatrix();
		viewMatrix = camera->getViewMatrix();
		projectionMatrix = renderer->getProjectionMatrix();

		midPointGUI();

		// Swap the buffers
		renderer->endScene();
		break;

		//render the interface to show the faulting algorithm working on the terrain
	case 7:
		// Clear the scene. (default blue colour)
		renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

		// Generate the view matrix based on the camera's position.
		camera->update();

		// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
		worldMatrix = renderer->getWorldMatrix();
		viewMatrix = camera->getViewMatrix();
		projectionMatrix = renderer->getProjectionMatrix();

		// Send geometry data, set shader parameters, render object with shader
		videoDemo->sendData(renderer->getDeviceContext());
		shader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"water"), textureMgr->getTexture(L"sand"), textureMgr->getTexture(L"grass"), textureMgr->getTexture(L"snow"), textureMgr->getTexture(L"rock"), light);
		shader->render(renderer->getDeviceContext(), videoDemo->getIndexCount());

		// Render GUI
		visualFaultDemoGUI();

		// Swap the buffers
		renderer->endScene();
		break;

		//render the interface to show the particle deposition algorithm working on the terrain
	case 8:
		// Clear the scene. (default blue colour)
		renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

		// Generate the view matrix based on the camera's position.
		camera->update();

		// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
		worldMatrix = renderer->getWorldMatrix();
		viewMatrix = camera->getViewMatrix();
		projectionMatrix = renderer->getProjectionMatrix();

		// Send geometry data, set shader parameters, render object with shader
		videoDemo->sendData(renderer->getDeviceContext());
		shader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"water"), textureMgr->getTexture(L"sand"), textureMgr->getTexture(L"grass"), textureMgr->getTexture(L"snow"), textureMgr->getTexture(L"rock"), light);
		shader->render(renderer->getDeviceContext(), videoDemo->getIndexCount());

		// Render GUI
		visualParticleDepositionDemoGUI();

		// Swap the buffers
		renderer->endScene();
		break;

		//render the interface to show the mid-point displacement algorithm working on the terrain
	case 9:
		// Clear the scene. (default blue colour)
		renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

		// Generate the view matrix based on the camera's position.
		camera->update();

		// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
		worldMatrix = renderer->getWorldMatrix();
		viewMatrix = camera->getViewMatrix();
		projectionMatrix = renderer->getProjectionMatrix();

		// Send geometry data, set shader parameters, render object with shader
		videoDemo->sendData(renderer->getDeviceContext());
		shader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"water"), textureMgr->getTexture(L"sand"), textureMgr->getTexture(L"grass"), textureMgr->getTexture(L"snow"), textureMgr->getTexture(L"rock"), light);
		shader->render(renderer->getDeviceContext(), videoDemo->getIndexCount());

		// Render GUI
		visualMidPointDisplacementDemoGUI();

		// Swap the buffers
		renderer->endScene();
		break;

		//render the interface to show the voronoi regions algorithm working on the terrain
	case 10:
		// Clear the scene. (default blue colour)
		renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

		// Generate the view matrix based on the camera's position.
		camera->update();

		// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
		worldMatrix = renderer->getWorldMatrix();
		viewMatrix = camera->getViewMatrix();
		projectionMatrix = renderer->getProjectionMatrix();

		// Send geometry data, set shader parameters, render object with shader
		videoDemo->sendData(renderer->getDeviceContext());
		shader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"water"), textureMgr->getTexture(L"sand"), textureMgr->getTexture(L"grass"), textureMgr->getTexture(L"snow"), textureMgr->getTexture(L"rock"), light);
		shader->render(renderer->getDeviceContext(), videoDemo->getIndexCount());

		// Render GUI
		visualVoronioRegionsDemoGUI();

		// Swap the buffers
		renderer->endScene();
		break;

		//render the interface to show the algorithm being used in a sample terrain
	case 11:
		// Clear the scene. (default blue colour)
		renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

		// Generate the view matrix based on the camera's position.
		camera->update();

		// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
		worldMatrix = renderer->getWorldMatrix();
		viewMatrix = camera->getViewMatrix();
		projectionMatrix = renderer->getProjectionMatrix();

		// Send geometry data, set shader parameters, render object with shader
		m_Terrain->sendData(renderer->getDeviceContext());
		shader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"water"), textureMgr->getTexture(L"sand"), textureMgr->getTexture(L"grass"), textureMgr->getTexture(L"snow"), textureMgr->getTexture(L"rock"), light);
		shader->render(renderer->getDeviceContext(), m_Terrain->getIndexCount());

		// Render GUI
		gui();

		// Swap the buffers
		renderer->endScene();

		
		break;
	}
		return true;
	
}

//render the correct GUI interface based on the gamestate
void App1::gui()
{
	switch (gameState)
	{
		//set the UI for the free terrain mode
	case 1:
		freeTerrainMenu();
		break;

		//set the UI for the main menu where the user will choose whether to use training mode or free terrain
	case 0:
		mainMenu();
		break;

		//set the UI to the training menu where the user will learn about the technique
	case 2:
		trainingMenu();
		break;

	//set the UI to display the sample terrain where the user will learn to use the algorithms individually
	case 11:
		sampleTerrainsGUI();
			break;
	}
}

//gamestate 0: Display the main menu, allowing the user to choose between the training mode or free terrain mode
void App1::mainMenu()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	ImGui::SetWindowPos(ImVec2(0.f, screenHeight_ * 0.05), 0);
	ImGui::SetWindowSize(ImVec2(screenWidth_, screenHeight_ * 0.95), 0);

	// Build UI
	//Create Menu Bar at top of screen for user to quick travel through app
	menuBar();

	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Text("Camera Pos: (%.2f, %.2f, %.2f)", camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);

	//check to see if all the training algorithms have been used
	//if they have let the user go to free terrain mode
	if (trainingComplete == true)
	{
		if (ImGui::Button("Go to free terrain"))
		{
			gameState = 1;
		}
	}
	else
	{
		ImGui::Text("Complete the tutorial to unlock the free terrain mode.");
	}

	//switch the game screen to the training menu
	if (ImGui::Button("Go to training mode"))
	{
		gameState = 2;
	}

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

//gamestate 1: Display the free terrain to let the user use all the algorithms at the same time
void App1::freeTerrainMenu()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	//Create Menu Bar at top of screen for user to quick travel through app
	menuBar();

	ImGui::SetWindowFontScale(1.f);
	ImGui::SetWindowPos(ImVec2(screenWidth_ - (screenWidth_ / 4), 0.0f), 0);
	ImGui::SetWindowSize(ImVec2(screenWidth_ / 4, screenHeight_ * 0.9), 0);
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Text("Camera Pos: (%.2f, %.2f, %.2f)", camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);

	//ImGui::SliderInt("Terrain Res.", &terrainResolution, 2, 1024);

	//reset the terrain
	if (ImGui::Button("Regenerate Terrain")) {
		if (terrainResolution != m_Terrain->GetResolution()) {
			m_Terrain->Resize(terrainResolution);
		}
		m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
	}

	//smoothen the terrain
	if (ImGui::Button("Smooth"))
	{
		m_Terrain->smooth = true;
		m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
		m_Terrain->smooth = false;
	}

	//show menu to edit terrain using the particle deposition algorithm
	if (particleDepositionMenu == true)
	{
		if (ImGui::Button("Particle Deposition"))
		{
			m_Terrain->particleDeposition = true;
			m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
			m_Terrain->particleDeposition = false;
		}
		ImGui::Text("Number of Iterations");
		ImGui::SliderInt("I", &m_Terrain->particleDepositionIterations, 1, 250);

		ImGui::Text("Number of Particles Dropped");
		ImGui::SliderInt("N", &m_Terrain->particlesDropped_, 1, 2500);

		ImGui::Text("Width in Y Value");
		ImGui::SliderInt("X", &m_Terrain->particleDropX_, 1, 50);

		ImGui::Text("Width in X Value");
		ImGui::SliderInt("Y", &m_Terrain->particleDropY_, 1, 50);

		if (ImGui::Button("Close Menu"))
		{
			particleDepositionMenu = false;
		}
	}

	//show menu to edit terrain using the voronoi regions algorithm
	if (voronoiMenu == true)
	{
		if (ImGui::Button("Voronoi Regions"))
		{
			m_Terrain->voronoi = true;
			m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
			m_Terrain->voronoi = false;
		}

		ImGui::Text("Number of Voronio Regions");
		ImGui::SliderInt("V", &m_Terrain->voronoiRegions, 1, 50);

		if (ImGui::Button("Close Menu"))
		{
			voronoiMenu = false;
		}
	}

	//show menu to edit terrain using the faulting algorithm
	if (faultMenu == true)
	{
		if (ImGui::Button("Fault"))
		{
			m_Terrain->fault = true;
			m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
			m_Terrain->fault = false;
		}
		ImGui::Text("Fault Iterations");
		ImGui::SliderInt("F.", &m_Terrain->faultNumbers, 1, 100);

		ImGui::Text("Fault Value");
		ImGui::SliderFloat("Fault Value", &m_Terrain->faultValue, 1.0f, 20.0f, "Pos : (%.3f)", 1.0f);

		if (ImGui::Button("Close Menu"))
		{
			faultMenu = false;
		}
	}

	//show menu to edit terrain using the midpoint displacement algorithm
	if (midpointDisplacementMenu == true)
	{
		ImGui::Text("Max Height of Corners");
		ImGui::SliderInt("H.", &m_Terrain->cornerHeight, 1, 30);

		ImGui::Text("Smoothness of Terrain (Lower Value = smoother)");
		ImGui::SliderFloat("S.", &m_Terrain->midPointDisplacementValue, 1, 20, "Pos : (%.3f)", 1.0f);

		if (ImGui::Button("Mid point Displacement"))
		{
			m_Terrain->mid_point_ = true;
			m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
			m_Terrain->mid_point_ = false;
		}


		if (ImGui::Button("Close Menu"))
		{
			midpointDisplacementMenu = false;
		}
	}

	//show menu to edit terrain textures height
	if (terrainSliders == true)
	{
		ImGui::Text("Height of bottom of snow");
		ImGui::SliderFloat("Snow", &shader->snow_, 6.0f, 12.0f, "Pos : (%.3f)", 1.0f);

		ImGui::Text("Height of bottom of rock");
		ImGui::SliderFloat("Rock", &shader->rock_, 4.0f, 10.0f, "Pos : (%.3f)", 1.0f);

		ImGui::Text("Height of bottom of grass");
		ImGui::SliderFloat("Grass", &shader->grass_, 1.0f, 8.0f, "Pos : (%.3f)", 1.0f);

		ImGui::Text("Height of bottom of water");
		ImGui::SliderFloat("Water", &shader->water_, 0.0f, 6.0f, "Pos : (%.3f)", 1.0f);

		if (ImGui::Button("Close Menu"))
		{
			terrainSliders = false;
		}
	}

	//open the particle deposition menu
	if (ImGui::Button("Open Particle Deposition Menu"))
	{
		particleDepositionMenu = true;
		voronoiMenu = false;
		faultMenu = false;
		midpointDisplacementMenu = false;
		terrainSliders = false;
	}

	//open the voronoi regions menu
	if (ImGui::Button("Open Voronoi Menu"))
	{
		particleDepositionMenu = false;
		voronoiMenu = true;
		faultMenu = false;
		midpointDisplacementMenu = false;
		terrainSliders = false;
	}

	//open the faulting menu
	if (ImGui::Button("Open Fault Menu"))
	{
		particleDepositionMenu = false;
		voronoiMenu = false;
		faultMenu = true;
		midpointDisplacementMenu = false;
		terrainSliders = false;
	}

	//open the mid-point displacement menu
	if (ImGui::Button("Open Mid Point Displacement Menu"))
	{
		particleDepositionMenu = false;
		voronoiMenu = false;
		faultMenu = false;
		midpointDisplacementMenu = true;
		terrainSliders = false;
	}

	//open the terrain sliders menu
	if (ImGui::Button("Open Terrain Sliders"))
	{
		particleDepositionMenu = false;
		voronoiMenu = false;
		faultMenu = false;
		midpointDisplacementMenu = false;
		terrainSliders = true;
	}

	//return the user to the main menu
	if (ImGui::Button("Go Back to Main Menu"))
	{
		gameState = 0;
	}

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

//gamestate 2: Display the training menu to let the user choose an algorithm to learn about
void App1::trainingMenu()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	ImGui::SetWindowPos(ImVec2(0.f, screenHeight_ * 0.05), 0);
	ImGui::SetWindowSize(ImVec2(screenWidth_, screenHeight_ * 0.95), 0);

	// Build UI
	//Create Menu Bar at top of screen for user to quick travel through app
	menuBar();

	ImGui::SetWindowFontScale(1.f);
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Text("Camera Pos: (%.2f, %.2f, %.2f)", camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
	ImGui::SetWindowFontScale(2.f);
	//display buttons to take the user to the desrired training algorithm
	if (ImGui::Button("Go to Fault"))
	{
		gameState = 3;
	}

	if (ImGui::Button("Go to Particle Deposition"))
	{
		gameState = 4;
	}

	if (ImGui::Button("Go to Voronio Regions"))
	{
		gameState = 5;
	}

	if (ImGui::Button("Go to Midpoint Displacement"))
	{
		gameState = 6;
	}

	//take the user back to the main menu
	if (ImGui::Button("Go back to main menu"))
	{
		gameState = 0;
	}

	//close the interface
	if (ImGui::MenuItem("Close Interface"))
	{
		exit(0);
	}

	ImGui::SetWindowFontScale(1.f);
	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

//create a menu bar throughout the interface to let the user quick travel through the pages
void App1::menuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Go to Main Menu"))
			{
				gameState = 0;
				m_Terrain->midPointDisplacementTutorial = false;
				m_Terrain->voronoiRegionTutorial = false;
				m_Terrain->particleDepositionTutorial = false;
				m_Terrain->faultTutorial = false;
			}
			if (ImGui::MenuItem("Restart Training Mode"))
			{
				gameState = 2;
				m_Terrain->midPointDisplacementTutorial = false;
				m_Terrain->voronoiRegionTutorial = false;
				m_Terrain->particleDepositionTutorial = false;
				m_Terrain->faultTutorial = false;
			}
			if (trainingComplete == true)
			{
				if (ImGui::MenuItem("Restart Free Terrain"))
				{
					gameState = 1;
					m_Terrain->midPointDisplacementTutorial = false;
					m_Terrain->voronoiRegionTutorial = false;
					m_Terrain->particleDepositionTutorial = false;
					m_Terrain->faultTutorial = false;
				}
			}
			if (ImGui::MenuItem("Close Interface"))
			{
				exit(0);
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

//gamestate 11: Display a terrain where the user will be instructed on how to use each of the algorithms individually and will be able to use them
void App1::sampleTerrainsGUI()
{
	//check to see what algorithm is currently being learned, and display the appropriate interface to show the user how to use the algorithm
	if (faultSection[3] == true)
	{
		faultSampleTerrain();
	}

	else if (ParticleDepositionSection[3] == true)
	{
		ParticleDepositionSampleTerrain();
	}

	else if (VoronioRegionsSection[3] == true)
	{
		VoronoiRegionsSampleTerrain();
	}

	else if (midPointSection[3] == true)
	{
		midPointSampleTerrain();
	}

	//if the user has completed the training for the algorithm, let the user use the specific algorithm freely on the terain
	else
	{
		// Force turn off unnecessary shader stages.
		renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
		renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
		renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);


		// Build UI
		ImGui::SetWindowFontScale(2.f);
		ImGui::SetWindowPos(ImVec2(screenWidth_ - (screenWidth_ / 5), 0.0f), 0);
		ImGui::SetWindowSize(ImVec2(screenWidth_ / 5, screenHeight_ * 0.9), 0);
		ImGui::Text("FPS: %.2f", timer->getFPS());
		ImGui::Text("Camera Pos: (%.2f, %.2f, %.2f)", camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
		ImGui::SetWindowFontScale(1.f);
		ImGui::Checkbox("Wireframe mode", &wireframeToggle);

		//ImGui::SliderInt("Terrain Resolution", &terrainResolution, 2, 1024);

		//reset the terrain
		if (ImGui::Button("Regenerate Terrain")) {
			if (terrainResolution != m_Terrain->GetResolution()) {
				m_Terrain->Resize(terrainResolution);
			}
			m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
		}

		//smoothen the terrain
		if (ImGui::Button("Smooth"))
		{
			m_Terrain->smooth = true;
			m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
			m_Terrain->smooth = false;
		}

		//if the user was learning about particle deposition, display the menu to let the user use particle deposition on the terrain
		if (particleDepositionMenu == true)
		{
			if (ImGui::Button("Particle Deposition"))
			{
				m_Terrain->particleDeposition = true;
				m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
				m_Terrain->particleDeposition = false;
			}

			ImGui::SliderInt("Particle Deposition: Number of Particles Dropped", &m_Terrain->particlesDropped_, 1, 2500);

			ImGui::SliderInt("Particle Deposition: Width in Y Value", &m_Terrain->particleDropX_, 1, 50);

			ImGui::SliderInt("Particle Deposition: Width in X Value", &m_Terrain->particleDropY_, 1, 50);

			if (ImGui::Button("Close Menu"))
			{
				particleDepositionMenu = false;
			}
		}

		if (m_Terrain->particleDepositionTutorial == true)
		{
			if (ImGui::Button("Open Particle Deposition Menu"))
			{
				particleDepositionMenu = true;
			}
		}


		//if the user was learning about voronoi regions, display the menu to let the user use voronoi regions on the terrain
		if (voronoiMenu == true)
		{
			if (ImGui::Button("Voronoi Regions"))
			{
				m_Terrain->voronoi = true;
				m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
				m_Terrain->voronoi = false;
			}

			ImGui::SliderInt("Number of Voronoi Regions", &m_Terrain->voronoiRegions, 1, 50);

			if (ImGui::Button("Close Menu"))
			{
				voronoiMenu = false;
			}
		}

		if (m_Terrain->voronoiRegionTutorial == true)
		{
			if (ImGui::Button("Open Voronoi Menu"))
			{
				voronoiMenu = true;
			}

		}


		//if the user was learning about faulting, display the menu to let the user use faulting on the terrain
		if (faultMenu == true)
		{
			if (ImGui::Button("Fault"))
			{
				m_Terrain->fault = true;
				m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
				m_Terrain->fault = false;
			}

			ImGui::SliderInt("Fault Iterations", &m_Terrain->faultNumbers, 1, 100);
			if (ImGui::Button("Close Menu"))
			{
				faultMenu = false;
			}
		}
		
		if (m_Terrain->faultTutorial == true)
		{
			if (ImGui::Button("Open Fault Menu"))
			{
				faultMenu = true;
			}
		}


		//if the user was learning about mid-point dispalcement, display the menu to let the user use mid-point displacement on the terrain
		if (midpointDisplacementMenu == true)
		{
			ImGui::SliderInt("Max Height of Corners:", &m_Terrain->cornerHeight, 1, 50);
			ImGui::SliderFloat("Smoothness of Terrain (Lower Value = smoother):", &m_Terrain->midPointDisplacementValue, 1, 30, "Pos : (%.3f)", 1.0f);
			if (ImGui::Button("Mid point Displacement"))
			{
				m_Terrain->mid_point_ = true;
				m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
				m_Terrain->mid_point_ = false;
			}


			if (ImGui::Button("Close Menu"))
			{
				midpointDisplacementMenu = false;
			}
		}

		if (m_Terrain->midPointDisplacementTutorial == true)
		{
			if (ImGui::Button("Open Mid Point Displacement Menu"))
			{
				midpointDisplacementMenu = true;
			}
		}


		//let the user change the height of the textures
		if (terrainSliders == true)
		{
			ImGui::SliderFloat("Height of bottom of snow:", &shader->snow_, 7.0f, 10.0f, "Pos : (%.3f)", 1.0f);

			ImGui::SliderFloat("Height of bottom of rock:", &shader->rock_, 5.0f, 9.0f, "Pos : (%.3f)", 1.0f);

			ImGui::SliderFloat("Height of bottom of grass:", &shader->grass_, 2.0f, 7.0f, "Pos : (%.3f)", 1.0f);

			ImGui::SliderFloat("Height of top of water:", &shader->water_, 0.0f, 5.0f, "Pos : (%.3f)", 1.0f);

			if (ImGui::Button("Close Menu"))
			{
				terrainSliders = false;
			}
		}
	
		if (ImGui::Button("Open Terrain Sliders"))
		{
			terrainSliders = true;
		}


		//take the user back to the training method when they are finished using the algorithm on the terrain
		if (ImGui::Button("Go Back to Training Menu"))
		{
			gameState = 2;
			midpointDisplacementMenu = false;
			voronoiMenu = false;
			particleDepositionMenu = false;
			faultMenu = false;
			m_Terrain->midPointDisplacementTutorial = false;
			m_Terrain->voronoiRegionTutorial = false;
			m_Terrain->particleDepositionTutorial = false;
			m_Terrain->faultTutorial = false;
		}

		// Render UI
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
}

//gamestate 7: Show the user how the faulting algorithm works on a terrain
void App1::visualFaultDemoGUI()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	//Create Menu Bar at top of screen for user to quick travel through app
	menuBar();


	ImGui::SetWindowFontScale(1.f);
	ImGui::SetWindowPos(ImVec2(0.0f, screenHeight_ - (screenHeight_ / 4)), 0);
	ImGui::SetWindowSize(ImVec2(screenWidth_, screenHeight_ * 0.25), 0);
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Text("Camera Pos: (%.2f, %.2f, %.2f)", camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
	ImGui::SetWindowFontScale(1.f);
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);

	//describe algorithm in steps using video demo
	//when user clicks next button the next fault line will increase and the next text will be displayed
	if (faultLine == 0)
	{
		ImGui::Text("The first step is for the application is to pick two random points on the edge of the terrain");
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			faultLine++;
		}
	}

	else if (faultLine == 1)
	{
		ImGui::Text("For the purpose of the demonstartion these values will be the lower left corner and upper right corner");

		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			faultLine++;
		}
	}

	else if (faultLine == 2)
	{
		ImGui::Text("A loop will run for each point, if a point is on the side further away from the cameras perspective it will be raised, otherwise it will be lowered.");

		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			faultLine++;
		}
	}

	else if (faultLine == 3)
	{
		ImGui::Text("Click the arrow to see the loop in action.");

		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			faultLine++;
			videoDemo->faultTutorial = true;
			videoDemo->faultLoopNumber = 1;
			videoDemo->faultXLoop = 1;
			videoDemo->faulYLoop = 1;
		}
	}

	else if (faultLine == 4)
	{
		if (videoDemo->faultTutorial == true)
		{
			videoDemo->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
		}
		else if (videoDemo->faultTutorial == false)
		{
			faultLine++;
		}
	}

	else if (faultLine == 5)
	{
		ImGui::Text("A second fault with a smaller fault value can be applied to the same terrain to generate more detail.");

		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			faultLine++;
		}
	}

	else if (faultLine == 6)
	{
		ImGui::Text("In this example the two points will be at either end down the middle of the terrain.");

		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			faultLine++;
		}
	}

	else if (faultLine == 7)
	{
		ImGui::Text("Click the arrow to see the loop in action.");

		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			faultLine++;

			videoDemo->faultTutorial = true;
			videoDemo->faultLoopNumber = 2;
			videoDemo->faultXLoop = 1;
			videoDemo->faulYLoop = 1;
		}
	}

	else if (faultLine == 8)
	{
		if (videoDemo->faultTutorial == true)
		{
			videoDemo->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
		}
		else if (videoDemo->faultTutorial == false)
		{
			faultLine++;
		}
	}

	//after final step switch game state to allow user to use algorithm on terrain
	else if (faultLine == 9)
	{
		ImGui::Text("Through several iterations and smoothing techniques, a believable terrain could be created.");

		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			m_Terrain->Flat();
			m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
			m_Terrain->faultTutorial = true;
			videoDemo->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
			faultLine = 0;
			faultSection[3] = true;
			gameState = 11;
			camera->setPosition(0.0f, 45.0f, -30.0f);
		}
	}
	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

//gamestate 11: Show the user how to use the faulting algorithm on a terrain
void App1::faultSampleTerrain()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);


	// Build UI
	//Create Menu Bar at top of screen for user to quick travel through app
	menuBar();


	ImGui::SetWindowFontScale(1.f);
	ImGui::SetWindowPos(ImVec2(0.0f, screenHeight_ - (screenHeight_ / 3)), 0);
	ImGui::SetWindowSize(ImVec2(screenWidth_, screenHeight_ * 0.33), 0);
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Text("Camera Pos: (%.2f, %.2f, %.2f)", camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
	ImGui::SetWindowFontScale(1.f);
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);

	//show how algorithm works in steps using terrain example
	//when user completes the task, the next fault line will increase and the next task will be displayed

	if (faultLine == 0)
	{
		ImGui::Text("To begin, click the button to open the fault menu.");

		if (ImGui::Button("Open Fault Menu"))
		{
			faultLine = 1;
		}
	}

	else if (faultLine == 1)
	{
		ImGui::Text("For the first example, we will create seven fault lines through the terrain.");
		ImGui::Text("To achieve this we will set the fault iterations to seven.");
		ImGui::SliderInt("Fault Iterations", &m_Terrain->faultNumbers, 1, 100);
		m_Terrain->faultValue = 7;
		if (m_Terrain->faultNumbers == 7)
		{
			if (ImGui::Button("Fault"))
			{
				faultLine++;
			}
		}
	}

	else if (faultLine == 2)
	{
		ImGui::Text("To complete the fault, press the fault button.");
		if (ImGui::Button("Fault"))
		{
			m_Terrain->fault = true;
			m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
			m_Terrain->fault = false;
			faultLine++;
		}
	}

	else if (faultLine == 3)
	{
		ImGui::Text("The fault has been created on the terrain.");
		ImGui::Text("To help blend the terrain together we can smoothen the terrain. Click the smooth button.");
		if (ImGui::Button("Smooth"))
		{
			m_Terrain->smooth = true;
			m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
			m_Terrain->smooth = false;
			faultLine++;
		}
	}

	//after final step allow user to use algorithm on terrain or return to training menu
	else if (faultLine == 4)
	{
		ImGui::Text("Congratulations, you have completed the faulting section.");
		ImGui::Text("You can continue to use this algorithm on the terrain or continue back to the main menu");

		ImGui::SliderInt("Fault Iterations", &m_Terrain->faultNumbers, 1, 100);
		ImGui::SliderFloat("Height of Fault", &m_Terrain->faultValue, 1, 20);
		if (ImGui::Button("Fault"))
		{
			m_Terrain->fault = true;
			m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
			m_Terrain->fault = false;
		}

		if (ImGui::Button("Smooth"))
		{
			m_Terrain->smooth = true;
			m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
			m_Terrain->smooth = false;
		}
		if (ImGui::Button("Reset Terrain"))
		{
			m_Terrain->Flat();
			m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
		}
		if (ImGui::Button("Return to Training Menu"))
		{
			faultSection[3] = false;
			faultComplete = true;
			gameState = 2;
			m_Terrain->Flat();
			m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
		}
	}
	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

//gamestate 8: Show the user how the particle deposition algorithm works on a terrain
void App1::visualParticleDepositionDemoGUI()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	//Create Menu Bar at top of screen for user to quick travel through app
	menuBar();


	ImGui::SetWindowFontScale(1.f);
	ImGui::SetWindowPos(ImVec2(0.0f, screenHeight_ - (screenHeight_ / 4)), 0);
	ImGui::SetWindowSize(ImVec2(screenWidth_, screenHeight_ * 0.25), 0);
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Text("Camera Pos: (%.2f, %.2f, %.2f)", camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
	ImGui::SetWindowFontScale(1.f);
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);

	//describe algorithm in steps using video demo
	//when user clicks next button the next particle deposition line will increase and the next text will be displayed

	if (ParticleDepositionLine == 0)
	{
		ImGui::Text("The first step for the application is to pick drop a random particle on the terrain");
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (videoDemo->particleDepositionNumber == 0)
		{
			videoDemo->particleDepositionTutorial = true;
			videoDemo->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
			videoDemo->particleDepositionTutorial = false;
			videoDemo->particleDepositionNumber = 1;
		}

		if (ImGui::SmallButton("<-"))
		{
			ParticleDepositionLine++;
		}
	}

	else if (ParticleDepositionLine == 1)
	{
		ImGui::Text("This process will constantly repeat itself for the specified number of times.");

		if (videoDemo->particleDepositionNumber == 1)
		{
			videoDemo->particleDepositionTutorial = true;
			videoDemo->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
			videoDemo->particleDepositionTutorial = false;
			videoDemo->particleDepositionNumber = 2;
		}


		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			ParticleDepositionLine++;
		}
	}

	else if (ParticleDepositionLine == 2)
	{
		ImGui::Text("");
		if (videoDemo->particleDepositionNumber == 2)
		{
			videoDemo->particleDepositionTutorial = true;
			videoDemo->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
			videoDemo->particleDepositionTutorial = false;
			videoDemo->particleDepositionNumber = 3;
		}


		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			ParticleDepositionLine++;
		}
	}

	else if (ParticleDepositionLine == 3)
	{
		ImGui::Text("In this instance, the particle has fallen onto a point that is already above its neighbours.");

		if (videoDemo->particleDepositionNumber == 3)
		{
			videoDemo->particleDepositionTutorial = true;
			videoDemo->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
			videoDemo->particleDepositionTutorial = false;
			videoDemo->particleDepositionNumber = 4;
		}

		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			ParticleDepositionLine++;
		}
	}

	else if (ParticleDepositionLine == 4)
	{
		ImGui::Text("The particle will then fall randomly onto one of its surrounding neighbours, evening the terrain.");

		if (videoDemo->particleDepositionNumber == 4)
		{
			videoDemo->particleDepositionTutorial = true;
			videoDemo->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
			videoDemo->particleDepositionTutorial = false;
			videoDemo->particleDepositionNumber = 5;
		}

		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			ParticleDepositionLine++;
			particleDepocounter = 0;
		}
	}

	else if (ParticleDepositionLine == 5)
	{
		ImGui::Text("Click here to simulate the rest of the loop.");

		if (particleDepocounter < 2000)
		{
			for (int i = 0; i < 3; i++)
			{
				videoDemo->particleDepositionTutorial = true;
				videoDemo->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
				videoDemo->particleDepositionTutorial = false;
			}
		}
		particleDepocounter++;
		if (particleDepocounter >= 2000)
		{
			particleDepocounter = 0;
			ParticleDepositionLine = 6;
			videoDemo->particleDepositionNumber = 6;
		}
	}

	else if (ParticleDepositionLine == 6)
	{
	ImGui::Text("The same process can be used, but instead of adding particles, delete the particles instead, a volcanic structure an be created.");

	ImGui::SameLine(screenWidth_ / 1.1, 1);
	if (ImGui::SmallButton("<-"))
	{
		ParticleDepositionLine++;
	}
	}

	else if (ParticleDepositionLine == 7)
	{

	if (particleDepocounter < 300)
	{
		for (int i = 0; i < 3; i++)
		{
		videoDemo->particleDepositionTutorial = true;
		videoDemo->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
		videoDemo->particleDepositionTutorial = false;
		}
	}
	particleDepocounter++;
		if (particleDepocounter >= 300)
		{
		ParticleDepositionLine = 8;
		}
	}

	//after final step switch game state to allow user to use algorithm on terrain
	else if (ParticleDepositionLine == 8)
	{
	ImGui::Text("Finished.");

	ImGui::SameLine(screenWidth_ / 1.1, 1);
	if (ImGui::SmallButton("<-"))
	{
		m_Terrain->Flat();
		m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
		m_Terrain->particleDepositionTutorial = true;
		videoDemo->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
		ParticleDepositionSection[3] = true;
		ParticleDepositionLine = 0;
		gameState = 11;
		camera->setPosition(0.0f, 45.0f, -30.0f);
	}
	}
	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

//gamestate 11: Show the user how to use the particle deposition algorithm on a terrain
void App1::ParticleDepositionSampleTerrain()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	//Create Menu Bar at top of screen for user to quick travel through app
	menuBar();


	ImGui::SetWindowFontScale(1.f);
	ImGui::SetWindowPos(ImVec2(0.0f, screenHeight_ - (screenHeight_ / 3)), 0);
	ImGui::SetWindowSize(ImVec2(screenWidth_, screenHeight_ * 0.33), 0);
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Text("Camera Pos: (%.2f, %.2f, %.2f)", camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
	ImGui::SetWindowFontScale(1.f);
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);

	//show how algorithm works in steps using terrain example
	//when user completes the task, the next particle deposition line will increase and the next task will be displayed
	if (ParticleDepositionLine == 0)
	{
		ImGui::Text("To begin open the particle deposition menu.");

		if (ImGui::Button("Open Particle Deposition Menu"))
		{
			ParticleDepositionLine++;
		}
	}

	else if (ParticleDepositionLine == 1)
	{
		ImGui::Text("The first mountain we will create will a square mountain, each side having a width of 20 and 1500 particles being dropped.");
		ImGui::Text("To achieve this, set both widths to 20 and the total particles dropped to 1500");

		ImGui::SliderInt("Particle Deposition: Number of Particles Dropped", &m_Terrain->particlesDropped_, 1, 2500);
		ImGui::SliderInt("Particle Deposition: Width in X Value", &m_Terrain->particleDropX_, 1, 50);
		ImGui::SliderInt("Particle Deposition: Width in Y Value", &m_Terrain->particleDropY_, 1, 50);

		if (m_Terrain->particlesDropped_ == 1500)
		{
			if (m_Terrain->particleDropX_ == 20)
			{
				if (m_Terrain->particleDropY_ == 20)
				{
					if (ImGui::Button("Particle Deposition"))
					{
						ParticleDepositionLine++;
						m_Terrain->particleDeposition = true;
						m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
						m_Terrain->particleDeposition = false;
					}
				}
			}
		}
	}

	else if (ParticleDepositionLine == 2)
	{
		ImGui::Text("This has formed a square mountain. The dimensions can be changed to make the mountain taller, or have an increased width.");
		ImGui::Text("This time, we will set the width in the X direction to be 30, the width in the Y direction to be 15 and the total particles dropped to be 2000.");

		ImGui::SliderInt("Particle Deposition: Number of Particles Dropped", &m_Terrain->particlesDropped_, 1, 2500);
		ImGui::SliderInt("Particle Deposition: Width in X Value", &m_Terrain->particleDropX_, 1, 50);
		ImGui::SliderInt("Particle Deposition: Width in Y Value", &m_Terrain->particleDropY_, 1, 50);

		if (m_Terrain->particlesDropped_ == 2000)
		{
			if (m_Terrain->particleDropX_ == 30)
			{
				if (m_Terrain->particleDropY_ == 15)
				{
					if (ImGui::Button("Particle Deposition"))
					{
						ParticleDepositionLine++;
						m_Terrain->particleDeposition = true;
						m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
						m_Terrain->particleDeposition = false;
					}
				}
			}
		}
	}

	else if (ParticleDepositionLine == 3)
	{
		ImGui::Text("This has formed a mountain extending longer in he X direction. The same proces can be used to create a smaller mountain longer in the Y direction.");
		ImGui::Text("This time, we will set the width in the X direction to be 10, the width in the Y direction to be 20 and the total particles dropped to be 749.");

		ImGui::SliderInt("Particle Deposition: Number of Particles Dropped", &m_Terrain->particlesDropped_, 1, 2500);
		ImGui::SliderInt("Particle Deposition: Width in X Value", &m_Terrain->particleDropX_, 1, 50);
		ImGui::SliderInt("Particle Deposition: Width in Y Value", &m_Terrain->particleDropY_, 1, 50);

		if (m_Terrain->particlesDropped_ == 749)
		{
			if (m_Terrain->particleDropX_ == 10)
			{
				if (m_Terrain->particleDropY_ == 20)
				{
					if (ImGui::Button("Particle Deposition"))
					{
						ParticleDepositionLine++;
						m_Terrain->particleDeposition = true;
						m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
						m_Terrain->particleDeposition = false;
					}
				}
			}
		}
	}

	else if (ParticleDepositionLine == 4)
	{
		ImGui::Text("To fill the terrain up with mountains, we can do this process multiple times in quick succession.");
		ImGui::Text("To do this, move the number of iterations to 300.");
		ImGui::SliderInt("Number of Iterations", &particleDepoIteration, 1, 300);

		if (particleDepoIteration == 300)
		{
			if (ImGui::Button("Particle Deposition"))
			{
				m_Terrain->particleDropX_ = 20;
				m_Terrain->particleDropY_ = 20;
				m_Terrain->particlesDropped_ = 850;
				for (int i = 0; i < 100; i++)
				{
					m_Terrain->particleDeposition = true;
					m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
					m_Terrain->particleDeposition = false;
				}
				m_Terrain->particleDropX_ = 25;
				m_Terrain->particleDropY_ = 10;
				m_Terrain->particlesDropped_ = 1000;
				for (int i = 0; i < 100; i++)
				{
					m_Terrain->particleDeposition = true;
					m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
					m_Terrain->particleDeposition = false;
				}
				m_Terrain->particleDropX_ = 15;
				m_Terrain->particleDropY_ = 25;
				m_Terrain->particlesDropped_ = 1250;
				for (int i = 0; i < 100; i++)
				{
					m_Terrain->particleDeposition = true;
					m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
					m_Terrain->particleDeposition = false;
				}
				ParticleDepositionLine++;
			}
		}
	}

	else if (ParticleDepositionLine == 5)
	{
		ImGui::Text("Once you have created multiple mountains that overlap you can blend them together using the smooth button. Try this now.");

		if (ImGui::Button("Smooth"))
		{
					ParticleDepositionLine++;
					m_Terrain->smooth = true;
					m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
					m_Terrain->smooth = false;
		}
	}

	//after final step allow user to use algorithm on terrain or return to training menu
	else if (ParticleDepositionLine == 6)
	{
		ImGui::Text("Congratulations, you have completed the particle deposition section.");
		ImGui::Text("You can continue to use this algorithm on the terrain or continue back to the main menu");


		ImGui::SliderInt("Particle Deposition: Number of Particles Dropped", &m_Terrain->particlesDropped_, 1, 2500);
		ImGui::SliderInt("Particle Deposition: Width in X Value", &m_Terrain->particleDropX_, 1, 50);
		ImGui::SliderInt("Particle Deposition: Width in Y Value", &m_Terrain->particleDropY_, 1, 50);

		if (ImGui::Button("Particle Deposition"))
		{
			m_Terrain->particleDeposition = true;
			m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
			m_Terrain->particleDeposition = false;
		}
		if (ImGui::Button("Smooth"))
		{
			m_Terrain->smooth = true;
			m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
			m_Terrain->smooth = false;
		}
		if (ImGui::Button("Reset Terrain"))
		{
			m_Terrain->Flat();
			m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
		}
		if (ImGui::Button("Return to Training Menu"))
		{
			m_Terrain->Flat();
			m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
			ParticleDepositionSection[3] = false;
			particleDepositionComplete = true;
			gameState = 2;
		}
	}
	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

//gamestate 9: Show the user how the mid-point displacement algorithm works on a terrain
void App1::visualMidPointDisplacementDemoGUI()
{
	//set camera position
	camera->setPosition(3.0f, 20.0f, -10.0f);

	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	//Create Menu Bar at top of screen for user to quick travel through app
	menuBar();


	ImGui::SetWindowFontScale(1.f);
	ImGui::SetWindowPos(ImVec2(0.0f, screenHeight_ - (screenHeight_ / 4)), 0);
	ImGui::SetWindowSize(ImVec2(screenWidth_, screenHeight_ * 0.25), 0);
	ImGui::Text("FPS: %.2f", timer->getFPS()); 
	
	ImGui::SetWindowFontScale(1.f);
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);

	//describe algorithm in steps using video demo
	//when user clicks next button the next mid-point displacement line will increase and the next text will be displayed
	if (midPointLine == 0)
	{
		ImGui::Text("The first step for the application is to pick initialise the four corner values");
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (videoDemo->midPointDisplacementNumber == 0)
		{
			videoDemo->mid_point_Tutorial = true;
			videoDemo->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
			videoDemo->mid_point_Tutorial = false;
			videoDemo->midPointDisplacementNumber = 1;
		}

		if (ImGui::SmallButton("<-"))
		{
			midPointLine++;
		}
	}

	else if (midPointLine == 1)
	{
		ImGui::Text("Next, the centre of the square of the four corners is given a value, displaced slightly from the average total of these corners.");

		if (videoDemo->midPointDisplacementNumber == 1)
		{
			videoDemo->mid_point_Tutorial = true;
			videoDemo->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
			videoDemo->mid_point_Tutorial = false;
			videoDemo->midPointDisplacementNumber = 2;
		}


		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			midPointLine++;
		}
	}

	else if (midPointLine == 2)
	{
		ImGui::Text("Once the centre point has been found, the next step will be to find the centre point of the diamond shapes that have been created.");
		ImGui::Text("(as this is the first iteration the diamond will only have 3 edges with the fourth out of range of the terrain).");

		if (videoDemo->midPointDisplacementNumber == 2)
		{
			videoDemo->mid_point_Tutorial = true;
			videoDemo->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
			videoDemo->mid_point_Tutorial = false;
			videoDemo->midPointDisplacementNumber = 3;
		}


		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			midPointLine++;
		}
	}

	else if (midPointLine == 3)
	{
		ImGui::Text("The square step will then repeat itself, assiging a value to the centre point of all the squares based on the corner values.");

		if (videoDemo->midPointDisplacementNumber == 3)
		{
			videoDemo->mid_point_Tutorial = true;
			videoDemo->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
			videoDemo->mid_point_Tutorial = false;
			videoDemo->midPointDisplacementNumber = 4;
		}

		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			midPointLine++;
		}
	}

	else if (midPointLine == 4)
	{
		ImGui::Text("The diamond step will repeat itself finding the centre points of the diamonds, starting the diamond-sqaure loop.");

		if (videoDemo->midPointDisplacementNumber == 4)
		{
			videoDemo->mid_point_Tutorial = true;
			videoDemo->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
			videoDemo->mid_point_Tutorial = false;
			videoDemo->midPointDisplacementNumber = 5;
		}

		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			midPointLine++;
		}
	}

	else if (midPointLine == 5)
	{
		ImGui::Text("Square Step.");

		if (videoDemo->midPointDisplacementNumber == 5)
		{
			videoDemo->mid_point_Tutorial = true;
			videoDemo->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
			videoDemo->mid_point_Tutorial = false;
			videoDemo->midPointDisplacementNumber = 6;
		}

		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			midPointLine++;
		}

	}

	else if (midPointLine == 6)
	{
		ImGui::Text("Diamond Step.");

		if (videoDemo->midPointDisplacementNumber == 6)
		{
			videoDemo->mid_point_Tutorial = true;
			videoDemo->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
			videoDemo->mid_point_Tutorial = false;
			videoDemo->midPointDisplacementNumber = 7;
		}

		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			midPointLine++;
		}
	}

	//after final step switch game state to allow user to use algorithm on terrain
	else if (midPointLine == 7)
	{
		ImGui::Text("Finished.");
		if (ImGui::SmallButton("<-"))
		{
			m_Terrain->Flat();
			m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
			m_Terrain->midPointDisplacementTutorial = true;
			videoDemo->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
			midPointSection[3] = true;
			midPointLine = 0;
			gameState = 11;
			camera->setPosition(0.0f, 45.0f, -30.0f);
		}
	}
	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

//gamestate 11: Show the user how to use the mid-point displacement algorithm on a terrain
void App1::midPointSampleTerrain()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	//Create Menu Bar at top of screen for user to quick travel through app
	menuBar();


	ImGui::SetWindowFontScale(1.f);
	ImGui::SetWindowPos(ImVec2(0.0f, screenHeight_ - (screenHeight_ / 3)), 0);
	ImGui::SetWindowSize(ImVec2(screenWidth_, screenHeight_ * 0.33), 0);
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Text("Camera Pos: (%.2f, %.2f, %.2f)", camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
	ImGui::SetWindowFontScale(1.f);
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);

	//show how algorithm works in steps using terrain example
	//when user completes the task, the next mid-point displacement line will increase and the next task will be displayed
	if (midPointLine == 0)
	{
		ImGui::Text("To begin, open the mid point displacement menu");

		if (ImGui::Button("Open Mid Point Displacement Menu"))
		{
			midPointLine++;
		}
	}

	else if (midPointLine == 1)
	{
		ImGui::Text("The first terrain we will create will have a max height of 20 units, with a displacement error of 5 units");
		ImGui::Text("To achieve this, move the corner height values to 20 and the smoothness of the terrain to 5.");

		ImGui::SliderInt("Max Height of Corners:", &m_Terrain->cornerHeight, 1, 50);
		ImGui::SliderFloat("Smoothness of Terrain (Lower Value = smoother):", &m_Terrain->midPointDisplacementValue, 1, 30, "Pos : (%.3f)", 1.0f);
		if (m_Terrain->cornerHeight == 20)
		{
			if (m_Terrain->midPointDisplacementValue > 4.9)
			{
				if (m_Terrain->midPointDisplacementValue < 5.1)
				{
					if (ImGui::Button("Run Algorithm"))
					{
						m_Terrain->mid_point_ = true;
						m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
						m_Terrain->mid_point_ = false;
						midPointLine++;
					}
				}
			}
		}
	}
	
	else if (midPointLine == 2)
	{
		ImGui::Text("This has created a smooth terrain. If we wanted the terrain to be taller we could increase the max height of the corners. Move these to 30 and keep the smoothness to 5.");

		ImGui::SliderInt("Max Height of Corners:", &m_Terrain->cornerHeight, 1, 50);
		ImGui::SliderFloat("Smoothness of Terrain (Lower Value = smoother):", &m_Terrain->midPointDisplacementValue, 1, 30, "Pos : (%.3f)", 1.0f);
		if (m_Terrain->cornerHeight == 30)
		{
			if (m_Terrain->midPointDisplacementValue > 4)
			{
				if (m_Terrain->midPointDisplacementValue < 6)
				{
					if (ImGui::Button("Run Algorithm"))
					{
						m_Terrain->mid_point_ = true;
						m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
						m_Terrain->mid_point_ = false;
						midPointLine++;
					}
				}
			}
		}
	}
	
	else if (midPointLine == 3)
	{
		ImGui::Text("This has created a terrain which has a higher height value than the last but is smoother. We can increase the slopes in the terrain by increasing the slope value.");
		ImGui::Text("This time set the max height of the corners back to 20 but set the smoothness of the terrain to 10.");

		ImGui::SliderInt("Max Height of Corners:", &m_Terrain->cornerHeight, 1, 50);
		ImGui::SliderFloat("Smoothness of Terrain (Lower Value = smoother):", &m_Terrain->midPointDisplacementValue, 1, 30, "Pos : (%.3f)", 1.0f);
		if (m_Terrain->cornerHeight == 20)
		{
			if (m_Terrain->midPointDisplacementValue > 9)
			{
				if (m_Terrain->midPointDisplacementValue < 11)
				{
					if (ImGui::Button("Run Algorithm"))
					{
						m_Terrain->mid_point_ = true;
						m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
						m_Terrain->mid_point_ = false;
						midPointLine++;
					}
				}
			}
		}
	}

	else if (midPointLine == 4)
	{
		ImGui::Text("The slopes on these terrains are too large and do not create a good terrain. We can fix this by blending the terrain together using the smooth button.");
		ImGui::Text("This will average out the terrain height values based on their neighbouring points.");

		if (ImGui::Button("Smooth"))
		{
			midPointLine++;
			m_Terrain->smooth = true;
			m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
			m_Terrain->smooth = false;
		}
	}

	//after final step allow user to use algorithm on terrain or return to training menu
	else if (midPointLine == 5)
	{
		ImGui::Text("Congratulations, you have completed the Mid Point Displacement section.");
		ImGui::Text("You can continue to use this algorithm on the terrain or continue back to the main menu");


		ImGui::SliderInt("Max Height of Corners:", &m_Terrain->cornerHeight, 1, 50);
		ImGui::SliderFloat("Smoothness of Terrain (Lower Value = smoother):", &m_Terrain->midPointDisplacementValue, 1, 30, "Pos : (%.3f)", 1.0f);

		if (ImGui::Button("Mid Point Displacement"))
		{
			m_Terrain->mid_point_ = true;
			m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
			m_Terrain->mid_point_ = false;
		}
		if (ImGui::Button("Smooth"))
		{
			m_Terrain->smooth = true;
			m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
			m_Terrain->smooth = false;
		}
		if (ImGui::Button("Reset Terrain"))
		{
			m_Terrain->Flat();
			m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
		}
		if (ImGui::Button("Return to Training Menu"))
		{
			m_Terrain->Flat();
			m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
			midPointSection[3] = false;
			midPointDisplacementComplete = true;
			gameState = 2;
		}
	}
	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

//gamestate 10: Show the user how the voronoi regions algorithm works on a terrain
void App1::visualVoronioRegionsDemoGUI()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	//Create Menu Bar at top of screen for user to quick travel through app
	menuBar();


	ImGui::SetWindowFontScale(1.f);
	ImGui::SetWindowPos(ImVec2(0.0f, screenHeight_ - (screenHeight_ / 4)), 0);
	ImGui::SetWindowSize(ImVec2(screenWidth_, screenHeight_ * 0.25), 0);
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::SetWindowFontScale(1.f);
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);

	//describe algorithm in steps using video demo
	//when user clicks next button the next voronoi regions line will increase and the next text will be displayed
	if (VoronioRegionsLine == 0)
	{
		ImGui::Text("The first step for the application is to pick target points on the terrain");
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		
		videoDemo->voronioRegionsNumber = 0;
		if (ImGui::SmallButton("<-"))
		{
			VoronioRegionsLine++;
		}
	}

	else if (VoronioRegionsLine == 1)
	{
		ImGui::Text("For the purpose of the tutorial, 4 points have been randomly picked on the terrain and have been given a height value to help distinguise the seperate areas.");
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		
		if (videoDemo->voronioRegionsNumber == 0)
		{
			videoDemo->voronoiTutorial = true;
			videoDemo->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
			videoDemo->voronoiTutorial = false;
			videoDemo->voronioRegionsNumber = 1;
		}
		if (ImGui::SmallButton("<-"))
		{
			VoronioRegionsLine++;
		}
	}

	else if (VoronioRegionsLine == 2)
	{
		ImGui::Text("A loop will now run to for every point on the terrain to check which target point is closest to the current point being checked.");
		ImGui::Text("The point that is being checked will be assigned the same height value as the target point it is closest too.");
		ImGui::SameLine(screenWidth_ / 1.1, 1);
	
		if (ImGui::SmallButton("<-"))
		{
			VoronioRegionsLine++;
		}
	}

	else if (VoronioRegionsLine == 3)
	{
		ImGui::Text("Click here to see the loop in action.");
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		

		if (ImGui::SmallButton("<-"))
		{
			VoronioRegionsLine++;
			videoDemo->voronioXLoop = 1;
			videoDemo->voronioYLoop = 1;
		}
	}

	else if (VoronioRegionsLine == 4)
	{
		ImGui::Text("");
		ImGui::SameLine(screenWidth_ / 1.1, 1);

		if (videoDemo->voronioRegionsNumber == 1)
		{
			videoDemo->voronoiTutorial = true;
			videoDemo->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
			videoDemo->voronoiTutorial = false;
		}
		else if (videoDemo->voronioRegionsNumber == 2)
		{
			VoronioRegionsLine++;
		}
	}

	//after final step switch game state to allow user to use algorithm on terrain
	else if (VoronioRegionsLine == 5)
	{
		ImGui::Text("Finished.");
		ImGui::SameLine(screenWidth_ / 1.1, 1);


		if (ImGui::SmallButton("<-"))
		{
			m_Terrain->Flat();
			m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
			m_Terrain->voronoiRegionTutorial = true;
			videoDemo->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
			VoronioRegionsSection[3] = true;
			VoronioRegionsLine = 0;
			gameState = 11;
			camera->setPosition(0.0f, 45.0f, -30.0f);
		}
	}
	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

//gamestate 11: Show the user how to use the voronoi regions algorithm on a terrain
void App1::VoronoiRegionsSampleTerrain()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	//Create Menu Bar at top of screen for user to quick travel through app
	menuBar();


	ImGui::SetWindowFontScale(1.f);
	ImGui::SetWindowPos(ImVec2(0.0f, screenHeight_ - (screenHeight_ / 3)), 0);
	ImGui::SetWindowSize(ImVec2(screenWidth_, screenHeight_ * 0.33), 0);
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Text("Camera Pos: (%.2f, %.2f, %.2f)", camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
	ImGui::SetWindowFontScale(1.f);
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);

	//show how algorithm works in steps using terrain example
	//when user completes the task, the next voronoi regions line will increase and the next task will be displayed
	if (VoronioRegionsLine == 0)
	{
		ImGui::Text("To begin open the Voronoi Regions Menu");
	
		if (ImGui::Button("Open Voronoi Regions Menu"))
		{
			VoronioRegionsLine++;
		}
	}
	
	else if (VoronioRegionsLine == 1)
	{
		ImGui::Text("For the first example we will split the terrain into four different areas");
		ImGui::Text("To achieve this, set the slider to four Voronoi Regions");

		ImGui::SliderInt("Number of Voronoi Regions", &m_Terrain->voronoiRegions, 1, 50);

		if (m_Terrain->voronoiRegions == 4)
		{
			m_Terrain->voronoi = true;
			m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
			m_Terrain->voronoi = false;
			VoronioRegionsLine++;
		}
	}

	else if (VoronioRegionsLine == 2)
	{
		ImGui::Text("This has created three different areas in the terrain.");
		ImGui::Text("This time, we will create eight different areas. Move the slider to eight.");

		ImGui::SliderInt("Number of Voronoi Regions", &m_Terrain->voronoiRegions, 1, 50);

		if (m_Terrain->voronoiRegions == 8)
		{
			m_Terrain->voronoi = true;
			m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
			m_Terrain->voronoi = false;
			VoronioRegionsLine++;
		}
	}
	else if (VoronioRegionsLine == 3)
	{
		ImGui::Text("Congratulations, you have completed the Voronoi Regions section.");
		ImGui::Text("You can continue to use this algorithm on the terrain or continue back to the main menu");

		ImGui::SliderInt("Number of Voronoi Regions", &m_Terrain->voronoiRegions, 1, 50);

		if (ImGui::Button("Voronoi Regions"))
		{
			m_Terrain->voronoi = true;
			m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
			m_Terrain->voronoi = false;
		}
		if (ImGui::Button("Smooth"))
		{
			m_Terrain->smooth = true;
			m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
			m_Terrain->smooth = false;
		}
		if (ImGui::Button("Reset Terrain"))
		{
			m_Terrain->Flat();
			m_Terrain->Regenerate(renderer->getDevice(), renderer->getDeviceContext());
		}
		if (ImGui::Button("Return to Training Menu"))
		{
			VoronioRegionsSection[3] = false;
			voronoiRegionsComplete = true;
			gameState = 2;
		}
	}
	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}


//display the correct interface section while the user is learning about the faulting algorithm
void App1::faultGUI()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	//Set the window position and size for the GUI Interface
	ImGui::SetWindowPos(ImVec2(0.f, screenHeight_ * 0.05), 0);
	ImGui::SetWindowSize(ImVec2(screenWidth_, screenHeight_ * 0.95), 0);
	// Build UI
	//Create Menu Bar at top of screen for user to quick travel through app
	menuBar();

	//display information on what faulting is
	if (faultSection[0] == false)
	{
		faultText();
	}

	//display the pros and cons of faulting
	else if (faultSection[1] == false)
	{
		faultProsCons();
	}
	
	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

//display information explaining what faulting is
void App1::faultText()
{
	//create header
	ImGui::BeginChildFrame(0, ImVec2(200, 31));
	ImGui::SetWindowFontScale(1.75f);
	ImGui::Text("What is Faulting");
	ImGui::EndChildFrame();

	//create first text of what it is
	ImGui::SetWindowFontScale(1.f);
	ImGui::Text("Faulting is a procedural method that will create a fault line throughout a terrain, and will raise one side and lower the other in the vertical axis.");

	//button for user to progress to next text
	if (faultLine == 0)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			faultLine++;
		}
	}

	//create second text
	if (faultLine > 0)
	{
		ImGui::Text("The technique will be repeted several times, where the displacement will be refined every iteration to make more detailed appearances to the terrain.");
	}

	if (faultLine == 1)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			faultLine++;
		}
	}

	//create third text
	if (faultLine > 1)
	{
		ImGui::NewLine();
		ImGui::Text("To create a faulting algorithm the following steps are followed.");
	}

	if (faultLine == 2)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			faultLine++;
		}
	}

	//create fourth text
	if (faultLine > 2)
	{
		ImGui::Text("1. Pick two random edges on the terrain ");
	}

	if (faultLine == 3)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			faultLine++;
		}
	}

	//create fifth text
	if (faultLine > 3)
	{
		ImGui::Text("2. Pick a point on both of these edges and find the equation of the line between these points.");
	}

	if (faultLine == 4)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			faultLine++;
		}
	}

	//create sixth text
	if (faultLine > 4)
	{
		ImGui::Text("3. Determine which side every point in the terrain is on and raise or lower the point accordingly.");
	}

	if (faultLine == 5)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			faultLine++;
		}
	}

	//create seventh text
	if (faultLine > 5)
	{
		ImGui::Text("4. Repeat the previous three steps, decreasing the value of the fault that effects the terrain.");
	}

	if (faultLine == 6)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			faultLine++;
		}
	}

	//seperate sections
	ImGui::Separator();
	ImGui::NewLine();

	//create first section of when to use
	if (faultLine > 6)
	{
		//create header
		ImGui::BeginChildFrame(1, ImVec2(250, 31));
		ImGui::SetWindowFontScale(1.75f);
		ImGui::Text("When to use Faulting");
		ImGui::EndChildFrame();

		//create first text
		ImGui::SetWindowFontScale(1.f);
		ImGui::Text("Faulting in geology is where a rock will slide above or beneath the other causing a fault in the landscape.");
	}

	if (faultLine == 7)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			faultLine++;
		}
	}

	//create second text
	if (faultLine > 7)
	{

		ImGui::Text("It can be used in procedural generation when creating hilly landscape as it raises a section above the rest.");
	}

	if (faultLine == 8)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			faultLine++;
		}
	}

	//create third text
	if (faultLine > 8)
	{

		ImGui::Text("Through several repetitions of faulting with decreased effect on the terrain, it can create finer details in the hills.");
	}

	if (faultLine == 9)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			faultLine++;
		}
	}


	//complete section
	if (faultLine == 10)
	{
		ImGui::NewLine();
		if (ImGui::Button("Complete section", ImVec2(150, 30)))
		{
			faultLine = 0;
			faultSection[0] = true;
		}
	}
}

//display the pros and cons of faulting
void App1::faultProsCons()
{
	//create header
	ImGui::BeginChildFrame(0, ImVec2(200, 31));
	ImGui::SetWindowFontScale(1.75f);
	ImGui::Text("Pros of Faulting");
	ImGui::EndChildFrame();

	//create first text of pros
	ImGui::SetWindowFontScale(1.f);
	ImGui::Text("1. Faulting combines well with other procedural algorithms when they are being used at the same time.");

	//button for user to progress to next text
	if (faultLine == 0)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			faultLine++;
		}
	}

	//create second text
	if (faultLine > 0)
	{
		ImGui::Text("2. You can stop the algorithm at any step whenever you want to see the result so you can see what effect it has had on the terrain at any given state.");
	}

	if (faultLine == 1)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			faultLine++;
		}
	}

	//create third text
	if (faultLine > 1)
	{
		
		ImGui::Text("3. It has been used in other terrains in the past and has given good terrain models, so the algortihm has been tried and tested.");
	}

	if (faultLine == 2)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			faultLine++;
		}
	}

	//seperate sections
	ImGui::Separator();
	ImGui::NewLine();

	if (faultLine > 2)
	{
		//create header
		ImGui::BeginChildFrame(1, ImVec2(200, 31));
		ImGui::SetWindowFontScale(1.75f);
		ImGui::Text("Cons of Faulting");
		ImGui::EndChildFrame();

		//create first text of pros
		ImGui::SetWindowFontScale(1.f);
		ImGui::Text("1. The algorithm will need to repeat itself a lot of times to get a believable terrain if faulting is the only procedural algorithm being used.");
	}

	//button for user to progress to next text
	if (faultLine == 3)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			faultLine++;
		}
	}

	//create second text
	if (faultLine > 3)
	{
		ImGui::Text("2. The terrain will need smoothing stages throughout to help the algorithm to make the terrain more believable.");
	}

	if (faultLine == 4)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			faultLine++;
		}
	}

	//complete section
	if (faultLine == 5)
	{
		ImGui::NewLine();
		if (ImGui::Button("Complete section", ImVec2(150, 30)))
		{
			faultLine = 0;
			faultSection[1] = true;
			gameState = 7;
		}
	}
}


//display the correct interface section while the user is learning about the mid-point displacement algorithm
void App1::midPointGUI()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	//Set the window position and size for the GUI Interface
	ImGui::SetWindowPos(ImVec2(0.f, screenHeight_ * 0.05), 0);
	ImGui::SetWindowSize(ImVec2(screenWidth_, screenHeight_ * 0.95), 0);
	// Build UI
	//Create Menu Bar at top of screen for user to quick travel through app
	menuBar();


	//display information on what mid-point displacement is
	if (midPointSection[0] == false)
	{
		midPointText();
	}
	//display the pros and cons of mid-point displacement
	else if (midPointSection[1] == false)
	{
		midPointProsCons();
	}
	
	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

//display information explaining what mid-point displacement is
void App1::midPointText()
{
	//create header
	ImGui::BeginChildFrame(0, ImVec2(376, 31));
	ImGui::SetWindowFontScale(1.75f);
	ImGui::Text("What is Mid-Point Displacement");
	ImGui::EndChildFrame();

	//create first text of what it is
	ImGui::SetWindowFontScale(1.f);
	ImGui::Text("Midpoint Displacement is a procedural method used to procedurally generate mountainous terrains.");

	//button for user to progress to next text
	if (midPointLine == 0)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			midPointLine++;
		}
	}

	//create second text
	if (midPointLine > 0)
	{
		ImGui::Text("To begin the algorithm, the four corners of the terrain are assigned and displaced a random value within the pre set limits.");
	}

	if (midPointLine == 1)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			midPointLine++;
		}
	}

	//create third text
	if (midPointLine > 1)
	{
		ImGui::Text("Next, the first square step is performed, where the position of the middle of the suare of the four initial corners is found.");
	}

	if (midPointLine == 2)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			midPointLine++;
		}
	}

	//create fourth text
	if (midPointLine > 2)
	{
		ImGui::Text("The average height of the corners of the square is calculated and slightly randomly altered and this value will be used to displace");
		ImGui::Text("the point at the middle of the square.");
	}

	if (midPointLine == 3)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			midPointLine++;
		}
	}

	//create fifth text
	if (midPointLine > 3)
	{
		ImGui::Text("The reason the height value is displaced is to help create randomness to the terrain to help make it unique.");
	}

	if (midPointLine == 4)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			midPointLine++;
		}
	}

	//create fifth text
	if (midPointLine > 4)
	{
		ImGui::Text("The next step is the diamond step, where a diamond formation of existing points is found and the new target point to be displaced ");
		ImGui::Text("will be the centre of the diamond.");
	}

	if (midPointLine == 5)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			midPointLine++;
		}
	}

	//create sixth text
	if (midPointLine > 5)
	{
		ImGui::Text("The same process of displacing this new point as used in the square step will occur generating a new point on the terrain");
		ImGui::Text("to be used in future steps.");
	}

	if (midPointLine == 6)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			midPointLine++;
		}
	}

	//create seventh text
	if (midPointLine > 6)
	{
		ImGui::Text("The process will then repeat itself moving back to the square step, decreasing the size of the squares and diamonds until all");
		ImGui::Text("the points have been filled, decreasing the random offset applied to the new point after every iteration");

	}

	if (midPointLine == 7)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			midPointLine++;
		}
	}

	//create seventh text
	if (midPointLine > 7)
	{
		ImGui::Text("The reason the average random offset value decreases is to ensure the terrain does not have any sharp inclines and to help keep");
		ImGui::Text("adjacent points as smooth as possible.");

	}

	if (midPointLine == 8)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			midPointLine++;
		}
	}


	//seperate sections
	ImGui::Separator();
	ImGui::NewLine();

	//create first section of when to use
	if (midPointLine > 8)
	{
		//create header
		ImGui::BeginChildFrame(1, ImVec2(426, 31));
		ImGui::SetWindowFontScale(1.75f);
		ImGui::Text("When to use Mid Point Dispacement");
		ImGui::EndChildFrame();

		//create first text
		ImGui::SetWindowFontScale(1.f);
		ImGui::Text("Midpoint Displacement is very good at generating mountainous terrains.");
	}

	if (midPointLine == 9)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			midPointLine++;
		}
	}

	//create second text
	if (midPointLine > 9)
	{

		ImGui::Text("This could be generating 2D mountains in the background for platformers or generating 3D mountainous to explore.");
	}

	if (midPointLine == 10)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			midPointLine++;
		}
	}

	//create third text
	if (midPointLine > 10)
	{

		ImGui::Text("You have the ability to determine how steep you want the mountain to be by increasing or decreasing the maximum or minimum value that");
		ImGui::Text("the initial four corners of the terrain can be.");
		if (midPointLine == 11)
		{
			ImGui::SameLine(screenWidth_ / 1.1, 1);
			if (ImGui::SmallButton("<-"))
			{
				midPointLine++;
			}
		}

	}

		//complete section
		if (midPointLine == 12)
		{
			ImGui::NewLine();
			if (ImGui::Button("Complete section", ImVec2(150, 30)))
			{
				midPointLine = 0;
				midPointSection[0] = true;
			}
		}
	
}

//display the pros and cons of mid-point displacement
void App1::midPointProsCons()
{
	//create header
	ImGui::BeginChildFrame(0, ImVec2(376, 31));
	ImGui::SetWindowFontScale(1.75f);
	ImGui::Text("Pros of Mid Point Displacement");
	ImGui::EndChildFrame();

	//create first text of pros
	ImGui::SetWindowFontScale(1.f);
	ImGui::Text("1. Midpoint Displacement can create very realistic mountainous terrains.");

	//button for user to progress to next text
	if (midPointLine == 0)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			midPointLine++;
		}
	}

	//create second text
	if (midPointLine > 0)
	{
		ImGui::Text("2. The midpoint displacement algorithm is very cheap to process.");
	}

	if (midPointLine == 1)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			midPointLine++;
		}
	}

	//create third text
	if (midPointLine > 1)
	{

		ImGui::Text("3. The process to create terrains using Midpoint displacement is not very hard to understand.");
	}

	if (midPointLine == 2)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			midPointLine++;
		}
	}

	//seperate sections
	ImGui::Separator();
	ImGui::NewLine();

	if (midPointLine > 2)
	{
		//create header
		ImGui::BeginChildFrame(1, ImVec2(426, 31));
		ImGui::SetWindowFontScale(1.75f);
		ImGui::Text("Cons of Mid Point Displacement");
		ImGui::EndChildFrame();

		//create first text of pros
		ImGui::SetWindowFontScale(1.f);
		ImGui::Text("1. Midpoint Displacement requires an odd number of vertices on each axis that forms the plane.");
	}
	//button for user to progress to next text

	if (midPointLine == 3)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			midPointLine++;
		}
	}

	//create second text
	if (midPointLine > 3)
	{
		ImGui::Text("2. The midpoint displacement algorithm is very costly on memory usage as it needs to store every single points on the terrains height value.");
	}

	if (midPointLine == 4)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			midPointLine++;
		}
	}

	//complete section
	if (midPointLine == 5)
	{
		ImGui::NewLine();
		if (ImGui::Button("Complete section", ImVec2(150, 30)))
		{
			midPointLine = 0;
			videoDemo->midPointDisplacementNumber = 0;
			midPointSection[1] = true;
			gameState = 9;
		}
	}
}


//display the correct interface section while the user is learning about the particle deposition algorithm
void App1::ParticleDepositionGUI()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	//Set the window position and size for the GUI Interface
	ImGui::SetWindowPos(ImVec2(0.f, screenHeight_ * 0.05), 0);
	ImGui::SetWindowSize(ImVec2(screenWidth_, screenHeight_ * 0.95), 0);

	// Build UI
	//Create Menu Bar at top of screen for user to quick travel through app
	menuBar();


	//display information on what particle deposition is
	if (ParticleDepositionSection[0] == false)
	{
		ParticleDepositionText();
	}
	//display the pros and cons of particle deposition
	else if (ParticleDepositionSection[1] == false)
	{
		ParticleDepositionProsCons();
	}

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

//display information explaining what particle deposition is
void App1::ParticleDepositionText()
{
	//create header
	ImGui::BeginChildFrame(0, ImVec2(376, 31));
	ImGui::SetWindowFontScale(1.75f);
	ImGui::Text("What is Particle Deposition");
	ImGui::EndChildFrame();

	//create first text of what it is
	ImGui::SetWindowFontScale(1.f);
	ImGui::Text("Particle Deposition Creates mountainous terrains by dropping particles onto the height map in a certain region.");

	//button for user to progress to next text
	if (ParticleDepositionLine == 0)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			ParticleDepositionLine++;
		}
	}

	//create second text
	if (ParticleDepositionLine > 0)
	{
		ImGui::Text("To begin the algorithm, a random point on the terrain will be selected.");
	}

	if (ParticleDepositionLine == 1)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			ParticleDepositionLine++;
		}
	}

	//create third text
	if (ParticleDepositionLine > 1)
	{
		ImGui::Text("The user will select a radius from the chosen point to be the zone where the particles are able to be dropped and");
		ImGui::Text("the number of particles that will be dropped.");
	}

	if (ParticleDepositionLine == 2)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			ParticleDepositionLine++;
		}
	}

	//create fourth text
	if (ParticleDepositionLine > 2)
	{
		ImGui::Text("The algorithm will loop for the number of times of particle dropped, dropping a single particle onto a random location");
		ImGui::Text("within the boundaries of the zone.");
	}

	if (ParticleDepositionLine == 3)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			ParticleDepositionLine++;
		}
	}

	//create fifth text
	if (ParticleDepositionLine > 3)
	{
		ImGui::Text("If a particle has been dropped onto the terrain, where the height of the particle is higher than its surrounding neighbours,");
		ImGui::Text("the particle will fall onto a random neighbour that is beneath the original point.");
	}

	if (ParticleDepositionLine == 4)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			ParticleDepositionLine++;
		}
	}
	//create fifth text
	if (ParticleDepositionLine > 4)
	{
		ImGui::Text("This will make sure there is not any random points that will be abnormally higher than its surrounding neighbours losing the  ");
		ImGui::Text("believability of the terrain.");
	}

	if (ParticleDepositionLine == 5)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			ParticleDepositionLine++;
		}
	}



	//seperate sections
	ImGui::Separator();
	ImGui::NewLine();

	//create first section of when to use
	if (ParticleDepositionLine > 5)
	{
		//create header
		ImGui::BeginChildFrame(1, ImVec2(426, 31));
		ImGui::SetWindowFontScale(1.75f);
		ImGui::Text("When to use Particle Deposition");
		ImGui::EndChildFrame();

		//create first text
		ImGui::SetWindowFontScale(1.f);
		ImGui::Text("Particle Deposition is very good at generating mountainous and volcanoes.");
	}

	if (ParticleDepositionLine == 6)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			ParticleDepositionLine++;
		}
	}

	//create second text
	if (ParticleDepositionLine > 6)
	{

		ImGui::Text("The algorithm can create mountains or volcanoes in the pre existing terrains to help create more features.");
	}

	if (ParticleDepositionLine == 7)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			ParticleDepositionLine++;
		}
	}

	//create third text
	if (ParticleDepositionLine > 7)
	{

		ImGui::Text("It is mainly used on existing terrains as it can not fill in a terrain with detail by itself unles multiple iterations of different values are ran");
		if (ParticleDepositionLine == 8)
		{
			ImGui::SameLine(screenWidth_ / 1.1, 1);
			if (ImGui::SmallButton("<-"))
			{
				ParticleDepositionLine++;
			}
		}

	}

	//complete section
	if (ParticleDepositionLine == 9)
	{
		ImGui::NewLine();
		if (ImGui::Button("Complete section", ImVec2(150, 30)))
		{
			ParticleDepositionLine = 0;
			ParticleDepositionSection[0] = true;
		}
	}

}

//display the pros and cons of particle deposition
void App1::ParticleDepositionProsCons()
{
	//create header
	ImGui::BeginChildFrame(0, ImVec2(376, 31));
	ImGui::SetWindowFontScale(1.75f);
	ImGui::Text("Pros of Particle Deposition");
	ImGui::EndChildFrame();

	//create first text of pros
	ImGui::SetWindowFontScale(1.f);
	ImGui::Text("1. Particle Deposition can create mountainous and volcanic structures very well.");

	//button for user to progress to next text
	if (ParticleDepositionLine == 0)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			ParticleDepositionLine++;
		}
	}

	//create second text
	if (ParticleDepositionLine > 0)
	{
		ImGui::Text("2. Particle Deposition combines very well with other techniques.");
	}

	if (ParticleDepositionLine == 1)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			ParticleDepositionLine++;
		}
	}


	//seperate sections
	ImGui::Separator();
	ImGui::NewLine();

	if (ParticleDepositionLine > 1)
	{
		//create header
		ImGui::BeginChildFrame(1, ImVec2(426, 31));
		ImGui::SetWindowFontScale(1.75f);
		ImGui::Text("Cons of Particle Deposition");
		ImGui::EndChildFrame();

		//create first text of pros
		ImGui::SetWindowFontScale(1.f);
		ImGui::Text("1. Particle Deposition requires multiple iterations before a believable result is obtained.");
	}

	//button for user to progress to next text
	if (ParticleDepositionLine == 2)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			ParticleDepositionLine++;
		}
	}

	//create second text
	if (ParticleDepositionLine > 2)
	{
		ImGui::Text("2. Particle Deposition is not as well suited to larger terrains.");
	}

	if (ParticleDepositionLine == 3)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			ParticleDepositionLine++;
		}
	}

	//complete section
	if (ParticleDepositionLine == 4)
	{
		ImGui::NewLine();
		if (ImGui::Button("Complete section", ImVec2(150, 30)))
		{
			ParticleDepositionLine = 0;
			ParticleDepositionSection[1] = true;
			gameState = 8;
			videoDemo->particleDepositionNumber = 0;
		}
	}
}


//display the correct interface section while the user is learning about the voronoi regions algorithm
void App1::VoronioRegionsGUI()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	//Set the window position and size for the GUI Interface
	ImGui::SetWindowPos(ImVec2(0.f, screenHeight_ * 0.05), 0);
	ImGui::SetWindowSize(ImVec2(screenWidth_, screenHeight_ * 0.95), 0);
	// Build UI
	//Create Menu Bar at top of screen for user to quick travel through app
	menuBar();


	//display information on what voronoi regions is
	if (VoronioRegionsSection[0] == false)
	{
		VoronioRegionsText();
	}
	//display the pros and cons of voronoi regions
	else if (VoronioRegionsSection[1] == false)
	{
		VoronioRegionsProsCons();
	}

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

//display information explaining what voronoi regions is
void App1::VoronioRegionsText()
{
	//create header
	ImGui::BeginChildFrame(0, ImVec2(376, 31));
	ImGui::SetWindowFontScale(1.75f);
	ImGui::Text("What is Voronio Regions");
	ImGui::EndChildFrame();

	//create first text of what it is
	ImGui::SetWindowFontScale(1.f);
	ImGui::Text("Voronio Regions splts up the terrain into a specified number of regions.");

	//button for user to progress to next text
	if (VoronioRegionsLine == 0)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			VoronioRegionsLine++;
		}
	}

	//create second text
	if (VoronioRegionsLine > 0)
	{
		ImGui::Text("To begin the algorithm, the user will determine how many zones they would like to create.");
	}

	if (VoronioRegionsLine == 1)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			VoronioRegionsLine++;
		}
	}

	//create third text
	if (VoronioRegionsLine > 1)
	{
		ImGui::Text("This will run a loop for the number of times the user has chosen, dropping a random particle onto the terrain every iteration.");
	}

	if (VoronioRegionsLine == 2)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			VoronioRegionsLine++;
		}
	}

	//create fourth text
	if (VoronioRegionsLine > 2)
	{
		ImGui::Text("Once all the particles have dropped onto the terrain, a loop will run for every point on the terrain.");	
	}

	if (VoronioRegionsLine == 3)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			VoronioRegionsLine++;
		}
	}

	//create fifth text
	if (VoronioRegionsLine > 3)
	{
		ImGui::Text("The loop will run a check using the distance formula to check which of the initial dropped points is nearest to the current point being checked.");
	}

	if (VoronioRegionsLine == 4)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			VoronioRegionsLine++;
		}
	}
	//create fifth text
	if (VoronioRegionsLine > 4)
	{
		ImGui::Text("Once the nearest point has been found, the current point that is being checked will be assigned a value that will match the nearest point.");	
	}

	if (VoronioRegionsLine == 5)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			VoronioRegionsLine++;
		}
	}



	//seperate sections
	ImGui::Separator();
	ImGui::NewLine();

	//create first section of when to use
	if (VoronioRegionsLine > 5)
	{
		//create header
		ImGui::BeginChildFrame(1, ImVec2(426, 31));
		ImGui::SetWindowFontScale(1.75f);
		ImGui::Text("When to use Voronoi Regions");
		ImGui::EndChildFrame();

		//create first text
		ImGui::SetWindowFontScale(1.f);
		ImGui::Text("In real world situation, Voronoi Regions can be used to determine the nearest shop to your location.");
	}

	if (VoronioRegionsLine == 6)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			VoronioRegionsLine++;
		}
	}

	//create second text
	if (VoronioRegionsLine > 6)
	{

		ImGui::Text("For Example, if you searched on Google maps to find the nearest restaurant, it will mark all the near restaurants with a point.");
		ImGui::Text("and will determine which shop region you are in.");
	}

	if (VoronioRegionsLine == 7)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			VoronioRegionsLine++;
		}
	}

	//create third text
	if (VoronioRegionsLine > 7)
	{

		ImGui::Text("In terrain generation it can be used to split up the terrain into certain areas.");
		if (VoronioRegionsLine == 8)
		{
			ImGui::SameLine(screenWidth_ / 1.1, 1);
			if (ImGui::SmallButton("<-"))
			{
				VoronioRegionsLine++;
			}
		}
	}
	
	if (VoronioRegionsLine > 8)
	{

		ImGui::Text("For Example, If you wanted to create certain biomes in your terrain, you could choose how many you would like and drop those points onto the terrain.");
		ImGui::Text("The Voronoi Regions Algorithm would then create the biomes based on what region each point would lay in.");
		if (VoronioRegionsLine == 9)
		{
			ImGui::SameLine(screenWidth_ / 1.1, 1);
			if (ImGui::SmallButton("<-"))
			{
				VoronioRegionsLine++;
			}
		}
	}

	//complete section
	if (VoronioRegionsLine == 10)
	{
		ImGui::NewLine();
		if (ImGui::Button("Complete section", ImVec2(150, 30)))
		{
			VoronioRegionsLine = 0;
			VoronioRegionsSection[0] = true;
		}
	}

}

//display the pros and cons of voronoi regions
void App1::VoronioRegionsProsCons()
{
	//create header
	ImGui::BeginChildFrame(0, ImVec2(376, 31));
	ImGui::SetWindowFontScale(1.75f);
	ImGui::Text("Pros of Voronoi Regions");
	ImGui::EndChildFrame();

	//create first text of pros
	ImGui::SetWindowFontScale(1.f);
	ImGui::Text("1. Voronoi Regions are very easy to understand.");

	//button for user to progress to next text
	if (VoronioRegionsLine == 0)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			VoronioRegionsLine++;
		}
	}

	//create second text
	if (VoronioRegionsLine > 0)
	{
		ImGui::Text("2. Voronoi Regions does not use a lot of memory.");
	}

	if (VoronioRegionsLine == 1)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			VoronioRegionsLine++;
		}
	}

	//create second text
	if (VoronioRegionsLine > 1)
	{
		ImGui::Text("3. Voronoi Regions Algorithm is quick to process.");
	}

	if (VoronioRegionsLine == 2)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			VoronioRegionsLine++;
		}
	}

	//seperate sections
	ImGui::Separator();
	ImGui::NewLine();

	if (VoronioRegionsLine > 2)
	{
		//create header
		ImGui::BeginChildFrame(1, ImVec2(426, 31));
		ImGui::SetWindowFontScale(1.75f);
		ImGui::Text("Cons of Voronoi Regions");
		ImGui::EndChildFrame();

		//create first text of pros
		ImGui::SetWindowFontScale(1.f);
		ImGui::Text("1. Voronoi Regions can not create techniques by itself.");
	}

	//button for user to progress to next text
	if (VoronioRegionsLine == 3)
	{
		ImGui::SameLine(screenWidth_ / 1.1, 1);
		if (ImGui::SmallButton("<-"))
		{
			VoronioRegionsLine++;
		}
	}

	//complete section
	if (VoronioRegionsLine == 4)
	{
		ImGui::NewLine();
		if (ImGui::Button("Complete section", ImVec2(150, 30)))
		{
			VoronioRegionsLine = 0;
			VoronioRegionsSection[1] = true;
			gameState = 10;
		}
	}
}