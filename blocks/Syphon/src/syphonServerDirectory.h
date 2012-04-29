/*
 syphonServerDirectory.h
 Cinder Syphon Implementation
 
 Created by astellato on 2/6/11
 syphonServerDirectory ny roger sodre 12/8/11
   
 Copyright 2011 astellato, bangnoise (Tom Butterworth) & vade (Anton Marini).
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "cinder/app/AppBasic.h"

class syphonServerDescription {
public:
	
	syphonServerDescription(std::string _uuid, std::string _appName, std::string _name ) {
		uuid		= _uuid;
		appName		= _appName;
		name		= _name;
	}
	
	std::string getUuid()			{ return uuid; }
	std::string getAppName()		{ return appName; }
	std::string getName()			{ return name; }
	
protected:
	std::string		uuid;
	std::string		appName;
	std::string		name;
};


class syphonServerDirectory {
public:
	syphonServerDirectory();
	~syphonServerDirectory();
	
	void		setup();
	void		update();
	void		clear();

	bool		hasChanged();
	int			getServerIndexByName( std::string _name );
	
	int			getServerCount()					{ return servers.size(); };
	syphonServerDescription & getServer(int i)		{ return *(servers[i]); } ;
	
protected:
	bool		bSetup;
	void		*mLocalDirectory;
	std::vector<syphonServerDescription*>	servers;
	
};



