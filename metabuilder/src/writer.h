#ifndef WRITER_H
#define WRITER_H

#include "common.h"

struct WriterTarget
{
	MetaBuilderContext* ctx;

	//This ID is specific to each build environment and is used to link targets with their dependencies.
	std::string			dependencyID;
};

void mbWriterDo(MetaBuilderContext* ctx);

#endif
