#include "GUI.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW\glfw3native.h>
#include "resource.h"
#include <sstream>
#include <iostream>

//main menu buttons
#define LOAD_BUTTON 1001
#define CREATE_BUTTON 1002
#define EDIT_BUTTON 1003

//editor buttons
#define OBSTACLE_BUTTON 1004
#define STAIR_BUTTON 1005
#define PERSON_BUTTON 1006
#define FIRE_BUTTON 1007
#define FLOOR_BUTTON 1008
#define END_BUTTON 1009
#define SAVE_BUTTON 1010

//simulation buttons
#define START_BUTTON 1011
#define PAUSE_BUTTON 1012
#define SETTINGS_BUTTON 1013

//editor and simulation
#define MENU_BUTTON 1014

GUI GUI::instance;
bool GUI::initDone = false;
std::mutex GUI::buttonMtx;

void GUI::init(HINSTANCE hInst, HWND hWnd) {
	this->hInst = hInst;
	this->hWnd = hWnd;

	//Main menu buttons
	loadBtn = CreateWindow("BUTTON", "Load map", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 10, 100, 100, hWnd, (HMENU)LOAD_BUTTON, hInst, NULL);

	createBtn = CreateWindow("BUTTON", "Create map", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 160, 10, 100, 100, hWnd, (HMENU)CREATE_BUTTON, hInst, NULL);

	editBtn = CreateWindow("BUTTON", "Edit map", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 310, 10, 100, 100, hWnd, (HMENU)EDIT_BUTTON, hInst, NULL);


	//Editor buttons
	obstacleBtn = CreateWindow("BUTTON", "Obstacle", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 10, 100, 100, hWnd, (HMENU)OBSTACLE_BUTTON, hInst, NULL);
	ShowWindow(obstacleBtn, SW_HIDE);

	stairBtn = CreateWindow("BUTTON", "Stairs", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 160, 100, 100, hWnd, (HMENU)STAIR_BUTTON, hInst, NULL);
	ShowWindow(stairBtn, SW_HIDE);

	personBtn = CreateWindow("BUTTON", "Person", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 160, 10, 100, 100, hWnd, (HMENU)PERSON_BUTTON, hInst, NULL);
	ShowWindow(personBtn, SW_HIDE);

	fireBtn = CreateWindow("BUTTON", "Fire", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 160, 160, 100, 100, hWnd, (HMENU)FIRE_BUTTON, hInst, NULL);
	ShowWindow(fireBtn, SW_HIDE);

	newFloorBtn = CreateWindow("BUTTON", "Add floor", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 160, 310, 100, 100, hWnd, (HMENU)FLOOR_BUTTON, hInst, NULL);
	ShowWindow(newFloorBtn, SW_HIDE);

	endBtn = CreateWindow("BUTTON", "End point", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 310, 100, 100, hWnd, (HMENU)END_BUTTON, hInst, NULL);
	ShowWindow(endBtn, SW_HIDE);

	saveBtn = CreateWindow("BUTTON", "Save", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 160, 460, 100, 100, hWnd, (HMENU)SAVE_BUTTON, hInst, NULL);
	ShowWindow(saveBtn, SW_HIDE);

	//Simulation buttons
	startBtn = CreateWindow("BUTTON", "Start", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 10, 100, 100, hWnd, (HMENU)START_BUTTON, hInst, NULL);
	ShowWindow(startBtn, SW_HIDE);

	pauseBtn = CreateWindow("BUTTON", "Pause", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 160, 100, 100, hWnd, (HMENU)PAUSE_BUTTON, hInst, NULL);
	ShowWindow(pauseBtn, SW_HIDE);

	settingsBtn = CreateWindow("BUTTON", "Settings", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 310, 100, 100, hWnd, (HMENU)SETTINGS_BUTTON, hInst, NULL);
	ShowWindow(settingsBtn, SW_HIDE);

	//Editor and simulation
	menuBtn = CreateWindow("BUTTON", "Main menu", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 460, 100, 100, hWnd, (HMENU)MENU_BUTTON, hInst, NULL);
	ShowWindow(menuBtn, SW_HIDE);

	glfwSetWMCmdHandler(wmCmdHandler);
}

