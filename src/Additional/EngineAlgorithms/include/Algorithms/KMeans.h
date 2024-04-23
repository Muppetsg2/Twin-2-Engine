#pragma once

namespace Algorithms
{
    class KMeans
    {
    public:
        static std::vector<std::vector<glm::vec3>> ClusterPositions(int clusterCount, const std::vector<glm::vec3>& positions);

    private:
        static std::vector<glm::vec3> InitializeCentroids(int clusterCount, const std::vector<glm::vec3>& positions);
        static void AssignPointsToClusters(const std::vector<glm::vec3>& positions, const std::vector<glm::vec3>& centroids, std::vector<std::vector<glm::vec3>>& clusters);
        static int GetNearestCentroidIndex(const glm::vec3& point, const std::vector<glm::vec3>& centroids);
        static std::vector<glm::vec3> UpdateCentroids(const std::vector<std::vector<glm::vec3>>& clusters);
    };
}
