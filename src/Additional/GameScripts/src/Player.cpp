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

    _audioComponent = GetGameObject()->GetComponent<AudioComponent>();

    InitPrices();
    CreateIndicator();
    _tilemap = SceneManager::FindObjectByName("MapGenerator")->GetComponent<Tilemap::HexagonalTilemap>();
    _concertAbility = GetGameObject()->GetComponent<ConcertAbilityController>();
    _money = GetGameObject()->GetComponent<MoneyGainFromTiles>();

    // CONCERT ABILITY INTIALIZATION
    _concertButtonFrameImage = SceneManager::FindObjectByName("ConcertButtonFrame")->GetComponent<Image>();
    _concertButtonObject = SceneManager::FindObjectByName("ConcertBg");
    _concertButton = _concertButtonObject->GetComponent<Button>();
    _concertText = SceneManager::FindObjectByName("ConcertCost")->GetComponent<Text>();

    _concertCircleImage = SceneManager::FindObjectByName("ConcertBg")->GetComponent<Image>();

    _concertButtonEventHandleId = _concertButton->GetOnClickEvent() += [&]() { ConcertCall(); };
    _concertButtonHoveringEventHandleId = _concertButton->GetOnHoverEvent() += [&]() { _isHoveringConcertButton = true; };

    _concertButtonDestroyedEventHandleId = _concertButtonObject->OnDestroyedEvent += [&](GameObject* gameObject) {
        if (_concertButton)
        {
            SPDLOG_INFO("GameObject OnDestroyed: {}", _concertButtonDestroyedEventHandleId);
            _concertButtonObject->OnDestroyedEvent -= _concertButtonDestroyedEventHandleId;
            _concertButtonObject = nullptr;
            SPDLOG_INFO("buttonEventHandleId: {}", _concertButtonEventHandleId);
            _concertButton->GetOnClickEvent() -= _concertButtonEventHandleId;
            _concertButton = nullptr;
        }
    };


    _concertAbility->OnEventAbilityStarted += [&](Playable* playable) -> void {
        _audioComponent->SetAudio(_abilitiesUseAudio);
        _audioComponent->Play();
        _concertCircleImage->SetLayer(0);
        _concertCircleImage->SetColor(_abilityActiveColor);
    };

    _concertAbility->OnEventAbilityFinished += [&](Playable* playable) -> void {
        PopularityGainingBonusBarController::Instance()->RemoveCurrentBonus(_concertAbility->GetAdditionalTakingOverSpeed());
    };

    _concertAbility->OnEventAbilityCooldownStarted += [&](Playable* playable) -> void {
        _concertCircleImage->SetColor(_abilityCooldownColor);
        _concertCircleImage->SetLayer(2);
    };

    _concertAbility->OnEventAbilityCooldownFinished += [&](Playable* playable) -> void {
        _audioComponent->SetAudio(_cooldownEndAudio);
        _audioComponent->Play();
        _concertCircleImage->SetFillProgress(0.f);
    };

    // ALBUM ABILITY INTIALIZATION
    _albumButtonFrameImage = SceneManager::FindObjectByName("AlbumButtonFrame")->GetComponent<Image>();
    _albumButtonObject = SceneManager::FindObjectByName("AlbumBg");
    _albumButton = _albumButtonObject->GetComponent<Button>();
    _albumText = SceneManager::FindObjectByName("AlbumCost")->GetComponent<Text>();

    _albumCircleImage = SceneManager::FindObjectByName("AlbumBg")->GetComponent<Image>();

    _albumButtonEventHandleId = _albumButton->GetOnClickEvent() += [&]() { AlbumCall(); };
    _albumButtonHoveringEventHandleId = _albumButton->GetOnHoverEvent() += [&]() { _isHoveringAlbumButton = true; };

    _albumButtonDestroyedEventHandleId = _albumButtonObject->OnDestroyedEvent += [&](GameObject* gameObject) {
        if (_albumButton)
        {
            SPDLOG_INFO("GameObject OnDestroyed: {}", _albumButtonDestroyedEventHandleId);
            _albumButtonObject->OnDestroyedEvent -= _albumButtonDestroyedEventHandleId;
            _albumButtonObject = nullptr;
            SPDLOG_INFO("buttonEventHandleId: {}", _albumButtonEventHandleId);
            _albumButton->GetOnClickEvent() -= _albumButtonEventHandleId;
            _albumButton = nullptr;
        }
    };

    OnEventAlbumStarted += [&](Playable* playable) -> void {
        _audioComponent->SetAudio(_abilitiesUseAudio);
        _audioComponent->Play();
        _albumCircleImage->SetLayer(0);
        _albumCircleImage->SetColor(_abilityActiveColor);
    };

    OnEventAlbumFinished += [&](Playable* playable) -> void {
        for (HexTile* tile : OwnTiles)
        {
            tile->DisableAlbumAffected();
        }
        _isShowingAlbumPossible = false;
    };

    OnEventAlbumCooldownStarted += [&](Playable* playable) -> void {
        _albumCircleImage->SetColor(_abilityCooldownColor);
        _albumCircleImage->SetLayer(2);
    };

    OnEventAlbumCooldownFinished += [&](Playable* playable) -> void {
        _audioComponent->SetAudio(_cooldownEndAudio);
        _audioComponent->Play();
        _albumCircleImage->SetFillProgress(0.f);
    };

    // FANS MEETING ABILITY INTIALIZATION
    _fansMeetingButtonFrameImage = SceneManager::FindObjectByName("FansMeetingButtonFrame")->GetComponent<Image>();
    _fansMeetingButtonObject = SceneManager::FindObjectByName("FansBg");
    _fansMeetingButton = _fansMeetingButtonObject->GetComponent<Button>();
    _fansMeetingText = SceneManager::FindObjectByName("FansCost")->GetComponent<Text>();

    _fansMeetingCircleImage = SceneManager::FindObjectByName("FansBg")->GetComponent<Image>();

    _fansMeetingButtonEventHandleId = _fansMeetingButton->GetOnClickEvent() += [&]() { FansMeetingCall(); };
    _fansMeetingButtonHoveringEventHandleId = _fansMeetingButton->GetOnHoverEvent() += [&]() { _isHoveringFansMeetingButton = true; };

    _fansMeetingButtonDestroyedEventHandleId = _fansMeetingButtonObject->OnDestroyedEvent += [&](GameObject* gameObject) {
        if (_fansMeetingButton)
        {
            SPDLOG_INFO("GameObject OnDestroyed: {}", _fansMeetingButtonDestroyedEventHandleId);
            _fansMeetingButtonObject->OnDestroyedEvent -= _fansMeetingButtonDestroyedEventHandleId;
            _fansMeetingButtonObject = nullptr;
            SPDLOG_INFO("buttonEventHandleId: {}", _fansMeetingButtonEventHandleId);
            _fansMeetingButton->GetOnClickEvent() -= _fansMeetingButtonEventHandleId;
            _fansMeetingButton->GetOnHoverEvent() -= _fansMeetingButtonHoveringEventHandleId;
            _fansMeetingButton = nullptr;
        }
    };

    OnEventFansMeetingStarted += [&](Playable* playable) -> void {
        _audioComponent->SetAudio(_abilitiesUseAudio);
        _audioComponent->Play();
        _fansMeetingCircleImage->SetLayer(0);
        _fansMeetingCircleImage->SetColor(_abilityActiveColor);
    };
    
    OnEventFansMeetingCooldownStarted += [&](Playable* playable) -> void {
        _fansMeetingCircleImage->SetColor(_abilityCooldownColor);
        _fansMeetingCircleImage->SetLayer(2);
    };
    
    OnEventFansMeetingCooldownFinished += [&](Playable* playable) -> void {
        _audioComponent->SetAudio(_cooldownEndAudio);
        _audioComponent->Play();
        _fansMeetingCircleImage->SetFillProgress(0.f);
    };

    // MONEY UI INITIALIZATION
    _moneyText = SceneManager::FindObjectByName("MoneyText")->GetComponent<Text>();
    _moneyText->SetText(std::wstring(L"Money: ").append(std::to_wstring(static_cast<int>(_money->money))).append(L"$"));

    _negativeMoneyText = SceneManager::FindObjectByName("NegativeMoneyText")->GetComponent<Text>();

    move = GetGameObject()->GetComponent<PlayerMovement>();
    move->OnFinishMoving += [this](GameObject* gameObject, HexTile* tile) { FinishMove(tile); };
    move->OnStartMoving += [this](GameObject* gameObject, HexTile* tile) { StartMove(tile); };
    if (patron && patron->GetPatronBonus() == PatronBonus::MOVE_RANGE) {
        float r = move->radius;
        int s = move->maxSteps;
        move->radius += patron->GetBonus();
        move->maxSteps += static_cast<int>(patron->GetBonus() * (s / r));
    }

    if (_starPrefab != nullptr) {
        GameObject* m = Twin2Engine::Manager::SceneManager::CreateGameObject(_starPrefab, GetTransform());
        m->GetTransform()->SetLocalPosition(glm::vec3(0.0f, 4.0, 0.0f));
        m->GetTransform()->SetLocalScale(glm::vec3(8.0f, 8.0, 8.0f));
    }
}
void Player::Update() {

    if (!GameManager::instance->gameStarted && _hexIndicator) _hexIndicator->SetActive(false);

    if (!GameManager::instance->gameStarted) return;

    _moneyText->SetText(std::wstring(L"Money: ").append(std::to_wstring(static_cast<int>(_money->money))).append(L"$"));

    if (_negativeMoneyText->GetGameObject()->GetActive())
    {
        int value = _money->money;
        int count = 0;

        do {
            ++count;
            value /= 10;
        } while (value);
        _negativeMoneyText->GetTransform()->SetLocalPosition(vec3(_negativeMoneyTextXOffset + count * _negativeMoneyTextLetterWidth, 0.0f, 0.0f));

        value = _money->money;
        if (_isHoveringFansMeetingButton)
        {
            value -= fansRequiredMoney;

        }
        else if (_isHoveringConcertButton)
        {
            value -= _concertAbility->GetCost();
        }
        else if (_isHoveringAlbumButton)
        {
            value -= albumRequiredMoney;
        }
        else
        {
            _negativeMoneyText->GetGameObject()->SetActive(false);
        }

        if (value >= 0)
        {
            _negativeMoneyText->SetColor(_enoughMoneyColor);
        }
        else
        {
            _negativeMoneyText->SetColor(_notEnoughMoneyColor);
        }
        _negativeMoneyText->SetText(std::to_wstring(value).append(L"$"));

    }

    if (!GameManager::instance->minigameActive && !GameManager::instance->gameOver) {
        UpdatePrices();

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
        _concertText->SetText(std::wstring(std::to_wstring(static_cast<int>(_concertAbility->GetCost())).append(L"$")));
        if (_concertAbility->IsUsed())
        {
            _concertCircleImage->SetFillProgress(100.0f - _concertAbility->GetAbilityRemainingTime() / _concertAbility->lastingTime * 100.0f);
        }
        else if (_concertAbility->IsOnCooldown())
        {
            _concertCircleImage->SetFillProgress(_concertAbility->GetCooldownRemainingTime() / _concertAbility->GetCooldown() * 100.0f);
        }
        else
        {
            _concertButton->SetInteractable(true);
        }


        // CONCERT ABILITY UI MANAGEMENT
        AlbumUpdate();
        _albumText->SetText(std::wstring(std::to_wstring(static_cast<int>(albumRequiredMoney)).append(L"$")));
        if (currAlbumTime > 0.0f)
        {
            _albumCircleImage->SetFillProgress(100.0f - currAlbumTime / albumTime * 100.0f);
        }
        else if (currAlbumCooldown > 0.0f)
        {
            _albumCircleImage->SetFillProgress(currAlbumCooldown / usedAlbumCooldown * 100.0f);
        }
        else
        {
            _albumButton->SetInteractable(true);
        }

        // FANS MEETINNG ABILITY UI MANAGEMENT
        UpdateFans();
        _fansMeetingText->SetText(std::wstring(std::to_wstring(static_cast<int>(fansRequiredMoney)).append(L"$")));
        if (currFansTime > 0.0f)
        {
            _fansMeetingCircleImage->SetFillProgress(100.0f - currFansTime / fansTime * 100.0f);
        }
        else if (currFansCooldown > 0.0f)
        {
            _fansMeetingCircleImage->SetFillProgress(currFansCooldown / usedFansCooldown * 100.0f);
        }
        else
        {
            _fansMeetingButton->SetInteractable(true);
        }

        if (GameManager::instance->gameStarted)
        {
            // FANS MEETING INTERFACE ELEMENT
            if (_isHoveringFansMeetingButton && !_isShowingFansMeetingAffectedTiles)
            {
                ShowAffectedTiles();

                _fansMeetingButtonObject->GetTransform()->Translate(vec3(0.0f, _buttonDeltaYMovement, 0.0f));
                _fansMeetingButtonFrameImage->SetSprite(_spriteButtonStep2);

                _negativeMoneyText->GetGameObject()->SetActive(true);
            }
            else if (!_isHoveringFansMeetingButton && _isShowingFansMeetingAffectedTiles)
            {
                HideAffectedTiles();

                //fansMeetingButtonObject->GetTransform()->Translate(vec3(0.0f, -_buttonDeltaYMovement, 0.0f));
                _fansMeetingButtonObject->GetTransform()->SetLocalPosition(vec3(0.0f, 0.0f, 0.0f));
                _fansMeetingButtonFrameImage->SetSprite(_spriteButtonStep1);

                //_negativeMoneyText->GetGameObject()->SetActive(false);
            }
            _isHoveringFansMeetingButton = false;

            // CONCERT INTERFACE ELEMENT
            if (!_concertAbility->IsUsed() && !_concertAbility->IsOnCooldown())
            {
                if (_isHoveringConcertButton && !_isShowingConcertPossible)
                {
                    PopularityGainingBonusBarController::Instance()->AddPossibleBonus(_concertAbility->GetAdditionalTakingOverSpeed());
                    _isShowingConcertPossible = true;

                    _concertButtonObject->GetTransform()->SetLocalScale(vec3(1.1f));
                    _audioComponent->SetAudio(_onHoverClickAudio);
                    _audioComponent->Play();

                    _concertButtonObject->GetTransform()->Translate(vec3(0.0f, _buttonDeltaYMovement, 0.0f));
                    _concertButtonFrameImage->SetSprite(_spriteButtonStep2);

                    _negativeMoneyText->GetGameObject()->SetActive(true);
                }
                else if (!_isHoveringConcertButton && _isShowingConcertPossible)
                {
                    PopularityGainingBonusBarController::Instance()->RemovePossibleBonus(_concertAbility->GetAdditionalTakingOverSpeed());
                    _isShowingConcertPossible = false;

                    _concertButtonObject->GetTransform()->SetLocalScale(vec3(1.0f));
                    _audioComponent->SetAudio(_offHoverClickAudio);
                    _audioComponent->Play();

                    _concertButtonObject->GetTransform()->SetLocalPosition(vec3(0.0f, 0.0f, 0.0f));
                    _concertButtonFrameImage->SetSprite(_spriteButtonStep1);

                    //_negativeMoneyText->GetGameObject()->SetActive(false);
                }
                _isHoveringConcertButton = false;
            }

            // ALBUM INTERFACE ELEMENT
            if (currAlbumTime <= 0.0f && currAlbumCooldown <= 0.0f)
            {
                if (_isHoveringAlbumButton && !_isShowingAlbumPossible)
                {
                    for (HexTile* tile : OwnTiles)
                    {
                        tile->EnableAlbumAffected();
                    }
                    _isShowingAlbumPossible = true;

                    _albumButtonObject->GetTransform()->SetLocalScale(vec3(1.1f));
                    _audioComponent->SetAudio(_onHoverClickAudio);
                    _audioComponent->Play();

                    _albumButtonObject->GetTransform()->Translate(vec3(0.0f, _buttonDeltaYMovement, 0.0f));
                    _albumButtonFrameImage->SetSprite(_spriteButtonStep2);

                    _negativeMoneyText->GetGameObject()->SetActive(true);
                }
                else if (!_isHoveringAlbumButton && _isShowingAlbumPossible)
                {
                    for (HexTile* tile : OwnTiles)
                    {
                        tile->DisableAlbumAffected();
                    }
                    _isShowingAlbumPossible = false;

                    _albumButtonObject->GetTransform()->SetLocalScale(vec3(1.0f));
                    _audioComponent->SetAudio(_offHoverClickAudio);
                    _audioComponent->Play();

                    //albumButtonObject->GetTransform()->Translate(vec3(0.0f, -_buttonDeltaYMovement, 0.0f));
                    _albumButtonObject->GetTransform()->SetLocalPosition(vec3(0.0f, 0.0f, 0.0f));
                    _albumButtonFrameImage->SetSprite(_spriteButtonStep1);

                    //_negativeMoneyText->GetGameObject()->SetActive(false);
                }
                _isHoveringAlbumButton = false;
            }
        }
    }
}

