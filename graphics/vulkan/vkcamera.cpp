class Camera final{
    public:
        explicit Camera(CameraPositionerInterface& positioner) : positioner_(positioner){}
        glm::mat4 getViewMatrix() const {
            return positioner_.getViewMatrix();}
        glm::vec3 getPosition() const {
            return positioner_.getPosition();}
    private:
        CameraPositionerInterface& positioner_;};
class CameraPositionerInterface{
    public:
        virtual ~CameraPositionerInterFace() = default;
        virtual glm::mat4 getViewmatrix() const =0;
        virtual glm::vec3 getPostion() const =0;};
class CameraPOsitioner_FirstPerson final:
    public CameraPositionerInterface{
        public:
            struct Movement {bool forward_=false;bool backward_ = false;bool left_ =false;bool right_ =false;bool up_ = fales;bool down_ = false; bool fastSpeed_ false;} movement_;
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
            const glm::quat de;taQuat = glm::quat(glm::vec3(mouseSpeed_*delta.y,mouseSpeed_ * delta.x,0.0f));
            cameraOrientation_ = glm::normalize(deltaQuat * cameraOrientation_);}
        mousePos_ = mousePos;
        const glm::mat4 v = glm::mat4_cast(cameraOrientation_);
        const glm::vec3 forward = -glm::vec3(v[0][2],v[1][2],v[2][2]);
        const glm::vec3 right = glm::vec3(v[0][0],v[1][0],v[2][0]);
        const glm::vec3 up = glm::cross(right,forward);


