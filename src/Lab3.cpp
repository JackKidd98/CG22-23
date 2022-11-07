#include <CanvasTriangle.h>
#include <DrawingWindow.h>
#include <Utils.h>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>
#include <CanvasPoint.h>
#include <Colour.h>
#include "time.h"
#include <ModelTriangle.h>
#include <unordered_map>
#include <iostream>
#include <fstream> 
#include <TextureMap.h>

#define WIDTH 320
#define HEIGHT 240

uint32_t convertColour(Colour colour){
    return ((255 << 24) + (colour.red << 16) + (colour.green << 8) + colour.blue);

}

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

void drawLine(DrawingWindow& window, CanvasPoint from, CanvasPoint to, Colour colour){
	
    float xDiff = to.x - from.x;
	float yDiff = to.y - from.y;
	
	int numberOfSteps = std::max(std::abs(xDiff), std::abs(yDiff) + 1);

	float xStepSize = xDiff / numberOfSteps;
	float yStepSize = yDiff / numberOfSteps;

	for(int i = 0; i < numberOfSteps; i++){
		float x = from.x + (xStepSize * i);
		float y = from.y + (yStepSize * i);

		window.setPixelColour(x, y, convertColour(colour));
	}
}

CanvasTriangle randomTrianglePoints(){

	CanvasPoint point1((float)(rand() % WIDTH), (float)(rand() % HEIGHT));
	CanvasPoint point2((float)(rand() % WIDTH), (float)(rand() % HEIGHT));
	CanvasPoint point3((float)(rand() % WIDTH), (float)(rand() % HEIGHT));

	CanvasTriangle triangle(point1, point2, point3);

	return triangle;

}

std::vector<CanvasPoint> interpolateCanvPoint(CanvasPoint from, CanvasPoint to, int numberOfValues){
	
	std::vector<CanvasPoint> val(numberOfValues);
	glm::vec3 intervals = (glm::vec3(to.x, to.y, to.depth) - glm::vec3(from.x, from.y, from.depth)) / (float)numberOfValues;

	for (int i = 0; i < numberOfValues; i++){
		glm::vec3 res = floor(glm::vec3(from.x, from.y, from.depth) + (float)i * intervals);
		val[i] = CanvasPoint(res.x, res.y, res.z);
	}

	return val;

}

void drawTriangle(DrawingWindow& window, CanvasTriangle triangle, Colour colour){

	drawLine(window, triangle.v0(), triangle.v1(), colour);
	drawLine(window, triangle.v1(), triangle.v2(), colour);
	drawLine(window, triangle.v2(), triangle.v0(), colour);

}

std::vector<CanvasPoint> sortVertices(std::vector<CanvasPoint> vertices){

	if (vertices[0].y > vertices[1].y){
		std::swap(vertices[0], vertices[1]);
	}
	
	if (vertices[1].y > vertices[2].y){
		std::swap(vertices[1], vertices[2]);
	}

	if (vertices[0].y > vertices[1].y){
		std::swap(vertices[0], vertices[1]);
	}
	
	return vertices;
}

CanvasPoint triangleExtraPoint(CanvasPoint top, CanvasPoint mid, CanvasPoint bot){

	float extraX = bot.x - top.x;
	float extraY = bot.y - top.y;
	float gradient = extraX / extraY;
	float midY = mid.y;
	float Y = (mid.y - top.y);
	float midX = top.x + Y * gradient;
	
	return CanvasPoint(midX, midY);

}

std::vector<CanvasPoint> rasteriseTriangle(CanvasTriangle triangle){

	std::vector<CanvasPoint> vertices;
	vertices.resize(3);

	for (int i = 0; i < 3; i++){
		vertices[i] = triangle.vertices[i];
	}


	std::vector<CanvasPoint> sorted = sortVertices(vertices);
	CanvasPoint top = sorted[0];
	CanvasPoint mid = sorted[1];
	CanvasPoint bot = sorted[2];

	CanvasPoint extra = triangleExtraPoint(top, mid, bot);
	std::vector<CanvasPoint> results;
	results.resize(4);
	results[0] = top;
	results[1] = mid;
	results[2] = bot;
	results[3] = extra;

	return results;

}

