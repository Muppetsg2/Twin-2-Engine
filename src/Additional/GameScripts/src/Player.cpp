#include <Player.h>

//using namespace Twin2Engine::Core;






void Player::Initialize() {
    InitPrices();
    CreateIndicator();

    //if (hexMesh == nullptr) hexMesh = HexGenerator::GenerateHexMesh(0.4f, 0.5f, 0.0f, 0.0f);
    //
    //hexIndicator = new GameObject("hexIndicator", { "MeshFilter", "MeshRenderer" });
    //hexIndicator->GetComponent<MeshFilter>()->mesh = hexMesh;
    //hexIndicator->GetComponent<MeshRenderer>()->material = GameManager::instance->IndicatorMaterial;
    //hexIndicator->GetComponent<MeshRenderer>()->material.color = GameManager::instance->IndicatorColor;
    //hexIndicator->SetActive(false);

    //albumTimer = GameManager::instance->playerInterface.albumTimer;
    //albumTimer->gameObject.SetActive(false);
    //albumButton = GameManager::instance->playerInterface.albumButton;
    //albumButton->onClick.AddListener(this, &Player::AlbumCall);
    //
    //fansTimer = GameManager::instance->playerInterface.fansTimer;
    //fansTimer->gameObject.SetActive(false);
    //fansButton = GameManager::instance->playerInterface.fansButton;
    //fansButton->onClick.AddListener(this, &Player::FansCall);

    move = GetGameObject()->GetComponent<PlayerMovement>();
    move->OnFinishMoving += [this](GameObject* gameObject, HexTile* tile) { FinishMove(tile); };
    move->OnStartMoving += [this](GameObject* gameObject, HexTile* tile) { StartMove(tile); };
    if (patron->patronBonus == PatronBonus::MoveRange) {
        float r = move->radius;
        int s = move->maxSteps;
        move->radius += patron->GetBonus();
        move->maxSteps += static_cast<int>(patron->GetBonus() * (s / r));
    }

    //GameManager::instance->miniGameManager.OnBadNote += [this]() { MinigameEnd(nullptr); };
    //GameManager::instance->miniGameManager.OnWin += [this]() { MinigameEnd(nullptr); };
    //GameManager::instance->miniGameManager.OnTimeEnd += [this]() { MinigameEnd(nullptr); };

    //RockPaperScisorsManager::Instance().OnPlayerWinEvent += [this]() { WonPaperRockScisors(nullptr); };
    //RockPaperScisorsManager::Instance().OnPlayerLoseEvent += [this]() { LostPaperRockScisors(nullptr); };
    //FansControllGameManager::Instance().OnPlayerWinEvent += [this]() { WonFansControl(nullptr); };
    //FansControllGameManager::Instance().OnPlayerLoseEvent += [this]() { LostFansControl(nullptr); };
}

void Player::Update() {
    if (!GameManager::instance->gameStarted) hexIndicator->SetActive(false);

    if (!GameManager::instance->minigameActive && !GameManager::instance->gameOver) {
        UpdatePrices();

        //GameManager::instance->playerInterface.albumText->text = "Album\n" + std::to_string(albumRequiredMoney) + "$";
        //GameManager::instance->playerInterface.fansText->text = "Fans Meeting\n" + std::to_string(fansRequiredMoney) + "$";

        if (isAlbumActive) {
            AlbumUpdate();
            currAlbumTime -= Time::GetDeltaTime();
            if (currAlbumTime < 0.0f) {
                currAlbumCooldown = albumCooldown;
                currAlbumTime = 0.0f;
            }
            //albumTimer->text = std::to_string(static_cast<int>(currAlbumTime)) + " s";
        }
        else {
            //if (albumTimer->gameObject->activeSelf) {
            //    currAlbumCooldown -= Time::deltaTime;
            //    albumTimer->text = "Cooldown: " + std::to_string(static_cast<int>(currAlbumCooldown)) + " s";
            //    if (currAlbumCooldown < 0.0f) {
            //        currAlbumCooldown = 0.0f;
            //        albumTimer->gameObject->SetActive(false);
            //    }
            //}
            //else {
            //    if (money.money < albumRequiredMoney) {
            //        albumButton->interactable = false;
            //    }
            //    else {
            //        albumButton->interactable = true;
            //        if (Input::GetKey(KeyCode::Z)) {
            //            AlbumCall();
            //        }
            //    }
            //}
        }
        AlbumStoppingTakingOverUpdate();

        if (isFansActive) {
            currFansTime -= Time::GetDeltaTime();
            if (currFansTime < 0.0f) {
                currFansCooldown = fansCooldown;
                currFansTime = 0.0f;
            }
            //fansTimer->text = std::to_string(static_cast<int>(currFansTime)) + " s";
        }
        else {
            //if (fansTimer->gameObject->activeSelf) {
            //    currFansCooldown -= Time::deltaTime;
            //    fansTimer->text = "Cooldown: " + std::to_string(static_cast<int>(currFansCooldown)) + " s";
            //    if (currFansCooldown < 0.0f) {
            //        currFansCooldown = 0.0f;
            //        fansTimer->gameObject->SetActive(false);
            //    }
            //}
            //else {
            //    if (money.money < fansRequiredMoney) {
            //        fansButton->interactable = false;
            //    }
            //    else {
            //        fansButton->interactable = true;
            //        if (Input::GetKey(KeyCode::C)) {
            //            FansCall();
            //        }
            //    }
            //}
        }

        if (move->reachEnd) {
            if (isFansActive) {
                if (CurrTile != tileBefore) {
                    FansExit();
                }
            }

            //if (!Input::GetMouseButtonDown(0)) {
            //    tileBefore = CurrTile;
            //}
        }
    }
}

