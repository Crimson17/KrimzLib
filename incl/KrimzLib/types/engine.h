#pragma once


namespace kl {
	class engine {
	public:
		// Engine properties
		double fpsLimit = -1;
		double deltaTime = 0;
		double gravity = 9.81;
		kl::color background = {};
		kl::camera engineCamera = {};

		// Outside functions that user defines
		std::function<void(void)> engineStart = []() {};
		std::function<void(kl::key key, kl::mouse mouse)> engineInput = [](kl::key key, kl::mouse mouse) {};
		std::function<void(void)> engineUpdate = []() {};

		// Constructor
		engine(kl::color background = {}) {
			this->background = background;
		}

		// Creates the engine
		void startNew(kl::size size, std::string name, double fov = 60) {
			engineWindow.windowStart = [&]() {
				/* 3D projection setup */
				kl::opengl::setup3D(fov, engineWindow.getSize());

				/* Enable depth buffer */
				kl::opengl::setDepthTest(true);

				/* Enable textures */
				kl::opengl::setTextures(true);

				/* Game start function call */
				engineStart();

				/* First stopwatch reset */
				engineTime.stopwatchReset();
			};

			engineWindow.windowUpdate = [&]() {
				/* Game input */
				engineInput(engineWindow.KEY, engineWindow.MOUSE);

				/* Game logic */
				engineUpdate();

				/* Apply physics */
				physicsUpdate();

				/* Clear frame and depth buffers */
				kl::opengl::clearBuffers(background);

				/* Reset the modelview matrix */
				kl::opengl::resetModelMatrix();

				/* Update camera rotation and position */
				kl::opengl::updateCamera(engineCamera);

				/* Render all game triangles */
				for (objItr = engineObjects.begin(); objItr != engineObjects.end(); objItr++) {
					if (objItr->visible) {
						kl::opengl::renderTriangles(objItr->triangles, objItr->position, objItr->rotation, objItr->size, objItr->texture);
					}
				}

				/* Swap front and back frame buffers */
				engineWindow.swapFrameBuffers();

				/* Delta time calculation */
				double wantedFrameTime = 1 / fpsLimit;
				do {
					deltaTime = engineTime.stopwatchElapsed();
				} while (deltaTime < wantedFrameTime);
				engineTime.stopwatchReset();

				/* Display the FPS */
				engineWindow.setTitle(int(1 / deltaTime));
			};

			engineWindow.windowEnd = [&]() {
				for (int i = 0; i < engineTextures.size(); i++) {
					kl::opengl::deleteTexture(engineTextures[i]);
				}
			};

			engineWindow.startNew(size, name, false, true, true);
		}
		void stop() {
			engineWindow.stop();
		}
		~engine() {
			this->stop();
		}

		// Creates a new game object
		kl::gameobject* newObject(kl::texture textureID) {
			engineObjects.push_back(kl::gameobject(textureID));
			return &engineObjects.back();
		}
		kl::gameobject* newObject(kl::texture textureID, std::string objFilePath) {
			// Load file
			FILE* fileStream = fopen(objFilePath.c_str(), "r");
			if (!fileStream) {
				return nullptr;
			}

			// Data buffers
			kl::gameobject tempObject(textureID);
			std::vector<kl::vec3> xyzCoords = {};
			std::vector<kl::vec2> uvCoords = {};
			std::string tempBuffer; tempBuffer.resize(100);
			int scanStatus = 0;
			kl::vec3 tempXYZ = {};
			kl::vec2 tempUV = {};
			int coordIndex0 = 0;
			int textureIndex0 = 0;
			int normalIndx0 = 0;
			int coordIndex1 = 0;
			int textureIndex1 = 0;
			int normalIndx1 = 0;
			int coordIndex2 = 0;
			int textureIndex2 = 0;
			int normalIndx2 = 0;

			// Parse .obj data
			while (scanStatus != -1) {
				if ((scanStatus = fscanf(fileStream, "v %lf %lf %lf", &tempXYZ.x, &tempXYZ.y, &tempXYZ.z)) == 3) {
					xyzCoords.push_back(tempXYZ);
				}
				else if ((scanStatus = fscanf(fileStream, "t %lf %lf", &tempUV.x, &tempUV.y)) == 2) {
					uvCoords.push_back(tempUV);
				}
				else if ((scanStatus = fscanf(fileStream, "f %d/%d/%d %d/%d/%d %d/%d/%d", &coordIndex0, &textureIndex0, &normalIndx0, &coordIndex1, &textureIndex1, &normalIndx1, &coordIndex2, &textureIndex2, &normalIndx2)) == 9) {
					tempObject.triangles.push_back(
						kl::triangle(
							kl::vertex(xyzCoords[--coordIndex0], uvCoords[--textureIndex0]),
							kl::vertex(xyzCoords[--coordIndex1], uvCoords[--textureIndex1]),
							kl::vertex(xyzCoords[--coordIndex2], uvCoords[--textureIndex2])
						)
					);
				}
				else {
					fgets(&tempBuffer[0], 100, fileStream);
				}
			}
			fclose(fileStream);

			// Save object in engine memory
			engineObjects.push_back(tempObject);
			return &engineObjects.back();
		}

		// Deletes a game object
		bool deleteGameObject(kl::gameobject* objectAddress) {
			for (objItr = engineObjects.begin(); objItr != engineObjects.end(); objItr++) {
				if (&*objItr == objectAddress) {
					engineObjects.erase(objItr);
					return true;
				}
			}
			return false;
		}

		// Adds a new texture to the engine
		kl::texture newTexture(kl::image& textureImage) {
			kl::texture tempTex = kl::opengl::newTexture(textureImage);
			engineTextures.push_back(tempTex);
			return tempTex;
		}
		kl::texture newTexture(kl::image&& textureImage) {
			return newTexture(textureImage);
		}

		// Deletes an engine texture
		bool deleteTexture(kl::texture textureID) {
			for (int i = 0; i < engineTextures.size(); i++) {
				if (engineTextures[i] == textureID) {
					kl::opengl::deleteTexture(textureID);
					engineTextures.erase(engineTextures.begin() + i);
					return true;
				}
			}
			return false;
		}

	private:
		kl::window engineWindow = {};
		kl::time engineTime = {};
		std::list<kl::gameobject> engineObjects = {};
		std::list<kl::gameobject>::iterator objItr = {};
		std::vector<kl::texture> engineTextures = {};

		// Computing object physics 
		void physicsUpdate() {
			for (objItr = engineObjects.begin(); objItr != engineObjects.end(); objItr++) {
				if (objItr->physics) {
					// Applying gravity
					objItr->velocity.y -= gravity * objItr->gravity * deltaTime;

					// Applying velocity
					objItr->position.x += objItr->velocity.x * deltaTime;
					objItr->position.y += objItr->velocity.y * deltaTime;
					objItr->position.z += objItr->velocity.z * deltaTime;

					// Applying angular momentum
					objItr->rotation.x += objItr->angular.x * deltaTime;
					objItr->rotation.y += objItr->angular.y * deltaTime;
					objItr->rotation.z += objItr->angular.z * deltaTime;
				}
			}
		}
	};
}
