#pragma once
//#include <GameEngine.h>
#include <core/Input.h>
#include <core/Transform.h>
#include <core/GameObject.h>
#include <core/CameraComponent.h>
#include <core/Time.h>
#include <tools/YamlConverters.h>

using namespace Twin2Engine::Core;

namespace GameScripts {
	class MovementController : public Component {
		public:
			/*
			* DRAG
			*/
			bool DragControl = true;
			bool Draging = false;
			float DragScale = 0.01f;
			glm::vec2 DragStartPos = glm::vec2(0.0f, 0.0f);
			glm::vec3 CameraStartPos = glm::vec3(0.0f);

			/*
			* PUSHING BORDER
			*/
			KEY UpKey = KEY::W;
			KEY DownKey = KEY::S;
			KEY LeftKey = KEY::A;
			KEY RightKey = KEY::D;

			bool WsadControl = true;
			float WsadVelocity = 0.5f;
			glm::vec2 WsadDir = glm::vec2(0.0f, 0.0f);

			/*
			* WSAD
			*/
			bool BorderControl = true;
			float BorderVelocity = 0.5f;
			glm::vec2 BorderDir = glm::vec2(0.0f, 0.0f);

			MovementController() : Component() {
                SPDLOG_INFO("MovementController");
            }

			virtual void Update() override {
				if (DragControl)
				{
					CheckDragInput();
					if (Draging)
					{
						DragMove();
					}
				}

				if (WsadControl)
				{
                    CheckWsadInput();
                    if (WsadDir.x != 0.0f || WsadDir.y != 0.0f) {
                        WsadMove();
                    }
				}

				if (BorderControl)
				{
					CheckBorderInput();
                    if (BorderDir.x != 0.0f || BorderDir.y != 0.0f) {
                        BorderMove();
                    }
				}
			}

			virtual YAML::Node Serialize() const override {
				YAML::Node node = Component::Serialize();
				node["type"] = "MovementController";
				node["DragControl"] = DragControl;
				node["DragScale"] = DragScale;
				node["WsadControl"] = WsadControl;
				node["WsadVelocity"] = WsadVelocity;
				node["BorderControl"] = BorderControl;
				node["BorderVelocity"] = BorderVelocity;

				return node;
			}

			virtual bool Deserialize(const YAML::Node& node) override {
				if (!Component::Deserialize(node))
					return false;

				DragControl = node["DragControl"].as<bool>();
				DragScale = node["DragScale"].as<float>();
				WsadControl = node["WsadControl"].as<bool>();
				WsadVelocity = node["WsadVelocity"].as<float>();
				BorderControl = node["BorderControl"].as<bool>();
				BorderVelocity = node["BorderVelocity"].as<float>();

				return true;
			}

#if _DEBUG
			virtual void DrawEditor() override {
				std::string id = std::string(std::to_string(this->GetId()));
				std::string name = std::string("MovementController##").append(id);

				if (ImGui::CollapsingHeader(name.c_str())) {
					bool b = IsEnable();
					ImGui::Checkbox(string("Enable##").append(id).c_str(), &b);
                    SetEnable(b);

                    b = DragControl;
                    ImGui::Checkbox(string("DragControl##").append(id).c_str(), &b);
                    DragControl = b;

					float v = DragScale;
					ImGui::DragFloat(string("DragScale##").append(id).c_str(), &v);
					DragScale = v;

					b = WsadControl;
					ImGui::Checkbox(string("WsadControl##").append(id).c_str(), &b);
					WsadControl = b;

					v = WsadVelocity;
					ImGui::DragFloat(string("WsadVelocity##").append(id).c_str(), &v);
					WsadVelocity = v;

					b = BorderControl;
					ImGui::Checkbox(string("BorderControl##").append(id).c_str(), &b);
					BorderControl = b;

					v = BorderVelocity;
					ImGui::DragFloat(string("BorderVelocity##").append(id).c_str(), &v);
					BorderVelocity = v;
				}
			}
#endif


            void CheckDragInput()
            {
                if (Input::IsMouseButtonDown(Input::GetMainWindow(), MOUSE_BUTTON::MIDDLE))
                {
                    if (!Draging)
                    {
                        DragStartPos = Input::GetCursorPos();
                        CameraStartPos = CameraComponent::GetMainCamera()->GetTransform()->GetGlobalPosition();
                    }
                    Draging = true;
                }
                else
                {
                    Draging = false;
                }
            }

