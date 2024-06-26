#include "MultiGraph.h"
#include "Exceptions.h"
#include "IntPair.h"
#include <iostream>
#include <iomanip>
#include <fstream>

#define INF 5000.0

MultiGraph::MultiGraph()
{}

MultiGraph::MultiGraph(const std::string& filePath)
{
    // Tokens
    std::string tokens[5];
    std::ifstream mapFile(filePath.c_str());

    if(!mapFile.is_open())
    {
        std::cout << "Unable to open " << filePath << std::endl;
        return;
    }

    // Read line by line
    std::string line;
    while (std::getline(mapFile, line))
    {
        // Empty Line Skip
        if(line.empty()) continue;
        // Comment Skip
        if(line[0] == '#') continue;

        // Tokenize the line
        int i = 0;
        std::istringstream stream(line);
        while(stream >> tokens[i]) i++;

        // Single token (Meaning it is a vertex)
        if(i == 1)
        {
            InsertVertex(tokens[0]);
        }
        // Exactly 5 tokens (Meaning it is an edge)
        else if(i == 5)
        {
            // Rename vars for readablity
            const std::string& vertexFromName = tokens[0];
            const std::string& vertexToName = tokens[1];
            const std::string& edgeName = tokens[2];
            float weight0 = static_cast<float>(std::atof(tokens[3].c_str()));
            float weight1 = static_cast<float>(std::atof(tokens[4].c_str()));
            AddEdge(edgeName, vertexFromName, vertexToName,
                    weight0, weight1);
        }
        else std::cerr << "Token Size Mismatch" << std::endl;
    }
}

void MultiGraph::PrintPath(const std::vector<int>& orderedVertexEdgeIndexList,
                           float heuristicWeight,
                           bool sameLine) const
{
    // Name is too long
    const std::vector<int>& ove = orderedVertexEdgeIndexList;
    // Invalid list
    // At least three items should be available
    if(ove.size() < 3) return;

    // Check vertex and an edge
    for(size_t i = 0; i < orderedVertexEdgeIndexList.size(); i += 2)
    {
        int vertexId = ove[i];
        if(vertexId >= static_cast<int>(vertexList.size()))
        {
            // Return if there is a bad vertex id
            std::cout << "VertexId " << vertexId
                      << " not found!" << std::endl;
            return;
        }


        const GraphVertex& vertex = vertexList[vertexId];
        std::cout << vertex.name;
        if(!sameLine) std::cout << "\n";
        // Only find and print the weight if next is available
        if(i == ove.size() - 1) break;
        int nextVertexId = ove[i + 2];
        if(nextVertexId >= static_cast<int>(vertexList.size()))
        {
            // Return if there is a bad vertex id
            std::cout << "VertexId " << vertexId
                    << " not found!" << std::endl;
            return;
        }

        // Find the edge between these two vertices
        int localEdgeId = ove[i + 1];
        if(localEdgeId >= static_cast<int>(vertex.edges.size()))
        {
            // Return if there is a bad vertex id
            std::cout << "EdgeId " << localEdgeId
                      << " not found in " << vertexId << "!" << std::endl;
            return;
        }

        const GraphEdge& edge = vertex.edges[localEdgeId];

        // Combine with heuristic (linear interpolation)
        float weight = Lerp(edge.weight[0], edge.weight[1],
                            heuristicWeight);

        std::cout << "-" << std::setfill('-')
                  << std::setw(4)
                  << weight << "->";
    }
    // Print endline on the last vertex if same line is set
    if(sameLine) std::cout << "\n";
    // Reset fill value because it "sticks" to the std out
    std::cout << std::setfill(' ');
    std::cout.flush();
}

