#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include "Camera.h"
#include "shader.hpp"
#include "vaoutils.hpp"
#include "scene.hpp"
#include "mesh.hpp"
#include "model.hpp"
#include "helper.hpp"
#include "animation.hpp"
#include "animator.hpp"
#include <filesystem>

namespace fs = std::filesystem;
// �^�ը�ƪ��ŧi
void framebuffer_size_callback(GLFWwindow* window, int width, int height); // �B�z�����j�p�վ�
void processInput(GLFWwindow* window, Animation* animations); // �B�z��L�P�ƹ���J
void mouse_callback(GLFWwindow* window, double xpos, double ypos); // �B�z�ƹ�����
void renderNode(Node* node); // ��V�����`�I
void updateNodeTransformations(Node* node, glm::mat4 transformationThusFar); // ��s�`�I�ܴ��x�}
void setUniformBoneTransforms(std::vector<glm::mat4> transforms, unsigned int shaderId); // �]�w���f�ܴ���ۦ⾹
//���o�ڥؿ�
string getRootPath();
// �����ܼƪ��w�q
bool VSYNC = true; // �O�_�ҥΫ����P�B
bool FULLSCREEN = false; // �O�_�ҥΥ��ù��Ҧ�
int WINDOW_WIDTH = 1920; // �����e��
int WINDOW_HEIGHT = 1080; // ��������
int FPS = 999999; // �̤j�V�v����

// ��v�������Ѽ�
glm::vec3 cameraPos = glm::vec3(2.0f, 2.0f, 5.0f); // ��v����l��m
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); // ��v���e��V
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); // ��v���W��V
bool canAdjustCameraPos = true;

// �ƹ������Ѽ�
bool firstMouse = true; // �Ĥ@�������ƹ�����
float yaw = -90.0f; // ��v���� Yaw ����
float pitch = 0.0f; // ��v���� Pitch ����
float lastX = 800.0f / 2.0; // �ƹ����̫� X �y��
float lastY = 600.0 / 2.0; // �ƹ����̫� Y �y��
float fov = 45.0f; // ��v�������d�� (Field of View)

// �ɶ������Ѽ�
float deltaTime = 0.0f; // �C�V���ɶ��t (�Ω󲾰ʭp��)

// �ʵe�޲z��
Animator animator = Animator(); // �Ω󱱨�ʵe���񪺪���

// �����`�I
Node* checkerFloor = createSceneNode(); // �a�O�������`�I
Node* character = createSceneNode(); // ���⪺�����`�I

