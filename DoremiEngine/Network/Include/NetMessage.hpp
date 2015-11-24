#pragma once

/**
TODO docs
*/
struct NetMessage
{
	short MessageSequence;		//2 byte
	short MessageID;			//2 byte
	unsigned char Data[252];	//252 byte
};

