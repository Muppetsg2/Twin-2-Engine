#pragma once

namespace Algorithms {
    template<typename T>
    class ObjectsKMeans {
    public:
        static std::vector<std::vector<T>> ClusterObjects(int clusterCount, const std::vector<T>& objectsToCluster, std::function<glm::vec3(const T&)> extractVector3);
    private:
        static std::vector<glm::vec3> InitializeCentroids(int clusterCount, const std::vector<T>& objectsToCluster, std::function<glm::vec3(const T&)> extractVector3);
        static void AssignPointsToClusters(const std::vector<T>& objectsToCluster, const std::vector<glm::vec3>& centroids, std::vector<std::vector<T>>& clusters, std::function<glm::vec3(const T&)> extractVector3);
        static int GetNearestCentroidIndex(const T& objectToCluster, const std::vector<glm::vec3>& centroids, std::function<glm::vec3(const T&)> extractVector3);
        static std::vector<glm::vec3> UpdateCentroids(const std::vector<std::vector<T>>& clusters, std::function<glm::vec3(const T&)> extractVector3);
    };
}