int main()
{

	std::string projectRoot = getRootPath();
	std::cout << "Root Directory: " << projectRoot << endl;
	// ��l�� GLFW
	glfwInit();
	// �w�q OpenGL ����
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // �D�������� 4
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6); // ���������� 6
	// �w�q�ϥ� Core Profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// �ҥΦh���ļ˧ܿ��� (AA)
	glfwWindowHint(GLFW_SAMPLES, 4);

	// �ϥ� GLFW �Ыص���
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Project", FULLSCREEN ? glfwGetPrimaryMonitor() : NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// �]�m OpenGL �W�U��
	glfwMakeContextCurrent(window);

	// �]�w�ƹ��^�ը��
	glfwSetCursorPosCallback(window, mouse_callback);

	// �]�m OpenGL �������j�p�P�����P�B
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// �T�ηƹ����
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// ���� GLAD �O�_���\�[�� OpenGL�A�Ȧb���\���~��ϥ� OpenGL ���
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// ���L�t�ά����H��
	printInfo();

	// �ҥβ`�״���
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); // �`�פ�����

	// �ҥέ��簣�H�����į�
	glEnable(GL_CULL_FACE);

	// �T�Τ��ت��C��ݰ�
	glDisable(GL_DITHER);

	// �ҥγz����
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// �p�G�T�Ϋ����P�B
	if (!VSYNC)
		glfwSwapInterval(0);

	// �[���ҫ��P�ʵe�귽

	std::string daeFile = projectRoot + "resource/vanguard/vanguard.dae";

	std::string animFile1 = projectRoot + "resource/vanguard/Offensive_Idle.dae";
	std::string animFile2 = projectRoot + "resource/vanguard/Running.dae";
	std::string animFile3 = projectRoot + "resource/vanguard/Left_Strafe.dae";
	std::string animFile4 = projectRoot + "resource/vanguard/Right_Strafe.dae";
	std::string animFile5 = projectRoot + "resource/vanguard/Running Backward.dae";
	std::string animFile6 = projectRoot + "resource/vanguard/Jump.dae";
	std::string animFile7 = projectRoot + "resource/vanguard/HipHopDancing.dae";
	std::string animFile8 = projectRoot + "resource/vanguard/Wave_Hip_Hop_Dance.dae";
	std::string animFile9 = projectRoot + "resource/vanguard/Front_Flip.dae";

	std::vector<TextureOverride> overrides = {
		{0, DIFFUSE, "textures/vanguard_diffuse1.png"},
		{0, NORMAL, "textures/vanguard_normal.png"},
		{0, SPECULAR, "textures/vanguard_specular.png"},
	};
	// �[���ҫ�
	Model m = Model(daeFile, overrides);
	vector<Mesh> squareMeshes = m.meshes;
	std::cout << "Loaded meshes: " << m.meshes.size() << std::endl;

	// �Ыس����`�I
	Node* root = createSceneNode();
	root->type = ROOT;

	//// �Ыئa�O����
	Mesh floorMesh;
	floorMesh.vertices = {
		glm::vec3(-40.0f, 0.0f, -40.0f),
		glm::vec3(-40.0f, 0.0f, 40.0f),
		glm::vec3(40.0f, 0.0f, 40.0f),
		glm::vec3(20.0f, 0.0f, -40.0f),
	};
	floorMesh.normals = {
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
	};
	floorMesh.indices = { 0, 1, 2, 2, 3, 0 };

	unsigned int floorVAO = generateBuffer(floorMesh);

	//// �]�m�a�O�`�I
	checkerFloor->type = GEOMETRY;
	checkerFloor->vertexArrayObjectIDs = { (int)floorVAO };
	checkerFloor->VAOIndexCounts = { (unsigned int)floorMesh.indices.size() };
	addChild(root, checkerFloor);

	// �t�m����`�I
	character->type = CHARACTER;
	character->scale = glm::vec3(0.01, 0.01, 0.01);

	for (int i = 0; i < m.meshes.size(); i++)
	{
		unsigned int charVAO = generateBuffer(squareMeshes[i]);
		character->vertexArrayObjectIDs.push_back(charVAO);
		character->VAOIndexCounts.push_back(squareMeshes[i].indices.size());

		character->textureIDs.push_back(m.diffuseMaps[i]);
		character->normalMapIDs.push_back(m.normalMaps[i]);
		character->specularMapIDs.push_back(m.specularMaps[i]);
	}

	addChild(root, character);

	// �[���ʵe
	Animation anim1(animFile1, &m);
	Animation anim2(animFile2, &m);
	Animation anim3(animFile3, &m);
	Animation anim4(animFile4, &m);
	Animation anim5(animFile5, &m);
	Animation anim6(animFile6, &m);
	Animation anim7(animFile7, &m);
	Animation anim8(animFile8, &m);
	Animation anim9(animFile9, &m);

	Animation animations[] = { anim1, anim2, anim3, anim4, anim5, anim6, anim7, anim8 , anim9 };
	//Animation animations[] = { anim1 };

	// �[���ۦ⾹
	Shader shader = Shader((projectRoot + "src/shaders/default.vert").c_str(),
		(projectRoot + "src/shaders/default.frag").c_str());

	Shader depthShader = Shader((projectRoot + "src/shaders/depth.vert").c_str(),
		(projectRoot + "src/shaders/depth.frag").c_str());

	// ��V�j��
	float frameTime = 1.0f / FPS;
	float lastFrame = 0.0f;

	unsigned int depthMap;
	unsigned int depthFBO;

	const unsigned int s_width = 4096, s_height = 4096;

	// �ͦ��`�׹�
	generateDepthMap(depthMap, depthFBO, s_width, s_height);

	std::cout << "Starting.." << std::endl;

	while (!glfwWindowShouldClose(window))
	{
		// �p��C�V���ɶ��t
		float now = glfwGetTime();
		if (!VSYNC && now - lastFrame <= frameTime)
		{
			continue;
		}
		deltaTime = now - lastFrame;
		std::cout << "FPS: " << (1.0f / deltaTime) << "\t\r" << std::flush;
		lastFrame = now;

		// �B�z��J�ç�s�ʵe
		processInput(window, animations);
		animator.updateAnimation(deltaTime);

		updateNodeTransformations(root, glm::mat4(1.0));

		auto transforms = animator.getFinalBoneMatrices();

		// ----------------- ��v���v ---------------
		glCullFace(GL_FRONT);
		glm::mat4 lightProjection = glm::perspective(glm::radians(fov), (float)s_width / (float)s_height, 0.1f, 100.0f);
		glm::vec3 lightPos = glm::vec3(-10.0f, 10.0f, 20.0f);
		glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;

		depthShader.use();

		setUniformBoneTransforms(transforms, depthShader.ID);

		glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, s_width, s_height);
		glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		renderNode(root);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glCullFace(GL_BACK);

		shader.use();

		// ---------------- ���v�B�z���� ------------

		setUniformBoneTransforms(transforms, shader.ID);

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

		glm::mat4 projection = glm::perspective(glm::radians(fov), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = glm::lookAt(cameraPos, character->position + glm::vec3(0.0f, 1.0f, 0.0f), cameraUp);

		glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(projection));
		glUniform3fv(3, 1, glm::value_ptr(cameraPos));

		glUniformMatrix4fv(5, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		renderNode(root);

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	std::cout << std::endl << "Terminating.." << std::endl;

	// ��V������פ� GLFW
	glfwTerminate();
	return 0;
}