void GUI::toMainMenu() {
	ShowWindow(loadBtn, SW_SHOW);
	ShowWindow(createBtn, SW_SHOW);
	ShowWindow(editBtn, SW_SHOW);

	ShowWindow(obstacleBtn, SW_HIDE);
	ShowWindow(stairBtn, SW_HIDE);
	ShowWindow(personBtn, SW_HIDE);
	ShowWindow(fireBtn, SW_HIDE);
	ShowWindow(newFloorBtn, SW_HIDE);
	ShowWindow(endBtn, SW_HIDE);
	ShowWindow(saveBtn, SW_HIDE);

	ShowWindow(startBtn, SW_HIDE);
	ShowWindow(pauseBtn, SW_HIDE);
	ShowWindow(settingsBtn, SW_HIDE);


	ShowWindow(menuBtn, SW_HIDE);
}

void GUI::toEditor() {
	ShowWindow(loadBtn, SW_HIDE);
	ShowWindow(createBtn, SW_HIDE);
	ShowWindow(editBtn, SW_HIDE);

	ShowWindow(obstacleBtn, SW_SHOW);
	ShowWindow(stairBtn, SW_SHOW);
	ShowWindow(personBtn, SW_SHOW);
	ShowWindow(fireBtn, SW_SHOW);
	ShowWindow(newFloorBtn, SW_SHOW);
	ShowWindow(endBtn, SW_SHOW);
	ShowWindow(saveBtn, SW_SHOW);

	ShowWindow(startBtn, SW_HIDE);
	ShowWindow(pauseBtn, SW_HIDE);
	ShowWindow(settingsBtn, SW_HIDE);


	ShowWindow(menuBtn, SW_SHOW);
}

void GUI::toSimulation() {
	ShowWindow(loadBtn, SW_HIDE);
	ShowWindow(createBtn, SW_HIDE);
	ShowWindow(editBtn, SW_HIDE);

	ShowWindow(obstacleBtn, SW_HIDE);
	ShowWindow(stairBtn, SW_HIDE);
	ShowWindow(personBtn, SW_HIDE);
	ShowWindow(fireBtn, SW_HIDE);
	ShowWindow(newFloorBtn, SW_HIDE);
	ShowWindow(endBtn, SW_HIDE);
	ShowWindow(saveBtn, SW_HIDE);

	ShowWindow(startBtn, SW_SHOW);
	ShowWindow(pauseBtn, SW_SHOW);
	ShowWindow(settingsBtn, SW_SHOW);


	ShowWindow(menuBtn, SW_SHOW);
}

