#include <PlayerMovement.h>
#include <physic/CollisionManager.h>
#include <physic/Ray.h>
#include <core/CameraComponent.h>
#include <core/Input.h>
#include <core/Time.h>
#include <cmath>
#include <string>

using namespace Twin2Engine::Physic;
using namespace Twin2Engine::Core;

void PlayerMovement::Initialize() {
	seeker = GetGameObject()->GetComponent<Seeker>();
}

void PlayerMovement::Update() {
    Ray ray = CameraComponent::GetMainCamera()->GetScreenPointRay(Input::GetCursorPos());// Camera.main.ScreenPointToRay(Input.mousePosition);
    RaycastHit raycastHit;

    Transform* transform = GetTransform();
    glm::vec3 position = transform->GetGlobalPosition();

    if (GameManager::instance->gameStarted && !GameManager::instance->minigameActive)
    {
        DrawCircle(steps, radius);

        if (CollisionManager::Instance()->Raycast(ray, raycastHit))
        {
            if (InCircle(raycastHit.position))
            {
                DrawLine(position, glm::vec3(raycastHit.position.x, position.y, raycastHit.position.z));
            }
            else
            {
                //lineRenderer->positionCount = 0;
            }
        }
        else
        {
            //lineRenderer->positionCount = 0;
        }
    }
    else
    {
        //lineRenderer->positionCount = 0;
        //circleRenderer->positionCount = 0;
    }

    if (Input::IsMouseButtonPressed(Input::GetMainWindow(), Twin2Engine::Core::MOUSE_BUTTON::LEFT) && reachEnd && !GameManager::instance->minigameActive)
    {
        if (CollisionManager::Instance()->Raycast(ray, raycastHit))
        {
            Generation::MapHexTile* t = raycastHit.collider->GetGameObject()->GetComponent<Generation::MapHexTile>();
            //if (raycastHit.transform != null && raycastHit.transform.gameObject.TryGetComponent(out HexTile t) && t.Type != TileType.Mountain && !(t.Type == TileType.RadioStation && t.currCooldown > 0f) && !t.IsFighting)
            if (t->type != Generation::MapHexTile::HexTileType::Mountain && !(t->type == Generation::MapHexTile::HexTileType::RadioStation && t->currCooldown > 0.0f) && !t->IsFighting)
            {
                //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                //if (!EventSystem.current.IsPointerOverGameObject())
                {
                    /*
                    if (!GameManager.instance.gameStarted)
                    {
                        transform.position = t.sterowiecPos;
                        OnStartMoving?.Invoke(gameObject, t);
                        destinatedTile = t;
                    }
                    else if (InCircle(raycastHit.transform.position))
                    {
                        if (seeker.IsDone())
                        {
                            tempDest = raycastHit.transform.position;
                            tempDest.y = t.sterowiecPos.y;
                            seeker.StartPath(transform.position, tempDest, OnPathComplete);
                            tempDestTile = t;
                        }
                    }
                    */

                    SetDestination(t);
                    position = transform->GetGlobalPosition();
                }
            }
            //else if (GameManager::instance->gameStarted && raycastHit.transform != null && raycastHit.transform.gameObject.TryGetComponent(out HexTile t2))
            else if (GameManager::instance->gameStarted)
            {
                //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                //HUDInfo obj = FindObjectOfType<HUDInfo>();
                //if (obj != null)
                //{
                //    obj.SetInfo("You cannot enter this field.", 1.5f);
                //}
            }
        }
    }

    //if (path == nullptr)
    //    return;
    //
    //if (currWaypoint >= path->vectorPath.Count /*|| currWaypoint >= actualEnd*/)
    //{
    //    transform->SetGlobalPosition(glm::vec3((path->vectorPath[currWaypoint - 1].x, position.y, path->vectorPath[currWaypoint - 1].z)));
    //    EndMoveAction();
    //    OnFinishMoving.Invoke(GetGameObject(), destinatedTile);
    //    destinatedTile = nullptr;
    //    return;
    //}
    //else
    //{
    //    reachEnd = false;
    //}
    
    //glm::vec3 waypoint(path->vectorPath[currWaypoint].x, position.y, path->vectorPath[currWaypoint].z);
    //glm::vec3 waypoint(destination.x, position.y, destination.z);

    /*
    // Sprawdzenie 1 do przodu
    if (currWaypoint + 1 < path.vectorPath.Count && !alreadyChecked)
    {
        Vector3 f1 = new(path.vectorPath[currWaypoint + 1].x, transform.position.y, path.vectorPath[currWaypoint + 1].z);

        HexTile t1 = FindTile(f1, TileType.Mountain | TileType.Water);

        if (t1 != null)
        {
            if (t1.Type == TileType.Mountain || t1.Type == TileType.Water)
            {
                destinatedTile = FindTile(waypoint, TileType.Empty | TileType.PointOfInterest | TileType.RadioStation);
                actualEnd = currWaypoint + 1;
                ++currWaypoint;
                return;
            }
        }
    }

    // Sprawdzenie 2 do przodu
    if (currWaypoint + 2 < path.vectorPath.Count)
    {
        Vector3 f2 = new(path.vectorPath[currWaypoint + 2].x, transform.position.y, path.vectorPath[currWaypoint + 2].z);

        HexTile t2 = FindTile(f2, TileType.Water | TileType.Mountain);

        if (t2 == null || t2.Type == TileType.Mountain || t2.Type == TileType.Water)
        {
            destinatedTile = FindTile(new(path.vectorPath[currWaypoint + 1].x, transform.position.y, path.vectorPath[currWaypoint + 1].z), TileType.Empty | TileType.PointOfInterest | TileType.RadioStation);
            actualEnd = currWaypoint + 2;
            alreadyChecked = true;
        }
    }
    */

    /*
    if (currWaypoint + 1 == actualEnd || currWaypoint == 0)
    {
        transform.position = Vector3.Lerp(transform.position, waypoint, Time.deltaTime * speed);
    }
    else
    {
        transform.position = Vector3.MoveTowards(transform.position, waypoint, Time.deltaTime * speed);
    }
    */

    if (!reachEnd) {
        float dist = glm::distance(position, destination);
        float walk_dist = Time::GetDeltaTime() * speed;

        if (dist <= walk_dist) {
            transform->SetGlobalPosition(destination); // = Vector3.MoveTowards(position, waypoint, Time::GetDeltaTime() * speed);
            reachEnd = true;
        }
        else {
            transform->SetGlobalPosition(position + glm::normalize(destination - position) * walk_dist);
        }
    }

    //float dist = glm::distance(position, destination);
    //float walk_dist = Time::GetDeltaTime() * speed;
    //
    ////if (currWaypoint + 1 == path->vectorPath.Count || currWaypoint == 0)
    //{
    //    transform->SetGlobalPosition(glm::vec3(std::lerp(position.x, waypoint.x, walk_dist), position.y, glm::vec3(std::lerp(position.z, waypoint.z, walk_dist))));// = Vector3.Lerp(transform.position, waypoint, Time.deltaTime * speed); //Time::GetDeltaTime() * speed
    //}
    //else
    //{
    //    if (dist <= walk_dist) {
    //        transform->SetGlobalPosition(destination); // = Vector3.MoveTowards(position, waypoint, Time::GetDeltaTime() * speed);
    //    }
    //    else {
    //        transform->SetGlobalPosition(position + glm::normalize(waypoint - position) * walk_dist);
    //    }
    //}
    //
    ////if (dist <= nextWaypointDistance)
    //{
    //    ++currWaypoint;
    //}
}   


