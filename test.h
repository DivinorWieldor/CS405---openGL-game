#ifndef TESTCAMERA_H
#define TESTCAMERA_H
//source: http://disq.us/p/1ddwy5t (Laurie Bradshaw)
//not tested

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

class camera {
	glm::vec3 m_pos;
	glm::quat m_orient;
public:
	camera(void) = default;
	camera(const camera&) = default;

	camera(const glm::vec3& pos) : m_pos(pos) {}
	camera(const glm::vec3& pos, const glm::quat& orient) : m_pos(pos), m_orient(orient) {}

	camera& operator =(const camera&) = default;

	const glm::vec3& position(void) const { return m_pos; }
	const glm::quat& orientation(void) const { return m_orient; }

	glm::mat4 view(void) const { return glm::translate(glm::mat4_cast(m_orient), m_pos); }

	void translate(const glm::vec3& v) { m_pos += v * m_orient; }
	void translate(float x, float y, float z) { m_pos += glm::vec3(x, y, z) * m_orient; }

	void rotate(float angle, const glm::vec3& axis) { m_orient *= glm::angleAxis(angle, axis * m_orient); }
	void rotate(float angle, float x, float y, float z) { m_orient *= glm::angleAxis(angle, glm::vec3(x, y, z) * m_orient); }

	void yaw(float angle) { rotate(angle, 0.0f, 1.0f, 0.0f); }
	void pitch(float angle) { rotate(angle, 1.0f, 0.0f, 0.0f); }
	void roll(float angle) { rotate(angle, 0.0f, 0.0f, 1.0f); }
};
#endif