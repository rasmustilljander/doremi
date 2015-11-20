#pragma once
#include <EntityComponent/Constants.hpp>
#include <string>

using namespace std;
// bit lines for each entity
// 1 if have component, 0 if not
class ComponentTable
{
    public:
    static ComponentTable* GetInstance();

    bool HasComponent(int pEntityID, int pMask);
    void AddComponent(int pEntityID, int pMask);
    void RemoveComponent(int pEntityID, int pMask);
    void RemoveEntity(int pEntityID);


    private:
    ComponentTable();
    ~ComponentTable();
    static ComponentTable* mSingleton;

    int mComponentTable[MAX_NUM_ENTITIES];
};
