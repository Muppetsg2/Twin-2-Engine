#pragma once

namespace Algorithms 
{
    template<typename T>
    class ObjectsKMeans 
    {
    public:
        
        static std::vector<std::vector<T>> ClusterObjects(int clusterCount, const std::vector<T>& objectsToCluster, std::function<glm::vec3(const T&)> extractVector3);
    private:
        
        static std::vector<glm::vec3> InitializeCentroids(int clusterCount, const std::vector<T>& objectsToCluster, std::function<glm::vec3(const T&)> extractVector3);
        
        static void AssignPointsToClusters(const std::vector<T>& objectsToCluster, const std::vector<glm::vec3>& centroids, std::vector<std::vector<T>>& clusters, std::function<glm::vec3(const T&)> extractVector3);
        
        static int GetNearestCentroidIndex(const T& objectToCluster, const std::vector<glm::vec3>& centroids, std::function<glm::vec3(const T&)> extractVector3);
        
        static std::vector<glm::vec3> UpdateCentroids(const std::vector<std::vector<T>>& clusters, std::function<glm::vec3(const T&)> extractVector3);
    };
}

template<typename T>
std::vector<std::vector<T>> Algorithms::ObjectsKMeans<T>::ClusterObjects(int clusterCount, const std::vector<T>& objectsToCluster, std::function<glm::vec3(const T&)> extractVector3)
{
    std::vector<glm::vec3> centroids = InitializeCentroids(clusterCount, objectsToCluster, extractVector3);

    clusterCount = centroids.size();

    std::vector<std::vector<T>> clusters(clusterCount);
    for (int i = 0; i < clusterCount; i++) {
        clusters[i] = std::vector<T>();
    }

    bool converged = false;
    while (!converged) {
        AssignPointsToClusters(objectsToCluster, centroids, clusters, extractVector3);
        std::vector<glm::vec3> newCentroids = UpdateCentroids(clusters, extractVector3);

        converged = true;
        for (int i = 0; i < clusterCount; i++) {
            if (centroids[i] != newCentroids[i]) {
                converged = false;
                break;
            }
        }

        centroids = newCentroids;
    }

    return clusters;
}

template<typename T>
std::vector<glm::vec3> Algorithms::ObjectsKMeans<T>::InitializeCentroids(int clusterCount, const std::vector<T>& objectsToCluster, function<glm::vec3(const T&)> extractVector3)
{
    std::vector<glm::vec3> centroids;
    std::vector<glm::vec3> centroidsCandidates;
    for (const T& objectToCluster : objectsToCluster) {
        centroidsCandidates.push_back(extractVector3(objectToCluster));
    }
    for (int i = 0; i < clusterCount && !centroidsCandidates.empty(); i++) {
        random_device rd;
        mt19937 g(rd());
        uniform_int_distribution<int> distribution(0, centroidsCandidates.size() - 1);
        int index = distribution(g);
        centroids.push_back(centroidsCandidates[index]);
        centroidsCandidates.erase(centroidsCandidates.begin() + index);
    }
    return centroids;
}

template<typename T>
void Algorithms::ObjectsKMeans<T>::AssignPointsToClusters(const std::vector<T>& objectsToCluster, const std::vector<glm::vec3>& centroids, std::vector<std::vector<T>>& clusters, function<glm::vec3(const T&)> extractVector3)
{
    for (auto& cluster : clusters) {
        cluster.clear();
    }

    for (const T& objectToCluster : objectsToCluster) {
        int nearestCentroidIndex = GetNearestCentroidIndex(objectToCluster, centroids, extractVector3);
        clusters[nearestCentroidIndex].push_back(objectToCluster);
    }
}

template<typename T>
int Algorithms::ObjectsKMeans<T>::GetNearestCentroidIndex(const T& objectToCluster, const std::vector<glm::vec3>& centroids, std::function<glm::vec3(const T&)> extractVector3)
{
    int nearestIndex = 0;
    float minDistance = distance(extractVector3(objectToCluster), centroids[0]);
    for (size_t i = 1; i < centroids.size(); i++) {
        float distance = distance(extractVector3(objectToCluster), centroids[i]);
        if (distance < minDistance) {
            minDistance = distance;
            nearestIndex = i;
        }
    }
    return nearestIndex;
}

template<typename T>
std::vector<glm::vec3> Algorithms::ObjectsKMeans<T>::UpdateCentroids(const std::vector<std::vector<T>>& clusters, std::function<glm::vec3(const T&)> extractVector3)
{
    std::vector<glm::vec3> newCentroids;
    for (const auto& cluster : clusters) {
        if (cluster.empty()) {
            newCentroids.push_back(glm::vec3(0.0f));
        }
        else {
            glm::vec3 centroid(0.0f);
            for (const auto& objectInCluster : cluster) {
                centroid += extractVector3(objectInCluster);
            }
            centroid /= static_cast<float>(cluster.size());
            newCentroids.push_back(centroid);
        }
    }
    return newCentroids;
}
