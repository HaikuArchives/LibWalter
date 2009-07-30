/*
	EasyPath:	A class for handling the various parts of a string-based file path
	Copyright 2008 DarkWyrm <darkwyrm@gmail.com>
	Released under the MIT license
*/
#include "EasyPath.h"
#include <Path.h>

EasyPath::EasyPath(const char *string)
{
	SetTo(string);
}


EasyPath::EasyPath(const BString &string)
{
	SetTo(string);
}


EasyPath::EasyPath(const EasyPath &path)
{
	SetTo(path);
}


EasyPath::EasyPath(const entry_ref &ref)
{
	SetTo(ref);
}


EasyPath::EasyPath(void)
	:	fFileNamePos(-1),
		fExtensionPos(-1)
{
}


EasyPath::~EasyPath(void)
{
}


EasyPath &
EasyPath::operator =(const EasyPath &path)
{
	SetTo(path);
	return *this;
}


EasyPath &
EasyPath::operator =(const char *string)
{
	SetTo(string);
	return *this;
}


void
EasyPath::SetTo(const char *string)
{
	fFullPath = string;
	
	if (!string) {
		fBaseName = string;
		fFolder = string;
		fExtensionPos = -1;
		fFileNamePos = -1;
	} else {
		fFileNamePos = fFullPath.FindLast("/") + 1;
		if (fFileNamePos > 0) {
			fBaseName = fFullPath.String() + fFileNamePos;
			fFolder = fFullPath;
			fFolder.Truncate(fFileNamePos - 1);
		} else {
			fFileNamePos = 0;
			fBaseName = fFullPath;
			fFolder = "";
		}
		
		fExtensionPos = fFullPath.FindLast(".") + 1;
		if (fExtensionPos < fFileNamePos)
			fExtensionPos = 0;
		
		// if the period is the first character, then we don't really have
		// an extension. .profile is an example of this. As a result, we look
		// for the period as the second character or later.
		if (fExtensionPos > 1)
			fBaseName.Truncate(fExtensionPos - fFileNamePos - 1);
	}
}


void
EasyPath::SetTo(const EasyPath &path)
{
	fFullPath = path.fFullPath;
	fBaseName = path.fBaseName;
	fFolder = path.fFolder;
	fFileNamePos = path.fFileNamePos;
	fExtensionPos = path.fExtensionPos;
}


void
EasyPath::SetTo(const entry_ref &ref)
{
	BPath path(&ref);
	SetTo(path.Path());
}


void
EasyPath::SetTo(const BString &string)
{
	SetTo(string.String());
}


const char *
EasyPath::GetFullPath(void) const
{
	return (fFullPath.CountChars() > 0) ? fFullPath.String() : NULL;
}


const char *
EasyPath::GetFolder(void) const
{
	return (fFolder.CountChars() > 0) ? fFolder.String() : NULL;
}


const char *
EasyPath::GetFileName(void) const
{
	int32 count = fFullPath.CountChars();
	return (count > 0 && fFileNamePos != count) ? fFullPath.String() + fFileNamePos : NULL;
}


const char *
EasyPath::GetBaseName(void) const
{
	return (fBaseName.CountChars() > 0) ? fBaseName.String() : NULL;
}


const char *
EasyPath::GetExtension(void) const
{
	return (fExtensionPos > 0) ? fFullPath.String() + fExtensionPos : NULL;
}


EasyPath &
EasyPath::operator <<(const char *string)
{
	Append(string);
	return *this;
}


EasyPath &
EasyPath::operator <<(const BString &string)
{
	Append(string);
	return *this;
}


void
EasyPath::Append(const char *string)
{
	if (!string)
		return;
	
	int32 count = fFullPath.CountChars();
	bool base = false;
	
	if (count > 0 && fFullPath[count - 1] == '/')
		base = true;
	
	if (base) {
		if (string[0] == '/')
			fFullPath << (string + 1);
		else
			fFullPath << string;
	} else {
		if (string[0] == '/')
			fFullPath << string;
		else
			fFullPath << "/" << string;
	}
	
	BString s(fFullPath);
	SetTo(s.String());
}


void
EasyPath::Append(const BString &string)
{
	Append(string.String());
}