void Player::OnDestroy()
{
    Playable::OnDestroy();

    if (PopularityGainingBonusBarController::Instance())
        PopularityGainingBonusBarController::Instance()->RemoveCurrentBonus(TakeOverSpeed);

    // FANS EVENTS
    if (_fansMeetingButton != nullptr) {
        if (_fansMeetingButtonEventHandleId != -1) {
            _fansMeetingButton->GetOnClickEvent() -= _fansMeetingButtonEventHandleId;
            _fansMeetingButtonEventHandleId = -1;
        }

        if (_fansMeetingButtonHoveringEventHandleId != -1) {
            _fansMeetingButton->GetOnHoverEvent() -= _fansMeetingButtonHoveringEventHandleId;
            _fansMeetingButtonHoveringEventHandleId = -1;
        }
    }

    if (_fansMeetingButtonDestroyedEventHandleId != -1 && _fansMeetingButtonObject != nullptr) {
        _fansMeetingButtonObject->OnDestroyedEvent -= _fansMeetingButtonDestroyedEventHandleId;
        _fansMeetingButtonDestroyedEventHandleId = -1;
    }

    if (_concertButton != nullptr) {
        if (_concertButtonEventHandleId != -1) {
            _concertButton->GetOnClickEvent() -= _concertButtonEventHandleId;
            _concertButtonEventHandleId = -1;
        }

        if (_concertButtonHoveringEventHandleId != -1) {
            _concertButton->GetOnHoverEvent() -= _concertButtonHoveringEventHandleId;
            _concertButtonHoveringEventHandleId = -1;
        }
    }

    if (_concertButtonDestroyedEventHandleId != -1 && _concertButtonObject != nullptr) {
        _concertButtonObject->OnDestroyedEvent -= _concertButtonDestroyedEventHandleId;
        _concertButtonDestroyedEventHandleId = -1;
    }

    if (_albumButton != nullptr) {
        if (_albumButtonEventHandleId != -1) {
            _albumButton->GetOnClickEvent() -= _albumButtonEventHandleId;
            _albumButtonEventHandleId = -1;
        }


        if (_albumButtonHoveringEventHandleId != -1) {
            _albumButton->GetOnHoverEvent() -= _albumButtonHoveringEventHandleId;
            _albumButtonHoveringEventHandleId = -1;
        }
    }

    if (_albumButtonDestroyedEventHandleId != -1 && _albumButtonObject != nullptr) {
        _albumButtonObject->OnDestroyedEvent -= _albumButtonDestroyedEventHandleId;
        _albumButtonDestroyedEventHandleId = -1;
    }
}