            void DragMove()
            {
                glm::vec2 curpos = Input::GetCursorPos();
                //glm::vec3 vec(curpos.x - DragStartPos.x, 0.0f, curpos.y - DragStartPos.y);
                glm::vec3 vec(curpos.x - DragStartPos.x, 0.0f, DragStartPos.y - curpos.y);
                vec *= -DragScale;
                CameraComponent::GetMainCamera()->GetTransform()->SetGlobalPosition(CameraStartPos + vec);
            }

            void CheckWsadInput()
            {
                if (Input::IsKeyPressed(LeftKey))
                {
                    WsadDir.x -= 1.0f;
                }
                if (Input::IsKeyPressed(RightKey))
                {
                    WsadDir.x += 1.0f;
                }
                if (Input::IsKeyPressed(UpKey))
                {
                    WsadDir.y -= 1.0f;
                }
                if (Input::IsKeyPressed(DownKey))
                {
                    WsadDir.y += 1.0f;
                }

                if (Input::IsKeyReleased(LeftKey))
                {
                    WsadDir.x += 1.0f;
                }
                if (Input::IsKeyReleased(RightKey))
                {
                    WsadDir.x -= 1.0f;
                }
                if (Input::IsKeyReleased(UpKey))
                {
                    WsadDir.y += 1.0f;
                }
                if (Input::IsKeyReleased(DownKey))
                {
                    WsadDir.y -= 1.0f;
                }

                if (Input::IsKeyUp(RightKey) && Input::IsKeyUp(LeftKey)) {
                    WsadDir.x = 0.0f;
                }

                if (Input::IsKeyUp(UpKey) && Input::IsKeyUp(DownKey)) {
                    WsadDir.y = 0.0f;
                }
            }

            void WsadMove()
            {
                glm::vec2 dir = glm::normalize(WsadDir) * WsadVelocity * Time::GetDeltaTime();
                glm::vec3 cp = CameraComponent::GetMainCamera()->GetTransform()->GetGlobalPosition();
                cp += glm::vec3(dir.x, 0.0f, dir.y);
                CameraComponent::GetMainCamera()->GetTransform()->SetGlobalPosition(CameraComponent::GetMainCamera()->GetTransform()->GetGlobalPosition() + glm::vec3(dir.x, 0.0f, dir.y));
            }


            void CheckBorderInput()
            {
                vec2 mousePos = Input::GetCursorPos();
                vec2 halfSize = ((vec2)Twin2Engine::Graphic::Window::GetInstance()->GetContentSize()) * 0.5f;

                if ((mousePos.x < -halfSize.x)
                    || (mousePos.x > halfSize.x)
                    || (mousePos.y < -halfSize.y)
                    || (mousePos.y > halfSize.y))
                {
                    BorderDir.x = 0.0f;
                    BorderDir.y = 0.0f;
                }
                else
                {
                    if (mousePos.x < (-halfSize.x + 0.25f * halfSize.x))
                    {
                        BorderDir.x = -1.0f;
                    }
                    else if (mousePos.x > (halfSize.x - 0.25f * halfSize.x))
                    {
                        BorderDir.x = 1.0f;
                    }
                    else
                    {
                        BorderDir.x = 0.0f;
                    }

                    if (mousePos.y < (-halfSize.y + 0.25f * halfSize.y))
                    {
                        BorderDir.y = 1.0f;
                    }
                    else if (mousePos.y > (halfSize.y - 0.25f * halfSize.y))
                    {
                        BorderDir.y = -1.0f;
                    }
                    else
                    {
                        BorderDir.y = 0.0f;
                    }
                }
            }

            void BorderMove()
            {
                glm::vec2 dir = glm::normalize(BorderDir) * BorderVelocity * Time::GetDeltaTime();
                CameraComponent::GetMainCamera()->GetTransform()->SetGlobalPosition(CameraComponent::GetMainCamera()->GetTransform()->GetGlobalPosition() + glm::vec3(dir.x, 0.0f, dir.y));
            }
	};
}