#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

// const float MAX_VERT = 0.95f;

glm::mat4 Camera::viewMatrix() const{
	mat4 cRotation = transpose(mat4(vec4(right, 0), vec4(up, 0), vec4(-dir, 0), vec4(0, 0, 0, 1)));
	mat4 cTranslation = glm::translate(mat4(1.f), -pos);
	return cRotation*cTranslation;
}

// void Camera::rotateVertical(float radians){
	// mat4 rotationMatrix = glm::rotate(mat4(1.f), radians, right);
	// vec3 newDir = normalize(vec3(rotationMatrix * vec4(dir, 0)));

	// if(abs(newDir.y) < MAX_VERT){
		// dir = newDir;
		// up = normalize(cross(right, dir));
	// }
// }

// void Camera::rotateHorizontal(float radians){
	// mat4 rotationMatrix = glm::rotate(mat4(1.f), radians, vec3(0, 1, 0));
	// dir = normalize(vec3(rotationMatrix * vec4(dir, 0)));
	// right = normalize(cross(dir, vec3(0, 1, 0)));
	// up = normalize(cross(right, dir));
// }


void Camera::move(vec3 movement){
	pos += movement.x * right + movement.y * up + movement.z * dir;
}

void Camera::rotateSpherical(float x, float y) {
	phi += 50.0f * x;
	theta += 50.0f * y;

    if (theta > 179.0f) {
        theta = 179.0f;
    }
    if (theta < 0.1f) {
        theta = 0.1f;
    }

    float radius = glm::length(vec3(0, 0, 0) - pos);

    pos.x = radius * sin(radians(theta)) * sin(radians(phi));
    pos.y = radius * cos(radians(theta));
    pos.z = radius * sin(radians(theta)) * cos(radians(phi));

    dir = normalize(vec3(0, 0, 0) - pos);
	right = normalize(cross(dir, vec3(0, 1, 0)));
	up = normalize(cross(right, dir));
}