void Player::StartPlayer(HexTile* startUpTile)
{
    move->StartUp(startUpTile);

    GameObject* MovingAroundTut = SceneManager::FindObjectByName("MovingAroundTut");
    if (MovingAroundTut != nullptr) {
        MovingAroundTut->SetActive(true);
    }

    PopularityGainingBonusBarController::Instance()->AddCurrentBonus(TakeOverSpeed);
}

void Player::AlbumCall() {
    if (currAlbumTime <= 0.0f && currAlbumCooldown <= 0.0f && _money->SpendMoney(albumRequiredMoney)) {
        currAlbumTime = albumTime;
        _albumButton->SetInteractable(false);
        _albumButtonObject->GetTransform()->SetLocalScale(vec3(1.0f));
        UseAlbum();

        for (HexTile* tile : OwnTiles)
        {
            tile->EnableAlbumAffected();
        }
        _isShowingAlbumPossible = true;

        _albumButtonObject->GetTransform()->SetLocalPosition(vec3(0.0f, 0.0f, 0.0f));
        _albumButtonFrameImage->SetSprite(_spriteButtonStep1);
    }
    else {
        _audioComponent->SetAudio(_notEnoughResAudio);
        _audioComponent->Play();
    }
}

void Player::FansMeetingCall() {
    if (currFansTime <= 0.0f && currFansCooldown <= 0.0f && _money->SpendMoney(fansRequiredMoney)) {
        currFansTime = fansTime;
        _fansMeetingButton->SetInteractable(false);
        _fansMeetingButtonObject->GetTransform()->SetLocalScale(vec3(1.0f));
        UseFans();

        _fansMeetingButtonObject->GetTransform()->SetLocalPosition(vec3(0.0f, 0.0f, 0.0f));
        _fansMeetingButtonFrameImage->SetSprite(_spriteButtonStep1);
    }
    else {
        _audioComponent->SetAudio(_notEnoughResAudio);
        _audioComponent->Play();
    }
}