void fillTriangle(DrawingWindow& window, CanvasTriangle triangle, Colour colour){

	std::vector<CanvasPoint> vertices = rasteriseTriangle(triangle);
	CanvasPoint top = vertices[0];
	CanvasPoint mid = vertices[1];
	CanvasPoint bot = vertices[2];
	CanvasPoint extra = vertices[3];

	std::vector<CanvasPoint> topToMid =  interpolateCanvPoint(top, mid, std::max(abs(mid.y - top.y), abs(mid.x - top.x)) + 1);
	std::vector<CanvasPoint> topToExtra = interpolateCanvPoint(top, extra, std::max(abs(extra.y - top.y), abs(mid.x - top.x)) + 1);
	std::vector<CanvasPoint> botToMid = interpolateCanvPoint(bot, mid, std::max(abs(bot.y - mid.y), abs(mid.x - top.x)) + 1);
	std::vector<CanvasPoint> botToExtra = interpolateCanvPoint(bot, extra, std::max(abs(bot.y - mid.y), abs(mid.x - top.x)) + 1);

	for (int i = 0; i < botToMid.size(); i++){
		drawLine(window, botToMid[i], botToExtra[i], colour);
	}

	for (int i = 0; i < topToMid.size(); i++){
		drawLine(window, topToMid[i], topToExtra[i], colour);
	}
}

void drawFilledTriangle(DrawingWindow& window, CanvasTriangle triangle, Colour rimColour, Colour fillColour){
	
    fillTriangle(window, triangle, fillColour);
	drawTriangle(window, triangle, rimColour);
}

/*std::vector<ModelTriangle> readOBJ (const char *filename, float scale, std::unordered_map<std::string, Colour> colours){
	
	std::ifstream in(filename, std::ios::in);
	std::string line;
	std::vector<ModelTriangle> objects;
	std::vector<glm::vec3> vertices;
	Colour colour;

	while (std::getline(in, line)){

		if (line.substr(0, 1) == "v"){
			std::istringstream v(line.substr(2));
			glm::vec3 points;

			double x, y, z;

			v >> x;
			v >> y;
			v >> z;

			points = glm::vec3(x * scale, y * scale, z * scale);
			vertices.push_back(points);
		}

		else if (line.substr(0, 1) == "f"){
			int i, j, k;
			const char *assign = line.c_str();
			sscanf(assign, "f %i/%i/%i", &i, &j, &k);

			ModelTriangle triangle(vertices[i-1], vertices[j-1], vertices[k-1], colour);
			objects.push_back(triangle);
		}

		else if (line.substr(0,1) == "usemtl"){
			std::vector<std::string> token = split(line, ' ');
			colour = colours[token[1]];
		}

		return objects;
	}

}*/

void textureMapping(DrawingWindow& window, TextureMap texture){
	
}

void draw(DrawingWindow& window) {

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

void handleEvent(SDL_Event event, DrawingWindow& window) {

	if (event.type == SDL_KEYDOWN) {

		if (event.key.keysym.sym == SDLK_LEFT) {
			std::cout << "LEFT" << std::endl;
		}
		else if (event.key.keysym.sym == SDLK_RIGHT){
			 std::cout << "RIGHT" << std::endl;
		}
		else if (event.key.keysym.sym == SDLK_UP){
			 std::cout << "UP" << std::endl;
		}
		else if (event.key.keysym.sym == SDLK_DOWN){
			 std::cout << "DOWN" << std::endl;
		}
		else if (event.key.keysym.sym == SDLK_l){
			std::cout << "l" << std::endl;
			//drawLine(window, from.x(0,0), to.y (0,100), Colour(rand() % 255, rand() %255, rand() % 255));
		
			// ME NO UNDERSTAND HOW DRAWLINE WORKS CLEARLY, BUT TRIANGLE WORK SO WHY NO WORK?!?
		}
		else if (event.key.keysym.sym == SDLK_u){
			std::cout << "u" << std::endl;
			drawTriangle(window, randomTrianglePoints(), Colour(rand() % 255, rand() % 255, rand() % 255));
		}
		else if (event.key.keysym.sym == SDLK_f){
			 std::cout << "f" << std::endl;
			 drawFilledTriangle(window, randomTrianglePoints(), Colour(0xFF, 0xFF, 0xFF), Colour(rand() % 255, rand() % 255, rand() % 255));
		}
		else if (event.key.keysym.sym == SDLK_r)
			std::cout << "r" << std::endl;
			// readOBJ()
	} 
	
	else if (event.type == SDL_MOUSEBUTTONDOWN) {
		window.savePPM("output.ppm");
		window.saveBMP("output.bmp");
	}
}

int main(int argc, char *argv[]) {
	srand(time(NULL));
	DrawingWindow window = DrawingWindow(WIDTH, HEIGHT, false);
	SDL_Event event;
	
	while (true) {
		// We MUST poll for events - otherwise the window will freeze !
		if (window.pollForInputEvents(event)) handleEvent(event, window);
	
		// Need to render the frame at the end, or nothing actually gets shown on the screen !
		window.renderFrame();
	}
}
