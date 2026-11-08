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