void PlayerMovement::OnPathComplete(Path* p) {
    //if (!p->error)
    //{
    //    if (p->vectorPath.Count <= maxSteps)
    //    {
    //        path = p;
    //        currWaypoint = 0;
    //        //actualEnd = p.vectorPath.Count;
    //
    //        destination = tempDest;
    //        destinatedTile = tempDestTile;
    //        OnStartMoving.Invoke(GetGameObject(), destinatedTile);
    //    }
    //    else
    //    {
    //        //!!!!!!!!!!!!!!!
    //        //HUDInfo obj = FindObjectOfType<HUDInfo>();
    //        //if (obj != null)
    //        //{
    //        //    obj.SetInfo("The specified field is too far away.", 2f);
    //        //}
    //        EndMoveAction();
    //        OnFindPathError.Invoke(GetGameObject(), tempDestTile);
    //    }
    //}
    //else
    {
        EndMoveAction();
        OnFindPathError.Invoke(GetGameObject(), tempDestTile);
    }

    tempDestTile = nullptr;
}

void PlayerMovement::EndMoveAction() {
    reachEnd = true;
    //alreadyChecked = false;
    path = nullptr;
}

void PlayerMovement::MoveAndSetDestination(Generation::MapHexTile* dest) {
    if (reachEnd && !GameManager::instance->minigameActive)
    {
        if (dest->type != Generation::MapHexTile::HexTileType::Mountain && !(dest->type == Generation::MapHexTile::HexTileType::RadioStation && dest->currCooldown > 0.0f) && !dest->IsFighting)
        {
            SetDestination(dest);
        }
    }
}



