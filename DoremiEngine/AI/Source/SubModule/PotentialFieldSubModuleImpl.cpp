#pragma once
#include <Internal/SubModule/PotentialFieldSubModuleImpl.hpp>
#include <Internal/PotentialField/PotentialFieldImpl.hpp>
#include <Internal/PotentialField/PotentialGroupImpl.hpp>
#include <Internal/PotentialField/PotentialFieldActorImpl.hpp>

#include <iostream>
#include <fstream>
#include <algorithm>
namespace DoremiEngine
{
    namespace AI
    {
        PotentialFieldSubModuleImpl::PotentialFieldSubModuleImpl(AIContext& p_aiContext) : m_context(p_aiContext) {}
        PotentialFieldSubModuleImpl::~PotentialFieldSubModuleImpl() {}
        PotentialField* PotentialFieldSubModuleImpl::CreateNewField(const float& p_width, const float& p_height, const int& p_numberOfQuadsWidth,
                                                                    const int& p_numberOfQuadsHeight, const DirectX::XMFLOAT3& p_center,
                                                                    const std::string& p_fieldName)
        {
            // TODOKO review if we need to create a 3d grid or if a 2d grid is enough just to see where they can move in x and z
            using namespace std;
            using namespace DirectX;

            // Strip the fieldname of illeagal characters
            std::string fixedFieldName = p_fieldName;
            char charsToRemove[] = "/\|?<>:*\""; // These chars are illegal in files in windows

            for(unsigned int i = 0; i < strlen(charsToRemove); ++i)
            {
                // you need include <algorithm> to use general algorithms like std::remove()
                fixedFieldName.erase(std::remove(fixedFieldName.begin(), fixedFieldName.end(), charsToRemove[i]), fixedFieldName.end());
            }

            PotentialField* newField = new PotentialFieldImpl(m_context);
            PotentialFieldGridPoint* grid;
            float quadWidth = p_width / static_cast<float>(p_numberOfQuadsWidth);
            float quadHeight = p_height / static_cast<float>(p_numberOfQuadsHeight);
            float halfWidth = p_width / 2.0f;
            float halfHeight = p_height / 2.0f;
            XMFLOAT2 bottomLeft = XMFLOAT2(p_center.x - halfWidth, p_center.z - halfHeight);
            grid = (PotentialFieldGridPoint*)malloc(p_numberOfQuadsHeight * p_numberOfQuadsWidth * sizeof(PotentialFieldGridPoint));
            // TODOXX Wont work if standard values are not 0
            memset(grid, 0, p_numberOfQuadsHeight * p_numberOfQuadsWidth * sizeof(PotentialFieldGridPoint));
            newField->SetGrid(grid);
            newField->SetCenter(p_center);
            newField->SetHeight(p_height);
            newField->SetWidth(p_width);
            newField->SetQuadSize(XMFLOAT2(quadWidth, quadHeight));
            newField->SetNumberOfQuads(p_numberOfQuadsWidth, p_numberOfQuadsHeight);
            newField->SetName(fixedFieldName);
            newField->SetNeedUpdating(true);
            m_fields.push_back(newField);
            return newField;
        }

