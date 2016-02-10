#include <Internal/RigidBodyManagerImpl.hpp>
#include <Internal/PhysicsModuleImplementation.hpp>
#include <cstdint>

#include <iostream>
using namespace std;
namespace DoremiEngine
{
    namespace Physics
    {
        RigidBodyManagerImpl::RigidBodyManagerImpl(InternalPhysicsUtils& p_utils) : m_utils(p_utils), m_meshCooker(new MeshCooker(p_utils)) {}
        RigidBodyManagerImpl::~RigidBodyManagerImpl() {}

        int RigidBodyManagerImpl::AddBoxBodyDynamic(int p_id, XMFLOAT3 p_position, XMFLOAT4 p_orientation, XMFLOAT3 p_dims, int p_materialID)
        {


            // really silly debug code TODOJB remove
            // static bool derp = true;
            // if (derp)
            //{
            //    derp = false;
            //    int matID = m_utils.m_physicsMaterialManager->CreateMaterial(0, 0, 0);
            //    m_utils.m_rigidBodyManager->AddBoxBodyDynamic(99999, XMFLOAT3(0, 10, 0), XMFLOAT4(0, 0, 0, 1), XMFLOAT3(5, 5, 5), matID);
            //    m_utils.m_rigidBodyManager->SetDrain(99999, true);
            //}

            // Create a world matrix (only translation) i think
            PxVec3 position = PxVec3(p_position.x, p_position.y, p_position.z);
            PxQuat orientation = PxQuat(p_orientation.x, p_orientation.y, p_orientation.z, p_orientation.w);
            PxVec3 dims = PxVec3(p_dims.x, p_dims.y, p_dims.z);
            // Creates the actual body.
            PxTransform transform = PxTransform(position, orientation);
            // This body is dynamic
            PxRigidDynamic* body = m_utils.m_physics->createRigidDynamic(transform);
            // Hard coded flag so that we trigger the onSleep and onAwake callbacks for all dynamic bodies
            body->setActorFlag(PxActorFlag::eSEND_SLEEP_NOTIFIES, true);
            // Create a shape for the body
            body->createShape(PxBoxGeometry(dims), *m_utils.m_physicsMaterialManager->GetMaterial(p_materialID));
            // Give the body some mass (since it is dynamic. Static objects probably don't need mass)
            PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
            // Add the now fully created body to the scene
            m_utils.m_worldScene->addActor(*body);

            // Finally add the body to our list
            m_bodies[p_id] = body;
            m_IDsByBodies[body] = p_id;

            /*
            And now we have added a box to the world at the given position
            I'm not too sure how we update the box, or the scene, or perform
            any form of interaction however*/

            //// DEBUG CODE TODOJB REMOVE
            // if (p_id == 99999)
            //{
            //    PxShape* shape;
            //    body->getShapes(&shape, 1);
            //    PxFilterData data = PxFilterData(0, 0, 0, 1);
            //    shape->setSimulationFilterData(data);
            //    shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false);
            //    //body->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
            //    //SetTrigger(99999, true);
            //}
            // else
            //{
            // Hax to get callbacks to work (Set a common flag on every object)
            SetCallback(p_id, (1 << 0), (1 << 0));
            //}
            // Kinda redundant return...
            return p_id;
        }

        int RigidBodyManagerImpl::AddBoxBodyStatic(int p_id, XMFLOAT3 p_position, XMFLOAT4 p_orientation, XMFLOAT3 p_dims, int p_materialID)
        {
            // Create a world matrix (only translation) i think
            PxVec3 position = PxVec3(p_position.x, p_position.y, p_position.z);
            PxQuat orientation = PxQuat(p_orientation.x, p_orientation.y, p_orientation.z, p_orientation.w);
            PxVec3 dims = PxVec3(p_dims.x, p_dims.y, p_dims.z);
            // Creates the physics object shape thingy, which collides with stuff. Shapes are just objects. I
            PxMaterial* material = m_utils.m_physicsMaterialManager->GetMaterial(p_materialID);
            // PxShape* shape = m_utils.m_physics->createShape(PxBoxGeometry(dims), *m_utils.m_physicsMaterialManager->GetMaterial(p_materialID));
            PxTransform transform = PxTransform(position, orientation);
            // This body is static
            PxRigidStatic* body = m_utils.m_physics->createRigidStatic(transform);
            // Create a shape for this actor
            body->createShape(PxBoxGeometry(dims), *m_utils.m_physicsMaterialManager->GetMaterial(p_materialID));

            // Add the now fully created body to the scene
            m_utils.m_worldScene->addActor(*body);
            // We're done with the shape. Release itS

            // Finally add the body to our list
            m_bodies[p_id] = body;
            m_IDsByBodies[body] = p_id;

            // Hax to get callbacks to work (Set a common flag on every object)
            SetCallback(p_id, (1 << 0), (1 << 0));

            /*
            And now we have added a box to the world at the given position
            I'm not too sure how we update the box, or the scene, or perform
            any form of interaction however*/

            // Kinda redundant return...
            return p_id;
        }

