#pragma once

using namespace glm;

struct FrustumPlane {
	vec3 point = { 0.f, 0.f, 0.f };
	vec3 normal = { 0.f, 1.f, 0.f };
};

struct Frustum
{
    FrustumPlane topFace;
    FrustumPlane bottomFace;

    FrustumPlane rightFace;
    FrustumPlane leftFace;

    FrustumPlane farFace;
    FrustumPlane nearFace;
};