void MultiGraph::PrintEntireGraph() const
{
    for(size_t i = 0; i < vertexList.size(); i++)
    {
        const GraphVertex& v = vertexList[i];
        std::cout << v.name << "\n";
        for(size_t j = 0; j < v.edges.size(); j++)
        {
            const GraphEdge& edge = v.edges[j];

            // List the all vertex names and weight
            std::cout << "    -"
                      << std::setfill('-')
                      << std::setw(4) << edge.weight[0]
                      << "-"
                      << std::setw(4) << edge.weight[1]
                      << "-> ";
            std::cout << vertexList[edge.endVertexIndex].name;
            std::cout << " (" << edge.name << ")" << "\n";
        }
    }
    // Reset fill value because it "sticks" to the std out
    std::cout << std::setfill(' ');
    std::cout.flush();
}

float MultiGraph::Lerp(float w0, float w1, float alpha)
{
    float Beta;
    Beta = w0*(1-alpha) + w1*alpha;
    return Beta;
}

void MultiGraph::InsertVertex(const std::string& vertexName)
{
    int size = vertexList.size();
    
    for(int i=0;i<size;i++){
        if(vertexList[i].name==vertexName) throw DuplicateVertexException(vertexName);
    }
    
    // The vertex can be added
    
    GraphVertex A;
    A.name = vertexName;
    vertexList.push_back(A);
}

void MultiGraph::RemoveVertex(const std::string& vertexName)
{
    int size = vertexList.size(), i, j, I, edge_size, remEdgeInd;
    bool flag = true;
    
    for(i=0;i<size;i++){
        if(vertexList[i].name==vertexName){ 
            flag=false;
            I=i;
        }
    }
    
    if(flag) throw VertexNotFoundException(vertexName);
    
    // Start removing process now
    
    for(i=0;i<size;i++){
        edge_size=vertexList[i].edges.size();
        remEdgeInd=-1;
        for(j=0;j<edge_size;j++){
            if(vertexList[i].edges[j].endVertexIndex==I){
                remEdgeInd=j;
                break;
            }
        }
        if(remEdgeInd!=-1){
            vertexList[i].edges.erase(vertexList[i].edges.begin() + remEdgeInd);
        }
    }
    
    vertexList.erase(vertexList.begin() + I);
    
}

void MultiGraph::AddEdge(const std::string& edgeName,
                         const std::string& vertexFromName,
                         const std::string& vertexToName,
                         float weight0, float weight1)
{
    int size = vertexList.size(), i, j, endVerInd;
    bool flag1 = true, flag2 = true;
    
    for(i=0;i<size;i++){
        if(vertexList[i].name==vertexFromName) flag1=false;
        if(vertexList[i].name==vertexToName){ 
            flag2=false;
            endVerInd = i;
        }
    }
    
    if(flag1) throw VertexNotFoundException(vertexFromName);
    if(flag2) throw VertexNotFoundException(vertexToName);
    
    // The vertices exist
    
    for(i=0;i<size;i++){
        if(vertexList[i].name==vertexFromName){
            
            int edge_size=vertexList[i].edges.size();
            
            for(j=0;j<edge_size;j++){
                if(vertexList[i].edges[j].name==edgeName && vertexList[i].edges[j].endVertexIndex==endVerInd){ 
                    throw SameNamedEdgeException(edgeName, vertexFromName, vertexToName);
                }
            }
            
            GraphEdge E = {edgeName, {weight0, weight1}, endVerInd};
            
            vertexList[i].edges.push_back(E);
            
        }
    }
    
}

void MultiGraph::RemoveEdge(const std::string& edgeName,
                            const std::string& vertexFromName,
                            const std::string& vertexToName)
{
    int size = vertexList.size(), i, j, startVerInd, remEdgeInd;
    bool flag1 = true, flag2 = true, flag = true;
    
    for(i=0;i<size;i++){
        if(vertexList[i].name==vertexFromName){ 
            flag1=false;
            startVerInd = i;
        }
        if(vertexList[i].name==vertexToName) flag2=false;
    }
    
    if(flag1) throw VertexNotFoundException(vertexFromName);
    if(flag2) throw VertexNotFoundException(vertexToName);
    
    // The vertices exist
    
    int edge_size = vertexList[startVerInd].edges.size();
    
    for(j=0;j<edge_size;j++){
        if(vertexList[startVerInd].edges[j].name==edgeName){ 
            flag=false;
            remEdgeInd=j;
        }
    }
    
    if(flag) throw EdgeNotFoundException(vertexFromName, edgeName);
    
    vertexList[startVerInd].edges.erase(vertexList[startVerInd].edges.begin() + remEdgeInd);
    
}