        void RigidBodyManagerImpl::SetKinematicActor(int p_bodyID, bool p_kinematic)
        {
            if(m_bodies.find(p_bodyID) == m_bodies.end())
            {
                cout << "shit went wrong" << endl;
            }
            // TODOXX det kan krångla om man antar att det är en PxRigidDynamic* om det är en static. Borde kollas om det är en dynamic.
            ((PxRigidDynamic*)m_bodies[p_bodyID])->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, p_kinematic);
        }

        void RigidBodyManagerImpl::MoveKinematicActor(int p_bodyID, XMFLOAT3 p_moveVector)
        {
            if(m_bodies.find(p_bodyID) == m_bodies.end())
            {
                cout << "shit went wrong" << endl;
            }
            if((uint32_t)((PxRigidDynamic*)m_bodies[p_bodyID])->getRigidBodyFlags() & PxRigidBodyFlag::eKINEMATIC == PxRigidBodyFlag::eKINEMATIC)
            {
                PxVec3 currentPos = ((PxRigidDynamic*)m_bodies[p_bodyID])->getGlobalPose().p;
                PxVec3 targetPos = PxVec3(p_moveVector.x, p_moveVector.y, p_moveVector.z);
                targetPos += currentPos;
                ((PxRigidDynamic*)m_bodies[p_bodyID])->setKinematicTarget(PxTransform(targetPos));
            }
            else
            {
                // do nothing
            }
        }

        void RigidBodyManagerImpl::SetWorldPositionKinematic(int p_bodyID, XMFLOAT3 p_position)
        {
            if(m_bodies.find(p_bodyID) == m_bodies.end())
            {
                cout << "shit went wrong" << endl;
            }
            if((uint32_t)((PxRigidDynamic*)m_bodies[p_bodyID])->getRigidBodyFlags() & PxRigidBodyFlag::eKINEMATIC == PxRigidBodyFlag::eKINEMATIC)
            {

                ((PxRigidDynamic*)m_bodies[p_bodyID])->setKinematicTarget(PxTransform(p_position.x, p_position.y, p_position.z));
            }
            else
            {
                // do nothing
            }
        }

        void RigidBodyManagerImpl::AddMeshBodyStatic(int p_id, XMFLOAT3 p_position, XMFLOAT4 p_orientation, vector<XMFLOAT3>& p_vertexPositions,
                                                     vector<int>& p_indices, int p_materialID)
        {
            // Get a mesh
            PxTriangleMesh* mesh = m_meshCooker->CookMesh(p_vertexPositions, p_indices);
            // Get it into a geometry
            PxTriangleMeshGeometry meshGeometry;
            meshGeometry.triangleMesh = mesh;
            bool valid = meshGeometry.isValid();

            // Create the transform
            PxVec3 position = PxVec3(p_position.x, p_position.y, p_position.z);
            PxQuat orientation = PxQuat(p_orientation.x, p_orientation.y, p_orientation.z, p_orientation.w);
            PxTransform transform = PxTransform(position, orientation);
            // Create the body
            PxRigidStatic* body = m_utils.m_physics->createRigidStatic(transform);

            // Get the material
            PxMaterial* material = m_utils.m_physicsMaterialManager->GetMaterial(p_materialID);
            // Create a shape
            body->createShape(meshGeometry, *material);
            // Add to scene
            m_utils.m_worldScene->addActor(*body);

            // Finally add the body to our list
            m_bodies[p_id] = body;
            m_IDsByBodies[body] = p_id;

            // Hax to get callbacks to work (Set a common flag on every object)
            // SetCallback(p_id, (1 << 0), (1 << 0));
        }
        void RigidBodyManagerImpl::AddMeshBodyDynamic(int p_id, XMFLOAT3 p_position, XMFLOAT4 p_orientation, vector<XMFLOAT3>& p_vertexPositions,
                                                      vector<int>& p_indices, int p_materialID)
        {
            // implementation pending
        }

