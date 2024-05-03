#include <Algorithms/KMeans.h>

using namespace Algorithms;

using namespace glm;
using namespace std;

vector<vector<vec3>> KMeans::ClusterPositions(int clusterCount, const vector<vec3>& positions)
{
    vector<vec3> centroids = InitializeCentroids(clusterCount, positions);

    clusterCount = centroids.size();

    vector<vector<vec3>> clusters(clusterCount);
    for (int i = 0; i < clusterCount; i++)
    {
        clusters[i] = vector<vec3>();
    }

    bool converged = false;
    while (!converged)
    {
        AssignPointsToClusters(positions, centroids, clusters);
        vector<vec3> newCentroids = UpdateCentroids(clusters);

        converged = true;
        for (int i = 0; i < clusterCount; i++)
        {
            if (centroids[i] != newCentroids[i])
            {
                converged = false;
                break;
            }
        }

        centroids = newCentroids;
    }

    return clusters;
}

vector<vec3> KMeans::InitializeCentroids(int clusterCount, const vector<vec3>& positions)
{
    vector<vec3> centroids;
    vector<vec3> centroidsCandidates = positions;
    std::random_device rd;
    std::mt19937 g(rd());
    for (int i = 0; i < clusterCount && !centroidsCandidates.empty(); i++)
    {
        std::uniform_int_distribution<int> distribution(0, centroidsCandidates.size() - 1);
        int index = distribution(g);
        centroids.push_back(centroidsCandidates[index]);
        centroidsCandidates.erase(centroidsCandidates.begin() + index);
    }
    return centroids;
}

void KMeans::AssignPointsToClusters(const vector<vec3>& positions, const vector<vec3>& centroids, vector<vector<vec3>>& clusters)
{
    for (auto& cluster : clusters)
    {
        cluster.clear();
    }

    for (const auto& position : positions)
    {
        int nearestCentroidIndex = GetNearestCentroidIndex(position, centroids);
        clusters[nearestCentroidIndex].push_back(position);
    }
}

int KMeans::GetNearestCentroidIndex(const vec3& point, const vector<vec3>& centroids)
{
    int nearestIndex = 0;
    float minDistance = glm::distance(point, centroids[0]);
    for (size_t i = 1; i < centroids.size(); i++)
    {
        float distance = glm::distance(point, centroids[i]);
        if (distance < minDistance)
        {
            minDistance = distance;
            nearestIndex = i;
        }
    }
    return nearestIndex;
}

vector<vec3> KMeans::UpdateCentroids(const vector<vector<vec3>>& clusters)
{
    vector<vec3> newCentroids;
    for (const auto& cluster : clusters)
    {
        if (cluster.empty())
        {
            newCentroids.push_back(vec3(0.0f));
        }
        else
        {
            vec3 centroid(0.0f);
            for (const auto& position : cluster)
            {
                centroid += position;
            }
            centroid /= static_cast<float>(cluster.size());
            newCentroids.push_back(centroid);
        }
    }
    return newCentroids;
}
