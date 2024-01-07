#include <glad/glad.h>
#include <glm/glm.hpp>

#include <iostream>
#include <ostream>
#include <string>

#include "camera.h"
#include "../world/world.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    MovementSpeed = SPEED;
    updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
    Position = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

void Camera::update(World* world)
{
    if (!fly) {
        glm::vec3 new_pos = Position;
        
        float y_block = world->getBlockInWorld((int)Position.x, (int)(new_pos.y-0.05f), (int)Position.z);
        if (y_block <= 0.0f || y_block == 4.0f)
        {
            new_pos.y -= 0.05f;
        }

        Position = new_pos;
    }
    
}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(Position, Position + Front, Up);
}

// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime, World* world)
{
    if( direction == INCSPEED)
    {
        if (MovementSpeed == SPEED) 
        {
            MovementSpeed = SPEED * 3.0f;
        }
        else 
        {
            MovementSpeed = SPEED;
        }
        return;
    }

    glm::vec3 old_pos = Position;
    glm::vec3 new_pos = Position;
    glm::vec3 delta(0.0f);
    glm::vec3 new_delta(0.0f);
    float velocity = MovementSpeed * deltaTime;

    if (direction == FORWARD)
    {
        delta += Front * velocity;
        //Position += glm::vec3(0.0f, 0.0f, 1.0f) * velocity;
    }
    if (direction == BACKWARD)
    {
        delta -= Front * velocity;
        //Position -= glm::vec3(0.0f, 0.0f, 1.0f) * velocity;
    }
    if (direction == LEFT)
    {
        delta -= Right * velocity;
        //Position -= glm::vec3(1.0f, 0.0f, 0.0f) * velocity;
    }
    if (direction == RIGHT)
    {
        delta += Right * velocity;
        //Position += glm::vec3(1.0f, 0.0f, 0.0f) * velocity;
    }
    if (direction == DOWNWARD)
    {
        delta -= glm::vec3(0.0f, 1.0f, 0.0f) * velocity;
    }
    if (direction == UPWARD)
    {
        if (fly) {
            delta += glm::vec3(0.0f, 1.0f, 0.0f) * velocity;
        } else {
            float player = world->getBlockInWorld((int)Position.x, (int)(Position.y), (int)Position.z);
            float ground = world->getBlockInWorld((int)Position.x, (int)(Position.y) - 1, (int)Position.z);
            if (ground > 0.0f) 
            {
                if (ground == 4.0f) {
                    delta += glm::vec3(0.0f, 0.25f, 0.0f);
                } else {
                    delta += glm::vec3(0.0f, 1.75f, 0.0f);
                }
            }
        }
    }

    
    // gravity
    //Position.y = 0.0f; // <-- this one-liner keeps the user at the ground level (xz plane)

    // collisions separated by x, y, z
    new_pos += delta;
    
    if (collisions)
    {
        float x_block = world->getBlockInWorld((int)(new_pos.x+0.0f), (int)Position.y, (int)Position.z);
        if (x_block <= 0.0f || x_block == 4.0f)
        {
            new_delta.x = delta.x;
        }

        if (fly || direction == UPWARD)
        {
            float y_block = world->getBlockInWorld((int)Position.x, (int)(new_pos.y+0.0f), (int)Position.z);
            if (y_block <= 0.0f || y_block == 4.0f)
            {
                new_delta.y = delta.y;
            }
        }

        float z_block = world->getBlockInWorld((int)Position.x, (int)Position.y, (int)(new_pos.z+0.0f));
        if (z_block <= 0.0f || z_block == 4.0f)
        {
            new_delta.z = delta.z;
        }
    }
    else {
        new_delta = delta;
    }
    //std::cout << "movement:" << delta.x << " y:" << delta.y << " z:" << delta.z << std::endl;
    //std::cout << "movement:" << new_delta.x << " y:" << new_delta.y << " z:" << new_delta.z << std::endl;


    Position += new_delta;
}

// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}

// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void Camera::ProcessMouseScroll(float yoffset)
{
    Zoom -= (float)yoffset;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
}

// calculates the front vector from the Camera's (updated) Euler Angles
void Camera::updateCameraVectors()
{
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front, WorldUp));  
    // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up = glm::normalize(glm::cross(Right, Front));
}