        PotentialField* PotentialFieldSubModuleImpl::CreateNewFieldFromFile(const std::string& p_fileName)
        {
            using namespace std;
            using namespace DirectX;

            // Strip the filename of illeagal characters
            std::string fixedFileName = p_fileName;
            char charsToRemove[] = "/\|?<>:*\""; // These chars are illegal in files in windows

            for(unsigned int i = 0; i < strlen(charsToRemove); ++i)
            {
                // you need include <algorithm> to use general algorithms like std::remove()
                fixedFileName.erase(std::remove(fixedFileName.begin(), fixedFileName.end(), charsToRemove[i]), fixedFileName.end());
            }

            // Change the file name and then open the file
            string fullFileName = m_context.WorkingDirectory + "PotentialFields/" + fixedFileName + ".drmpf";
            ifstream file(fullFileName, ofstream::in | ofstream::binary);
            if(!file.is_open())
            {
                // TODOKO log error, couldnt open file
                return nullptr;
            }
            // create variable to contatin what the field gives us
            int quadsX;
            int quadsZ;

            XMFLOAT3 center;

            float width;
            float height;

            // read some one of a kind values
            file.read((char*)&center, sizeof(XMFLOAT3));

            file.read((char*)&width, sizeof(float));
            file.read((char*)&height, sizeof(float));

            // Save down nubmer of quads in x and z so we know how many to read later on. Also used to calc quad size
            file.read((char*)&quadsX, sizeof(int));
            file.read((char*)&quadsZ, sizeof(int));

            // Now that we know how many quads there is allocate some memory
            PotentialFieldGridPoint* grid = (PotentialFieldGridPoint*)malloc(sizeof(PotentialFieldGridPoint) * quadsX * quadsZ);

            // Now we save down every quad in the grid
            file.read((char*)grid, sizeof(PotentialFieldGridPoint) * quadsX * quadsZ);
            // Everything we need is saved, close file
            file.close();

            // Calculate the size of a quad
            XMFLOAT2 quadSize;

            quadSize.x = width / static_cast<float>(quadsX);
            quadSize.y = height / static_cast<float>(quadsZ);
            // Create a new field
            PotentialField* newField = new PotentialFieldImpl(m_context);

            // Save the values to the field
            newField->SetGrid(grid);
            newField->SetCenter(center);
            newField->SetHeight(height);
            newField->SetWidth(width);
            newField->SetQuadSize(quadSize);
            newField->SetNumberOfQuads(quadsX, quadsZ);
            newField->SetName(fixedFileName);
            // This field should already be updated to our best knowledge
            newField->SetNeedUpdating(false);
            m_fields.push_back(newField);

            return newField;
        }

        bool PotentialFieldSubModuleImpl::SaveFieldToFile(const PotentialField& p_fieldToSave, const std::string& p_fileName)
        {
            using namespace std;
            using namespace DirectX;

            // Get the needed values from the field
            int quadsX = p_fieldToSave.GetNumberOfQuadsWidth();
            int quadsZ = p_fieldToSave.GetNumberOfQuadsHeight();

            const PotentialFieldGridPoint* grid = p_fieldToSave.GetGrid();
            XMFLOAT3 center = p_fieldToSave.GetCenter();

            XMFLOAT2 quadSize = p_fieldToSave.GetQuadSize();
            float width = quadSize.x * static_cast<float>(quadsX);
            float height = quadSize.y * static_cast<float>(quadsZ);

            // Strip the filename of illeagal characters
            std::string fixedFileName = p_fileName;
            char charsToRemove[] = "/\|?<>:*\""; // These chars are illegal in files in windows

            for(unsigned int i = 0; i < strlen(charsToRemove); ++i)
            {
                // you need include <algorithm> to use general algorithms like std::remove()
                fixedFileName.erase(std::remove(fixedFileName.begin(), fixedFileName.end(), charsToRemove[i]), fixedFileName.end());
            }
            // Change the file name and then open the file, if no one exists a new file should be created
            string fullFileName = m_context.WorkingDirectory + "PotentialFields/" + fixedFileName + ".drmpf";
            ofstream file(fullFileName, ofstream::out | ofstream::binary);
            if(!file.is_open())
            {
                // TODOKO log error, couldnt create file?
                return false;
            }
            // Save down some one of a kind values
            file.write((char*)&center, sizeof(XMFLOAT3));

            file.write((char*)&width, sizeof(float));
            file.write((char*)&height, sizeof(float));

            // Save down nubmer of quads in x and z so we know how many to read later on. Also used to calc quad size
            file.write((char*)&quadsX, sizeof(int));
            file.write((char*)&quadsZ, sizeof(int));

            // Now we save down every quad in the grid
            file.write((char*)grid, sizeof(PotentialFieldGridPoint) * quadsX * quadsZ);

            // Everything we need is saved, close file
            file.close();
            return true;
        }

