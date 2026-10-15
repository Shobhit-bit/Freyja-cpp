#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

class CameraPositionerInterface{
    public:
        virtual ~CameraPositionerInterface() = default;
        virtual glm::mat4 getViewMatrix() const =0;
        virtual glm::vec3 getPosition() const =0;};
class Camera final{
    public:
        explicit Camera(CameraPositionerInterface& positioner) : positioner_(&positioner){}
        glm::mat4 getViewMatrix() const {
            return positioner_->getViewMatrix();}
        glm::vec3 getPosition() const {
            return positioner_->getPosition();}
    private:
        const CameraPositionerInterface* positioner_;};   // pointer so Camera can be re-assigned
class CameraPositioner_FirstPerson final:
    public CameraPositionerInterface{
        public:
            struct Movement {bool forward_=false;bool backward_ = false;bool left_ =false;bool right_ =false;bool up_ = false;bool down_ = false; bool fastSpeed_ = false;} movement_;
            float mouseSpeed_ =4.0f;
            float acceleration_ =150.0f;
            float damping_ =0.2f;
            float maxSpeed_ = 10.0f;
            float fastCoef_ = 10.0f;
        private:
            glm::vec2 mousePos_ = glm::vec2(0);
            glm::vec3 cameraPosition_ = glm::vec3(0.0f,10.0f,10.0f);
            glm::quat cameraOrientation_ = glm::quat(glm::vec3(0));
            glm::vec3 moveSpeed_ =glm::vec3(0.0f);
    public:
        CameraPositioner_FirstPerson() = default;
        CameraPositioner_FirstPerson(const glm::vec3&pos,const glm::vec3& target,const glm::vec3& up) : cameraPosition_(pos),cameraOrientation_(glm::lookAt(pos,target,up)){}
    void update(double deltaSeconds,const glm::vec2& mousePos,bool mousePressed){
        if(mousePressed){
            const glm::vec2 delta = mousePos-mousePos_;
            const glm::quat deltaQuat = glm::quat(glm::vec3(mouseSpeed_*delta.y,mouseSpeed_ * delta.x,0.0f));
            cameraOrientation_ = glm::normalize(deltaQuat * cameraOrientation_);}
        mousePos_ = mousePos;
        const glm::mat4 v = glm::mat4_cast(cameraOrientation_);
        const glm::vec3 forward = -glm::vec3(v[0][2],v[1][2],v[2][2]);
        const glm::vec3 right = glm::vec3(v[0][0],v[1][0],v[2][0]);
        const glm::vec3 up = glm::cross(right,forward);
        glm::vec3 accel(0.0f);
        if(movement_.forward_) accel+=forward;
        if(movement_.backward_) accel -=forward;
        if(movement_.left_) accel -= right;
        if(movement_.right_) accel +=right;
        if(movement_.up_) accel +=up;
        if(movement_.down_) accel -= up;
        if(movement_.fastSpeed_) accel *= fastCoef_;
        if ( accel == glm::vec3(0)){
            moveSpeed_ -= moveSpeed_ * std::min((1.0f/damping_) * static_cast<float>(deltaSeconds),1.0f);}
        else{
            moveSpeed_ += accel * acceleration_ * static_cast<float>(deltaSeconds);
            const float maxSpeed = movement_.fastSpeed_ ? maxSpeed_ * fastCoef_ : maxSpeed_;
            if (glm::length(moveSpeed_) > maxSpeed)
                moveSpeed_ = glm::normalize(moveSpeed_) * maxSpeed;}
        cameraPosition_ += moveSpeed_ * static_cast<float>(deltaSeconds);}
    virtual glm::mat4 getViewMatrix() const override{
        const glm::mat4 t = glm::translate(glm::mat4(1.0f),-cameraPosition_);
        const glm::mat4 r = glm::mat4_cast(cameraOrientation_);
        return r*t;}
    virtual glm::vec3 getPosition() const override{
        return cameraPosition_;}
    void setPosition(const glm::vec3& pos){
        cameraPosition_ = pos;}
    void setUpVector(const glm::vec3& up){
        const glm::mat4 view = getViewMatrix();
        const glm::vec3 dir = -glm::vec3(view[0][2],view[1][2],view[2][2]);
        cameraOrientation_ = glm::lookAt(cameraPosition_,cameraPosition_+dir,up);}
    void resetMousePosition(const glm::vec2& p){
        mousePos_ = p;}};