void Player::AlbumCall() {
    //if (money->SpendMoney(albumRequiredMoney)) {
    //    currAlbumTime = albumTime;
    //    //albumTimer->gameObject.SetActive(true);
    //    //albumButton->interactable = false;
    //    // StartCoroutine(AlbumFunc());
    //}
    //else {
    //    std::cout << "Not having required money for album" << std::endl;
    //}
}

void Player::FansCall() {
    //if (money->SpendMoney(fansRequiredMoney)) {
    //    currFansTime = fansTime;
    //    //fansTimer->gameObject.SetActive(true);
    //    //fansButton->interactable = false;
    //    // fansCorountine = StartCoroutine(FansFunc());
    //}
    //else {
    //    std::cout << "Not having required money for fans" << std::endl;
    //}
}

void Player::StartMove(HexTile* tile) {
    if (CurrTile != nullptr) {
        CurrTile->StopTakingOver(this);
    }
    CurrTile = tile;

    GameManager::instance->changeTile = true;

    if (!GameManager::instance->gameStarted) {
        GameManager::instance->gameStarted = true;
        hexIndicator->SetActive(true);
        hexIndicator->GetTransform()->SetGlobalPosition(CurrTile->sterowiecPos);

        //if (islandsWon == 0) {
        //    GameTimer::Instance().ResetTimer();
        //}
        //
        //GameTimer::Instance().StartTimer();

        //HUDInfo* obj = FindObjectOfType<HUDInfo>();
        //if (obj != nullptr) {
        //    obj->ResetInfo();
        //}
    }
    else {
        hexIndicator->SetActive(false);
    }

    if (lost) {
        //HUDInfo* obj = FindObjectOfType<HUDInfo>();
        //if (obj != nullptr) {
        //    obj->ResetInfo();
        //}
        lost = false;
    }
}

void Player::FinishMove(HexTile* tile) {
    if (CurrTile != tile) {
        CurrTile->StopTakingOver(this);
        CurrTile = tile;
    }

    if (CurrTile == nullptr) {
        return;
    }

    hexIndicator->SetActive(true);
    hexIndicator->GetTransform()->SetGlobalPosition(CurrTile->sterowiecPos);

    if (tile->GetMapHexTile()->type == Generation::MapHexTile::HexTileType::RadioStation && tile->state != TileState::Occupied) {
        GameManager::instance->StartMinigame();
    }
    else {
        CurrTile->StartTakingOver(this);
    }
}


void Player::MinigameEnd() {
    //int good = GameManager::instance->miniGameManager.GetGoodNotes();
    //GameManager::instance->EndMinigame();
    //CurrTile->StartTakingOver(this);
    //
    //float radius = 0.5f * good;
    //
    //if (patron->patronBonus == PatronBonus::AbilitiesRange) {
    //    radius += patron->GetBonus();
    //}
    //
    //if (radius > 0) {
    //    CurrTile->StartTakingOver(this);
    //    //StartCoroutine(RadioStationFunc(radius, radioTime));
    //}
    //
    //CurrTile->currCooldown = CurrTile->radioStationCooldown;
}