void GUI::resize(int width, int height) {
	int x, y, w, h;
	int buttonGapX = static_cast<int>(static_cast<double>(width) * gapPercX);
	int buttonGapY = static_cast<int>(static_cast<double>(height) * gapPercY);

	int topGap = static_cast<int>(static_cast<double>(height) * topAllignPerc);
	int bottomGap = static_cast<int>(static_cast<double>(height) * topAllignPerc);

	int availHeight = height - topGap - bottomGap - (buttonGapY * 5);

	int editH = static_cast<int>(static_cast<double>(availHeight) / 6.0);

	//button size
	w = static_cast<int>(static_cast<double>(width) * wPerc);
	int mw = w;
	if (w > maxW) {
		w = maxW;
		mw = menuMaxW;
	}
	h = static_cast<int>(static_cast<double>(height) * hPerc);
	int mh = h;
	if (h > maxH) {
		h = maxH;
		mh = menuMaxH;
	}

	//positions for main menu buttons
	x = static_cast<int>(static_cast<double>(width) * leftAllignPerc);
	y = static_cast<int>(static_cast<double>(height) * topAllignPerc);
	SetWindowPos(loadBtn, HWND_TOP, x, y, mw, mh, SWP_NOZORDER | SWP_NOACTIVATE);

	x = static_cast<int>(static_cast<double>(width) * centreAllignPerc);
	x -= mw / 2;
	SetWindowPos(createBtn, HWND_TOP, x, y, mw, mh, SWP_NOZORDER | SWP_NOACTIVATE);

	x = static_cast<int>(static_cast<double>(width) * rightAllignPerc);
	x -= mw / 2;
	SetWindowPos(editBtn, HWND_TOP, x, y, mw, mh, SWP_NOZORDER | SWP_NOACTIVATE);

	//positions for editor buttons (except save)
	x = static_cast<int>(static_cast<double>(width) * leftAllignPerc);
	//y = static_cast<int>(static_cast<double>(height) * topAllignPerc);
	y = topGap;
	SetWindowPos(obstacleBtn, HWND_TOP, x, y, w, editH, SWP_NOZORDER | SWP_NOACTIVATE);

	//y += editH + static_cast<int>(static_cast<double>(height) * gapPercY);
	y += editH + buttonGapY;
	SetWindowPos(personBtn, HWND_TOP, x, y, w, editH, SWP_NOZORDER | SWP_NOACTIVATE);

	//y += editH + static_cast<int>(static_cast<double>(height) * gapPercY);
	y += editH + buttonGapY;
	SetWindowPos(fireBtn, HWND_TOP, x, y, w, editH, SWP_NOZORDER | SWP_NOACTIVATE);

	//y += editH + static_cast<int>(static_cast<double>(height) * gapPercY);
	y += editH + buttonGapY;
	SetWindowPos(stairBtn, HWND_TOP, x, y, w, editH, SWP_NOZORDER | SWP_NOACTIVATE);

	//y += editH + static_cast<int>(static_cast<double>(height) * gapPercY);
	y += editH + buttonGapY;
	SetWindowPos(endBtn, HWND_TOP, x, y, w, editH, SWP_NOZORDER | SWP_NOACTIVATE);

	//y += editH + static_cast<int>(static_cast<double>(height) * gapPercY);
	y += editH + buttonGapY;
	SetWindowPos(newFloorBtn, HWND_TOP, x, y, w, editH, SWP_NOZORDER | SWP_NOACTIVATE);

	//positions for simulation buttons
	x = static_cast<int>(static_cast<double>(width) * leftAllignPerc);
	y = static_cast<int>(static_cast<double>(height) * topAllignPerc);
	SetWindowPos(startBtn, HWND_TOP, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);

	y += h + static_cast<int>(static_cast<double>(height) * gapPercY);
	SetWindowPos(pauseBtn, HWND_TOP, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);

	y += h + static_cast<int>(static_cast<double>(height) * gapPercY);
	SetWindowPos(settingsBtn, HWND_TOP, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);

	//position for menu button
	x = static_cast<int>(static_cast<double>(width) * rightAllignPerc);
	x -= w / 2;
	y = static_cast<int>(static_cast<double>(height) * topAllignPerc);

	SetWindowPos(menuBtn, HWND_TOP, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);

	//position for save map button
	y += h + static_cast<int>(static_cast<double>(height) * gapPercY);
	SetWindowPos(saveBtn, HWND_TOP, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);

}

GUI * GUI::initGUI(GLFWwindow *window) {
	//static GUI instance;
	//could keep it declared here as static, not a ptr, just pass it to stuff? Dont think this will actually work because of wmCmdHandler
	//not a ptr should still be possible though

	//have bool for initialised or not, dont create here just init, return address
	if (!initDone) {
		HINSTANCE hInst = GetModuleHandle(NULL);
		HWND hWnd = glfwGetWin32Window(window);

		instance.init(hInst, hWnd);
		initDone = true;
	}

	return &instance;
}

