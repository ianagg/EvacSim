#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <glad\glad.h>
//#include <Windows.h>
//#include <GLFW\glfw3.h>
#include "GUI.h"
#include <GLM\glm.hpp>
#include <GLM\gtc\matrix_transform.hpp>
//#define GLFW_EXPOSE_NATIVE_WIN32
//#include <GLFW\glfw3native.h>

#include <iostream>
#include <chrono>
#include "ShaderLoader.h"
#include "Person.h"
#include "Grid.h"
#include "AStar.h"
#include "Camera.h"
#include <map>
#include "FileUtils.h"
#include <ShObjIdl.h>
#define _WINNT_WIN32 0600
#include <sstream>
#include "Building.h"

//#include "vld.h"

using ms = std::chrono::milliseconds;
using get_time = std::chrono::steady_clock;

unsigned int WIDTH = 640;
unsigned int HEIGHT = 360;
GLfloat viewportAspect = (GLfloat)WIDTH / (GLfloat)HEIGHT;
const GLfloat targetWidth = 32.0f / 9.0f;
const GLfloat targetHeight = 2.0f;
const GLfloat targetAspect = 16.0f / 9.0f;

glm::mat4 projectionMatrix;

bool mouseInWindow = false;

ShaderLoader shaderLoader;

Camera c = Camera();

enum class State{MainMenu, Editor, Simulation};
State currentState;
enum class Placing{Obstacle, Fire, Stairs, Person, Exit};
Placing currentlyPlacing;

enum class SimState{NotStarted, Running, Paused, EndSim};
SimState currentSimState;

Building *building = nullptr;

GUI *gui = nullptr;

//timer
std::chrono::time_point<std::chrono::system_clock> startTime, endTime;
int elapsed_seconds;

bool finish = false;
bool resetTick = false;


void cursorEnterCallback(GLFWwindow *window, int entered) {
	if (entered) {
		mouseInWindow = true;
	}
	else {
		mouseInWindow = false;
	}
}

void transformCursorCoords(double &cursorX, double &cursorY);
void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
	//will be able to just change to State::Editor
	if (currentState != State::MainMenu && action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT && mouseInWindow) {
		if (gui->lockButtonMutex()) {
			double cursorX;
			double cursorY;
			glfwGetCursorPos(window, &cursorX, &cursorY);
			transformCursorCoords(cursorX, cursorY);

			switch (currentState) {
				case State::Simulation: {

					break;
				}
				case State::Editor: {
					switch (currentlyPlacing) {
						case Placing::Obstacle: {
							building->placeObstacle(cursorX, cursorY);
							break;
						}

						case Placing::Stairs: {
							building->placeStairs(cursorX, cursorY);
							break;
						}

						case Placing::Person: {
							building->placePersonStart(cursorX, cursorY);
							break;
						}

						case Placing::Fire: {
							building->placeFire(cursorX, cursorY);
							break;
						}

						case Placing::Exit: {
							building->placeExit(cursorX, cursorY);
							break;
						}
					}
					break;
				}
			}

			gui->unlockButtonMutex();
		}
		else {
			//std::cout << "could not lock mutex" << std::endl;
		}
	}
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
	WIDTH = width;
	HEIGHT = height;
	glViewport(0, 0, width, height);

	gui->resize(WIDTH, HEIGHT);

	viewportAspect = static_cast<GLfloat>(width)/ static_cast<GLfloat>(height);

	//window is wider than or equal to target aspect ratio
	if (viewportAspect >= targetAspect) {
		projectionMatrix = glm::ortho
		(
			(-viewportAspect / targetAspect * targetWidth / 2.0f * c.getZoom()) + c.getX(),
			(viewportAspect / targetAspect * targetWidth / 2.0f * c.getZoom()) + c.getX(),
			(-targetHeight / 2.0f * c.getZoom()) + c.getY(),
			(targetHeight / 2.0f * c.getZoom()) + c.getY(),
			-1.0f, 1.0f
		);
	}
	//window is taller than target aspect ratio
	else {
		projectionMatrix = glm::ortho
		(
			(-targetWidth / 2.0f * c.getZoom()) + c.getX(),
			(targetWidth / 2.0f * c.getZoom()) + c.getX(),
			(-targetAspect / viewportAspect * targetHeight / 2.0f * c.getZoom()) + c.getY(),
			(targetAspect / viewportAspect * targetHeight / 2.0f * c.getZoom()) + c.getY(),
			-1.0f, 1.0f
		);
	}
}

void windowSizeCallback(GLFWwindow *window, int width, int height) {
	glfwSetWindowSize(window, width, height);
}

void scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
	c.changeZoom(xoffset, yoffset);
}

void cleanup() {
	if (building != nullptr) {
		delete building;
		building = nullptr;
	}

	c.reset();
	framebufferSizeCallback(nullptr, WIDTH, HEIGHT);
}

std::string fileDlg(IFileDialog *fd, HRESULT &res) {
	std::string fName;
	DWORD dwFlags;

	res = fd->GetOptions(&dwFlags);

	if (SUCCEEDED(res)) {
		res = fd->SetOptions(dwFlags | FOS_FORCEFILESYSTEM);

		if (SUCCEEDED(res)) {
			COMDLG_FILTERSPEC rgSpec[] = {
				{L"Binary file", L"*.bin"}
			};
			res = fd->SetFileTypes(ARRAYSIZE(rgSpec), rgSpec);

			if (SUCCEEDED(res)) {
				res = fd->SetFileTypeIndex(1);

				if (SUCCEEDED(res)) {
					res = fd->SetDefaultExtension(L"bin");

					if (SUCCEEDED(res)) {
						res = fd->Show(NULL);

						if (SUCCEEDED(res)) {
							IShellItem *pItem;
							res = fd->GetResult(&pItem);

							if (SUCCEEDED(res)) {
								wchar_t *pszFilePath;//LPWSTR??
								res = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

								if (SUCCEEDED(res)) {
									std::wstringstream ss;
									ss << pszFilePath;
									std::wstring ws = ss.str();
									//std::wcout << ws << std::endl;

									int wsLen = ws.length() + 1;
									const wchar_t *orig = ws.c_str();
									size_t converted = 0;

									//not platform independent, wchar_t is 4 bytes on linux
									size_t sLen = wsLen * 2;
									char *nString = new char[sLen];

									wcstombs_s(&converted, nString, sLen, orig, _TRUNCATE);

									fName = std::string(nString);

									delete[] nString;
									CoTaskMemFree(pszFilePath);
								}
								pItem->Release();
							}
						}
					}
				}
			}
		}
	}

	fd->Release();
	CoUninitialize();

	return fName;
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		std::string fName;

		switch (key) {
			case GLFW_KEY_UP: {
				c.setHeldU(true);
				break;
			}

			case GLFW_KEY_DOWN: {
				c.setHeldD(true);
				break;
			}

			case GLFW_KEY_LEFT: {
				c.setHeldL(true);
				break;
			}

			case GLFW_KEY_RIGHT: {
				c.setHeldR(true);
				break;
			}
		}
	}
	else if (action == GLFW_RELEASE) {
		switch (key) {
			case GLFW_KEY_UP: {
				c.setHeldU(false);
				break;
			}

			case GLFW_KEY_DOWN: {
				c.setHeldD(false);
				break;
			}

			case GLFW_KEY_LEFT: {
				c.setHeldL(false);
				break;
			}

			case GLFW_KEY_RIGHT: {
				c.setHeldR(false);
				break;
			}
		}
	}
}

void transformCursorCoords(double &cursorX, double &cursorY) {
	double oldX = cursorX;
	double oldY = cursorY;
	cursorX /= WIDTH;
	cursorX -= 0.5;
	cursorY /= HEIGHT;
	cursorY -= 0.5;

	glm::vec4 viewport = glm::vec4(0, 0, WIDTH, HEIGHT);
	glm::vec3 screenPos = glm::vec3(oldX, HEIGHT - oldY - 1, 0.0f);
	glm::vec3 worldPos = glm::unProject(screenPos, c.getViewMat(), projectionMatrix, viewport);

	if (viewportAspect >= targetAspect) {
		cursorX *= viewportAspect / targetAspect * targetWidth;

		cursorY *= targetHeight;
	}
	else {
		cursorX *= targetWidth;

		cursorY *= targetAspect / viewportAspect * targetHeight;
	}

	cursorX *= 1.0f;
	cursorY *= -1.0f;

	cursorX *= c.getZoom();
	cursorY *= c.getZoom();

	cursorX += c.getX();
	cursorY += c.getY();
}

std::string generateStats() {
	if (startTime != endTime) {
		endTime = std::chrono::system_clock::now();

		elapsed_seconds += std::chrono::duration_cast<std::chrono::milliseconds> (endTime - startTime).count();

		startTime = endTime;
	}

	std::stringstream t;
	int p = building->getEveryone();
	t << "Current speed: " << Person::getSpeed() / 0.0001 << std::endl;
	t << "Current fire speed: " << (2020 - GridCell::getSpeed()) / 20 << std::endl;
	t << "Alive: " << p - building->getDeadone() << "/" << p << std::endl;
	t << "Dead: " << building->getDeadone() << "/" << p << std::endl;
	t << "Time: " << (double)elapsed_seconds / 1000 << "s" << std::endl;

	return t.str();
}

