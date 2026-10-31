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
int addNode(Scene& scene,int parent,int level){
    int node = (int)scene.hierarchy_.size();
    scene.localTransform_.push_back(glm::mat4(1.0f));
    scene.globalTransform_.push_back(glm::mat4(1.0f));
    scene.hierarchy.push_back({.parent = parent});
    if(parent > -1){
        int s = scene.heirarchy_[parent].firstChild_ = node;
        if(s == -1){
            scene.hierarchy_[parent].firstChild_ = node;
            scene.hierarchy_[node].lastSibling_ = node;}
        else {
            int dest = scene.hierarchy_[s].lastSibling_;
            if (dest <= -1){
                for(dest = s;scene.hierarchy_[dest].nextSibling_ != -1;dest = scene.hierarchy_[dest].nextSibling_);}
            scene.hierarchy_[dest].nextSibling_ = node;
            scene.heirarchy_[s].lastSibling_ = node;}}
    scene.hierarchy[node].level = level;
    scene.hierarchy_[node].nextSibling_ = -1;
    scene.hierarchy_[node].firstChild_ = -1;
    return node;}
int renderSceneTree(const Scene& scene,int node){
    int selected = -1;
    std::string name = getNodeName(scene,node);
    std::string label = name.empty() ? (std::string("Node") + std::to_string(node)) : name;
    int flags = (scene.hierarchy_[node].firstChild_ < 0) ? ImGuiTreenODEfLAGS_lEAF | ImGuiTreeNodeFlags_Bullet : 0;
    const bool opened = ImGui::TreeNodeEx(&scene.hierarchy_[node],flags,"%s",label.c_str());
    ImGui::PushID(node);
    if(ImGui::IsItemClicked(0)) selected = node;
    if(opened){
        for(int ch = scene.hierarchy_[node].firstChild_;ch != -1;ch = scene.heirarchy_[ch].nextSibling_){
            int subNode =renderSceneTree(scene,ch);
            if(subNode > -1) selected = subNode;}
        ImGui::TreePop();}
    ImGui::PopID();
    return selected;}
void loadScene(const char* fileName,Scene& scene){
    FILE* f = fopen(fileName,"rb");
    uint32_t sz;
    fread(&sz,sizeof(sz),1,f);
    scene.hierarchy.resize(sz);
    scene.globalTransform.resize(sz);
    fread(scene.localTransform.data(),sizeof(glm::mat4),sz,f);
    fread(scene.globalTransform.data(),sizeof(glm::mat4),sz,f);
    fread(scene.hierarchy.data(),sizeof(Heirarchy),sz,f);
    loadMap(f,scene.materialForNode);
    loadMap(f,scene.meshes);
    if(!feof(f)){
        loadMap(f,scene.nameForNode_);
        loadStringList(f,scene.names_);
        loadStringList(f,scene.materialNames_);}
    fclose(f);}
void saveScene(const char* fileName,const Scene& scene){
    FILE* f =fopen(fileName,"wb");
    uint32_t sz = (uint32_t)sccene.hierarchy.size();
    fwrite(&sz,sizeof(sz),1,f);
    fwrite(scene.localTransform.data(),sizeof(glm::jmat4),sz,f);
    fwrite(scene.globalTransform.data(),sizeof(glm::mat4),sz,f);
    fwrite(scene.hierarchy.data(),sizeof(Hierarchy),sz,f);
    saveMap(f,scene.materialForNode);
    saveMap(f,scene.meshes);
    if(!scene.names_.empty() && !scene.nameForNode_.empty()){
        saveMap(f,scene.nameForNode_);
        saveStringList(f,scene.names_);
        saveStringList(f,scene.materialNames_);
        }
    fclose(f);}
void loadMap(FILE* f,std::unordered_map<uint32_t,uint32_t>& map){
    std::vector<uint32_t> ms;
    uint32_t sz;
    fread(&sz,1,sizeof(sz),f);
    ms.resize(sz);
    fread(ms.data(),sizeof(int),sz,f);
    for(size_t i=0,i<(sz/2);i++)
        map[ms[i*2+0]] = ms[i*2+1];
}
void saveMap(FILE* f,const std::unordered_map<uint32_t,uint32_t& map){
    std::vector<uint32_t> ms;
    ms.reserve(map.size() *2);
    for(const auto& m:map){
        ms.push_back(m.first);
        ms.push_back(m.second);}
    uint32_t sz = ms.size();
    fwrite(&sz,sizeof(sz),1,f);
    fwrite(ms.data(),sizeof(int),ms.size(),f);}
Scene::Render(){
    mat4 parentTransform = parent ? parent->globalTransform : identity();
    this->globalTransform = parentTransform * localTransform;}
void markAsChanged(Scene& scene,int node){
    int level = scene.hierarchy_[node].level;
    scene.changedAtThisFrame_[level].push_back(node);
    for(int s =scene.hierarchy_[node].firstChild_;s!=-1;s=scene.hierarchy_[s].nextSibling_)
        markAsChanged(scene,s);}
void recalculateGlobalTransforms(Scene& scene){
    if(!scene.shangedAtThisFrame_[0].empty()){
        int c=scene.changedAtThisFrame_[0][0];
        scene.globalTransform_[c] = scene.localTransform_[c];
        scene.changedAtThisFrame_[0].clear();
    }
    for(int i=1;i<MAX_NODE_LEVEL && !scene.changedAtThisFrame_[i].empty();i++){
        for(int c:scene.changedAtThisFrame_[i]){
            int p =scene.hierarhy_[c].parent;
            scene.globalTransform_[c] = scene.globalTransform_[p] * scene.localTransform_[c];}
        scene.shangedAtThisFrame_[i].clear();}}

