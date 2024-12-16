
#ifndef CAMERA_H
#define CAMERA_H

// �ޤJ���n���禡�w�AGLM �Ω�x�}�M�V�q�p��
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>
#include <vector>

// �w�q�۾����ʪ���V�T�|�A�K���H�B�z��J
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// �۾����w�]�ѼơA�p�������B�t�סB�F�ӫשM�Y��
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 10.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;
const glm::vec3 POSITION = glm::vec3(0.0f, 0.0f, 0.0f);
const glm::vec3 WORLDUP = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 FRONT = glm::vec3(0.0f, 0.0f, -1.0f);

// �۾����A�B�z��J�íp��������کԨ��B�V�q�M�x�}
class Camera {
public:
    // �۾��ݩʡA�]�A��m�B��V�M�V�q
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    // �کԨ��]�������M�������^
    float Yaw;
    float Pitch;

    // �۾��ﶵ�A�p���ʳt�שM�F�ӫ�
    float MovementSpeed;
    float MouseSensitivity;
    float zoom;

    // �ϥΦV�q���c�y�禡�A��l�Ƭ۾��ݩ�
    Camera(glm::vec3 position = POSITION, glm::vec3 up = WORLDUP, float yaw = YAW, float pitch = PITCH)
        : Front(FRONT), Yaw(yaw), Pitch(pitch), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        updateCameraVectors(); // ��s�۾��V�q
    }

    // ��^�۾������ϯx�}�A�ϥ� `lookAt` �禡�p��
    glm::mat4 GetViewMatrix() {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // �ھڿ�J���ʬ۾�
    void Move(Camera_Movement direction, float deltaTime) {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
        if (direction == UP)
            Position += Up * velocity;
        if (direction == DOWN)
            Position -= Up * velocity;
    }

    // �ھڹ��п�J����۾�
    void RotateMouse(float xoffset, float yoffset, GLboolean constrainPitch = true) {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;
        Yaw += xoffset;
        Pitch += yoffset;
        if (constrainPitch) {
            if (Pitch > 89.0f) Pitch = 89.0f;
            if (Pitch < -89.0f) Pitch = -89.0f;
        }
        updateCameraVectors(); // ��s�۾��V�q
    }

    // �ϥΨ���¶�S�w��V����۾��]���H���ת�ܡ^
    void RotateRad(Camera_Movement direction, float angle, GLboolean constrainPitch = true) {
        switch (direction) {
        case UP: Pitch -= angle; break;
        case DOWN: Pitch += angle; break;
        case LEFT: Yaw += angle; break;
        case RIGHT: Yaw -= angle; break;
        default: break;
        }
        if (constrainPitch) {
            if (Pitch > 89.0f) Pitch = 89.0f;
            if (Pitch < -89.0f) Pitch = -89.0f;
        }
        updateCameraVectors(); // ��s�۾��V�q
    }

    // ���۾�¶�J�I�i��y�D����
    void Orbit(Camera_Movement direction, float radius, float angle) {
        // 1. �������[���
        Position += Front * radius;

        // 2. �ھڵ��w��V�M���׶i�����
        RotateRad(direction, angle);

        // 3. ��s�۾��V�q
        updateCameraVectors();

        // 4. �N�۾����ʦ^���A�Ϩ�¶�۵J�I����
        Position -= Front * radius;
    }

    // �B�z�u���Y��
    void Zoom(float yoffset) {
        if (zoom >= 1.0f && zoom <= 45.0f)
            zoom -= yoffset;
        if (zoom <= 1.0f)
            zoom = 1.0f;
        if (zoom >= 45.0f)
            zoom = 45.0f;
    }

private:
    // ��s�۾����V�q�A�ھڼکԨ��p��s����V�V�q
    void updateCameraVectors() {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        Right = glm::normalize(glm::cross(Front, WorldUp)); // �p��k�V�q
        Up = glm::normalize(glm::cross(Right, Front)); // �p��W�V�q
    }
};

#endif
