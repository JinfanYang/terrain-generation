#ifndef TERRAIN_CAMERA_H
#define TERRAIN_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

class Camera{

public:
    glm::vec3 camera_pos;
    glm::vec3 camera_point = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::mat4 getViewMatrix(){
        calculateCameraPosition();

        glm::mat4 view = glm::lookAt(camera_pos, camera_point, camera_up);
        return view;
    }

    float camera_distance = 200.0f;
    float angleAround = 0.0f;
    float pitch = 60.0f;
    float yaw = 0.0f;

    void moveForward(){
        camera_point += glm::vec3(0.0f, 0.0f, 5.0f);
    }

    void moveBackward() {
        camera_point -= glm::vec3(0.0f, 0.0f, 5.0f);
    }

    void moveLeft() {
        camera_point -= glm::vec3(5.0f, 0.0f, 0.0f);
    }

    void moveRight() {
        camera_point += glm::vec3(5.0f, 0.0f, 0.0f);
    }

    void calculateZoom(int type) {
        if (camera_distance > 5.0f) {
            float zoomLevel = 5.0f;
            if (type == 1) {
                // Zoom in
                camera_distance -= zoomLevel;
            } else {
                // Zoom out
                camera_distance += zoomLevel;
            }
        }
    }

    void calculatePitch(int type) {
        float pitchLevel = 2.0f;
        if (type == 1){
            // Look up
            pitch -= pitchLevel;
        }
        else {
            // Look down
            pitch += pitchLevel;
        }
    }

    void calculateAround(int type) {
        float angleChange = 10.0f;
        if (type == 1) {
            angleAround += angleChange;
        } else {
            angleAround -= angleChange;
        }
        yaw = 180 - angleAround;
    }

    void calculateCameraPosition() {
        float horizontalDistance = camera_distance * glm::cos(glm::radians(pitch));
        float verticalDistance = camera_distance * glm::sin(glm::radians(pitch));

        float offsetX = horizontalDistance * glm::sin(glm::radians(angleAround));
        float offsetZ = horizontalDistance * glm::cos(glm::radians(angleAround));

        camera_pos.x = camera_point.x - offsetX;
        camera_pos.z = camera_point.z - offsetZ;
        camera_pos.y = camera_point.y + verticalDistance;
    }

};

#endif //TERRAIN_CAMERA_H