void GUI::wmCmdHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	int wmId = LOWORD(wParam);

	if (buttonMtx.try_lock()) {
		switch (wmId) {
			case LOAD_BUTTON: {
				if (instance.loadBtnAction) {
					if (instance.loadBtnAction()) {
						instance.toSimulation();
					}
					else {
						MessageBox(hWnd, "Not a valid map file, it may be corrupted or have been created with another application", "Error", MB_OK);
					}
				}

				break;
			}

			case CREATE_BUTTON: {
				if (instance.createBtnAction) {
					if (DialogBox(instance.hInst, MAKEINTRESOURCE(IDD_DIALOG_NEW_BUILDING), hWnd, createMapDlg)) {
						instance.toEditor();
					}
				}

				break;
			}

			case EDIT_BUTTON: {
				if (instance.editBtnAction) {
					if (instance.editBtnAction()) {
						instance.toEditor();
					}
					else {
						MessageBox(hWnd, "Not a valid map file, it may be corrupted or have been created with another application", "Error", MB_OK);
					}
				}

				break;
			}

			case OBSTACLE_BUTTON: {
				if (instance.obstacleBtnAction) {
					instance.obstacleBtnAction();
				}
				break;
			}

			case STAIR_BUTTON: {
				if (instance.stairBtnAction) {
					instance.stairBtnAction();
				}
				break;
			}

			case PERSON_BUTTON: {
				if (instance.personBtnAction) {
					instance.personBtnAction();
				}
				break;
			}

			case FIRE_BUTTON: {
				if (instance.fireBtnAction) {
					instance.fireBtnAction();
				}
				break;
			}

			case FLOOR_BUTTON: {
				if (instance.newFloorBtnAction && instance.newFloorOKAction) {
					if (DialogBox(instance.hInst, MAKEINTRESOURCE(IDD_DIALOG_FLOOR), hWnd, createNewFloorDlg)) {
						MessageBox(hWnd, "New floor created successfully", "Success", MB_OK);
					}
				}
				break;
			}

			case END_BUTTON: {
				if (instance.endBtnAction) {
					instance.endBtnAction();
				}
				break;
			}

			case SAVE_BUTTON: {
				if (instance.saveBtnAction) {
					if (instance.saveBtnAction()) {
						MessageBox(hWnd, "Map saved successfully", "Success", MB_OK);
					}
					else {
						MessageBox(hWnd, "Failed to save map, please ensure that you have write permissions to the folder and are using the .bin file extension", "Error", MB_OK);
					}
				}
				break;
			}

			case START_BUTTON: {
				if (instance.startBtnAction) {
					instance.startBtnAction();
				}
				break;
			}

			case PAUSE_BUTTON: {
				if (instance.pauseBtnAction) {
					instance.pauseBtnAction();
				}
				break;
			}

			case SETTINGS_BUTTON: {

				if (instance.settingsBtnAction) {
					if (DialogBox(instance.hInst, MAKEINTRESOURCE(IDD_DIALOG_SETTINGS), hWnd, param)) {
						instance.toSimulation();
					}

				}
				break;
			}

			case MENU_BUTTON: {
				if (instance.menuBtnAction) {
					instance.menuBtnAction();
					instance.toMainMenu();
				}
				break;
			}
		}

		SetFocus(hWnd);

		buttonMtx.unlock();
	}
}