bool PlayerMovement::InCircle(glm::vec3 point) {
    glm::vec3 position = GetTransform()->GetGlobalPosition();
    return (point.x - position.x) * (point.x - position.x) + (point.z - position.z) * (point.z - position.z) < radius * radius;
}

void PlayerMovement::SetDestination(Generation::MapHexTile* dest) {
    glm::vec3 destPos = dest->GetTransform()->GetGlobalPosition() + glm::vec3(0.0f, 1.0f, 0.0f);
    if (!GameManager::instance->gameStarted)
    {
        GetTransform()->SetGlobalPosition(destPos);
        destinatedTile = dest;
        destination = destPos;
        GameManager::instance->gameStarted = true;
        OnStartMoving.Invoke(GetGameObject(), dest);
    }
    else if (InCircle(destPos))
    {
        reachEnd = false;
        destinatedTile = dest;
        destination = destPos;
        OnStartMoving.Invoke(GetGameObject(), dest);
        //if (seeker->IsDone())
        //{
        //    tempDest = dest->GetTransform()->GetGlobalPosition();
        //    tempDest.y = dest->sterowiecPos.y;
        //    tempDestTile = dest;
        //
        //    GraphNode node1 = AstarPath.active.GetNearest(transform.position, NNConstraint.Default).node;
        //    GraphNode node2 = AstarPath.active.GetNearest(tempDest, NNConstraint.Default).node;
        //
        //    if (PathUtilities.IsPathPossible(node1, node2))
        //    {
        //        seeker->StartPath(GetTransform()->GetGlobalPosition(), tempDest, OnPathComplete);
        //    }
        //    else
        //    {
        //        //!!!!!!!!!!!!!!!!!
        //        //HUDInfo obj = FindObjectOfType<HUDInfo>();
        //        //if (obj != null)
        //        //{
        //        //    obj.SetInfo("The specified field is not accessible", 2f);
        //        //}
        //        EndMoveAction();
        //        OnFindPathError.Invoke(GetGameObject(), tempDestTile);
        //    }
        //}
    }
}

void PlayerMovement::DrawCircle(int steps, float radius) {
    //circleRenderer->positionCount = steps;

    for (int i = 0; i < steps; ++i)
    {
        float p = (float)i / steps;
        float radian = p * 2 * M_PI;

        float x = glm::cos(radian) * radius;
        float z = glm::sin(radian) * radius;

        glm::vec3 position = GetTransform()->GetGlobalPosition();

        //circleRenderer->SetPosition(i, glm::vec3(position.x + x, position.y, position.z + z));
    }
}

void PlayerMovement::DrawLine(glm::vec3 startPos, glm::vec3 endPos) {
    //lineRenderer->positionCount = 2;
    //lineRenderer->SetPosition(0, startPos);
    //lineRenderer->SetPosition(1, endPos);
}

void PlayerMovement::OnDrawGizmos() {
    if (path != nullptr)
    {
        //Gizmos.color = glm::vec3(1.0f, 0.0f, 0.0f);
        //if (currWaypoint >= path->vectorPath.Count)
        //{
        //    Gizmos.DrawWireSphere(path->vectorPath[path->vectorPath.Count - 1], 0.2f);
        //}
        //else
        //{
        //    Gizmos.DrawWireSphere(path->vectorPath[currWaypoint], 0.2f);
        //}
    }
}

YAML::Node PlayerMovement::Serialize() const
{
    YAML::Node node = Component::Serialize();
    node["type"] = "PlayerMovement";
    //node["direction"] = light->direction;
    //node["power"] = light->power;

    return node;
}

bool PlayerMovement::Deserialize(const YAML::Node& node)
{
    if (!Component::Deserialize(node)) 
        return false;

    //light->direction = node["direction"].as<glm::vec3>();
    //light->power = node["power"].as<float>();

    return true;
}

#if _DEBUG
void PlayerMovement::DrawEditor()
{
    std::string id = std::string(std::to_string(this->GetId()));
    std::string name = std::string("PlayerMovement##Component").append(id);
    if (ImGui::CollapsingHeader(name.c_str())) {

        if (Component::DrawInheritedFields()) return;

        //glm::vec3 v = light->direction;
        //ImGui::DragFloat3(string("Direction##").append(id).c_str(), glm::value_ptr(v), .1f, -1.f, 1.f);
        //if (v != light->direction) {
        //    SetDirection(v);
        //
        //float p = light->power;
        //ImGui::DragFloat(string("Power##").append(id).c_str(), &p);
        //if (p != light->power) {
        //    SetPower(p);
        //}
    }
}
#endif