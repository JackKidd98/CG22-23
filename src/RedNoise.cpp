#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>

#define WIDTH 320
#define HEIGHT 240

std::vector<float>interpolateSingleFloats (float from, float to, float numberOfValues){
	float jump = (to - from) / (numberOfValues - 1);

	std::vector<float> result;

	for(int i = 0; i<numberOfValues; i++){
		result.push_back(from + jump * i);
	}
	return result;
}

std::vector<glm::vec3> interpolateThreeElementValues(glm::vec3 from, glm::vec3 to, int numberOfValues){
	glm::vec3 jump = (to - from) / (glm::vec3(numberOfValues - 1, numberOfValues -1, numberOfValues - 1));
	std::vector<glm::vec3> result;

	for(int i = 0; i < numberOfValues; i++){
		result.push_back(glm::vec3(from + float(i) * jump));
	}
	return result;
}

void draw(DrawingWindow &window) {
	window.clearPixels();

	glm::vec3 topLeft(255, 0, 0);        // red 
	glm::vec3 topRight(0, 0, 255);       // blue 
	glm::vec3 bottomRight(0, 255, 0);    // green 
	glm::vec3 bottomLeft(255, 255, 0);   // yellow

	for(float y = 0; y < window.height; y++){
		glm::vec3 left = (topLeft * (1 - (y / window.height)) + (bottomLeft * (y / window.height)));
		glm::vec3 right = (topRight * (1 - (y / window.height)) + (bottomRight * (y / window.height)));

		for(float x = 0; x < window.width; x++){
			glm::vec3 xaxis = (left * (1-(x/window.width)) + (right * (x/window.width)));
			uint32_t colour = (255 << 24) + (int(xaxis[0]) << 16) + (int(xaxis[1]) << 8) + int(xaxis[2]);
			window.setPixelColour(x, y, colour);
		}
	}
}

void handleEvent(SDL_Event event, DrawingWindow &window) {
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_LEFT) std::cout << "LEFT" << std::endl;
		else if (event.key.keysym.sym == SDLK_RIGHT) std::cout << "RIGHT" << std::endl;
		else if (event.key.keysym.sym == SDLK_UP) std::cout << "UP" << std::endl;
		else if (event.key.keysym.sym == SDLK_DOWN) std::cout << "DOWN" << std::endl;
	} else if (event.type == SDL_MOUSEBUTTONDOWN) {
		window.savePPM("output.ppm");
		window.saveBMP("output.bmp");
	}
}

int main(int argc, char *argv[]) {
	DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	SDL_Event event;
	glm::vec3 from(1, 4, 9.2);
	glm::vec3 to(4, 1, 9.8);
	std::vector<glm:: vec3> result = interpolateThreeElementValues(from, to, 4);

	for(size_t i = 0; i < result.size(); i++){
		std::cout << result[i][0]<<" ";
		std::cout << result[i][1]<<" ";
		std::cout << result[i][2]<<" ";
		std::cout << std::endl;
	}
	std::cout << std::endl;
	
	while (true) {
		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(event)) handleEvent(event, window);
		draw(window);
		// Need to render the frame at the end, or nothing actually gets shown on the screen !
		window.renderFrame();

		std::vector<float> result;
		//result = interpolateSingleFloats(2.2, 8.5, 7);
		//for(size_t i=0; i<result.size(); i++) std::cout << result[i] << " ";
		//std::cout << std::endl;
	}
}