INT_PTR CALLBACK GUI::createNewFloorDlg(HWND hDlg1, UINT message, WPARAM wParam, LPARAM lParam) {
	//BOOL error;
	INT_PTR result = (INT_PTR)FALSE;

	UNREFERENCED_PARAMETER(lParam);

	switch (message) {
		case WM_INITDIALOG: {
			//populate the combo boxes with valid values
			std::vector<int> r = instance.newFloorBtnAction();
			int max = r.back();
			r.pop_back();
			int min = r.back();
			r.pop_back();
			HWND comboHandle = GetDlgItem(hDlg1, IDC_COMBO_FLOOR_NUM);
			SendMessage(comboHandle, CB_ADDSTRING, (WPARAM)0, (LPARAM)(std::to_string(max).c_str()));
			SendMessage(comboHandle, CB_ADDSTRING, (WPARAM)0, (LPARAM)(std::to_string(min).c_str()));
			SendMessage(comboHandle, CB_SETCURSEL, 0, 0);

			if (r.size() > 0) {
				comboHandle = GetDlgItem(hDlg1, IDC_COMBO_FLOOR_POSITION);
				for (int i = 0; i < r.size(); i++) {
					SendMessage(comboHandle, CB_ADDSTRING, (WPARAM)i, (LPARAM)(std::to_string(r.at(i)).c_str()));
				}
				SendMessage(comboHandle, CB_SETCURSEL, 0, 0);
			}
			else {
				MessageBox(hDlg1, "Maximum number of floors reached", "Error", MB_OK);
				EndDialog(hDlg1, result);
				break;
			}

			return (INT_PTR)TRUE;
		}
		
		case WM_COMMAND: {
			if (HIWORD(wParam) == CBN_SELCHANGE) {
				//may not need
			}
			switch (LOWORD(wParam)) {
				case IDOK: {
					HWND comboHandle = GetDlgItem(hDlg1, IDC_COMBO_FLOOR_POSITION);
					int itemIndex = SendMessage(comboHandle, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
					TCHAR  selectedItem[256];
					(TCHAR)SendMessage(comboHandle, (UINT)CB_GETLBTEXT, (WPARAM)itemIndex, (LPARAM)selectedItem);

					std::string selectedStr = std::string(selectedItem);
					int floorPosition = stoi(selectedStr);


					comboHandle = GetDlgItem(hDlg1, IDC_COMBO_FLOOR_NUM);
					itemIndex = SendMessage(comboHandle, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
					selectedItem[256];
					(TCHAR)SendMessage(comboHandle, (UINT)CB_GETLBTEXT, (WPARAM)itemIndex, (LPARAM)selectedItem);

					selectedStr = std::string(selectedItem);
					int floorNum = stoi(selectedStr);

					instance.newFloorOKAction(floorNum, floorPosition);

					result = (INT_PTR)TRUE;
					EndDialog(hDlg1, result);
					break;
				}
				case IDCANCEL: {
					result = (INT_PTR)FALSE;
					EndDialog(hDlg1, result);
					break;
				}
			}
			break;
		}
	}

	return result;
}

INT_PTR CALLBACK GUI::createMapDlg(HWND hDlg1, UINT message, WPARAM wParam, LPARAM lParam) {
	BOOL error;
	int columns;
	int rows;
	INT_PTR result = (INT_PTR)FALSE;

	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
		case WM_INITDIALOG: {
			return (INT_PTR)TRUE;
		}
		
		case WM_COMMAND: {
			switch (LOWORD(wParam)) {
				case IDOK: {
					columns = GetDlgItemInt(hDlg1, IDC_EDIT_COLS, &error, true);
					rows = GetDlgItemInt(hDlg1, IDC_EDIT_ROWS, &error, true);

					std::stringstream ss;
					if (error == 0) {
						ss << "Please only enter numbers" << std::endl;;
					}
					else {
						if (columns < 5) {
							ss << "Minimum number of columns is 5" << std::endl;
						}
						else if (columns > 50) {
							ss << "Maximum number of columns is 50" << std::endl;
						}

						if (rows < 5) {
							ss << "Minimum number of rows is 5" << std::endl;
						}
						else if (rows > 50) {
							ss << "Maximum number of rows is 50" << std::endl;
						}
					}

					if (error == 0 || rows < 5 || rows > 50 || columns < 5 || columns > 50) {
						std::string e = ss.str();
						MessageBox(hDlg1, e.c_str(), "Error", MB_OK);
					}
					else {
						instance.createBtnAction(columns, rows);
						result = (INT_PTR)TRUE;
						EndDialog(hDlg1, result);
					}
					break;
				}
				case IDCANCEL: {
					result = (INT_PTR)FALSE;
					EndDialog(hDlg1, result);
					break;
				}
			}
			break;
		}
	}

	return result;
}

INT_PTR CALLBACK GUI::param(HWND hDlg1, UINT message, WPARAM wParam, LPARAM lParam) {
	BOOL error;
	int pSpeed;
	int fSpeed;
	INT_PTR result = (INT_PTR)FALSE;


	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
	case WM_INITDIALOG: {

		return (INT_PTR)TRUE;
	}
	case WM_COMMAND: {
		switch (LOWORD(wParam)) {
		case IDOK: {

			pSpeed = GetDlgItemInt(hDlg1, IDC_EDIT_P_SPEED, &error, true);
			fSpeed = GetDlgItemInt(hDlg1, IDC_EDIT_F_SPEED, &error, true);

		

			std::stringstream ss;
			if (error == 0) {
				ss << "Please only enter numbers" << std::endl;;

			}
			else {
				if (pSpeed < 1 && pSpeed != -1) {
					ss << "Minimum number of person's speed is 1" << std::endl;
				}
				else if (pSpeed > 100 && pSpeed != -1) {
					ss << "Maximum number  of person's speed  is 100" << std::endl;
				}

				if (fSpeed < 1 && fSpeed != -1) {
					ss << "Minimum number of fire speed is 1" << std::endl;
				}
				else if (fSpeed > 100 && fSpeed != -1) {
					ss << "Maximum number of fire speed is 100" << std::endl;
				}
			}

			if (error == 0 || (pSpeed < 1 && pSpeed != -1) || pSpeed > 100 || (fSpeed < 1 && fSpeed != -1) || fSpeed > 100) {
				std::string e = ss.str();
				MessageBox(hDlg1, e.c_str(), "Error", MB_OK);
			}


			else {
				instance.settingsBtnAction(pSpeed, fSpeed);
				result = (INT_PTR)TRUE;
				EndDialog(hDlg1, result);
			}
			break;
		}
		case IDCANCEL: {
			result = (INT_PTR)FALSE;
			EndDialog(hDlg1, result);
			break;
		}
		}
		break;
	}
	}

	return result;
}