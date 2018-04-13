#include <glm/glm.hpp>

class Camera{
public:
	glm::vec3 dir, right, up, pos;
    float phi, theta;

	Camera():dir(glm::vec3(0, 0, -1)), right(glm::vec3(1, 0, 0)),
        up(glm::vec3(0, 1, 0)), pos(glm::vec3(0, 0, 2)) {
        phi = 0.0f;
        theta = 90.0f;
    }
	Camera(glm::vec3 dir, glm::vec3 right, glm::vec3 up, glm::vec3 pos):dir(dir),
    right(right), up(up), pos(pos){}


	glm::mat4 viewMatrix() const;

	void rotateVertical(float radians);
	void rotateHorizontal(float radians);
    void rotateSpherical(float x, float y);
	void move(glm::vec3 movement);		//Moves in rotated frame
};