bool MultiGraph::HeuristicShortestPath(std::vector<int>& orderedVertexEdgeIndexList,
                                       const std::string& vertexNameFrom,
                                       const std::string& vertexNameTo,
                                       float heuristicWeight) const
{
    int size = vertexList.size(), i, j, FromInd, ToInd;
    bool flag1 = true, flag2 = true, flag = false;
    
    for(i=0;i<size;i++){
        if(vertexList[i].name==vertexNameFrom){ 
            flag1=false;
            FromInd = i;
        }
        if(vertexList[i].name==vertexNameTo){ 
            flag2=false;
            ToInd=i;
        }
    }
    
    if(flag1) throw VertexNotFoundException(vertexNameFrom);
    if(flag2) throw VertexNotFoundException(vertexNameTo);
    
    // The vertices exist
    
    std::vector<float> distance(size, INF);
    std::vector<int> previous(size);
    MinPairHeap<float, int> PQ;
    float dist, B;
    int curr;
    
    PQ.push(Pair<float, int> {0, FromInd});
    distance[FromInd]=0;
    
    while(!PQ.empty()){
        dist=PQ.top().key;
        curr=PQ.top().value;
        
        for(j=0;j<vertexList[curr].edges.size();j++){
            B=Lerp(vertexList[curr].edges[j].weight[0],vertexList[curr].edges[j].weight[1], heuristicWeight);
            
            if(dist+B < distance[vertexList[curr].edges[j].endVertexIndex]){
                distance[vertexList[curr].edges[j].endVertexIndex]=dist+B;
                previous[vertexList[curr].edges[j].endVertexIndex]=curr;
                
                PQ.push(Pair<float, int>{dist+B, vertexList[curr].edges[j].endVertexIndex});
            }
        }
        
        
        
        PQ.pop();
    }
    
    if(distance[ToInd]<INF) flag=true;
    else return false;
    
    std::vector<int> L;
    
    for(curr=ToInd;curr!=FromInd;curr=previous[curr]){
        
        L.push_back(curr);
        
        size=vertexList[previous[curr]].edges.size();
        for(i=0;i<size;i++){
            if(vertexList[previous[curr]].edges[i].endVertexIndex==curr){
                L.push_back(i);
            }
        }
        
    }
    
    orderedVertexEdgeIndexList.push_back(FromInd);
    
    i=L.size()-1;
    while(i){
        orderedVertexEdgeIndexList.push_back(L[i]);
        i--;
    }
    
    orderedVertexEdgeIndexList.push_back(ToInd);
    
    return flag;
}

