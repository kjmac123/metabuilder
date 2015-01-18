#include "mylib.h"
#include "core.h"

void CommonCodeTest()
{
	Platform::Init();
	int result = thisIsMyFunctionThereAreManyLikeItButThisOneIsMine(21);
	MB_LOGINFO("CommonCodeTest result: %i\n", result);
}