void Player::StartPaperRockScissors(Playable* playable) {
    GameObject* adjacentTiles[6];
    CurrTile->GetMapHexTile()->tile->GetAdjacentGameObjects(adjacentTiles);// HexMetrics::GetNeighboringGameObjects(CurrTile->tilemap, CurrTile->tilemapPosition);
    int PlayerField = 0;
    int EnemyField = 0;

    HexTile* t;
    for (auto& tile : adjacentTiles) {
        if (tile == nullptr) continue;
        t = tile->GetComponent<HexTile>();

        if (t->takenEntity == playable) {
            EnemyField += 1;
        }
        else if (t->takenEntity == this) {
            PlayerField += 1;
        }
    }

    fightingPlayable = playable;
    GameManager::instance->minigameActive = true;
    //RockPaperScisorsManager::Instance().StartNewGame(this, fightingPlayable, PlayerField, EnemyField);
}

void Player::WonPaperRockScissors(Playable* playable) {
    //fightingPlayable->LostPaperRockScisors(this);

    GameManager::instance->minigameActive = false;

    CurrTile->isFighting = false;
    if (CurrTile->takenEntity == fightingPlayable) {
        CurrTile->ResetTile();
        fightingPlayable->CheckIfDead(this);

        if (GameManager::instance->entities.size() == 1) {
            hexIndicator->SetActive(false);
        }
        else {
            FinishMove(CurrTile);
        }
    }
}

void Player::LostPaperRockScissors(Playable* playable) {
    GameManager::instance->minigameActive = false;

    CurrTile->StopTakingOver(CurrTile->occupyingEntity);
    CurrTile->isFighting = false;
    lost = true;

    //HUDInfo* obj = FindObjectOfType<HUDInfo>();
    //if (obj != nullptr) {
    //    obj->SetInfo("You lost. Move to another field.");
    //}
    //
    //fightingPlayable->WonPaperRockScisors(this);
}

void Player::StartFansControl(Playable* playable) {
    //FansControllGameManager::Instance().StartNewGame(this, fightingPlayable);
    fightingPlayable = playable;
    GameManager::instance->minigameActive = true;
}

void Player::WonFansControl(Playable* playable) {
    fightingPlayable->LostFansControl(this);

    GameManager::instance->minigameActive = false;

    CurrTile->isFighting = false;
    if (CurrTile->takenEntity == dynamic_cast<Playable*>(fightingPlayable)) {
        CurrTile->ResetTile();
        fightingPlayable->CheckIfDead(this);

        if (GameManager::instance->entities.size() == 1) {
            hexIndicator->SetActive(false);
        }
        else {
            FinishMove(CurrTile);
        }
    }
}

void Player::LostFansControl(Playable* playable) {
    GameManager::instance->minigameActive = false;

    CurrTile->StopTakingOver(CurrTile->occupyingEntity);
    CurrTile->isFighting = false;
    lost = true;

    //HUDInfo* obj = FindObjectOfType<HUDInfo>();
    //if (obj != nullptr) {
    //    obj->SetInfo("You lost. Move to another field.");
    //}

    fightingPlayable->WonFansControl(this);
}


void Player::FansControlDraw() {
    //fightingPlayable->WonPaperRockScisors(this);
    GameManager::instance->minigameActive = false;
    if (CurrTile->occupyingEntity == this && CurrTile->takenEntity == this) {
        CurrTile->ResetTile();
    }
}

void Player::OnDead() {
    GameManager::instance->GameOver();
}





YAML::Node Player::Serialize() const
{
    YAML::Node node = Component::Serialize();
    node["type"] = "Player";
    //node["direction"] = light->direction;
    //node["power"] = light->power;

    return node;
}

bool Player::Deserialize(const YAML::Node& node)
{
    if (!Component::Deserialize(node))
        return false;

    //light->direction = node["direction"].as<glm::vec3>();
    //light->power = node["power"].as<float>();

    return true;
}

#if _DEBUG
void Player::DrawEditor()
{
    std::string id = std::string(std::to_string(this->GetId()));
    std::string name = std::string("Player##Component").append(id);
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