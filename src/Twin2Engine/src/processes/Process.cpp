#include <processes/Process.h>
//#include <processes/SynchronizedProcess.h>

unsigned short Twin2Engine::Processes::Process::lastGPID = 0;
/*/
void Twin2Engine::Processes::Process::SetHierachyActive(bool& v)
{
	bool isActive = HierachyActive && Active;
	HierachyActive = v;

	if (Active) {
		for (auto p : childrenProcesses) {
			p->SetHierachyActive(v);
		}
	}

	if (!isActive && (HierachyActive && Active)) {
		OnActivation();
	}
	else if (isActive && !(HierachyActive && Active)) {
		OnDeactivation();
	}
}/**/

void Twin2Engine::Processes::Process::SetActive(bool v)
{
	//bool isActive = HierachyActive && Active;
	Active = v;
	if (Active) {
		OnActivation();
	}
	else {
		OnDeactivation();
	}

	//if (HierachyActive) {
	//	for (auto p : childrenProcesses) {
	//		p->SetHierachyActive(v);
	//	}
	//}
	//
	//if (!isActive && (HierachyActive && Active)) {
	//	OnActivation();
	//}
	//else if (isActive && !(HierachyActive && Active)) {
	//	OnDeactivation();
	//}
}

bool Twin2Engine::Processes::Process::IsActive()
{
	//return HierachyActive && Active;
	return Active;
}

void Twin2Engine::Processes::Process::SetName(std::string name)
{
	this->name = name;
	nameHash = std::hash<std::string>{}(name);
}

/*/
void Twin2Engine::Processes::Process::AddChild(SynchronizedProcess* process)
{
	childrenProcesses.insert(process);

	process->parent = this;
}

void Twin2Engine::Processes::Process::RemoveChild(SynchronizedProcess* process)
{
	childrenProcesses.erase(process);
}

Twin2Engine::Processes::SynchronizedProcess* Twin2Engine::Processes::Process::RemoveChild(unsigned short GPID)
{
	SynchronizedProcess* process = nullptr;

	for (auto p : childrenProcesses) {
		if (p->GPID == GPID) {
			process = p;
			break;
		}
	}

	if (process != nullptr) {
		process->parent = nullptr;
		childrenProcesses.erase(process);
	}

	return process;
}

void Twin2Engine::Processes::Process::ClearChildrenList() {
	childrenProcesses.clear();
}

void Twin2Engine::Processes::Process::UpdateChildrenProcesses()
{
	for (auto p : childrenProcesses) {
		p->Update();
	}
}/**/

void Twin2Engine::Processes::Process::Initialize() {

}

void Twin2Engine::Processes::Process::Update() {

}

void Twin2Engine::Processes::Process::OnActivation() {

}

void Twin2Engine::Processes::Process::OnDeactivation() {

}

YAML::Node Twin2Engine::Processes::Process::Serialize() const
{
	YAML::Node node;
	node["Active"] = Active;
	node["DontDestroyOnLoad"] = Active;
	node["name"] = name;

	//return node;
	return std::move(node);
}
