#include <RadioStation/RadioStation.h>
#include <RadioStation/RadioStationPlayingController.h>

#include <UI/Canvas.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;
using namespace Twin2Engine::Physic;
using namespace Twin2Engine::UI;

using namespace glm;
using namespace std;

void RadioStation::Initialize()
{
    Canvas* canvas = GetGameObject()->GetComponentInChildren<Canvas>();
    _cooldownTimerText = GetGameObject()->GetComponentInChildren<Text>();
    _cooldownTimerText->SetCanvas(canvas);
}

void RadioStation::Update()
{

    if (_takingOverTimer > 0.0f)
    {
        _takingOverTimer -= Time::GetDeltaTime();
        if (_takingOverTimer <= 0.0f)
        {
            for (HexTile* tile : _takenTiles)
            {
                tile->StopTakingOver(_takenPlayable);
            }
            _takenTiles.clear();
            _takingOverTimer = 0.0f;
        }
    }
    if (_cooldownTimer > 0.0f)
    {
        _cooldownTimer -= Time::GetDeltaTime();
        _cooldownTimerText->SetText(to_wstring((int) glm::round(_cooldownTimer)));
        if (_cooldownTimer <= 0.0f)
        {
            _cooldownTimer = 0.0f;
            _cooldownTimerText->GetGameObject()->SetActive(false);
        }
    }
}

void RadioStation::Play(Playable* playable)
{
    if (_cooldownTimer <= 0.0f && _takingOverTimer <= 0.0f)
    {
        RadioStationPlayingController::Instance()->Play(this, playable);
    }
}

float RadioStation::GetRemainingCooldownTime() const
{
    return _cooldownTimer;
}

void RadioStation::StartTakingOver(Playable* playable, float score)
{
    float usedRadius = score * takingRadius;

    _takingOverTimer = _takingOverTime;
    _cooldownTimer = _cooldown;
    _cooldownTimerText->GetGameObject()->SetActive(true);
    _takenPlayable = playable;

    if (playable->patron->GetPatronBonus() == PatronBonus::ABILITIES_RANGE) {
        usedRadius += playable->patron->GetBonus();
    }

    vector<ColliderComponent*> colliders;
    CollisionManager::Instance()->OverlapSphere(GetGameObject()->GetTransform()->GetGlobalPosition(), usedRadius, colliders);

    vec3 usedGlobalPosition = GetTransform()->GetGlobalPosition();
    usedGlobalPosition.y = 0.0f;

    HexTile* currentTile = GetTransform()->GetParent()->GetGameObject()->GetComponent<HexTile>();

    size_t size = colliders.size();
    ColliderComponent* col = nullptr;
    for (size_t index = 0ull; index < size; ++index){
    //for (auto col : colliders) {
        col = colliders[index];
        if (col)
        {
            auto tile = col->GetGameObject()->GetComponent<HexTile>();

            if (tile && tile != currentTile) {

                vec3 tileUsedGlobalPosition = tile->GetTransform()->GetGlobalPosition();
                tileUsedGlobalPosition.y = 0.0f;

                float mul = glm::distance(usedGlobalPosition, tileUsedGlobalPosition);
                mul = mul > 1.0f ? 1.0f - std::floor(mul) / usedRadius : 1.0f;

                if (mul > 0.0f) {
                    tile->StartRemotelyTakingOver(playable, mul);
                    _takenTiles.push_back(tile);
                }
            }
        }
    }
}



YAML::Node RadioStation::Serialize() const
{
    YAML::Node node = Component::Serialize();

    node["type"] = "RadioStation";
    node["cooldown"] = _cooldown;
    node["takingOverTime"] = _takingOverTime;
    node["takingRadius"] = takingRadius;

	return YAML::Node();
}

bool RadioStation::Deserialize(const YAML::Node& node)
{
    if (!Component::Deserialize(node)) return false;

    _cooldown = node["cooldown"].as<float>();
    _takingOverTime = node["takingOverTime"].as<float>();
    takingRadius = node["takingRadius"].as<float>();

	return true;
}

#if _DEBUG

bool RadioStation::DrawInheritedFields()
{
    if (Component::DrawInheritedFields()) return true;

    ImGui::InputFloat("TakingOver Time: ", &_takingOverTime);
    ImGui::InputFloat("Cooldown: ", &_cooldown);
    ImGui::InputFloat("TakingRadius: ", &takingRadius);

	return false;
}

void RadioStation::DrawEditor()
{
    string id = string(std::to_string(this->GetId()));
    string name = string("RadioStation##Component").append(id);

    if (ImGui::CollapsingHeader(name.c_str())) {

        if (DrawInheritedFields()) return;

        // TODO: Zrobic
    }
}

#endif