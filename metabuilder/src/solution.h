#ifndef SOLUTION_H
#define SOLUTION_H

#include "target.h"

class Solution : public MakeBlock
{
public:
	Solution();
	virtual ~Solution();
	
    virtual E_BlockType GetType() const;
	virtual bool		IsA(E_BlockType t) const;
		
	virtual void Process();
			
    std::string         intDir;
    std::string         outDir;
    
    TargetVector        targetVector;

protected:
};

//-----------------------------------------------------------------------------------------------------------------------------------------

void mbSolutionLuaRegister(lua_State* lua);

#endif
