# CS405---openGL-game
 Sabanci University CS405 OpenGL graphics engine writing and game making project

##Resources used:
	https://learnopengl.com/Getting-started/Hello-Window
	
##External Dependencies:
	glad
	GLFW
	glm
	
##Useful resources:
	https://github.com/JoeyDeVries/Cell -> mesh classes
	https://chanhaeng.blogspot.com/2018/09/quaternion-camera-implementation.html -> quaternion camera (for spaceship)
	https://github.com/ocornut/imgui -> easy to implement runtime GUI
	https://en.wikibooks.org/wiki/GLSL_Programming/Unity/Cookies -> light cookies (masking)
	
	
	
##Shortcomings and issues:
	limited depth perception -> add a "floor"
	can't see if light works properly -> add object without texture
	how does the current scene relate to the final project? -> tangentially it does, it is supposed to be in space (0 gravity)
		Current scene is made to test if capabilities of engine are working correctly. Goal was not to work towards game as of yet
	models cannot be loaded easily from outside files, must be done within source.cpp

##Free Camera: (unused)
can turn freely without quaternions
source: http://disq.us/p/1ddwy5t (Laurie Bradshaw)
	class camera{
		glm::vec3 m_pos;
		glm::quat m_orient;
	public:
		camera(void) = default;
		camera(const camera &) = default;

		camera(const glm::vec3 &pos) : m_pos(pos) {}
		camera(const glm::vec3 &pos, const glm::quat &orient) : m_pos(pos), m_orient(orient) {}

		camera &operator =(const camera &) = default;

		const glm::vec3 &position(void) const { return m_pos; }
		const glm::quat &orientation(void) const { return m_orient; }

		glm::mat4 view(void) const { return glm::translate(glm::mat4_cast(m_orient), m_pos); }

		void translate(const glm::vec3 &v) { m_pos += v * m_orient; }
		void translate(float x, float y, float z) { m_pos += glm::vec3(x, y, z) * m_orient; }

		void rotate(float angle, const glm::vec3 &axis) { m_orient *= glm::angleAxis(angle, axis * m_orient); }
		void rotate(float angle, float x, float y, float z) { m_orient *= glm::angleAxis(angle, glm::vec3(x, y, z) * m_orient); }

		void yaw(float angle) { rotate(angle, 0.0f, 1.0f, 0.0f); }
		void pitch(float angle) { rotate(angle, 1.0f, 0.0f, 0.0f); }
		void roll(float angle) { rotate(angle, 0.0f, 0.0f, 1.0f); }
	};
implementation by muffinamn007 (http://disq.us/p/1kicwh7): https://gitlab.com/muffinman007/OpenGL_360_Camera_Quarternion/blob/master/Camera.h