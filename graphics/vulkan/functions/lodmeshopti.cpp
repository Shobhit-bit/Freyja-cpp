void processLODs(std::vector<uint32_t>
indices,const std::vector<float>&
vertices,std::vector<std::vector<uint32_t>>&
outLods){
    size_t verticesCountIn = vertices.size()/3;
    size_t tergetIndicesCount = indices.size();
    uint8_t LOD =1;
    printf("\n LOD0: %i indices",int(indices.size()));
    outLods.push_back(indices);
    while(targetIndicesCount > 1024 && LOD <8){
        targetIndicesCount = indices.size()/2;
        bool sloppy=false;
        size_t numOptIndices =
    meshopt_simplify(indices.data(),indices.data(),(uint32_t)indices.size(),vertices.data(),veticesCountIn,sizeof(float) * 3,targetIndicesCount,0.02f);
        if