        int RigidBodyManagerImpl::AddSphereBodyDynamic(int p_id, XMFLOAT3 p_position, float p_radius)
        {
            PxVec3 position = PxVec3(p_position.x, p_position.y, p_position.z);
            // Creates the actual body.
            PxTransform transform = PxTransform(position);
            // This body is dynamic
            PxRigidDynamic* body = m_utils.m_physics->createRigidDynamic(transform);
            // Hard coded flag so that we trigger the onSleep and onAwake callbacks for all dynamic bodies
            body->setActorFlag(PxActorFlag::eSEND_SLEEP_NOTIFIES, true);
            // Create a shape for the body
            body->createShape(PxSphereGeometry(p_radius), *m_utils.m_physics->createMaterial(0, 0, 0));
            // Give the body some mass (since it is dynamic. Static objects probably don't need mass)
            PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
            // Add the now fully created body to the scene
            m_utils.m_worldScene->addActor(*body);

            // Finally add the body to our list
            m_bodies[p_id] = body;
            m_IDsByBodies[body] = p_id;

            SetCallback(p_id, (1 << 0), (1 << 0));
            return p_id;
        }

        int RigidBodyManagerImpl::AddCapsuleBodyDynamic(int p_id, XMFLOAT3 p_position, XMFLOAT4 p_orientation, float p_height, float p_radius)
        {
            PxVec3 position = PxVec3(p_position.x, p_position.y, p_position.z);
            PxQuat orientation = PxQuat(p_orientation.x, p_orientation.y, p_orientation.z, p_orientation.w);
            // Creates the actual body.
            PxTransform transform = PxTransform(position, orientation);
            // This body is dynamic
            PxRigidDynamic* body = m_utils.m_physics->createRigidDynamic(transform);
            // Hard coded flag so that we trigger the onSleep and onAwake callbacks for all dynamic bodies
            body->setActorFlag(PxActorFlag::eSEND_SLEEP_NOTIFIES, true);
            // Create a shape for the body
            body->createShape(PxCapsuleGeometry(p_radius, p_height / 2), *m_utils.m_physics->createMaterial(0, 0, 0));
            // Give the body some mass (since it is dynamic. Static objects probably don't need mass)
            PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
            // Add the now fully created body to the scene
            m_utils.m_worldScene->addActor(*body);

            // Finally add the body to our list
            m_bodies[p_id] = body;
            m_IDsByBodies[body] = p_id;

            SetCallback(p_id, (1 << 0), (1 << 0));
            return p_id;
        }

        void RigidBodyManagerImpl::SetTrigger(int p_id, bool p_isTrigger)
        {
            if(m_bodies.find(p_id) == m_bodies.end())
            {
                cout << "shit went wrong" << endl;
            }
            PxShape* shape;
            m_bodies[p_id]->getShapes(&shape, 1);
            shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
            shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
        }

        void RigidBodyManagerImpl::SetDrain(int p_id, bool p_isDrain)
        {
            if(m_bodies.find(p_id) == m_bodies.end())
            {
                cout << "shit went wrong" << endl;
            }
            PxShape* shape;
            m_bodies[p_id]->getShapes(&shape, 1);
            shape->setFlag(PxShapeFlag::ePARTICLE_DRAIN, p_isDrain);
        }

        void RigidBodyManagerImpl::SetCallbackFiltering(int p_bodyID, int p_thisIdMask, int p_notifyTouchOthersMask, int p_notifyLeaveOthersMask, int p_ignoreOthersMask)
        {
            if(m_bodies.find(p_bodyID) == m_bodies.end())
            {
                cout << "Physics. Rigid bodies. SetCallBackFiltering. No such body exists with ID: " << p_bodyID << endl;
                return;
            }
            PxFilterData filterData = PxFilterData(p_thisIdMask, p_notifyTouchOthersMask, p_notifyLeaveOthersMask, p_ignoreOthersMask);
            PxShape* shape;
            m_bodies.at(p_bodyID)->getShapes(&shape, 1);
            shape->setSimulationFilterData(filterData);
        }

        void RigidBodyManagerImpl::SetCallback(int p_bodyID, int p_filterGroup, int p_filterMask)
        {
            if(m_bodies.find(p_bodyID) == m_bodies.end())
            {
                cout << "shit went wrong" << endl;
            }
            PxFilterData filterData;
            filterData.word0 = p_filterGroup; // Own ID
            filterData.word1 = p_filterMask; // ID mask to filter pairs that trigger contact callback
            PxRigidActor* actor = m_bodies[p_bodyID];
            uint32_t numShapes = actor->getNbShapes();
            // Magic allocation of memory (i think)
            PxShape** shapes = (PxShape**)m_utils.m_allocator.allocate(sizeof(PxShape*) * numShapes, 0, __FILE__, __LINE__);
            actor->getShapes(shapes, numShapes);
            for(uint32_t i = 0; i < numShapes; i++)
            {
                PxShape* shape = shapes[i];
                // shape->setFlag(PxShapeFlag::ePARTICLE_DRAIN, true); // If eveything that isnt a particle should drain, not tested TODOKO
                shape->setSimulationFilterData(filterData);
            }
            if(shapes)
            {
                m_utils.m_allocator.deallocate(shapes);
                shapes = NULL;
            }
        }

