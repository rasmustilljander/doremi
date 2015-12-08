#pragma once
// Project specific
// Standard Libraries
#include <unordered_map>
#include <vector>

namespace Doremi
{
	namespace Core
	{

		class InputHandler
		{
		public:
			/** Is a singleton. Use this method to get the EventManager*/
			static InputHandler* GetInstance();

			void DeliverEvents();
		private:
            enum class UserCommand
                //TODOEA Lägga in detta i en textfil.
            {                   //Key       Code for it
                Jump = 1,      //Space     32
                Forward = 2,    //W         119
                Backward = 4,   //S         115
                Left = 8,       //A         97
                Right = 16,      //D         100
                Fire = 32,//LeftMouseClick   1
                All = Jump | Forward | Backward | Left | Right | Fire,//Seen this be done so doing it here aswell! =D
            };
            //TODOEA Ta reda på om int verkligen e så bra där va! 
            std::unordered_map<int, UserCommand> userCmds;
			// Private constructors because Singleton
            InputHandler();
			~InputHandler();
            void CheckInputsFromEngine();
            int m_maskWithInput = 0;
            //TODOEA Fixa skiten till riktiga skiten sen när den e vettigt kopplad till Engine=O
            std::vector<int> t_musInputFromModule;
            std::vector<int> t_keyboardInputFromModule;
			static InputHandler* m_singleton;

		};
	}
}

