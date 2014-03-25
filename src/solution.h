#ifndef SOLUTION_H
#define SOLUTION_H

#include "common.h"

class Solution : public MetaBuilderBlockBase
{
public:
	Solution(MetaBuilderBlockBase* parent);
	
    virtual E_BlockType Type() const;
	
	virtual void Process();
			
    std::string         name;
    std::string         intDir;
    std::string         outDir;
    
    TargetVector        targetVector;

protected:
};

void mbSolutionLuaRegister(lua_State* lua);

#endif
