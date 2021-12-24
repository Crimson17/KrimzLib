#include "KrimzLib.hpp"


int main() {
	/* Engine */
	kl::renderer renderer;

	/* Skybox */
	kl::skybox* clouds = nullptr;

	/* Meshes */
	kl::mesh* table_mes = nullptr;
	kl::mesh* katana_mes = nullptr;
	kl::mesh* horse_mes = nullptr;
	kl::mesh* tv_mes = nullptr;

	/* Textures */
	kl::texture* table_tex = nullptr;
	kl::texture* katana_tex = nullptr;
	kl::texture* horse_tex = nullptr;
	kl::texture* tv_tex = nullptr;

	/* Objects */
	kl::renderable* table = nullptr;
	kl::renderable* katanaL = nullptr;
	kl::renderable* katanaR = nullptr;
	kl::renderable* horse = nullptr;
	kl::array<kl::renderable*> tvs(101);

	/* User start */
	renderer.setup = [&]() {
		/* Skybox creation */
		clouds = new kl::skybox(
			"res/skybox/front.jpg",
			"res/skybox/back.jpg",
			"res/skybox/left.jpg",
			"res/skybox/right.jpg",
			"res/skybox/top.jpg",
			"res/skybox/bottom.jpg"
		);
		renderer.sky = clouds;

		/* Mesh creation */
		table_mes = new kl::mesh("res/objects/table.obj");
		katana_mes = new kl::mesh("res/objects/katana.obj");
		horse_mes = new kl::mesh("res/objects/horse.obj");
		tv_mes = new kl::mesh("res/objects/tv.obj");

		/* Texture creation */
		table_tex = new kl::texture("res/textures/table.jpg");
		katana_tex = new kl::texture("res/textures/katana.jpg");
		horse_tex = new kl::texture("res/textures/horse.jpg");
		tv_tex = new kl::texture("res/textures/tv.jpg");

		/* Object creation */
		table = renderer.newObject();
		katanaL = renderer.newObject();
		katanaR = renderer.newObject();
		horse = renderer.newObject();

		/* Mesh binding */
		table->mesh = table_mes;
		katanaL->mesh = katana_mes;
		katanaR->mesh = katana_mes;
		horse->mesh = horse_mes;

		/* Texture binding */
		table->texture = table_tex;
		katanaL->texture = katana_tex;
		katanaR->texture = katana_tex;
		horse->texture = horse_tex;

		/* Object properties setup */
		table->geometry.size = kl::vec3(1, 1, 1);
		table->geometry.rotation = kl::vec3(0, 45, 0);
		table->geometry.position = kl::vec3(0, -0.5, 2);
		table->physics.enabled = false;
		table->physics.angular.y = 18;
		table->physics.gravity = 0;

		katanaL->geometry.size = kl::vec3(2, 2, 2);
		katanaL->geometry.rotation = kl::vec3(0, 180, -42);
		katanaL->geometry.position = kl::vec3(-1, 0, 2);
		katanaL->physics.enabled = false;
		katanaL->physics.angular.y = 36;
		katanaL->physics.gravity = 0;
		
		katanaR->geometry.size = kl::vec3(2, 2, 2);
		katanaR->geometry.rotation = kl::vec3(0, 0, 42);
		katanaR->geometry.position = kl::vec3(1, 0, 2);
		katanaR->physics.enabled = false;
		katanaR->physics.angular.y = -36;
		katanaR->physics.gravity = 0;

		horse->geometry.size = kl::vec3(4, 4, 4);
		horse->geometry.rotation = kl::vec3(0, 0, 0);
		horse->geometry.position = kl::vec3(0, -0.02, 2);
		horse->physics.enabled = true;
		horse->physics.angular.y = 18;
		horse->physics.gravity = 0;

		/* Object array creation */
		int tvCounter = 0;
		float tvsStartPos = -tvs.size() / 2.0;
		tvs.forEach([&](auto tv) {
			*tv = renderer.newObject();

			(*tv)->mesh = tv_mes;
			(*tv)->texture = tv_tex;

			(*tv)->geometry.size = kl::vec3(2, 2, 2);
			(*tv)->geometry.rotation = kl::vec3(0, 180, 0);
			(*tv)->geometry.position = kl::vec3(tvsStartPos + tvCounter++, 0, -1);
		});
	};

	/* User update */
	bool camMoving = false;
	renderer.update = [&]() {
		/* Keyboard input */
		if (renderer.keys->w) {
			renderer.cam.moveForward(renderer.delta);
		}
		if (renderer.keys->s) {
			renderer.cam.moveBack(renderer.delta);
		}
		if (renderer.keys->d) {
			renderer.cam.moveRight(renderer.delta);
		}
		if (renderer.keys->a) {
			renderer.cam.moveLeft(renderer.delta);
		}
		if (renderer.keys->space) {
			renderer.cam.moveUp(renderer.delta);
		}
		if (renderer.keys->c) {
			renderer.cam.moveDown(renderer.delta);
		}
		if (renderer.keys->shift) {
			renderer.cam.speed = 5;
		}
		else {
			renderer.cam.speed = 2;
		}

		/* Mouse input */
		if (renderer.mouse->lmb) {
			camMoving = true;
			renderer.mouse->hide();

			// Fixing the camera jump on the first click
			renderer.mouse->position = renderer.frameCenter();
		}
		if (renderer.mouse->rmb) {
			camMoving = false;
			renderer.mouse->show();
		}
		if (camMoving) {
			kl::point frameCenter = renderer.frameCenter();
			renderer.cam.rotate(renderer.mouse->position, frameCenter);
			renderer.mouse->move(frameCenter);
		}

		/* Object updating */
		tvs.forEach([&](auto tv) {
			const float tvx = (*tv)->geometry.position.x;

			(*tv)->geometry.position.y = 1.5 * (sin((0.5 * tvx) + (3 * renderer.elapsed)) + 1);
		});
	};

	/* Renderer creation */
	renderer.createNew(kl::size(1600, 900));

	/* Deleting skybox */
	delete clouds;

	/* Deleting meshes */
	delete table_mes;
	delete katana_mes;
	delete horse_mes;

	/* Deleting textures */
	delete table_tex;
	delete katana_tex;
	delete horse_tex;

	return 0;
}