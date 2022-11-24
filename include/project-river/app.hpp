#pragma once

#include <thread>
#include <iostream>

#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "ui.hpp"

namespace gui 
{
	void setup();
	void begin();
	float estimateSystemScale();
	GLFWwindow* getCurrentWindow();
	void setSwapInterval(int newSwapInterval);
	void setClearColor(int red, int green, int blue, int alpha);
	void setGuiScale(float guiScale);
}