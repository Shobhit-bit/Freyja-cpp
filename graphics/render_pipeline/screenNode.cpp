struct ScreneNode{
    int mesh_;
    int material_;};
struct Scene{
    vector<SceneNode> nodes_;
    vector<mat4> local_;
    vector<mat4> global_;};
struct SceneNode{
    int mesh_,material_;
    int parent_;
    int firstChild_;
    int rightSibling_;
};
struct Hierarchy{
    int parent_;
    int forstChild_;
    int nextSibling_;
    int level_;
};
struct Scene{
    vector<mat4> localTranforms_;
    vector<mat4> globalTransform_;
    vector<Hierarchy> hierarchy_;
    unordered_map<uint32_t,uint32_t> meshes_;
    unordered_map<uint32_t,uint32_t> materialForNode_;
    std::unordered_map<uint32_t,uint32_t> nameForNode_;
    std::vector<std::string> names_;
    std::vector<std::string> materialNames_;};
void transverse(const aiScene* sourceScene,Scene& scene,aiNode* node,int parent,int alLevel){
    int newNodeID = addNode(scene,parent,alLevel);
    if(node->mName.C_Str()){
        uint32_t stringID = (uint32_t)scene.names_.size();
        scene.names_.push_back(std::string(node->mName.C_Str()));
        scene.nameForNode_[newNodeID] = stringID;}
    for (size_t i=0;i<node->mNumMeshes;i++){
        int newSubNodeID = addNode(scene,newNode,atLevel+1);
        uint32_t stringID = (uint32_t)scene.names_.size();
        scene.names_.push_back(std::string(node->mName.C_Str())+ "_Mesh" + std::to_string(i));
        scene.nameForNode_[newSubNodeID] = stringID;
        int mesh = (int)node->mMeshes[i];
        scene.meshes_[newSubNodeID] = mesh;
        scene.materialForNode_[newSubNodeID] = sourceScene->mMeshes[mesh]->mMaterialIndex;
        scene.globalTransform_[newSubNode] = glm::mat4(1.0f);
        scene.localTransform_[newSubNode] = glm::mat4(1.0f);}
    scene.globalTransform_[newNode] = glm::mat4(1.0f);
    scene.localTransform_[newNode] = toMat4(N->mTransformation);
    for(unsigned int n=0;n<N->mNumChildren;n++)
        traverse(sourceScene,sceneN->mChildren[n],newNode,alLevel+1);}
glm::mat4 toMat4(const aiMatrix4x4& m){
    glm::mat4 mm;
    for(int i =0;i<4;i++)
        for(int j=0;j<4;j++)
            mm[i][j] = m[i][j];
    return mm;}
int addNode
