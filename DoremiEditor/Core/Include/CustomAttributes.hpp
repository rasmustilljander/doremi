#pragma once
#include <DoremiEditor/Core/Include/SharedVariables.hpp>
#include <DirectXMath.h>
namespace DoremiEditor
{
    namespace Core
    {
        struct CharacterCustomAttributes // TODO should instead contain info on animations?
        {
            bool isRendered;
            bool isBBox;
            bool isCollider;
            bool isInteractable;
            float interactIntervalX;
            float interactIntervalY;
            float interactIntervalZ;
            int typeSpawner;
            int typeCheckPoint;
            int typeStartEnd;
            bool isAIground;
        };
        struct CustomAttributes
        {
            //bool isRendered;
            //bool isBBox;
            //bool isCollider;
            //bool isInteractable;
            //int checkPointID;
            //int startOrEndPoint;
            //bool isAIGround;
            //bool frequencyAffected;
            //DirectX::XMFLOAT3 startPos;
            //DirectX::XMFLOAT3 endPos;
            //float frequencyOffsetX;
            //float frequencyOffsetY;
            //bool isPotentialFieldCollidable;
            //int potentialFieldID;
            //int potentialFieldNeighbours[5];
            //bool isSpawner;
            //int maxSpawn;
            //int maxAlive;
            //int spawnTypeBlueprint;
            //float spawnFrequency;
            //bool isStatic;
            //int physicsType;
            //int typeBlueprint;
            //bool isDangerous;


            bool isRendered;
            bool isBBox;
            bool isCollider;
            bool isInteractable;
            int checkPointID;
            int spawnPointID; // New?
            int startOrEndPoint;
            bool isAIground;
            bool frequencyAffected;
            float interactableRange;
            DirectX::XMFLOAT3 interactableStartPos;
            DirectX::XMFLOAT3 interactableEndPos;
            DirectX::XMFLOAT2 interactableOffset;
            float interactableStartSpeed;
            float interactableEndSpeed;
            bool isPotentialFieldCollidable;
            int potentialFieldID;
            int potentialFieldNeighbour[5];
            bool isSpawner;
            int spawnMax;
            int spawnMaxAlive;
            int spawnTypeBlueprint;
            float spawnFrequency;
            bool isStatic;
            int physicsType;
            int typeBlueprint;
            bool isDangerous;
        };
    }
}