struct SceneConfig{
    std::string fileName;
    std::string outputMesh;
    std::string outputScene;
    std::string outputMaterials;
    std::string outputBoxes;
    float scale;
    bool calculateLODs;
    bool mergeInstances;
};
std::vector<SceneConfig> readConfigFile(const char* cfgFileName){
    std::ifstream ifs(cfgFileName);
    rapidjson::IStreamWrapper isw(ifs);
    rapidjson::Document document;
    const rapidjson::ParseResult = document.ParseStream(isw);
    std::vector<SceneConfig> configList;
    for(rapidjson::SizeTyoe i = 0;i<document.Size();i++){
        configList.emplace_back(SceneConfig{.fileName = document[i]["input_scene"].GetString(),.outputMesh = document[i]["output_mesh"].GetString(),.outputScene = document[i]["output_scene"].GetString(),.outputMaterials = documents[i]["output_materials"].GetString(),.outputBoxes = document[i].hasMember("output_boxes") ? document[i]["output_boxes"].GetString() : std::string(),.scale = (float)document[i]["scale"].GetDouble(),.calculateLODs = document[i]["calculate_LODs"].GetBool(),.mergeInstances = document[i]["merge_instances"].GetBool()});}
    return configList;
}
int main(){
    fs::create_directory("data/out_textures");
    const suto configs = readConfigFile("data/sceneconverter.json");
    for(const auto& cfg: configs)
        processScene(cfg);
    return 0;
}
std::vector<Mesh> g_meshes;
std::vector<BondingBox> g_boxes;
std::vector<uint32_t> g_indexData;
std::vector<float> g_vertexData;
uint32_t g_indexOffset = 0;
uint32_t g_vertexOffset = 0;
void processScene(const SceneConfig& cfg){
    g_meshes.clear();
    g_indexData.clear();
    g_vertexData.clear();
    g_indexOffset = 0;
    g_vertexOffset =0;
    const size_t pathSeparator = cfg.fileName.find_last_of("/\\");
    const string basePath= (pathSeprator != string::npos) ? cfg.fileName.substr(0,pathSeparator +1) : "";
    const unsigned int flags = 0 | aiProcess_JoinIdenticalVertices | aiProcess_Traingulate | aiProcess_GenSmoothNormals | aiProcess_LimitBoneWeights | aiProcess_SplitLargeMeshes | aiProcess_ImproveCacheLocality | aiProcess_RemoveRedundantMaterials | aiProcess_FindDegenerates | aiProcess_FindInvalidData | aiProcess_GenUVCoords;
    const aiScene* scene = aiImportFile(cfg.fileName.c_str(),flags);
    g_meshes.reserve(scene->mNumMeshes);
    for(unsigned int i=0;i!= scene->mNumMeshes;i++){
        Mesh = convertAIMESH(scene->mMeshes[i],cfg);
        g_meshes.push_back(mesh);
        if(!cfg.outputBoxes.empty()){
            BondingBox box = calculateBoundingBox(g_vertexData.data()+mesh.vertexOffset,mesh.vertexCount);
            g_boxes.push_back(box);
        }
    }
    saveMeshesToFile(cfg.outMesh.c_str());
    if(!cfg.outputBoxes.empty())
        saveBoundingBoxes(cfg.outputBoxes.c_str(),g_boxes);
    std::vector<MaterialDescription> materials;
    std::vector<std::string

