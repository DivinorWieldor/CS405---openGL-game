#include "chanCamera.h"
#include <iostream>

chanCamera::chanCamera(glm::vec3 position, glm::vec3 up) : MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM){
	Position = position;
	Orientation = glm::quat(0, 0, 0, -1);
	RightAngle = 0.f;
	UpAngle = 0.f;
	updateCameraVectors();
}

chanCamera::chanCamera(float posX, float posY, float posZ) : MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM){
	Position = glm::vec3(posX, posY, posZ);
	Orientation = glm::quat(0, 0, 0, -1);
	RightAngle = 0.f;
	UpAngle = 0.f;
	updateCameraVectors();
}

glm::mat4 chanCamera::GetViewMatrix(){
	// You should know the camera moves reverse relative to user input
	glm::quat reverseOrient = glm::conjugate(Orientation);
	glm::mat4 rot = glm::mat4_cast(reverseOrient);
	glm::mat4 translation = glm::translate(glm::mat4(1.0), -Position);

	return rot * translation;
}

void chanCamera::ProcessKeyboard(Camera_Movement direction, float deltaTime){
	float velocity = MovementSpeed * deltaTime;
	glm::quat qF = Orientation * glm::quat(0, 0, 0, -1) * glm::conjugate(Orientation);
	glm::vec3 Front = { qF.x, qF.y, qF.z };
	glm::vec3 Right = glm::normalize(glm::cross(Front, glm::vec3(0, 1, 0)));

	if (direction == Camera_Movement::FORWARD)
		Position += Front * velocity;
	if (direction == Camera_Movement::BACKWARD)
		Position -= Front * velocity;
	if (direction == Camera_Movement::LEFT)
		Position -= Right * velocity;
	if (direction == Camera_Movement::RIGHT)
		Position += Right * velocity;
}

void chanCamera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch){
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;
	RightAngle += xoffset;
	UpAngle += yoffset;

	updateCameraVectors();
}

void chanCamera::ProcessMouseScroll(float yoffset){
	if (Zoom >= 1.f && Zoom <= 45.f) Zoom -= yoffset;
	if (Zoom <= 1.f) Zoom = 1.f;
	if (Zoom >= 45.f) Zoom = 45.f;
}

void chanCamera::updateCameraVectors(){
	glm::quat aroundY = glm::angleAxis(glm::radians(-RightAngle), glm::vec3(0, 1, 0));// Yaw
	glm::quat aroundX = glm::angleAxis(glm::radians(UpAngle), glm::vec3(1, 0, 0));// Pitch

	Orientation = aroundY * aroundX;
}