// Needed by vkdemof.cpp (reinitCamera / GUI) but missing from the uploaded files
class CameraPositioner_MoveTo final:
    public CameraPositionerInterface{
    public:
        float dampingLinear_ = 10.0f;
        glm::vec3 dampingEulerAngles_ = glm::vec3(5.0f,5.0f,5.0f);
    private:
        glm::vec3 positionCurrent_ = glm::vec3(0.0f);
        glm::vec3 positionDesired_ = glm::vec3(0.0f);
        glm::vec3 anglesCurrent_ = glm::vec3(0.0f);
        glm::vec3 anglesDesired_ = glm::vec3(0.0f);
        glm::mat4 currentTransform_ = glm::mat4(1.0f);
        static glm::vec3 clipAngles(const glm::vec3& a){
            return glm::vec3(std::fmod(a.x,360.0f),std::fmod(a.y,360.0f),std::fmod(a.z,360.0f));}
        static glm::vec3 angleDelta(const glm::vec3& c,const glm::vec3& d){
            const glm::vec3 x = clipAngles(c)-clipAngles(d);
            return x - 360.0f * glm::round(x/360.0f);}
    public:
        CameraPositioner_MoveTo(const glm::vec3& pos,const glm::vec3& angles) : positionCurrent_(pos),positionDesired_(pos),anglesCurrent_(angles),anglesDesired_(angles){
            update(0.0f,glm::vec2(0),false);}
        void update(float deltaSeconds,const glm::vec2& mousePos,bool mousePressed){
            positionCurrent_ += dampingLinear_ * deltaSeconds * (positionDesired_ - positionCurrent_);
            anglesCurrent_ = clipAngles(anglesCurrent_);
            anglesDesired_ = clipAngles(anglesDesired_);
            anglesCurrent_ -= angleDelta(anglesCurrent_,anglesDesired_) * dampingEulerAngles_ * deltaSeconds;
            anglesCurrent_ = clipAngles(anglesCurrent_);
            const glm::vec3 a = glm::radians(anglesCurrent_);
            currentTransform_ = glm::translate(glm::yawPitchRoll(a.y,a.x,a.z),-positionCurrent_);}
        void setPosition(const glm::vec3& p){ positionCurrent_ = p;}
        void setAngles(float pitch,float yaw,float roll){ anglesCurrent_ = glm::vec3(pitch,yaw,roll);}
        void setDesiredPosition(const glm::vec3& p){ positionDesired_ = p;}
        void setDesiredAngles(float pitch,float yaw,float roll){ anglesDesired_ = glm::vec3(pitch,yaw,roll);}
        void setDesiredAngles(const glm::vec3& a){ anglesDesired_ = a;}
        virtual glm::vec3 getPosition() const override{ return positionCurrent_;}
        virtual glm::mat4 getViewMatrix() const override{ return currentTransform_;}};

class FramesPerSecondCounter {
    private:
        const float avgIntervalSec_ = 0.5f;
        unsigned int numFrames_ = 0;
        double accumulatedTime_ =0;
        float currentFPS_ =0.0f;
    public:
        explicit FramesPerSecondCounter(float avgIntervalSec = 0.5f) : avgIntervalSec_(avgIntervalSec){assert(avgIntervalSec > 0.0f);}
        bool tick(float deltaSeconds,bool frameRendered = true){
            if(frameRendered) numFrames_++;
            accumulatedTime_ += deltaSeconds;
            if(accumulatedTime_ < avgIntervalSec_)
                return false;
            currentFPS_ = static_cast<float>(numFrames_/accumulatedTime_);
            printf("FPS: %.1f\n" ,currentFPS_);
            numFrames_ = 0;
            accumulatedTime_ = 0;
            return true;}
    inline float getFPS() const {return currentFPS_;}};

// The OpenGL snippets (glNamedBufferSubData, PerFrameData) and the GLFW callbacks / main loop
// were removed from this file: they are Vulkan-irrelevant or now live in main() in vkdemof.cpp.
