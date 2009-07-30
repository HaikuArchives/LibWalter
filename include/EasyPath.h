/*
	EasyPath:	A class for handling the various parts of a string-based file path
	Copyright 2008 DarkWyrm <darkwyrm@gmail.com>
	Released under the MIT license
*/
#ifndef EASYPATH_H
#define	EASYPATH_H

#include <Entry.h>
#include <String.h>

class EasyPath
{
public:
							EasyPath(const char *string);
							EasyPath(const BString &string);
							EasyPath(const EasyPath &path);
							EasyPath(const entry_ref &ref);
							EasyPath(void);
	virtual					~EasyPath(void);
			
			EasyPath &		operator =(const EasyPath &path);
			EasyPath &		operator =(const char *string);
			void			SetTo(const char *string);
			void			SetTo(const EasyPath &path);
			void			SetTo(const entry_ref &ref);
			void			SetTo(const BString &string);
	
			const char *	GetFullPath(void) const;
			const char *	GetFolder(void) const;
			const char *	GetFileName(void) const;
			const char *	GetBaseName(void) const;
			const char *	GetExtension(void) const;
			
			EasyPath &		operator <<(const char *string);
			EasyPath &		operator <<(const BString &string);
			void			Append(const char *string);
			void			Append(const BString &string);
private:
			BString			fFullPath,
							fBaseName,
							fFolder;
					
			int32			fFileNamePos,
							fExtensionPos;
};

#endif