void setUniformBoneTransforms(std::vector<glm::mat4> transforms, unsigned int shaderId) {
	// �N���f�ഫ�x�}�]�w��ۦ⾹��
	for (int i = 0; i < transforms.size(); ++i) {
		string boneStr = "boneTransforms[" + std::to_string(i) + "]"; // �ͦ����f�W��
		int boneLocation = glGetUniformLocation(shaderId, boneStr.c_str()); // ��� uniform ��m
		glUniformMatrix4fv(boneLocation, 1, GL_FALSE, glm::value_ptr(transforms[i])); // �]�w�x�}
	}
}

void updateNodeTransformations(Node* node, glm::mat4 transformationThusFar) {
	// �p���e�`�I���ܴ��x�}
	glm::mat4 transformationMatrix =
		glm::translate(node->position) *
		glm::translate(node->referencePoint) *
		glm::rotate(node->rotation.y, glm::vec3(0, 1, 0)) *
		glm::rotate(node->rotation.x, glm::vec3(1, 0, 0)) *
		glm::rotate(node->rotation.z, glm::vec3(0, 0, 1)) *
		glm::scale(node->scale) *
		glm::translate(-node->referencePoint);

	node->currentTransformationMatrix = transformationThusFar * transformationMatrix;

	// ���j�B�z�l�`�I���ܴ�
	for (Node* child : node->children) {
		updateNodeTransformations(child, node->currentTransformationMatrix);
	}
}