        void RigidBodyManagerImpl::SetIgnoredDEBUG(int p_bodyID)
        {
            if(m_bodies.find(p_bodyID) == m_bodies.end())
            {
                cout << "Physics. Rigid bodies. SetIgnore. No such body with ID found. ID: " << p_bodyID << endl;
                return;
            }
            PxShape* shape;
            m_bodies.find(p_bodyID)->second->getShapes(&shape, 1);
            PxFilterData data = PxFilterData(1, 0, 0, 1);
            shape->setSimulationFilterData(data);
            shape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false);
            m_bodies.find(p_bodyID)->second->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
        }

        void RigidBodyManagerImpl::AddForceToBody(int p_bodyID, XMFLOAT3 p_force)
        {
            if(m_bodies.find(p_bodyID) == m_bodies.end())
            {
                cout << "shit went wrong" << endl;
            }
            if(m_bodies[p_bodyID]->isRigidDynamic())
            {
                ((PxRigidDynamic*)m_bodies[p_bodyID])->addForce(PxVec3(p_force.x, p_force.y, p_force.z));
            }
            else
            {
                throw std::runtime_error("Body is not dynamic and cannot apply force. ID: " + to_string(p_bodyID));
            }
        }

        void RigidBodyManagerImpl::AddTorqueToBody(int p_bodyID, XMFLOAT3 p_torque)
        {
            if(m_bodies.find(p_bodyID) == m_bodies.end())
            {
                cout << "shit went wrong" << endl;
            }
            if(m_bodies[p_bodyID]->isRigidDynamic())
            {
                ((PxRigidDynamic*)m_bodies[p_bodyID])->addTorque(PxVec3(p_torque.x, p_torque.y, p_torque.z));
            }
            else
            {
                throw std::runtime_error("Body is not dynamic and cannot apply torque. ID: " + to_string(p_bodyID));
            }
        }

        void RigidBodyManagerImpl::SetBodyVelocity(int p_bodyID, XMFLOAT3 p_v)
        {
            if(m_bodies.find(p_bodyID) == m_bodies.end())
            {
                cout << "shit went wrong" << endl;
            }
            if(m_bodies[p_bodyID]->isRigidDynamic())
            {
                ((PxRigidDynamic*)m_bodies[p_bodyID])->setLinearVelocity(PxVec3(p_v.x, p_v.y, p_v.z));
            }
            else
            {
                throw std::runtime_error("Body is not dynamic and cannot set velocity. ID: " + to_string(p_bodyID));
            }
        }

        void RigidBodyManagerImpl::SetBodyAngularVelocity(int p_bodyID, XMFLOAT3 p_v)
        {
            if(m_bodies.find(p_bodyID) == m_bodies.end())
            {
                cout << "shit went wrong" << endl;
            }
            // PxTransform pose = ((PxRigidDynamic*)m_bodisssssses[p_bodyID])->setMaxAngularVelocity(0);
            // pose.q = PxQuat(0, 0, 0, 1);
            if(m_bodies[p_bodyID]->isRigidDynamic())
            {
                ((PxRigidDynamic*)m_bodies[p_bodyID])->setAngularVelocity(PxVec3(p_v.x, p_v.y, p_v.z));
            }
            else
            {
                throw std::runtime_error("Body is not dynamic and set anglular velicity. ID: " + to_string(p_bodyID));
            }
        }

        void RigidBodyManagerImpl::SetBodyPosition(int p_bodyID, XMFLOAT3 p_v, XMFLOAT4 p_o)
        {
            if(m_bodies.find(p_bodyID) == m_bodies.end())
            {
                cout << "shit went wrong" << endl;
            }
            PxVec3 position = PxVec3(p_v.x, p_v.y, p_v.z);
            PxQuat orientation = PxQuat(p_o.x, p_o.y, p_o.z, p_o.w);
            PxTransform trans;
            trans.p = position;
            trans.q = orientation;
            m_bodies[p_bodyID]->setGlobalPose(trans);
            PxVec3 pos = m_bodies[p_bodyID]->getGlobalPose().p;
        }

        void RigidBodyManagerImpl::SetLinearDampening(int p_bodyID, float p_dampening)
        {
            if(m_bodies.find(p_bodyID) == m_bodies.end())
            {
                cout << "shit went wrong" << endl;
            }
            ((PxRigidDynamic*)m_bodies[p_bodyID])->setLinearDamping(p_dampening);
        }

        XMFLOAT3 RigidBodyManagerImpl::GetBodyPosition(int p_bodyID)
        {
            if(m_bodies.find(p_bodyID) == m_bodies.end())
            {
                cout << "shit went wrong" << endl;
            }
            PxVec3 p = m_bodies[p_bodyID]->getGlobalPose().p;
            return XMFLOAT3(p.x, p.y, p.z);
        }

        XMFLOAT4 RigidBodyManagerImpl::GetBodyOrientation(int p_bodyID)
        {
            if(m_bodies.find(p_bodyID) == m_bodies.end())
            {
                cout << "shit went wrong" << endl;
            }
            PxQuat q = m_bodies[p_bodyID]->getGlobalPose().q;
            return XMFLOAT4(q.x, q.y, q.z, q.w);
        }

        XMFLOAT3 RigidBodyManagerImpl::GetBodyVelocity(int p_bodyID)
        {
            if(m_bodies.find(p_bodyID) == m_bodies.end())
            {
                cout << "shit went wrong" << endl;
            }
            PxVec3 v = ((PxRigidDynamic*)m_bodies[p_bodyID])->getLinearVelocity();
            return XMFLOAT3(v.x, v.y, v.z);
        }

        XMFLOAT3 RigidBodyManagerImpl::GetBodyAngularVelocity(int p_bodyID)
        {
            if(m_bodies.find(p_bodyID) == m_bodies.end())
            {
                cout << "shit went wrong" << endl;
            }
            PxVec3 v = ((PxRigidDynamic*)m_bodies[p_bodyID])->getAngularVelocity();
            return XMFLOAT3(v.x, v.y, v.z);
        }

        float RigidBodyManagerImpl::GetLinearDampening(int p_bodyID) { return ((PxRigidDynamic*)m_bodies[p_bodyID])->getLinearDamping(); }

        std::vector<int>& RigidBodyManagerImpl::GetRecentlyWokenObjects() { return m_recentlyWokenObjects; }
        std::vector<int>& RigidBodyManagerImpl::GetRecentlySleepingObjects() { return m_recentlySleepingObjects; }

        bool RigidBodyManagerImpl::IsSleeping(int p_bodyID)
        {
            if(m_bodies.find(p_bodyID) == m_bodies.end())
            {
                cout << "shit went wrong" << endl;
            }
            bool isSleeping = true;
            if(m_bodies[p_bodyID]->isRigidDynamic())
            {
                isSleeping = ((PxRigidDynamic*)m_bodies[p_bodyID])->isSleeping();
            }
            return isSleeping;
        }

        void RigidBodyManagerImpl::RemoveBody(int p_bodyID)
        {
            if(m_bodies.find(p_bodyID) == m_bodies.end())
            {
                cout << "Physics: couldn't remove rigid body ID: " << p_bodyID << " since it did not exist" << endl;
                return;
            }
            m_bodies[p_bodyID]->release();
            m_IDsByBodies.erase(m_bodies[p_bodyID]);
            m_bodies.erase(p_bodyID);
        }

        unordered_map<PxRigidActor*, int>& RigidBodyManagerImpl::GetIDsByBodies() { return m_IDsByBodies; }

        void RigidBodyManagerImpl::SetRecentlyWokenObjects(PxActor** p_actors, int p_count)
        {
            for(size_t i = 0; i < p_count; i++)
            {
                if(p_actors[i]->isRigidBody())
                {
                    m_recentlyWokenObjects.push_back(m_IDsByBodies.find((PxRigidActor*)p_actors[i])->second);
                }
            }
        }
        void RigidBodyManagerImpl::SetRecentlySleepingObjects(PxActor** p_actors, int p_count)
        {
            for(size_t i = 0; i < p_count; i++)
            {
                if(p_actors[i]->isRigidBody())
                {
                    m_recentlySleepingObjects.push_back(m_IDsByBodies.find((PxRigidActor*)p_actors[i])->second);
                }
            }
        }

        void RigidBodyManagerImpl::ClearRecentlyWakeStatusLists()
        {
            m_recentlySleepingObjects.clear();
            m_recentlyWokenObjects.clear();
        }
    }
}