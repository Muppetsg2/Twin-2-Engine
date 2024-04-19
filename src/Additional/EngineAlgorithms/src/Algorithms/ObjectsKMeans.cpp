#include <Algorithms/ObjectsKMeans.h>

using namespace Algorithms;
using namespace glm;
using namespace std;

template<typename T>
vector<vector<T>> ObjectsKMeans<T>::ClusterObjects(int clusterCount, const vector<T>& objectsToCluster, function<vec3(const T&)> extractVector3)
{
    vector<vec3> centroids = InitializeCentroids(clusterCount, objectsToCluster, extractVector3);

    clusterCount = centroids.size();

    vector<vector<T>> clusters(clusterCount);
    for (int i = 0; i < clusterCount; i++) {
        clusters[i] = vector<T>();
    }

    bool converged = false;
    while (!converged) {
        AssignPointsToClusters(objectsToCluster, centroids, clusters, extractVector3);
        vector<vec3> newCentroids = UpdateCentroids(clusters, extractVector3);

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
vector<vec3> ObjectsKMeans<T>::InitializeCentroids(int clusterCount, const vector<T>& objectsToCluster, function<vec3(const T&)> extractVector3)
{
    vector<vec3> centroids;
    vector<vec3> centroidsCandidates;
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
void ObjectsKMeans<T>::AssignPointsToClusters(const vector<T>& objectsToCluster, const vector<vec3>& centroids, vector<vector<T>>& clusters, function<vec3(const T&)> extractVector3)
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
int ObjectsKMeans<T>::GetNearestCentroidIndex(const T& objectToCluster, const vector<vec3>& centroids, function<vec3(const T&)> extractVector3) 
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
vector<vec3> ObjectsKMeans<T>::UpdateCentroids(const vector<vector<T>>& clusters, function<vec3(const T&)> extractVector3)
{
    vector<vec3> newCentroids;
    for (const auto& cluster : clusters) {
        if (cluster.empty()) {
            newCentroids.push_back(vec3(0.0f));
        }
        else {
            vec3 centroid(0.0f);
            for (const auto& objectInCluster : cluster) {
                centroid += extractVector3(objectInCluster);
            }
            centroid /= static_cast<float>(cluster.size());
            newCentroids.push_back(centroid);
        }
    }
    return newCentroids;
}
