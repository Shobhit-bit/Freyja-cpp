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

