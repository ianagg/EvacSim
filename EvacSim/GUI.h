#pragma once
#include <Windows.h>
#include <GLFW\glfw3.h>
#include <vector>
#include <mutex>

class GUI {
private:
	static GUI instance;
	static bool initDone;

	static std::mutex buttonMtx;

	GUI() {};

	void init(HINSTANCE hInst, HWND hWnd);

	void toMainMenu();
	void toEditor();
	void toSimulation();

	static void wmCmdHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	HINSTANCE hInst;
	HWND hWnd;
	HWND loadBtn, createBtn, editBtn, obstacleBtn, stairBtn, personBtn, fireBtn, newFloorBtn, endBtn, saveBtn, startBtn, pauseBtn, settingsBtn, menuBtn;//could have remove floor btn but may be difficult
	
	using loadFunc = bool(*)();
	using createFunc = void(*)(int, int);
	using editFunc = bool(*)();

	using obstacleFunc = void(*)();
	using stairFunc = void(*)();
	using personFunc = void(*)();
	using fireFunc = void(*)();
	//using newFloorFunc = void(*)();
	using newFloorFunc1 = std::vector<int>(*)();
	using newFloorFunc2 = void(*)(int, int);
	using endFunc = void(*)();
	using saveFunc = bool(*)();

	using startFunc = void(*)();
	using pauseFunc = void(*)();
	using settingsFunc = void(*)(int, int);

	using menuFunc = void(*)();

	loadFunc loadBtnAction;
	createFunc createBtnAction;
	editFunc editBtnAction;

	obstacleFunc obstacleBtnAction;
	stairFunc stairBtnAction;
	personFunc personBtnAction;
	fireFunc fireBtnAction;
	newFloorFunc1 newFloorBtnAction;
	newFloorFunc2 newFloorOKAction;
	endFunc endBtnAction;
	saveFunc saveBtnAction;

	startFunc startBtnAction;
	pauseFunc pauseBtnAction;
	settingsFunc settingsBtnAction;

	menuFunc menuBtnAction;

	//max button size in pixels
	//size for main menu butons
	int menuMaxW = 150;
	int menuMaxH = 84;
	//int maxW = 150;
	//int maxH = 84;
	//size for other buttons
	int maxW = 100;
	int maxH = 40;

	//button size
	double wPerc = 0.2;
	double hPerc = 0.15;

	//button position parameters
	double gapPercX = 0.05;
	//double gapPercY = 0.04;
	double gapPercY = 0.03;
	double leftAllignPerc = 0.05;
	double centreAllignPerc = 0.5;
	double rightAllignPerc = 0.85;
	double topAllignPerc = 0.05;

public:
	static GUI * initGUI(GLFWwindow *window);

	~GUI() {};

	void resize(int width, int height);

	//make private?
	static INT_PTR CALLBACK createMapDlg(HWND hDlg1, UINT message, WPARAM wParam, LPARAM lParam);

	static INT_PTR CALLBACK createNewFloorDlg(HWND hDlg1, UINT message, WPARAM wParam, LPARAM lParam);

	static INT_PTR CALLBACK param(HWND hDlg1, UINT message, WPARAM wParam, LPARAM lParam);


	inline void setLoadFunc(loadFunc action) {
		this->loadBtnAction = action;
	}

	inline void setCreateFunc(createFunc action) {
		this->createBtnAction = action;
	}

	inline void setEditFunc(editFunc action) {
		this->editBtnAction = action;
	}

	inline void setObstacleFunc(obstacleFunc action) {
		this->obstacleBtnAction = action;
	}

	inline void setStairFunc(stairFunc action) {
		this->stairBtnAction = action;
	}

	inline void setPersonFunc(personFunc action) {
		this->personBtnAction = action;
	}

	inline void setFireFunc(fireFunc action) {
		this->fireBtnAction = action;
	}

	inline void setNewFloorFunc(newFloorFunc1 action1, newFloorFunc2 action2) {
		this->newFloorBtnAction = action1;
		this->newFloorOKAction = action2;
	}

	inline void setEndFunc(endFunc action) {
		this->endBtnAction = action;
	}

	inline void setSaveFunc(saveFunc action) {
		this->saveBtnAction = action;
	}

	inline void setStartFunc(startFunc action) {
		this->startBtnAction = action;
	}

	inline void setPauseFunc(pauseFunc action) {
		this->pauseBtnAction = action;
	}

	inline void setSetingsFunc(settingsFunc action) {
		this->settingsBtnAction = action;
	}

	inline void setMenuFunc(menuFunc action) {
		this->menuBtnAction = action;
	}

	inline bool lockButtonMutex() {
		return buttonMtx.try_lock();
	}

	inline void unlockButtonMutex() {
		buttonMtx.unlock();
	}

	inline HWND getWnd() {
		return this->hWnd;
	}

	inline void displayStats(std::string stats) {
		MessageBox(hWnd, stats.c_str(), "Statistics", MB_OK);
	}

	GUI(GUI const&) = delete;
	void operator=(GUI const&) = delete;
};