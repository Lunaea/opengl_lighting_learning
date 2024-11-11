#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
};

// Default camera values
const float YAW             = -90.0f;
const float PITCH           = 0.0f;
const float SPEED           = 2.5f;
const float SENSITIVITY     = 0.1f;
const float ZOOM            = 45.0f;

glm::mat4 lookAt(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up)
{
    glm::vec3 cameraDirection = glm::normalize(position - target);
    glm::vec3 right = glm::normalize(glm::cross(up, cameraDirection));

    glm::mat4 lookAt = glm::mat4(1.0f);
    lookAt[0] = glm::vec4(right.x, up.x, cameraDirection.x, 0.0f);
    lookAt[1] = glm::vec4(right.y, up.y, cameraDirection.y, 0.0f);
    lookAt[2] = glm::vec4(right.z, up.z, cameraDirection.z, 0.0f);
    
    glm::mat4 posMat = glm::mat4(1.0f);
    posMat[3] = glm::vec4(-position, 1.0f);

    return lookAt * posMat;
}

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // camera Attributes
    glm::vec3 Position{};
    glm::vec3 Front{};
    glm::vec3 Up{};
    glm::vec3 Right{};
    glm::vec3 WorldUp{};
    // Euler Angles
    float Yaw{};
    float Pitch{};
    float Roll{};
    // Camera Options
    float MovementSpeed{};
    float MouseSensitivity{};
    float Zoom{};

    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    // constructor with scaler values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw     += xoffset;
        Pitch   += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.9f)
                Pitch = 89.9f;
            if (Pitch < -89.9f)
                Pitch = -89.9f;
        }

        // update Front, Right, and Up Vectors using updated Euler Angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 78.0f)
            Zoom = 78.0f;
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate new Front vector
        glm::vec3 front;
        front.x = std::cos(glm::radians(Yaw)) * std::cos(glm::radians(Pitch));
        front.y = std::sin(glm::radians(Pitch));
        front.z = std::sin(glm::radians(Yaw)) * std::cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }
};
#endif