void updateScene(GLFWwindow *window) {
	switch (currentState) {
		case State::MainMenu: {
		finish = false;
		break;
		}

		case State::Editor: {
			if (c.update()) {
				framebufferSizeCallback(window, WIDTH, HEIGHT);
			}
			break;
		}

		case State::Simulation: {
			if (currentSimState == SimState::Running) {
				building->updateScene();
			}

			if (building->getEnd()) {
				currentSimState = SimState::EndSim;
			}


			if (currentSimState == SimState::EndSim && !finish) {
				gui->displayStats(generateStats());
				finish = true;
				//displayStats(gui->getWnd());
			}
			if (c.update()) {
				framebufferSizeCallback(window, WIDTH, HEIGHT);
			}
			break;
		}
	}
}

void renderScene(GLint program, GLfloat interpolation) {
	/* Render here */
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (currentState == State::Editor || currentState == State::Simulation) {
		glUseProgram(program);
		glUniformMatrix4fv(glGetUniformLocation(program, "ProjectionMatrix"), 1, GL_FALSE, &projectionMatrix[0][0]);


		building->draw(interpolation, currentState == State::Simulation);

		glUseProgram(0);
	}
}

DWORD getTickCount() {
	return std::chrono::duration_cast<ms>(get_time::now().time_since_epoch()).count();
}

//Begin button actions
//Actions for buttons on Main menu
bool loadBtnAction() {
	//try + catch
	HRESULT res = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(res)) {
		IFileOpenDialog *pFileOpen;
		res = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

		if (SUCCEEDED(res)) {
			std::string fName = fileDlg(pFileOpen, res);
			int length = fName.length();
			if (length > 4) {//must be greater than 4 because ".bin" on the end
				std::string fileEx = fName.substr(length - 4, 4);
				if ((fileEx.compare(std::string(".bin"))) != 0) {
					return false;
				}
				building = new Building(shaderLoader.getProgram(), true, fName);
				if (building->getLoadResult()) {
					currentState = State::Simulation;
					return true;
				}
				else {
					cleanup();
					return false;
				}
			}
		}
	}

	return false;
}

void createBtnOKAction(int cols, int rows) {
	cleanup();
	building = new Building(shaderLoader.getProgram(), cols, rows);
	currentState = State::Editor;
}

bool editBtnAction() {
	HRESULT res = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(res)) {
		IFileOpenDialog *pFileOpen;
		res = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

		if (SUCCEEDED(res)) {
			std::string fName = fileDlg(pFileOpen, res);
			int length = fName.length();
			if (length > 4) {//must be greater than 4 because ".bin" on the end
				std::string fileEx = fName.substr(length - 4, 4);
				if ((fileEx.compare(std::string(".bin"))) != 0) {
					std::cout << "hi" << std::endl;
					return false;
				}
				building = new Building(shaderLoader.getProgram(), false, fName);
				if (building->getLoadResult()) {
					currentState = State::Editor;
					return true;
				}
				else {
					cleanup();
					return false;
				}
			}
		}
	}

	return false;
}

//Actions for buttons in Editor
void obstacleBtnAction() {
	currentlyPlacing = Placing::Obstacle;
	building->resetStairPlacement();
}

void stairBtnAction() {
	currentlyPlacing = Placing::Stairs;
}

void personBtnAction() {
	currentlyPlacing = Placing::Person;
	building->resetStairPlacement();
}

void fireBtnAction() {
	currentlyPlacing = Placing::Fire;
	building->resetStairPlacement();
}

void settingsBtnAction(int personSpeed, int fireSpeed) {

	if (currentSimState != SimState::EndSim) {
		if (personSpeed != -1) {
			Person::setSpeed(personSpeed*0.0001);
		}
		if (fireSpeed != -1) {
			GridCell::setSpeed(2020 - fireSpeed * 20);
		}

		currentState = State::Simulation;
	}
}

std::vector<int> newFloorBtnAction() {
	std::vector<int> result;
	//get valid poistions to place the floor on the map
	building->getValidMapPositions(result);

	//get valid (logical) floor number - current min-1 and max+1
	building->getValidFloorNumbers(result);

	building->resetStairPlacement();

	return result;
}

void newFloorOKAction(int floorNum, int floorPosition) {
	//actually creates the new floor
	building->addFloor(floorNum, floorPosition);
	building->resetStairPlacement();
}