void Player::ConcertCall() {

    if (_concertAbility->Use())
    {
        _concertButton->SetInteractable(false);
        _concertButtonObject->GetTransform()->SetLocalScale(vec3(1.0f));
        PopularityGainingBonusBarController::Instance()->AddCurrentBonus(_concertAbility->GetAdditionalTakingOverSpeed());
        PopularityGainingBonusBarController::Instance()->RemovePossibleBonus(_concertAbility->GetAdditionalTakingOverSpeed());
        _isShowingConcertPossible = false;
        _isHoveringConcertButton = false;

        _concertButtonObject->GetTransform()->SetLocalPosition(vec3(0.0f, 0.0f, 0.0f));
        _concertButtonFrameImage->SetSprite(_spriteButtonStep1);
    }
    else {
        _audioComponent->SetAudio(_notEnoughResAudio);
        _audioComponent->Play();
    }
}

void Player::ShowAffectedTiles() {

    float usedRadius = fansRadius;
    _isShowingFansMeetingAffectedTiles = true;

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

            vec3 tileUsedGlobalPosition = tile->GetTransform()->GetGlobalPosition();
            tileUsedGlobalPosition.y = 0.0f;

            float mul = glm::distance(usedGlobalPosition, tileUsedGlobalPosition);
            mul = mul > 1.0f ? 1.0f - std::floor(mul) / usedRadius : 1.0f;

            if (mul > 0.0f) {
                tile->EnableAffected();
                _affectedTiles.push_back(tile);
            }
        }
    }

    _fansMeetingButtonObject->GetTransform()->SetLocalScale(vec3(1.1f));

    // AUDIO
    _audioComponent->SetAudio(_onHoverClickAudio);
    _audioComponent->Play();
}