void renderNode(Node* node) {
	// �]�w�`�I������ۦ⾹
	glUniform1ui(4, node->type);

	// �ھڸ`�I�����i��ø�s
	switch (node->type) {
	case CHARACTER:
		// ø�s����
		for (unsigned int i = 0; i < node->VAOIndexCounts.size(); i++) {
			if (node->vertexArrayObjectIDs[i] != -1) {
				// �j�w����K��
				if (node->textureIDs[i] >= 0) {
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, node->textureIDs[i]);
				}

				if (node->normalMapIDs[i] >= 0) {
					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, node->normalMapIDs[i]);
				}

				if (node->specularMapIDs[i] >= 0) {
					glActiveTexture(GL_TEXTURE2);
					glBindTexture(GL_TEXTURE_2D, node->specularMapIDs[i]);
				}

				// �]�w��e�`�I���ܴ��x�}
				glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(node->currentTransformationMatrix));
				glBindVertexArray(node->vertexArrayObjectIDs[i]); // �j�w VAO
				glDrawElements(GL_TRIANGLES, node->VAOIndexCounts[i], GL_UNSIGNED_INT, nullptr); // ø�s�T����
			}
		}
		break;
	case GEOMETRY:
		// ø�s�X����
		for (unsigned int i = 0; i < node->VAOIndexCounts.size(); i++) {
			glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(node->currentTransformationMatrix));
			glBindVertexArray(node->vertexArrayObjectIDs[i]);
			glDrawElements(GL_TRIANGLES, node->VAOIndexCounts[i], GL_UNSIGNED_INT, nullptr);
		}
		break;
	}

	// ���jø�s�l�`�I
	for (Node* child : node->children) {
		renderNode(child);
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// ��s�����j�p�ɡA���s�]�w���f
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window, Animation* animations) {
	// �B�z�ϥΪ̿�J
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true); // ���U ESC ��������

	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
	
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // ���U ALT ����ƹ����
		canAdjustCameraPos = true;
	
	}
	
	float speed = 4.0f * deltaTime; // �ھڮɶ��p�Ⲿ�ʳt��

	bool idle = true; // �P�_�O�_�B���R��A

	// �B�z������L��J
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		character->position.z += 1.2f * speed;
		cameraPos.z += 1.0f * speed;
		animator.playAnimation(&animations[1]); // ����e�i�ʵe
		idle = false;
	}
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		character->position.x += 0.75f * speed;
		cameraPos.x += 0.75f * speed;
		animator.playAnimation(&animations[2]); // ���񥪥����ʵe
		idle = false;
	}
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		character->position.x -= 0.75f * speed;
		cameraPos.x -= 0.75f * speed;
		animator.playAnimation(&animations[3]); // ����k�����ʵe
		idle = false;
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		character->position.z -= 0.5f * speed;
		cameraPos.z -= 0.5f * speed;
		animator.playAnimation(&animations[4]); // �����h�ʵe
		idle = false;
	}
	else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		character->position.z -= 0.2f * speed;
		cameraPos.z -= 0.2f * speed;
		animator.playAnimation(&animations[5]); // ������D�ʵe
		idle = false;
	}
	else if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {

		animator.playAnimation(&animations[6]); // �����L�ʵe
		idle = false;
	}
	else if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {

		animator.playAnimation(&animations[7]); // �����L�ʵe
		idle = false;
	}
	else if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
		character->position.z += 1.0f * speed;
		animator.playAnimation(&animations[8]); // �����L�ʵe
		idle = false;
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // ������U���ô��
		canAdjustCameraPos = false;
	}
	//// �ˬd�ʵe�O�_����
	//if (animator.isAnimationFinished()) {
	//	character->position.z += 2; // �ʵe������W�[��m
	//}
	// �p�G�����R��A����ݾ��ʵe
	if (idle) {
		animator.playAnimation(&animations[0]);
	}
}
void updateCameraPos(float CameraPosXOffset, float CameraPosZOffset ) {
	//// ��s��v����V
	//// �ھڷƹ����ʧ�s��v����V
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction); // ��s��v���e��V
	//std::cout << "CameraPosXOffset, CameraPosZOffset" << CameraPosXOffset << ", " << CameraPosZOffset << std::endl;
	// �T�w��v����m�A�ȧ�s��V
	
	cameraPos.z -= 0.2f * CameraPosZOffset;
	cameraPos.x += 0.2f * CameraPosXOffset;
	 
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	
	// �B�z�ƹ���J
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX; // �p��ƹ��� X �����q
	float yoffset = lastY - ypos; // �p��ƹ��� Y �����q
	lastX = xpos;
	lastY = ypos;
	
	float sensitivity = 0.1f; // �ƹ��F�ӫ�
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;   // ��s������ Yaw
	pitch += yoffset; // ��s������ Pitch

	// ���� Pitch ���׽d��
	if (pitch > 45.0f)
		pitch = 45.0f;
	if (pitch < -45.0f)
		pitch = -45.0f;
	if (!canAdjustCameraPos) return;
	updateCameraPos(xoffset, yoffset);
	
}

std::string getRootPath() {
	fs::path currentPath = fs::current_path();

	// �M�����ؿ�������]�t "src" �M "resource" ��Ƨ����M�׮ڥؿ�
	while (currentPath.has_parent_path()) {
		if (fs::exists(currentPath / "src") && fs::exists(currentPath / "resource")) {
			std::string rootPath = currentPath.string();

			// �N���|���� "\" ������ "/"
			std::replace(rootPath.begin(), rootPath.end(), '\\', '/');

			// �T�O���|�H "/" ����
			if (rootPath.back() != '/') {
				rootPath += '/';
			}

			return rootPath; // ��^�ڥؿ����|
		}
		currentPath = currentPath.parent_path(); // �V�W�M���@�h
	}

	throw std::runtime_error("�䤣��M�׮ڥؿ��A�нT�{���|�]�m�C");
}