void endBtnAction() {
	currentlyPlacing = Placing::Exit;
	building->resetStairPlacement();
}

bool saveBtnAction() {
	building->resetStairPlacement();

	HRESULT res = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(res)) {
		IFileSaveDialog *pFileSave;
		res = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));

		if (SUCCEEDED(res)) {
			std::string fName = fileDlg(pFileSave, res);
			int length = fName.length();
			if (length > 4) {//must be greater than 4 because ".bin" on the end
				std::string fileEx = fName.substr(length - 4, 4);
				if ((fileEx.compare(std::string(".bin"))) != 0) {
					return false;
				}
				else {
					building->saveMap(fName);
					return true;
				}
			}
		}
	}

	return false;
}

//Actions for buttons in Simulation
void startBtnAction() {
	switch (currentSimState) {
	case SimState::NotStarted: {
		startTime = std::chrono::system_clock::now();
		elapsed_seconds = 0;

		building->moveToExit();
		resetTick = true;
		currentSimState = SimState::Running;
		break;
	}

	case SimState::Running: {
		//do nothing
		break;
	}

	case SimState::EndSim: {
		break;
	}

	case SimState::Paused: {
		startTime = std::chrono::system_clock::now();
		currentSimState = SimState::Running;
		break;
	}
	}
}

void pauseBtnAction() {
	if (currentSimState == SimState::Running) {
		currentSimState = SimState::Paused;
	}

	gui->displayStats(generateStats());
}


//Actions for buttons on Editor and Simulation
void menuBtnAction() {
	cleanup();
	currentState = State::MainMenu;
	currentlyPlacing = Placing::Obstacle;
	currentSimState = SimState::NotStarted;
}
//End button actions

int init(GLFWwindow *&window) {

	srand(time(NULL));

	/* Initialize the library */
	if (!glfwInit()) {
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(WIDTH, HEIGHT, "Evacuation Simulator", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	currentState = State::MainMenu;

	glfwSetCursorEnterCallback(window, cursorEnterCallback);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetWindowSizeCallback(window, windowSizeCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);

	gui = GUI::initGUI(window);
	gui->setLoadFunc(loadBtnAction);
	gui->setCreateFunc(createBtnOKAction);
	gui->setEditFunc(editBtnAction);

	gui->setObstacleFunc(obstacleBtnAction);
	gui->setStairFunc(stairBtnAction);
	gui->setPersonFunc(personBtnAction);
	gui->setFireFunc(fireBtnAction);
	gui->setNewFloorFunc(newFloorBtnAction, newFloorOKAction);
	gui->setEndFunc(endBtnAction);
	gui->setSaveFunc(saveBtnAction);

	gui->setStartFunc(startBtnAction);
	gui->setPauseFunc(pauseBtnAction);
	gui->setSetingsFunc(settingsBtnAction);


	gui->setMenuFunc(menuBtnAction);

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_MULTISAMPLE);

	/* Load shader files */
	shaderLoader = ShaderLoader("basic.vert", "basic.frag");

	framebufferSizeCallback(window, WIDTH, HEIGHT);

	//Group::setGrids(&grids);
	//Person::setGrids(&grids);

	currentSimState = SimState::NotStarted;
	currentlyPlacing = Placing::Obstacle;
	currentState = State::MainMenu;

	return 1;
}

int main(void) {
	GLFWwindow *window = nullptr;

	if (!init(window) || window == nullptr) {
		return -1;
	}
	
	const int TICKS_PER_SECOND = 120;
	const int SKIP_TICKS = 1000 / TICKS_PER_SECOND;
	const int MAX_FRAMESKIP = 10;
	DWORD next_game_tick = GetTickCount();
	int loops;
	float interpolation;

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window)) {

		loops = 0;
		if (resetTick) {
			next_game_tick = GetTickCount();
			resetTick = false;
		}
		else {
			while (GetTickCount() > next_game_tick && loops < MAX_FRAMESKIP) {
				updateScene(window);
				next_game_tick += SKIP_TICKS;
				loops++;
			}

			if (currentState == State::MainMenu) {
				interpolation = 0.0f;
			}
			else {
				interpolation = static_cast<float>(GetTickCount() + SKIP_TICKS - next_game_tick)
					/ static_cast<float>(SKIP_TICKS);
			}

			renderScene(shaderLoader.getProgram(), interpolation);

			/* Swap front and back buffers */
			glfwSwapBuffers(window);
			/* Poll for and process events */
			glfwPollEvents();
		}
	}

	/* Clean-up */

	cleanup();
	//delete building;

	//delete gui;

	glfwTerminate();
	return 0;
}