void Player::HideAffectedTiles() {
    for (auto itr = _affectedTiles.begin(); itr != _affectedTiles.end(); ++itr)
    {
        (*itr)->DisableAffected();
    }
    _affectedTiles.clear();

    _isShowingFansMeetingAffectedTiles = false;

    _fansMeetingButtonObject->GetTransform()->SetLocalScale(vec3(1.0f));
    _audioComponent->SetAudio(_offHoverClickAudio);
    _audioComponent->Play();
}

void Player::StartMove(HexTile* tile) {
    if (CurrTile != nullptr) {
        CurrTile->StopTakingOver(this);
    }
    CurrTile = tile;

    GameManager::instance->changeTile = true;

    if (isFansActive) {
        if (CurrTile != tileBefore) {
            _fansMeetingCircleImage->SetColor(_abilityCooldownColor);
            _fansMeetingCircleImage->SetLayer(2);
            FansExit();
        }
    }

    if (_lost) {
        _lost = false;
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

    if (_hexIndicator)
    {
        _hexIndicator->SetActive(true);
        _hexIndicator->GetTransform()->SetGlobalPosition(CurrTile->sterowiecPos);
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


void Player::MinigameEnd() {}

void Player::ResetOnNewMap() {
    move->_info.WaitForFinding();
    move->_checkingInfo.WaitForFinding();

    move->_showedPathEnabled = false;
    move->_showedPathDisabled = false;

    move->_checkedTile = nullptr;

    if (move->_path)
    {
        delete move->_path;
        move->_path = nullptr;
    }

    move->reachEnd = true;

    GetTransform()->SetGlobalPosition(vec3(0.0f, -5.0f, 0.0f));

    // Clearing after abilities
    currAlbumTime = 0.0f;
    currAlbumCooldown = 0.0f;
    currFansTime = 0.0f;
    currFansCooldown = 0.0f;


    HideAffectedTiles();
    _fansMeetingButtonObject->GetTransform()->SetLocalPosition(vec3(0.0f, 0.0f, 0.0f));
    _fansMeetingButtonFrameImage->SetSprite(_spriteButtonStep1);
    _isHoveringFansMeetingButton = false;

    PopularityGainingBonusBarController::Instance()->RemovePossibleBonus(_concertAbility->GetAdditionalTakingOverSpeed());
    _isShowingConcertPossible = false;

    _concertButtonObject->GetTransform()->SetLocalScale(vec3(1.0f));

    _concertButtonObject->GetTransform()->SetLocalPosition(vec3(0.0f, 0.0f, 0.0f));
    _concertButtonFrameImage->SetSprite(_spriteButtonStep1);


    if (move->_showedPathTiles.size())
    {
        size_t showedPathTilesSize = move->_showedPathTiles.size();
        for (size_t index = 0ull; index < showedPathTilesSize; ++index)
        {
            move->_showedPathTiles[index]->DisableAffected();
        }
        move->_showedPathTiles.clear();
    }

    _money->money *= 0.5;
    if (CurrTile != nullptr) {
        CurrTile->StopTakingOver(this);
        CurrTile = nullptr;
        //move->_pointedTile = nullptr;
    }
    tileBefore = nullptr;

    OwnTiles.clear();
    _affectedTiles.clear();



    albumTakingOverTiles.clear();

    for (size_t index = 0ull; index < albumsIncreasingIntervalsCounter.size(); ++index)
    {
        albumsIncreasingIntervalsCounter[index] = 0.0f;
    }

    GetTransform()->SetGlobalPosition(glm::vec3(0.0, -2.0f, 0.0f));
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
    node["enoughMoneyColor"] = _enoughMoneyColor;
    node["notEnoughMoneyColor"] = _notEnoughMoneyColor;
    node["negativeMoneyTextXOffset"] = _negativeMoneyTextXOffset;
    node["negativeMoneyTextLetterWidth"] = _negativeMoneyTextLetterWidth;
    if (_starPrefab != nullptr) {
        node["starPrefab"] = _starPrefab->GetId();
    }

    node["spriteButtonStep1"] = SceneManager::GetSpriteSaveIdx(_spriteButtonStep1);
    node["spriteButtonStep2"] = SceneManager::GetSpriteSaveIdx(_spriteButtonStep2);

    node["onHoverClickAudio"] = SceneManager::GetAudioSaveIdx(_onHoverClickAudio);
    node["offHoverClickAudio"] = SceneManager::GetAudioSaveIdx(_offHoverClickAudio);
    node["abilitiesUseAudio"] = SceneManager::GetAudioSaveIdx(_abilitiesUseAudio);
    node["cooldownEndAudio"] = SceneManager::GetAudioSaveIdx(_cooldownEndAudio);
    node["notEnoughResAudio"] = SceneManager::GetAudioSaveIdx(_notEnoughResAudio);

    return node;
}

bool Player::Deserialize(const YAML::Node& node)
{
    if (!node["abilityActiveColor"] || !node["abilityCooldownColor"] || !node["starPrefab"] || !node["spriteButtonStep1"] || !node["spriteButtonStep2"] 
        || !Playable::Deserialize(node))
        return false;

    _abilityActiveColor = node["abilityActiveColor"].as<vec4>();
    _abilityCooldownColor = node["abilityCooldownColor"].as<vec4>();
    _enoughMoneyColor = node["enoughMoneyColor"].as<vec4>();
    _notEnoughMoneyColor = node["notEnoughMoneyColor"].as<vec4>();
    _negativeMoneyTextXOffset = node["negativeMoneyTextXOffset"].as<float>();
    _negativeMoneyTextLetterWidth = node["negativeMoneyTextLetterWidth"].as<float>();

    if (node["starPrefab"]) {
        _starPrefab = PrefabManager::GetPrefab(SceneManager::GetPrefab(node["starPrefab"].as<size_t>()));
    }
    _spriteButtonStep1 = SceneManager::GetSprite(node["spriteButtonStep1"].as<size_t>());
    _spriteButtonStep2 = SceneManager::GetSprite(node["spriteButtonStep2"].as<size_t>());

    _onHoverClickAudio = SceneManager::GetAudio(node["onHoverClickAudio"].as<size_t>());
    _offHoverClickAudio = SceneManager::GetAudio(node["offHoverClickAudio"].as<size_t>());
    _abilitiesUseAudio = SceneManager::GetAudio(node["abilitiesUseAudio"].as<size_t>());
    _cooldownEndAudio = SceneManager::GetAudio(node["cooldownEndAudio"].as<size_t>());
    _notEnoughResAudio = SceneManager::GetAudio(node["notEnoughResAudio"].as<size_t>());

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
        ImGui::ColorEdit4("EnoughMoneyColor", (float*)(&_enoughMoneyColor));
        ImGui::ColorEdit4("NotEnoughMoneyColor", (float*)(&_notEnoughMoneyColor));
        ImGui::InputFloat("NegativeMoneyTextXOffset", &_negativeMoneyTextXOffset);
        ImGui::InputFloat("NegativeMoneyTextLetterWidth", &_negativeMoneyTextLetterWidth);

        // TODO: Zrobic
        if (ImGui::Button("Show Affected"))
            ShowAffectedTiles();
        if (ImGui::Button("Hide Affected"))
            HideAffectedTiles();
    }
}
#endif