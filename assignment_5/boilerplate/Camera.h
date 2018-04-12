#include <glm/glm.hpp>

class Camera{
public:
	glm::vec3 dir, right, up, pos;

	Camera():dir(glm::vec3(0, 0, -1)), right(glm::vec3(1, 0, 0)), up(glm::vec3(0, 1, 0)), pos(glm::vec3(0, 0, 2)){}
	Camera(glm::vec3 dir, glm::vec3 right, glm::vec3 up, glm::vec3 pos):dir(dir), right(right), up(up), pos(pos){}


	glm::mat4 viewMatrix() const;

	void rotateVertical(float radians);
	void rotateHorizontal(float radians);
	void move(glm::vec3 movement);		//Moves in rotated frame
};
