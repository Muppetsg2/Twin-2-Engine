#include <Player.h>
#include <manager/SceneManager.h>
#include <Abilities/ConcertAbilityController.h>
#include <AreaTaking/GetMoneyFromTiles.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Manager;
using namespace Twin2Engine::Physic;
using namespace Twin2Engine::UI;

void Player::Initialize() {
    Playable::Initialize();

    audioComponent = GetGameObject()->GetComponent<AudioComponent>();

    InitPrices();
    CreateIndicator();
    _tilemap = SceneManager::FindObjectByName("MapGenerator")->GetComponent<Tilemap::HexagonalTilemap>();
    concertAbility = GetGameObject()->GetComponent<ConcertAbilityController>();
    money = GetGameObject()->GetComponent<MoneyGainFromTiles>();

    // ALBUM ABILITY INTIALIZATION
    albumButtonFrameImage = SceneManager::FindObjectByName("AlbumButtonFrame")->GetComponent<Image>();
    albumButtonObject = SceneManager::FindObjectByName("AlbumBg");
    albumButton = albumButtonObject->GetComponent<Button>();
    albumText = SceneManager::FindObjectByName("AlbumCost")->GetComponent<Text>();

    albumCircleImage = SceneManager::FindObjectByName("AlbumBg")->GetComponent<Image>();

    albumButtonEventHandleId = albumButton->GetOnClickEvent() += [&]() { AlbumCall(); };
    albumButtonHoveringEventHandleId = albumButton->GetOnHoverEvent() += [&]() { isHoveringAlbumButton = true; };

    albumButtonDestroyedEventHandleId = albumButtonObject->OnDestroyedEvent += [&](GameObject* gameObject) {
        if (albumButton)
        {
            SPDLOG_INFO("GameObject OnDestroyed: {}", albumButtonDestroyedEventHandleId);
            albumButtonObject->OnDestroyedEvent -= albumButtonDestroyedEventHandleId;
            albumButtonObject = nullptr;
            SPDLOG_INFO("buttonEventHandleId: {}", albumButtonEventHandleId);
            albumButton->GetOnClickEvent() -= albumButtonEventHandleId;
            albumButton = nullptr;
        }
    };

    OnEventAlbumStarted += [&](Playable* playable) -> void {
            audioComponent->SetAudio("res/music/Abilities/AbilitiesUse.mp3");
            audioComponent->Play();
            albumCircleImage->SetLayer(0);
            albumCircleImage->SetColor(_abilityActiveColor);
            //albumCircleImage->GetGameObject()->SetActive(true);
    };

    OnEventAlbumCooldownStarted += [&](Playable* playable) -> void {
            albumCircleImage->SetColor(_abilityCooldownColor);
            albumCircleImage->SetLayer(2);
    };

    OnEventAlbumCooldownFinished += [&](Playable* playable) -> void {
            audioComponent->SetAudio("res/music/Abilities/CooldownEnd.mp3");
            audioComponent->Play();
            albumCircleImage->SetFillProgress(0.f);
            //albumCircleImage->GetGameObject()->SetActive(false);
    };

    //albumCircleImage->GetGameObject()->SetActive(false);

    // FANS MEETING ABILITY INTIALIZATION
    fansMeetingButtonFrameImage = SceneManager::FindObjectByName("FansMeetingButtonFrame")->GetComponent<Image>();
    fansMeetingButtonObject = SceneManager::FindObjectByName("FansBg");
    fansMeetingButton = fansMeetingButtonObject->GetComponent<Button>();
    fansMeetingText = SceneManager::FindObjectByName("FansCost")->GetComponent<Text>();

    fansMeetingCircleImage = SceneManager::FindObjectByName("FansBg")->GetComponent<Image>();

    fansMeetingButtonEventHandleId = fansMeetingButton->GetOnClickEvent() += [&]() { FansMeetingCall(); };
    fansMeetingButtonHoveringEventHandleId = fansMeetingButton->GetOnHoverEvent() += [&]() { isHoveringFansMeetingButton = true; };

    fansMeetingButtonDestroyedEventHandleId = fansMeetingButtonObject->OnDestroyedEvent += [&](GameObject* gameObject) {
        if (fansMeetingButton)
        {
            SPDLOG_INFO("GameObject OnDestroyed: {}", fansMeetingButtonDestroyedEventHandleId);
            fansMeetingButtonObject->OnDestroyedEvent -= fansMeetingButtonDestroyedEventHandleId;
            fansMeetingButtonObject = nullptr;
            SPDLOG_INFO("buttonEventHandleId: {}", fansMeetingButtonEventHandleId);
            fansMeetingButton->GetOnClickEvent() -= fansMeetingButtonEventHandleId;
            fansMeetingButton->GetOnHoverEvent() -= fansMeetingButtonHoveringEventHandleId;
            fansMeetingButton = nullptr;
        }
    };

    OnEventFansMeetingStarted += [&](Playable* playable) -> void {
        audioComponent->SetAudio("res/music/Abilities/AbilitiesUse.mp3");
        audioComponent->Play();
        fansMeetingCircleImage->SetLayer(0);
        fansMeetingCircleImage->SetColor(_abilityActiveColor);
        //fansMeetingCircleImage->GetGameObject()->SetActive(true);
    };
    
    OnEventFansMeetingCooldownStarted += [&](Playable* playable) -> void {
        fansMeetingCircleImage->SetColor(_abilityCooldownColor);
        fansMeetingCircleImage->SetLayer(2);
    };
    
    OnEventFansMeetingCooldownFinished += [&](Playable* playable) -> void {
        audioComponent->SetAudio("res/music/Abilities/CooldownEnd.mp3");
        audioComponent->Play();
        fansMeetingCircleImage->SetFillProgress(0.f);
        //fansMeetingCircleImage->GetGameObject()->SetActive(false);
    };

    //fansMeetingCircleImage->GetGameObject()->SetActive(false);

    // CONCERT ABILITY INTIALIZATION
    concertButtonFrameImage = SceneManager::FindObjectByName("ConcertButtonFrame")->GetComponent<Image>();
    concertButtonObject = SceneManager::FindObjectByName("ConcertBg");
    concertButton = concertButtonObject->GetComponent<Button>();
    concertText = SceneManager::FindObjectByName("ConcertCost")->GetComponent<Text>();

    concertCircleImage = SceneManager::FindObjectByName("ConcertBg")->GetComponent<Image>();

    concertButtonEventHandleId = concertButton->GetOnClickEvent() += [&]() { ConcertCall(); };
    concertButtonHoveringEventHandleId = concertButton->GetOnHoverEvent() += [&]() { isHoveringConcertButton = true; };

    concertButtonDestroyedEventHandleId = concertButtonObject->OnDestroyedEvent += [&](GameObject* gameObject) {
            if (concertButton)
            {
                SPDLOG_INFO("GameObject OnDestroyed: {}", concertButtonDestroyedEventHandleId);
                concertButtonObject->OnDestroyedEvent -= concertButtonDestroyedEventHandleId;
                concertButtonObject = nullptr;
                SPDLOG_INFO("buttonEventHandleId: {}", concertButtonEventHandleId);
                concertButton->GetOnClickEvent() -= concertButtonEventHandleId;
                concertButton = nullptr;
            }
    };


    concertAbility->OnEventAbilityStarted += [&](Playable* playable) -> void {
        audioComponent->SetAudio("res/music/Abilities/AbilitiesUse.mp3");
        audioComponent->Play();
        concertCircleImage->SetLayer(0);
        concertCircleImage->SetColor(_abilityActiveColor);
        //concertCircleImage->GetGameObject()->SetActive(true);
    };

    concertAbility->OnEventAbilityFinished += [&](Playable* playable) -> void {
        PopularityGainingBonusBarController::Instance()->RemoveCurrentBonus(concertAbility->GetAdditionalTakingOverSpeed());
    };
    
    concertAbility->OnEventAbilityCooldownStarted += [&](Playable* playable) -> void {
        concertCircleImage->SetColor(_abilityCooldownColor);
        concertCircleImage->SetLayer(2);
    };

    concertAbility->OnEventAbilityCooldownFinished += [&](Playable* playable) -> void {
        audioComponent->SetAudio("res/music/Abilities/CooldownEnd.mp3");
        audioComponent->Play();
        concertCircleImage->SetFillProgress(0.f);
        //concertCircleImage->GetGameObject()->SetActive(false);
    };

    //concertCircleImage->GetGameObject()->SetActive(false);

    // MONEY UI INITIALIZATION
    moneyText = SceneManager::FindObjectByName("MoneyText")->GetComponent<Text>();
    moneyText->SetText(std::wstring(L"Money: ").append(std::to_wstring(static_cast<int>(money->money))).append(L"$"));


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
    if (patron && patron->GetPatronBonus() == PatronBonus::MOVE_RANGE) {
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
    if (_starPrefab != nullptr) {
        GameObject* m = Twin2Engine::Manager::SceneManager::CreateGameObject(_starPrefab, GetTransform());
        m->GetTransform()->SetLocalPosition(glm::vec3(0.0f, 4.0, 0.0f));
        m->GetTransform()->SetLocalScale(glm::vec3(8.0f, 8.0, 8.0f));
    }

    albumButton->SetInteractable(true);
    fansMeetingButton->SetInteractable(true);
    concertButton->SetInteractable(true);
}

void Player::Update() {
    if (Input::IsKeyPressed(KEY::Z))
    {
        SPDLOG_INFO("Using Album");
        AlbumCall();
    }
    if (Input::IsKeyPressed(KEY::X))
    {
        SPDLOG_INFO("Using Fans");
        FansMeetingCall();
    }
    if (Input::IsKeyPressed(KEY::C))
    {
        SPDLOG_INFO("Using Concert");
        ConcertCall();
    }

    // CONCERT ABILITY UI MANAGEMENT
    if (concertAbility->IsUsed())
    {
        concertCircleImage->SetFillProgress(100.0f - concertAbility->GetAbilityRemainingTime() / concertAbility->lastingTime * 100.0f);
        //concertText->SetText(std::wstring((L"Concert: " + std::to_wstring(static_cast<int>(concertAbility->GetAbilityRemainingTime())) + L"s")));
        //concertText->SetText(std::wstring(std::to_wstring(static_cast<int>(glm::round(concertAbility->GetAbilityRemainingTime())))));
    }
    else if (concertAbility->IsOnCooldown())
    {
        concertCircleImage->SetFillProgress(concertAbility->GetCooldownRemainingTime() / concertAbility->GetCooldown() * 100.0f);
        //concertText->SetText(std::wstring((L"Cooldown: " + std::to_wstring(static_cast<int>(concertAbility->GetCooldownRemainingTime())) + L"s")));
        //concertText->SetText(std::wstring(std::to_wstring(static_cast<int>(glm::round(concertAbility->GetCooldownRemainingTime())))));
    }
    else
    {
        //if (money->money < concertAbility->GetCost()) {
        //    concertButton->SetInteractable(false);
        //}
        //else {
        //    concertButton->SetInteractable(true);
        //}
        concertButton->SetInteractable(true);
        concertText->SetText(std::wstring(std::to_wstring(static_cast<int>(concertAbility->GetCost())).append(L"$")));
    }

    moneyText->SetText(std::wstring(L"Money: ").append(std::to_wstring(static_cast<int>(money->money))).append(L"$"));

    if (!GameManager::instance->gameStarted && hexIndicator) hexIndicator->SetActive(false);

    if (!GameManager::instance->minigameActive && !GameManager::instance->gameOver) {
        UpdatePrices();

        //GameManager::instance->playerInterface.albumText->text = "Album\n" + std::to_string(albumRequiredMoney) + "$";
        //GameManager::instance->playerInterface.fansText->text = "Fans Meeting\n" + std::to_string(fansRequiredMoney) + "$";

        AlbumUpdate();
        // CONCERT ABILITY UI MANAGEMENT
        if (currAlbumTime > 0.0f)
        {
            albumCircleImage->SetFillProgress(100.0f - currAlbumTime / albumTime * 100.0f);
            //albumText->SetText(std::wstring((L"Album: " + std::to_wstring(static_cast<int>(currAlbumTime)) + L"s")));
            //albumText->SetText(std::wstring(std::to_wstring(static_cast<int>(glm::round(currAlbumTime)))));
        }
        else if (currAlbumCooldown > 0.0f)
        {
            albumCircleImage->SetFillProgress(currAlbumCooldown / usedAlbumCooldown * 100.0f);
            //albumText->SetText(std::wstring((L"Cooldown: " + std::to_wstring(static_cast<int>(currAlbumCooldown)) + L"s")));
            //albumText->SetText(std::wstring(std::to_wstring(static_cast<int>(glm::round(currAlbumCooldown)))));
        }
        else
        {
            //if (money->money < concertAbility->GetCost()) {
            //    concertButton->SetInteractable(false);
            //}
            //else {
            //    concertButton->SetInteractable(true);
            //}
            albumButton->SetInteractable(true);
            albumText->SetText(std::wstring(std::to_wstring(static_cast<int>(albumRequiredMoney)).append(L"$")));
        }

        UpdateFans();
        // FANS MEETINNG ABILITY UI MANAGEMENT
        if (currFansTime > 0.0f)
        {
            fansMeetingCircleImage->SetFillProgress(100.0f - currFansTime / fansTime * 100.0f);
            //fansMeetingText->SetText(std::wstring((L"Fans Meeting: " + std::to_wstring(static_cast<int>(currFansTime)) + L"s")));
            //fansMeetingText->SetText(std::wstring(std::to_wstring(static_cast<int>(glm::round(currFansTime)))));
        }
        else if (currFansCooldown > 0.0f)
        {
            fansMeetingCircleImage->SetFillProgress(currFansCooldown / usedFansCooldown * 100.0f);
            //fansMeetingText->SetText(std::wstring((L"Cooldown: " + std::to_wstring(static_cast<int>(currFansCooldown)) + L"s")));
            //fansMeetingText->SetText(std::wstring(std::to_wstring(static_cast<int>(glm::round(currFansCooldown)))));
        }
        else
        {
            //if (money->money < concertAbility->GetCost()) {
            //    concertButton->SetInteractable(false);
            //}
            //else {
            //    concertButton->SetInteractable(true);
            //}
            fansMeetingButton->SetInteractable(true);
            fansMeetingText->SetText(std::wstring(std::to_wstring(static_cast<int>(fansRequiredMoney)).append(L"$")));
        }
        
        if (GameManager::instance->gameStarted)
        {
            // FANS MEETING INTERFACE ELEMENT
            if (isHoveringFansMeetingButton && !isShowingFansMeetingAffectedTiles)
            {
                ShowAffectedTiles();

                fansMeetingButtonObject->GetTransform()->Translate(vec3(0.0f, _buttonDeltaYMovement, 0.0f));
                fansMeetingButtonFrameImage->SetSprite(_spriteButtonStep2);
            }
            else if (!isHoveringFansMeetingButton && isShowingFansMeetingAffectedTiles)
            {
                HideAffectedTiles();

                //fansMeetingButtonObject->GetTransform()->Translate(vec3(0.0f, -_buttonDeltaYMovement, 0.0f));
                fansMeetingButtonObject->GetTransform()->SetLocalPosition(vec3(0.0f, 0.0f, 0.0f));
                fansMeetingButtonFrameImage->SetSprite(_spriteButtonStep1);
            }
            isHoveringFansMeetingButton = false;

            // CONCERT INTERFACE ELEMENT
            if (!concertAbility->IsUsed() && !concertAbility->IsOnCooldown())
            {
                if (isHoveringConcertButton && !isShowingConcertPossible)
                {
                    PopularityGainingBonusBarController::Instance()->AddPossibleBonus(concertAbility->GetAdditionalTakingOverSpeed());
                    isShowingConcertPossible = true;

                    concertButtonObject->GetTransform()->SetLocalScale(vec3(1.1f));
                    audioComponent->SetAudio("res/music/Abilities/UI/OnHoverClick.mp3");
                    audioComponent->Play();

                    concertButtonObject->GetTransform()->Translate(vec3(0.0f, _buttonDeltaYMovement, 0.0f));
                    concertButtonFrameImage->SetSprite(_spriteButtonStep2);
                }
                else if (!isHoveringConcertButton && isShowingConcertPossible)
                {
                    PopularityGainingBonusBarController::Instance()->RemovePossibleBonus(concertAbility->GetAdditionalTakingOverSpeed());
                    isShowingConcertPossible = false;

                    concertButtonObject->GetTransform()->SetLocalScale(vec3(1.0f));
                    audioComponent->SetAudio("res/music/Abilities/UI/OffHoverClick.mp3");
                    audioComponent->Play();

                    //concertButtonObject->GetTransform()->Translate(vec3(0.0f, -_buttonDeltaYMovement, 0.0f));
                    concertButtonObject->GetTransform()->SetLocalPosition(vec3(0.0f, 0.0f, 0.0f));
                    concertButtonFrameImage->SetSprite(_spriteButtonStep1);
                }
                isHoveringConcertButton = false;
            }

            // ALBUM INTERFACE ELEMENT
            if (currAlbumTime <= 0.0f && currAlbumCooldown <= 0.0f)
            {
                if (isHoveringAlbumButton && !isShowingAlbumPossible)
                {
                    for (HexTile* tile : OwnTiles)
                    {
                        tile->EnableAlbumAffected();
                    }
                    isShowingAlbumPossible = true;

                    albumButtonObject->GetTransform()->SetLocalScale(vec3(1.1f));
                    audioComponent->SetAudio("res/music/Abilities/UI/OnHoverClick.mp3");
                    audioComponent->Play();

                    albumButtonObject->GetTransform()->Translate(vec3(0.0f, _buttonDeltaYMovement, 0.0f));
                    albumButtonFrameImage->SetSprite(_spriteButtonStep2);
                }
                else if (!isHoveringAlbumButton && isShowingAlbumPossible)
                {
                    for (HexTile* tile : OwnTiles)
                    {
                        tile->DisableAlbumAffected();
                    }
                    isShowingAlbumPossible = false;

                    albumButtonObject->GetTransform()->SetLocalScale(vec3(1.0f));
                    audioComponent->SetAudio("res/music/Abilities/UI/OffHoverClick.mp3");
                    audioComponent->Play();

                    //albumButtonObject->GetTransform()->Translate(vec3(0.0f, -_buttonDeltaYMovement, 0.0f));
                    albumButtonObject->GetTransform()->SetLocalPosition(vec3(0.0f, 0.0f, 0.0f));
                    albumButtonFrameImage->SetSprite(_spriteButtonStep1);
                }
                isHoveringAlbumButton = false;
            }
        }

        if (move != nullptr) {
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
        else {
            SPDLOG_ERROR("Move was nullptr");
        }
    }
}

void Player::OnDestroy()
{
    Playable::OnDestroy();

    if (PopularityGainingBonusBarController::Instance())
        PopularityGainingBonusBarController::Instance()->RemoveCurrentBonus(TakeOverSpeed);

    // FANS EVENTS
    if (fansMeetingButton != nullptr) {
        if (fansMeetingButtonEventHandleId != -1) {
            fansMeetingButton->GetOnClickEvent() -= fansMeetingButtonEventHandleId;
            fansMeetingButtonEventHandleId = -1;
        }

        if (fansMeetingButtonHoveringEventHandleId != -1) {
            fansMeetingButton->GetOnHoverEvent() -= fansMeetingButtonHoveringEventHandleId;
            fansMeetingButtonHoveringEventHandleId = -1;
        }
    }

    if (fansMeetingButtonDestroyedEventHandleId != -1 && fansMeetingButtonObject != nullptr) {
        fansMeetingButtonObject->OnDestroyedEvent -= fansMeetingButtonDestroyedEventHandleId;
        fansMeetingButtonDestroyedEventHandleId = -1;
    }

    if (concertButton != nullptr) {
        if (concertButtonEventHandleId != -1) {
            concertButton->GetOnClickEvent() -= concertButtonEventHandleId;
            concertButtonEventHandleId = -1;
        }

        if (concertButtonHoveringEventHandleId != -1) {
            concertButton->GetOnHoverEvent() -= concertButtonHoveringEventHandleId;
            concertButtonHoveringEventHandleId = -1;
        }
    }

    if (concertButtonDestroyedEventHandleId != -1 && concertButtonObject != nullptr) {
        concertButtonObject->OnDestroyedEvent -= concertButtonDestroyedEventHandleId;
        concertButtonDestroyedEventHandleId = -1;
    }

    if (albumButton != nullptr) {
        if (albumButtonEventHandleId != -1) {
            albumButton->GetOnClickEvent() -= albumButtonEventHandleId;
            albumButtonEventHandleId = -1;
        }


        if (albumButtonHoveringEventHandleId != -1) {
            albumButton->GetOnHoverEvent() -= albumButtonHoveringEventHandleId;
            albumButtonHoveringEventHandleId = -1;
        }
    }

    if (albumButtonDestroyedEventHandleId != -1 && albumButtonObject != nullptr) {
        albumButtonObject->OnDestroyedEvent -= albumButtonDestroyedEventHandleId;
        albumButtonDestroyedEventHandleId = -1;
    }
}

void Player::StartPlayer(HexTile* startUpTile)
{
    move->StartUp(startUpTile);

    PopularityGainingBonusBarController::Instance()->AddCurrentBonus(TakeOverSpeed);
}

void Player::AlbumCall() {
    if (currAlbumTime <= 0.0f && currAlbumCooldown <= 0.0f && money->SpendMoney(albumRequiredMoney)) {
        currAlbumTime = albumTime;
        albumButton->SetInteractable(false);
        albumButtonObject->GetTransform()->SetLocalScale(vec3(1.0f));
        UseAlbum();

        //albumButtonObject->GetTransform()->Translate(vec3(0.0f, -_buttonDeltaYMovement, 0.0f));
        albumButtonObject->GetTransform()->SetLocalPosition(vec3(0.0f, 0.0f, 0.0f));
        albumButtonFrameImage->SetSprite(_spriteButtonStep1);
    }
    else {
        audioComponent->SetAudio("res/music/Abilities/NotEnoughtRes.mp3");
        audioComponent->Play();
    }
}

void Player::FansMeetingCall() {
    if (currFansTime <= 0.0f && currFansCooldown <= 0.0f && money->SpendMoney(fansRequiredMoney)) {
        currFansTime = fansTime;
        fansMeetingButton->SetInteractable(false);
        fansMeetingButtonObject->GetTransform()->SetLocalScale(vec3(1.0f));
        UseFans();

        //fansMeetingButtonObject->GetTransform()->Translate(vec3(0.0f, -_buttonDeltaYMovement, 0.0f));
        fansMeetingButtonObject->GetTransform()->SetLocalPosition(vec3(0.0f, 0.0f, 0.0f));
        fansMeetingButtonFrameImage->SetSprite(_spriteButtonStep1);
    }
    else {
        audioComponent->SetAudio("res/music/Abilities/NotEnoughtRes.mp3");
        audioComponent->Play();
    }
}

void Player::ConcertCall() {

    if (concertAbility->Use())
    {
        concertButton->SetInteractable(false);
        concertButtonObject->GetTransform()->SetLocalScale(vec3(1.0f));
        PopularityGainingBonusBarController::Instance()->AddCurrentBonus(concertAbility->GetAdditionalTakingOverSpeed());
        PopularityGainingBonusBarController::Instance()->RemovePossibleBonus(concertAbility->GetAdditionalTakingOverSpeed());
        isShowingConcertPossible = false;
        isHoveringConcertButton = false;

        //concertButtonObject->GetTransform()->Translate(vec3(0.0f, -_buttonDeltaYMovement, 0.0f));
        concertButtonObject->GetTransform()->SetLocalPosition(vec3(0.0f, 0.0f, 0.0f));
        concertButtonFrameImage->SetSprite(_spriteButtonStep1);
    }
    else {
        audioComponent->SetAudio("res/music/Abilities/NotEnoughtRes.mp3");
        audioComponent->Play();
    }
}

void Player::ShowAffectedTiles() {

    float usedRadius = fansRadius;
    isShowingFansMeetingAffectedTiles = true;

    if (patron->GetPatronBonus() == PatronBonus::ABILITIES_RANGE) {
        usedRadius += patron->GetBonus();
    }

    vector<ColliderComponent*> colliders;
    ColliderComponent* col;
    CollisionManager::Instance()->OverlapSphere(CurrTile->GetGameObject()->GetTransform()->GetGlobalPosition(), usedRadius, colliders);

    vec3 usedGlobalPosition = GetTransform()->GetGlobalPosition();
    usedGlobalPosition.y = 0.0f;

    size_t size = colliders.size();
    for (size_t index = 0ull; index < size; ++index)
    {
        col = colliders[index];
        if (col)
        {
            HexTile* tile = col->GetGameObject()->GetComponent<HexTile>();

            if (tile->GetMapHexTile()->type == Generation::MapHexTile::HexTileType::Water
                || tile->GetMapHexTile()->type == Generation::MapHexTile::HexTileType::Mountain) continue;

            //if (tile && tile != CurrTile) {

            vec3 tileUsedGlobalPosition = tile->GetTransform()->GetGlobalPosition();
            tileUsedGlobalPosition.y = 0.0f;

            float mul = glm::distance(usedGlobalPosition, tileUsedGlobalPosition);
            mul = mul > 1.0f ? 1.0f - std::floor(mul) / usedRadius : 1.0f;

            if (mul > 0.0f) {
                tile->EnableAffected();
                affectedTiles.push_back(tile);
            }
            //}
        }
    }

    fansMeetingButtonObject->GetTransform()->SetLocalScale(vec3(1.1f));
    // AUDIO
    audioComponent->SetAudio("res/music/Abilities/UI/OnHoverClick.mp3");
    audioComponent->Play();
}

void Player::HideAffectedTiles() {
    for (auto itr = affectedTiles.begin(); itr != affectedTiles.end(); ++itr)
    {
        (*itr)->DisableAffected();
    }
    affectedTiles.clear();

    isShowingFansMeetingAffectedTiles = false;

    fansMeetingButtonObject->GetTransform()->SetLocalScale(vec3(1.0f));
    audioComponent->SetAudio("res/music/Abilities/UI/OffHoverClick.mp3");
    audioComponent->Play();
}

void Player::StartMove(HexTile* tile) {
    if (CurrTile != nullptr) {
        CurrTile->StopTakingOver(this);
    }
    CurrTile = tile;

    GameManager::instance->changeTile = true;

    //if (!GameManager::instance->gameStarted) {
    //    GameManager::instance->gameStarted = true;
    //    hexIndicator->SetActive(true);
    //    hexIndicator->GetTransform()->SetGlobalPosition(CurrTile->sterowiecPos);
    //
    //    //if (islandsWon == 0) {
    //    //    GameTimer::Instance().ResetTimer();
    //    //}
    //    //
    //    //GameTimer::Instance().StartTimer();
    //
    //    //HUDInfo* obj = FindObjectOfType<HUDInfo>();
    //    //if (obj != nullptr) {
    //    //    obj->ResetInfo();
    //    //}
    //}
    //else if (hexIndicator) {
    //    hexIndicator->SetActive(false);
    //}

    if (lost) {
        //HUDInfo* obj = FindObjectOfType<HUDInfo>();
        //if (obj != nullptr) {
        //    obj->ResetInfo();
        //}
        lost = false;
    }
}

void Player::FinishMove(HexTile* tile) {
    SPDLOG_INFO("Finished movement");

    if (CurrTile != tile) {
        CurrTile->StopTakingOver(this);
        CurrTile = tile;
    }

    if (CurrTile == nullptr) {
        return;
    }

    if (hexIndicator)
    {
        hexIndicator->SetActive(true);
        hexIndicator->GetTransform()->SetGlobalPosition(CurrTile->sterowiecPos);
    }

    if (tile->GetMapHexTile()->type == Generation::MapHexTile::HexTileType::RadioStation && tile->state != TileState::OCCUPIED) {
        tile->GetGameObject()->GetComponentInChildren<RadioStation>()->Play(this);
        CurrTile->StartTakingOver(this);
        //GameManager::instance->StartMinigame();
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

//void Player::StartPaperRockScissors(Playable* playable) {
//    GameObject* adjacentTiles[6];
//    CurrTile->GetMapHexTile()->tile->GetAdjacentGameObjects(adjacentTiles);// HexMetrics::GetNeighboringGameObjects(CurrTile->tilemap, CurrTile->tilemapPosition);
//    int PlayerField = 0;
//    int EnemyField = 0;
//
//    HexTile* t;
//    for (auto& tile : adjacentTiles) {
//        if (tile == nullptr) continue;
//        t = tile->GetComponent<HexTile>();
//
//        if (t->takenEntity == playable) {
//            EnemyField += 1;
//        }
//        else if (t->takenEntity == this) {
//            PlayerField += 1;
//        }
//    }
//
//    fightingPlayable = playable;
//    GameManager::instance->minigameActive = true;
//    //RockPaperScisorsManager::Instance().StartNewGame(this, fightingPlayable, PlayerField, EnemyField);
//}

void Player::WonPaperRockScissors(Playable* playable) {
    GameManager::instance->minigameActive = false;
    CurrTile->isFighting = false;

    playable->LostPaperRockScissors(this);

    if (CurrTile->ownerEntity == playable) {
        CurrTile->ResetTile();
    }

    //playable->CheckIfDead(this);

    FinishMove(CurrTile);
    fightingPlayable = nullptr;
    minigameChoice = MinigameRPS_Choice::NONE;
}

void Player::LostPaperRockScissors(Playable* playable) {
    GameManager::instance->minigameActive = false;
    CurrTile->isFighting = false;

    CurrTile->StopTakingOver(this);
    //lost = true;

    GameObject* tiles[6];
    CurrTile->GetMapHexTile()->tile->GetAdjacentGameObjects(tiles);
    for (int i = 0; i < 6; ++i) {
        if (tiles[i] != nullptr) {
            move->reachEnd = true;
            move->MoveAndSetDestination(tiles[i]->GetComponent<HexTile>());
            break;
        }
    }
    fightingPlayable = nullptr;
    minigameChoice = MinigameRPS_Choice::NONE;
}

float Player::GetMaxRadius() const {
    return (move->maxSteps + 0.25) * _tilemap->GetDistanceBetweenTiles();
}

void Player::OnDead() {
    GameManager::instance->GameOver();
}

YAML::Node Player::Serialize() const
{
    YAML::Node node = Playable::Serialize();
    node["type"] = "Player";
    node["abilityActiveColor"] = _abilityActiveColor;
    node["abilityCooldownColor"] = _abilityCooldownColor;
    if (_starPrefab != nullptr) {
        node["starPrefab"] = _starPrefab->GetId();
    }

    node["spriteButtonStep1"] = SceneManager::GetSpriteSaveIdx(_spriteButtonStep1);
    node["spriteButtonStep2"] = SceneManager::GetSpriteSaveIdx(_spriteButtonStep2);

    //node["albumCircleImage"] = albumCircleImage->GetId();
    //node["fansMeetingCircleImage"] = fansMeetingCircleImage->GetId();
    //node["concertCircleImage"] = concertCircleImage->GetId();

    return node;
}

bool Player::Deserialize(const YAML::Node& node)
{
    if (!node["abilityActiveColor"] || !node["abilityCooldownColor"] || !Playable::Deserialize(node))
        return false;

    _abilityActiveColor = node["abilityActiveColor"].as<vec4>();
    _abilityCooldownColor = node["abilityCooldownColor"].as<vec4>();

    if (node["starPrefab"]) {
        _starPrefab = PrefabManager::GetPrefab(SceneManager::GetPrefab(node["starPrefab"].as<size_t>()));
    }
    _spriteButtonStep1 = SceneManager::GetSprite(node["spriteButtonStep1"].as<size_t>());
    _spriteButtonStep2 = SceneManager::GetSprite(node["spriteButtonStep2"].as<size_t>());

    //albumCircleImage = (Image*)SceneManager::GetComponentWithId(node["albumCircleImage"].as<size_t>());
    //fansMeetingCircleImage = (Image*)SceneManager::GetComponentWithId(node["fansMeetingCircleImage"].as<size_t>());
    //concertCircleImage = (Image*)SceneManager::GetComponentWithId(node["concertCircleImage"].as<size_t>());

    return true;
}

#if _DEBUG
void Player::DrawEditor()
{
    std::string id = std::string(std::to_string(this->GetId()));
    std::string name = std::string("Player##Component").append(id);
    if (ImGui::CollapsingHeader(name.c_str())) {

        if (Playable::DrawInheritedFields()) return;

        ImGui::ColorEdit4("AbilityActiveColor", (float*)(&_abilityActiveColor));
        ImGui::ColorEdit4("AbilityCooldownColor", (float*)(&_abilityCooldownColor));
        // TODO: Zrobic
        if (ImGui::Button("Show Affected"))
            ShowAffectedTiles();
        if (ImGui::Button("Hide Affected"))
            HideAffectedTiles();
    }
}
#endif