        PotentialGroup* PotentialFieldSubModuleImpl::CreateNewPotentialGroup()
        {
            PotentialGroup* newGroup = new PotentialGroupImpl();
            return newGroup;
        }
        PotentialFieldActor* PotentialFieldSubModuleImpl::CreateNewActor(const DirectX::XMFLOAT3& p_position, const float& p_charge,
                                                                         const float& p_range, const bool& p_static, const AIActorType& p_actorType) // TODOKO position should possibly be float2. Pointer so we dont need to send pos every frame?
        {
            PotentialFieldActor* newActor = new PotentialFieldActorImpl();
            newActor->SetStatic(p_static); // needs to be done first since set position is dependent on static or not
            newActor->SetPosition(p_position); // TODOKO Might produce a error since no static objects will have a good position
            newActor->SetCharge(p_charge);
            newActor->SetRange(p_range);
            newActor->SetActorType(p_actorType);

            return newActor;
        }
        void PotentialFieldSubModuleImpl::AttachActor(PotentialField& o_field, PotentialFieldActor* p_actor) { o_field.AddActor(p_actor); }

        void PotentialFieldSubModuleImpl::EraseActor(PotentialFieldActor* op_actor)
        {
            size_t length = m_fields.size();
            for (size_t i = 0; i < length; i++)
            {
                m_fields[i]->RemoveActor(op_actor);
            }
            std::cout << "Will delete Actor now...";
            delete op_actor;
        }
        void PotentialFieldSubModuleImpl::EraseActor(PotentialFieldActor* op_actor, PotentialField* op_field)
        {
            op_field->RemoveActor(op_actor);
            delete op_actor;
        }

        PotentialField* PotentialFieldSubModuleImpl::FindBestPotentialField(const DirectX::XMFLOAT3& p_position)
        {
            // Take out all PF that you are within
            using namespace std;
            using namespace DirectX;

            vector<PotentialField*> possibleMatches;
            PotentialField* returnField = nullptr;
            for(size_t i = 0; i < m_fields.size(); i++)
            {
                XMFLOAT3 fieldCenter = m_fields[i]->GetCenter();
                XMFLOAT2 quadSize = m_fields[i]->GetQuadSize();

                int quadsX = m_fields[i]->GetNumberOfQuadsWidth(); // Get number of quads in x and y
                int quadsY = m_fields[i]->GetNumberOfQuadsHeight();

                float fieldHalfWidth = quadSize.x * static_cast<float>(quadsX) * 0.5f; // Half the fields width and height
                float fieldHalfHeight = quadSize.y * static_cast<float>(quadsY) * 0.5f;

                float fieldLeftBoundary = fieldCenter.x - fieldHalfWidth; // takes out the fields left, right and top, bottom boundary positions
                float fieldRightBoundary = fieldCenter.x + fieldHalfWidth;
                float fieldTopBoundary = fieldCenter.z + fieldHalfHeight;
                float fieldBottomBoundary = fieldCenter.z - fieldHalfHeight;

                if(p_position.x <= fieldRightBoundary && p_position.x >= fieldLeftBoundary && p_position.z <= fieldTopBoundary &&
                   p_position.z >= fieldBottomBoundary) // If inside the field
                {
                    possibleMatches.push_back(m_fields[i]); // Still need to check which field is closest in Y
                }
            }
            // Check which possible field is closest in Y
            float closestY = numeric_limits<float>::max();
            for(size_t i = 0; i < possibleMatches.size(); i++)
            {
                XMFLOAT3 fieldCenter = possibleMatches[i]->GetCenter();

                float distanceY = p_position.y - fieldCenter.y;
                distanceY = abs(distanceY);

                if(distanceY < closestY)
                {
                    returnField = possibleMatches[i];
                    closestY = distanceY;
                }
            }
            if(returnField == nullptr)
            {
                // TODOKO log error / message
            }
            return returnField;
        }
        void PotentialFieldSubModuleImpl::AddActorToEveryPotentialField(PotentialFieldActor* p_actor)
        {
            size_t length = m_fields.size();
            for(size_t i = 0; i < length; i++)
            {
                m_fields[i]->AddActor(p_actor);
            }
        }
    }
}