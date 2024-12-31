#pragma once

namespace RE
{
	template <class T>
	void AttachNode(NiNode* a_root, T* a_obj)
	{
		if (TaskQueueInterface::ShouldUseTaskQueue())
		{
			TaskQueueInterface::GetSingleton()->QueueNodeAttach(a_obj, a_root);
		}
		else
		{
			a_root->AttachChild(a_obj, true);
		}
	}

	template <class T>
	void UpdateNode(T* a_obj)
	{
		if (TaskQueueInterface::ShouldUseTaskQueue())
		{
			TaskQueueInterface::GetSingleton()->QueueUpdateNiObject(a_obj);
		}
		else
		{
			NiUpdateData updateData;
			a_obj->Update(updateData);
		}
	}
}