bool MultiGraph::FilteredShortestPath(std::vector<int>& orderedVertexEdgeIndexList,
                                      const std::string& vertexNameFrom,
                                      const std::string& vertexNameTo,
                                      float heuristicWeight,
                                      const std::vector<std::string>& edgeNames) const
{
    int size = vertexList.size(), i, j, k, FromInd, ToInd;
    bool flag1 = true, flag2 = true, flag = false;
    
    for(i=0;i<size;i++){
        if(vertexList[i].name==vertexNameFrom){ 
            flag1=false;
            FromInd = i;
        }
        if(vertexList[i].name==vertexNameTo){ 
            flag2=false;
            ToInd=i;
        }
    }
    
    if(flag1) throw VertexNotFoundException(vertexNameFrom);
    if(flag2) throw VertexNotFoundException(vertexNameTo);
    
    // The vertices exist
    
    std::vector<float> distance(size, INF);
    std::vector<int> previous(size);
    MinPairHeap<float, int> PQ;
    float dist, B;
    int curr;
    flag1=true;
    
    PQ.push(Pair<float, int> {0, FromInd});
    distance[FromInd]=0;
    
    while(!PQ.empty()){
        dist=PQ.top().key;
        curr=PQ.top().value;
        
        for(j=0;j<vertexList[curr].edges.size();j++){
            
            for(k=0;k<edgeNames.size();k++){
                if(vertexList[curr].edges[j].name==edgeNames[k]) flag1=false;
            }
            
            if(flag1){
                B=Lerp(vertexList[curr].edges[j].weight[0],
                       vertexList[curr].edges[j].weight[1], 
                       heuristicWeight);
                
                if(dist+B < distance[vertexList[curr].edges[j].endVertexIndex]){
                    distance[vertexList[curr].edges[j].endVertexIndex]=dist+B;
                    previous[vertexList[curr].edges[j].endVertexIndex]=curr;
                    
                    PQ.push(Pair<float, int>{dist+B, vertexList[curr].edges[j].endVertexIndex});
                }
            }
            flag1=true;
        }
        
        
        
        PQ.pop();
    }
    
    if(distance[ToInd]<INF) flag=true;
    else return false;
    
    std::vector<int> L;
    
    for(curr=ToInd;curr!=FromInd;curr=previous[curr]){
        
        L.push_back(curr);
        
        size=vertexList[previous[curr]].edges.size();
        for(i=0;i<size;i++){
            if(vertexList[previous[curr]].edges[i].endVertexIndex==curr){
                L.push_back(i);
            }
        }
        
    }
    
    orderedVertexEdgeIndexList.push_back(FromInd);
    
    i=L.size()-1;
    while(i){
        orderedVertexEdgeIndexList.push_back(L[i]);
        i--;
    }
    
    orderedVertexEdgeIndexList.push_back(ToInd);
    
    return flag;
}

int MultiGraph::BiDirectionalEdgeCount() const
{
    int count = 0, i, j, k, destInd;

    for(i = 0; i < vertexList.size(); i++){
        for(j = 0; j < vertexList[i].edges.size(); j++){
            destInd = vertexList[i].edges[j].endVertexIndex;

            for(k = 0; k < vertexList[destInd].edges.size(); k++){
                if (vertexList[destInd].edges[k].endVertexIndex == i &&
                    vertexList[destInd].edges[k].name == vertexList[i].edges[j].name) {
                    count++;
                }
            }
        }
    }

    return count / 2;
}


int MultiGraph::MaxDepthViaEdgeName(const std::string& vertexName,
                                    const std::string& edgeName) const
{
    int size = vertexList.size(), i, count=0, startInd;
    bool flag = true;
    
    for(i=0;i<size;i++){
        if(vertexList[i].name==vertexName){ 
            flag=false;
            startInd=i;
        }
    }
    
    if(flag) throw VertexNotFoundException(vertexName);
    
    // The vertex exists
    
    std::vector<int> depth(size, 0);
    MinPairHeap<int, int> Q;
    int d, curr;
    flag=false;
    
    Q.push(Pair<int, int> {0, startInd});
    
    while(!Q.empty()){
        d=Q.top().key;
        curr=Q.top().value;
        
        for(i=0;i<vertexList[curr].edges.size();i++){
            
            if(vertexList[curr].edges[i].endVertexIndex==startInd) continue;
            
            if(vertexList[curr].edges[i].name==edgeName){
                flag=true;
            }
            
            if(flag){
                
                if(d+1 > depth[vertexList[curr].edges[i].endVertexIndex]){
                    depth[vertexList[curr].edges[i].endVertexIndex]=d+1;
                    
                    Q.push(Pair<int, int> {d+1, vertexList[curr].edges[i].endVertexIndex});
                }
                
                
            }
            
            flag=false;
            
        }
        
        Q.pop();
    }
    
    
    for(i=0;i<depth.size();i++){
        if(depth[i] > count){
            count=depth[i];
        }
    }
    
    return count;
}
