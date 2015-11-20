#pragma once
namespace Doremi
{
	namespace Core 
	{
		struct PhysicsComponent
		{
			float posX;
			float posY;
			PhysicsComponent(float p_posX, float p_posY) : posX(p_posX), posY(p_posY)
			{}
			PhysicsComponent() : posX(0), posY(0)
			{}
		};
	}
}